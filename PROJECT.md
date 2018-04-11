---
title: "MQTT: meterkast"
date: 2018-04-11
publishdate: 2018-04-11
categories: [projects]
draft: false
keywords: [mqtt,meterkast,project]
aliases: [/projects/mqtt-meterkast/]
---

# MQTT: Meterkast
In de meterkast van de Tkkrlab hackerspace zijn door Friedzombie drie relais en drie energiemeters geplaatst.
Die relais en energiemeters zijn, samen met de space- en verlichtingsschakelaars verbonden met een Arduino Ethernet.

<!--more-->

# Hardware
De hardware voor dit project bestaat uit:
 - 1x Arduino Ethernet
 - 1x Prototyping shield
 - 5x NPN transistor
 - 1x LM2596 DC-DC converter module

## Pinout van de Arduino
| Naam         | Pin    |
|--------------|--------|
| Ingang 1     | 3      |
| Ingang 2     | 4      |
| Ingang 3     | 5      |
| Ingang 4     | 6      |
| Ingang 5     | 7      |
| Ingang 6     | 8      |
| Interne knop | 9      |
| Uitgang 1    | A0     |
| Uitgang 2    | A1     |
| Uitgang 3    | A2     |
| Uitgang 4    | A3     |
| Uitgang 5    | A4     |
| Interne LED  | A5     |

# Aansluitingen en MQTT mapping
| Naam         | Verbonden met                                 | MQTT kanaal                    |
|--------------|-----------------------------------------------|--------------------------------|
| Ingang 1     | Space status schakelaar                       | tkkrlab/meterkast/input/0      |
| Ingang 2     | Lichtschakelaar                               | tkkrlab/meterkast/input/1      |
| Ingang 3     | Energiemeter 1: Soldeertafel                  | tkkrlab/meterkast/input/2      |
| Ingang 4     | Energiemeter 2: Vergadertafel                 | tkkrlab/meterkast/input/3      |
| Ingang 5     | Energiemeter 3: Meterkast en Verlichting      | tkkrlab/meterkast/input/4      |
| Ingang 6     | Energiemeter 4: Keuken en Boiler              | tkkrlab/meterkast/input/5      |
| Uitgang 1    | Relay 1: Soldeertafel                         | tkkrlab/meterkast/output/0     |
| Uitgang 2    | Relay 2: Verlichting 1                        | tkkrlab/meterkast/output/1     |
| Uitgang 3    | Relay 3: Verlichting 2 (Note: niet geplaatst) | tkkrlab/meterkast/output/2     |
| Uitgang 4    | Relay 4: Keuken                               | tkkrlab/meterkast/output/3     |
| Uitgang 5    | LED in verlichtingsschakelaar                 | tkkrlab/meterkast/output/4     |

# Groepenverdeling meterkast
| Groep        | Functie                 |
|--------------|-------------------------|
| 1            | Soldeertafel            |
| 2            | Vergadertafel           |
| 3            | Meterkast               |
| 4            | Verlichting             |
| 5            | Keuken                  |
| 6            | Boiler                  |

# Resources
 - [Source code](https://github.com/tkkrlab/mqtt-meterkast)

# People who worked on this project
- Friedzombie
- Renze
