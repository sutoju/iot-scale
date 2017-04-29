# ESP8266 IoT Scale 
A smart scale for following the amount of food waste produced from a household. Created @ Junction Tokyo 2017

## Hardware

```
    +-------------+
    | Load sensor |
    +-------------+
           |
+-------------------------+
| Differential Amplifier  |
|       (LM324)           |
+-------------------------+
           |
     +----------+
     | ESP8266  |
     +----------+
           |
    +-------------+
    | MQTT broker |
    +-------------+
```