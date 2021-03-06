# Gloo
Gloo - the robotic pet
Author: Alberto Tam Yong

##Demo video
https://youtu.be/xaD0OcOek-Y

##Website
https://albertotech.com/gloo

##High Level Description
Gloo is a robotic pet with emotions that you can play with. It uses a TFT display to show its expressions and a camera to detect its surroundings. An RGB LED allows the robot to show even more details with you. Gloo is able to recognize colors and faces, and it can play pee-ka-boo.

##User Guide
###Controls
- To wake up Gloo, show him one of its toys: the green marker, the red marker, or a face.
- When Gloo sees you, he will smile at you and express what toy or feature it recognizes:
- Red or Green depending on the marker, or Blue if it sees your face.
- To play pee-ka-boo, hide his toys and all faces. Once it goes sad, show him a toy or face, and he will get excited.
- If nothing happens for longer than 8 seconds, it will go to normal “stand-by” mode.
- If nobody plays with him for over 15 seconds, it will go to sleep.

###Special Considerations
- The Raspberry Pi takes some time to boot up. From a cold start, Gloo takes 1 minute to come to live.

##Technologies and Components
###Peripherals and Devices:
- 1 x Teensy 3.1
- 2 x Hobby Servo Motors - SG90
- 1 x RGB LED
- 1 x TFT Display
- 1 x Raspberry Pi B+
- 1 x Webcam 

The Teensy 3.1 was programmed on the Arduino IDE environment. The TFT Display operates using SPI. The Teensy 3.1 and the Raspberry Pi communicate with each other via UART. The Teensy 3.1 can also communicate with a computer via USB by using UART. The RGB LED is controlled using PWM. On the Raspberry Pi, the image processing is done using OpenCV on Python 2.7.

###Materials (with links)
- Teensy 3.1 (retired, replaced by Teensy 3.2) http://pjrc.com/store/teensy32.html
- TFT display http://pjrc.com/store/display_ili9341.html
- Raspberry Pi B+ (or any other Raspberry Pi) https://www.sparkfun.com/products/13297
- Webcam - Logitech C270 [Amazon](http://www.amazon.com/Logitech-Webcam-Widescreen-Calling-Recording/dp/B004FHO5Y6/ref=sr_1_2?s=pc&ie=UTF8&qid=1450209522&sr=1-2&keywords=logitech+webcam)
- Pan/Tilt Set with 2 micro servos (SG90) https://acrobotic.com/acs-00008
- RGB LED (common anode) https://www.sparkfun.com/products/10821
- 400-contact breadboard https://www.sparkfun.com/products/12002
- 170-contact breadboard https://www.sparkfun.com/products/12043
- Male-to-male jumper wires https://www.sparkfun.com/products/11026
- Female-to-female jumper wires https://www.sparkfun.com/products/12796
- External battery power bank (with 2 USB ports) [Amazon](http://www.amazon.com/KMASHI-10000mAh-External-Portable-Powerful/dp/B00JM59JPG/ref=sr_1_1?ie=UTF8&qid=1450210301&sr=8-1&keywords=powerbank)
- 2 x Micro-B usb cables [Amazon](http://www.amazon.com/Aukey-Hi-speed-Charging-Smartphones-CB-D10/dp/B00SWAS0DY/ref=sr_1_1?ie=UTF8&qid=1450210350&sr=8-1&keywords=usb+cable+aukey)

###Other Materials
- Micro SD card adaptor http://pjrc.com/store/sd_adaptor.html 
- Saleae Logic Analyzer https://www.sparkfun.com/products/13195

##Source Files

###Source Code: (Code developed and compiled on Arduino IDE)
####Arduino/gloo_6/gloo_6.ino
- Main code with all the functions and state machines.
- SM1_Tick(): Updates servo
- SM2_Tick(): Updates TFT
- SM4_Tick(): Reads camera input
- SM5_Tick(): RGB LED control
- SM_Serial_Check_Tick(): For input menu using a UART connection

####Raspberry_Pi/color_track_4.py
- Python code running on Raspberry Pi B+. The Raspberry Pi was set up with the following packages: OpenCV and numpy.

###Headers: (Add the libraries to your Arduino IDE from the /lib folder)
####SPI.h
- Used for basic SPI communication implementation.

####Adafruit_GFX.h
- Assisting library that includes functions for drawing basic shapes, such as circles, squares, lines and letters.

####Adafruit_ILI9341.h
- Library that includes all the protocols and commands to control the TFT display.

####Servo.h
- Used for controlling servo motors with PWM.

