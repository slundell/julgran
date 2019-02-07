# julgran
Water monitor for Christmas trees. ESP8266, MQTT and OpenHAB2

Use whatever water sensor you want. It needs a HIGH input to detect low water (trivial to change in the .ino though)
Publishes reading changes to MQTT. OpenHAB2 with a MQTT binding reads this and starts blinking the Christmas tree lights 
until watered. In my setup, it does not blink in the night, or when nobody is at home. Adapt the .rules to your needs.