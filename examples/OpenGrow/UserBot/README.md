# GroLab™ UserBot Basic Example
Source code for Arduino with GroLab UserBot Shield to interface with GroNode.
The code uses as base the SPI port for th RF Communications using the respective pins:
<br /><br />
NRF24L01+ Control Pins (Arduino Side)<br />
SCK  - Digital Pin 13<br />
MISO - Digital Pin 12<br />
MOSI - Digital Pin 11<br />
SS   - Digital Pin 10<br />
CE   - Digital Pin 9<br />
<br />
The code use as example the sampling of 3 Buttons and a LM35 Temperature sensor.<br />
Button 1 - Analogue Input A0 <br />
LM35 Temperature Sensor - Analogue Input A1<br />
LED - Digital PWM pin 3 

<br />
<br />
You can find the wiring diagram below.<br />

![Fritzing Schematic for UserBot Example](https://www.open-grow.co.uk/shop/img/opengrowimgs/schematics/description/userbot-schematic-button-lm35-led.png)
<br />
