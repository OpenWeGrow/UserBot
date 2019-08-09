# GroLab™ UserBot
Source code for Arduino using a GroLab UserBot Shield to interface with the GroLab GroNode.
This repository has all the base code to RF interface your Arduino with GroNode using the UserBot Shield by Open Grow™.
<br />
The shield uses the Nordic SemiConductor NRF24L01+ chip in the SPI port of the Arduino board and one extra pin for RX/TX Control.<br /><br />
Control Pins (Arduino Side)<br />
SCK  - Digital Pin 13<br />
MISO - Digital Pin 12<br />
MOSI - Digital Pin 11<br />
SS   - Digital Pin 10<br />
CE   - Digital Pin 9<br />
<br />
If you need to control a lamp, you should have a relay, if you need to sample an analogue voltage for any purpose, you need to use Arduino analogue pins or get an external ADC.<br />
This is a plug'n'play shield for your Arduino that does not need any external power, and allows you to interface with whatever sensor or device you may need.
<br />And below an overview of the GroLab™ UserBot Shield for Arduino<br />
![alt text](https://www.open-grow.co.uk/shop/img/opengrowimgs/schematics/description/UserBot_Info.png)
<br />You will find several examples of usage of this shield, with the most complete and basic one being found at ../examples/OpenGrow/UserBot that implements one button, one temperature sensor and one LED.
<br />All the necessary, code and schematics can be found in the respective example page.