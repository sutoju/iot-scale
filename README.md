# ESP8266 IoT Scale 
A smart scale for following the amount of food waste produced from a household. Created @ Junction Tokyo 2017 with <3.

![scale](https://github.com/sutoju/iot-scale/blob/master/hw/scale.png?raw=true)

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

The full schematics is [here](https://github.com/sutoju/iot-scale/blob/master/hw/schema.pdf).

## License

![Creative Commons License](https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png)

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-nc-sa/4.0/)