/*
Listen_and_sendPID
010 removed sleep code now using on/off curcuit high pin 8 for off. is changing pin for A/C(pin 7)
(b) is with test output
009 28 Dec 2013  Adding sleep feature (also will capture AC on request).
008 24 dec 2013  Corrected delay to give aprox 15mSec between can sends
007 15 Nov 2013  Reworked PID send to sequence between RPM and others. fixed mil light(007a)
006 22 Aug 2013  Revised to use timers to send PID's and Can bus messages.  Reworked recieve routine to not stop if a partial message is recieved (similar to IPOD controler program)
005 started a rewrite to have elm updates not slow loop and less frequent and include AFM
004 April 2013 works with Laguna and E46 cluster for RPM.  Temp dropped so shortend delay from 10 to 8 shorter delay will require the MPG calculation to be altered.  
    Currently no MPG calculation exists. (shortening delay did not fix temp see version 005 for fix)

 Arduino Connections:
ELM wires go in pins D2 and D3, RX,TX
Also connect to the ELM, 5V Power and ground.  No other connections needed

 This program is designed to extract the RPM, temp etc using PID's using the ELM327 clone
then output the signals on CAN bus for a BMW E46 instrument cluster

A_declarations :has variables and constants
A_OBDII_Dec    :Global Declarations that are needed for the OBDII subroutines
A_MCP_DEC      :Declarations needed for the MCP2515 subroutines
B_main         :has void setup,loop,send_rpm,send_temp,send_fuel,
C_ELM_init     :Intializes the ELM comunication with the ECU
C_OBDII        :Subroutines for using PID's
C_can          :Can message subroutines
D_MCP2515      :MCP2515 library converted to subroutine for easy altering























*/

