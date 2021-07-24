****************
* C++ Joystick *
****************

Created for Linux, untested elsewhere.

****************
* C++ Files    *
****************

joyServer.cpp 
joyClient.cpp

*****************
* C++ compiling *
*****************

g++ joyServer.cpp -o joyServer
g++ joyClient.cpp -o joyClient

**********************
* Command line Usage *
**********************

joyClient <IP address> <port>
joyServer <port>

*****************
* Runtime Usage *
*****************

joyClient
     Q1. How many Buttons?
     Q2. How many axis?

***************
* Description *
***************

joyClient - Tx - Transmits to IP address/port - uses /dev/input/js0
joyServer - Rx - Recieves on port - only outputs data - actions up to users modification
