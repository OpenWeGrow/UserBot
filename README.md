# GroLab™ UserBot Shield
![Open Grow UserBot Shield - Arduino Compatible](https://user-images.githubusercontent.com/53873020/63694532-8f40d980-c80e-11e9-8539-5cb5e81fc7bd.png)
<br />
<br />
Source code for Arduino using a [GroLab UserBot Shield](https://opengrow.pt/shop/grolab-modules/23-grolab-userbot-shield-for-arduino) to interface with the [GroLab GroNode](https://opengrow.pt/gronode).
This repository has all the base code to RF interface your Arduino with GroNode using the UserBot Shield by Open Grow™.
<br />
The shield uses the Nordic SemiConductor NRF24L01+ chip in the SPI port of the Arduino board and one extra pin for RX/TX Control.
<br />
If you need to control a lamp, you should have a relay, if you need to sample an analogue voltage for any purpose, you need to use Arduino analog pins or get an external ADC.<br />
This is a plug'n'play shield for your Arduino that does not need any external power, and allows you to interface with whatever sensor or device you may need.
<br />
### Control Pins (Arduino Side)
SCK  - Digital Pin 13<br />
MISO - Digital Pin 12<br />
MOSI - Digital Pin 11<br />
SS   - Digital Pin 10<br />
CE   - Digital Pin 9<br />
### Overview
Below you can find an overview of the GroLab™ UserBot Shield for Arduino<br />
![Userbot Shield Breakout Board](https://www.open-grow.co.uk/shop/img/opengrowimgs/schematics/description/UserBot_Info.png)
### Examples
You will find several examples of usage of this shield, with the most complete and basic one being found at ../examples/OpenGrow/UserBot that implements one button, one temperature sensor and one LED.
<br />All the necessary, code and schematics can be found in the respective example page.
<br />
### Documentation
The official GroLab UserBot manual [can be found here](https://opengrow.pt/downloads/manuals/grolab/userbot/UserBot-Shield-(EN-ES-FR-PT-IT)-2019V01.pdf).
It is available in English, Spanish, French, Portuguese, and Italian.
### Support
If you encounter any issue, please contact our [Support Center](https://opengrow.pt/support).
<br />
<br />
