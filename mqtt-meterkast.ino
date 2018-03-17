#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define PIN_SWITCH1 3
#define PIN_SWITCH2 4
#define PIN_SWITCH3 5
#define PIN_SWITCH4 6
#define PIN_SWITCH5 7
#define PIN_SWITCH6 8
#define PIN_INTSWITCH 9
#define PIN_RELAY1 A0
#define PIN_RELAY2 A1
#define PIN_RELAY3 A2
#define PIN_RELAY4 A3
#define PIN_EXTLED A4
#define PIN_LED A5

//Config
#define NUM_RELAYS 4
#define NUM_LEDS 2
#define NUM_SWITCHES 6
#define POLL_INTERVAL 5

byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x66, 0x29 };
IPAddress ip(10, 42, 1, 3);
IPAddress server(10, 42, 1, 2);

//Variables
const uint8_t switches[NUM_SWITCHES] = {PIN_SWITCH6, PIN_SWITCH5, PIN_SWITCH4, PIN_SWITCH3, PIN_SWITCH2, PIN_SWITCH1};
const uint8_t relays[NUM_RELAYS] = {PIN_RELAY1, PIN_RELAY2, PIN_RELAY3, PIN_RELAY4};
bool relayState[NUM_RELAYS] = {0,0,0,0};
uint8_t switchTimeout[NUM_SWITCHES] = {0,0,0,0,0,0};
bool switchState[NUM_SWITCHES] = {1,1,1,1,1,1};
unsigned long lastPollTime = 0;

//Objects
EthernetClient ethClient;
PubSubClient client(ethClient);

void setup() {
  pinMode(PIN_INTSWITCH, INPUT);
  pinMode(PIN_SWITCH1, INPUT);
  pinMode(PIN_SWITCH2, INPUT);
  pinMode(PIN_SWITCH3, INPUT);
  pinMode(PIN_SWITCH4, INPUT);
  pinMode(PIN_SWITCH5, INPUT);
  pinMode(PIN_SWITCH6, INPUT);
  pinMode(PIN_RELAY1, OUTPUT);
  pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_RELAY3, OUTPUT);
  pinMode(PIN_RELAY4, OUTPUT);
  pinMode(PIN_EXTLED, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac); //, ip);
}

void toggleRelay(uint8_t relay) {
  if (relay>=NUM_RELAYS) return;
  if (relayState[relay]) {
    relayState[relay] = false;
  } else {
    relayState[relay] = true;
  }
  client.publish("tkkrlab/meterkast/debug",String("Relay "+String(relay)+" set to state "+String(relayState[relay])).c_str());
  digitalWrite(relays[relay], HIGH);
  digitalWrite(PIN_LED, HIGH);
  delay(200);
  digitalWrite(relays[relay], LOW);
  digitalWrite(PIN_LED, LOW);
}


bool fallbackEnabled = false;
uint8_t fbCounter = 0;
bool fallbackState = false;
void fallback() {
  if (!digitalRead(PIN_INTSWITCH)) {
    fbCounter++;
    if (fbCounter>250) {
      fallbackEnabled = !fallbackEnabled;
      client.publish("tkkrlab/meterkast/debug",String("Fallback "+String(fallbackEnabled)).c_str());
      fbCounter = 0;
    }
  } else {
    fbCounter = 0;
  }
  digitalWrite(PIN_LED, fallbackEnabled);
  if (fallbackEnabled) {
    for (uint8_t i = 0; i<NUM_SWITCHES; i++) {
      if (checkSwitch(i)) {
        if (i==0) {
          //Spaceswitch
        }
        if (i==1) {
          //Lightswitch
          fallbackState = !fallbackState;
          digitalWrite(PIN_EXTLED, fallbackState);
          for (uint8_t j = 0; j<NUM_RELAYS; j++) {
            if (relayState[j]!=fallbackState) {
              toggleRelay(j);
            }
          }
        }
      }
    }
  }
}

bool checkSwitch(uint8_t sw) {
  if (sw>=NUM_SWITCHES) return false;
  bool changed = false;
  uint8_t pin = switches[sw];
  bool state = digitalRead(pin);
  if (switchTimeout[sw]>0) {
    switchTimeout[sw]--;
  }else if (switchTimeout[sw]==0) {
    if (switchState[sw] != state) {
      switchState[sw] = state;
      switchTimeout[sw] = 80;
      changed = true;
    }
  }
  return changed;
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadString = "";
  for (int i=0;i<length;i++) {
    payloadString = payloadString + ((char)payload[i]);
  }

  client.publish("tkkrlab/meterkast/debug", String("Callback length "+String(length)+"("+payloadString+")").c_str());
  
  uint8_t targetState = payloadString.toInt();
  if (targetState>1) targetState = 1;
  uint8_t output = getValue(topic,'/',3).toInt();
  client.publish("tkkrlab/meterkast/debug",String(String(targetState)+" on "+String(output)+" ("+payloadString+")").c_str());
  if (output<NUM_RELAYS) {
    if (relayState[output]!=targetState) {
      toggleRelay(output);
      client.publish("tkkrlab/meterkast/debug",String("Toggle "+String(output)).c_str());
    } else {
      client.publish("tkkrlab/meterkast/debug",String("NO toggle "+String(output)).c_str());
    }
  } else if (output==NUM_RELAYS) {
    digitalWrite(PIN_EXTLED, targetState);
    client.publish("tkkrlab/meterkast/debug",String("LED "+String(output)).c_str());
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect("meterkast")) {
      client.subscribe("tkkrlab/meterkast/output/#");
      client.publish("tkkrlab/meterkast/debug","Connected.");
    } else {
      unsigned long currentTime = millis();
      if (currentTime - lastPollTime >= POLL_INTERVAL) {
        fallback();
        lastPollTime = currentTime;
      }
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentTime = millis();
  if (currentTime - lastPollTime >= POLL_INTERVAL) {
    fallback();
    lastPollTime = currentTime;

    //client.publish("tkkrlab/meterkast/polltime",String(lastPollTime).c_str());
    
    for (uint8_t i = 0; i<NUM_SWITCHES; i++) {
      if (checkSwitch(i)) {
        String topic = "tkkrlab/meterkast/input/"+String(i);
        client.publish(topic.c_str(),String((!switchState[i])&0x01).c_str());
      }
    }
  }
}
