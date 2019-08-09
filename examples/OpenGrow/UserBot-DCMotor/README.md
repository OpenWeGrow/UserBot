# GroLab™ UserBot DC Motor Example
Source code for Arduino with GroLab UserBot Shield controlling a DC Motor.
The code uses as base the SPI port for th RF Communications using the respective pins:
<br /><br />
NRF24L01+ Control Pins (Arduino Side)<br />
SCK  - Digital Pin 13<br />
MISO - Digital Pin 12<br />
MOSI - Digital Pin 11<br />
SS   - Digital Pin 10<br />
CE   - Digital Pin 9<br />
<br />
And the digital PWM pin 5 of the Arduino to switch a N-Type MOSFET that powers the DC Motor<br />
DC Motor - Digital Pin 5 <br />
<br />
You can find the schematic below.<br />

![Fritzing Schematic for UserBot Example](https://www.open-grow.co.uk/shop/img/opengrowimgs/schematics/description/UserBot-breadboard-peristaltic-dcmotor.png)
<br />
