/*
 This program is designed to Sense RPM and Temperature input
then output the signals on CAN bus for a BMW E46 instrument cluster
V005 V003 with added dash lights.
v004 SCRAPPED
V003 Added check for zero if time exceeds time for slow RPM.  Alter beginning time so inital RPM is not negative.  #defines for TP and temp.  Fixed error in overheat light error checking for bad pulse
V002 changed to counting 1 rev before calculating RPM -  Works on bench test
 
  
A_declarations : has variables and constants
B_main         :has void setup,loop,send_rpm,send_temp,send_fuel,and both inerupts 
C_can          :has all of the files from CAN.cpp in the CAn library+ little from Mcp2515.cpp
D_mcp2515      :has all files from mcp2515.cpp + my_spi.h
only uses spi library


 Arduino Connections:
 Digital Two (D2) is RPM input. 
 Digital Three (D3) is injection pulse input
 Analog Zero (A0) is Temperature input.
 Analog Three (A3) grounded is check engine light
 Digital Seven (D7) AC off or on (High is off) output


  





















*/

