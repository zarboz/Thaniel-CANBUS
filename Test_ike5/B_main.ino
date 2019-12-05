SoftwareSerial ELM(2, 3); // RX, TX;  ELM will be serial name

void setup()
{

  // enable debugging
  //#define _DEBUG
  pinMode(slaveSelectPin,OUTPUT);
  pinMode(11,OUTPUT);
  digitalWrite(slaveSelectPin,HIGH);
  digitalWrite(11,HIGH);
  len= 8;  //length set at 8 for all messages sent
  extended = 0x0;  //set as not extended format for all messages sent

  // Can bus speeds  (E46 is 500 Kbps)
  // 500 Kbps       = MCP2515_SPEED_500000,
  CAN_begin(MCP2515_SPEED_500000);  // set can baud rate
  CAN_setMode (CAN_MODE_NORMAL); // set can mode

  //set up serial port for debugging with data to PC screen
  Serial.begin(115200);  
Serial.println(VERSION);
  rMSB = 0x0F; //set up test values
  tempValue = 0xA9; //set up test values
  set_testvalues(); //set up test values
  del = 5; //Minimum time between can bus sends (could be longer due to program slownes)
  PID_del = 5; //Minimum time between PID sends (could be longer as waits for response)
}

void loop()
{

  if(millis() - time_test > 150) set_testvalues(); //after x second get new values.

  //The main loop calls subroutines to send the actual messages

    //the commented out section used for Debugging
  //***************************RPM print***************** 
  //  Serial.print(time_PID);
  //  Serial.print(" ");
  //  Serial.println(RPM);
  //****End RPM print ****************************************   

  //***************************MPG Print*****************  
  //  Serial.print(timeMPG);
  //  Serial.print(" ");
  //  Serial.println(RPM);
  //****End MPG print **************************************** 

  //  send_fuel();//ARBID: 545 100ms-400ms
  //  send_temp(); //ARBID: 329 20ms-30ms
  //  send_RPM(); //ARBID: 316, 40ms-60ms

  if (PID_flag == 0 && (millis() - time_PID) > PID_del) PID_send(); //send PID for RPM at regular intervals  &&  millis() - time_PID > 50

  if (PID_flag > 0) PID_rec(); //check for message ELM if a PID has been sent (flag > 0).  Use timer or other means instead of flag?

  if (millis() - time_CAN > del) CAN_bus_send(); //send CAN bus messages at regular intervals


}// End of Main Void

//***************************************************************************
void CAN_bus_send() // Send CAN bus messages at regular intervals
{//rotates through 3 different messages.  Sends the RPM twice (every other one)

  if(CAN_flag == 0 || CAN_flag == 2) send_RPM(); //ARBID: 316
  if(CAN_flag == 1) send_temp(); //ARBID: 329 20ms
  //if(CAN_flag == 2) send_RPM(); //ARBID: 316
  if(CAN_flag == 3) send_fuel(); //ARBID: 545

  CAN_flag ++; //increment flag
  if(CAN_flag > 3) CAN_flag = 0;  //if flag exceeds max reset to 0 (should this be zero or 1)?
  time_CAN = millis(); //reset timer for RPM send
}

//***************************************************************************
void PID_send() // ELM_send("01 XX 1\r");  //Sends pid to ELM XX being the PID
{
  if(PID_flag_o == 0) ELM_send("01 0C 1\r"); //RPM send PID to ELM
  if(PID_flag_o == 1) ELM_send("01 05 1\r");  //Sends pid ENGINE_COOLANT_TEMP
  if(PID_flag_o == 2) ELM_send("01 0C 1\r"); //RPM send PID to ELM
  if(PID_flag_o == 3) ELM_send("01 10 1\r");  //Sends pid ENGINE_maf
  if(PID_flag_o == 4) ELM_send("01 0C 1\r"); //RPM send PID to ELM
  if(PID_flag_o == 5) ELM_send("01 01 1\r");  //Sends pid ENGINE_MIL status
  if(PID_flag_o == 6) ELM_send("01 0C 1\r"); //RPM send PID to ELM
  if(PID_flag_o == 7) ELM_send("01 11 1\r");  //Sends pid ENGINE_TPS status 


  PID_flag_o ++; //Pid >1 set PID flag so program will listen for response.  Increment do send next message
  if(PID_flag_o > 7) PID_flag_o = 0; //if flag exceeds max reset to 0.  back to start
  PID_flag = 1; //set flag to know to wait for ELM response
  time_PID = millis(); //reset timer for RPM send
}

//***************************************************************************
void send_RPM() //ARBID 316, RPM
{
  id = 0x316;  //From ECU
  data[0]= 0x05;
  data[1]= 0x62;
  data[2]= rLSB;  //RPM LSB0xFF
  data[3]= rMSB; //RPM MSB "0x0F" = 600 RPM0x4F
  data[4]= 0x65;
  data[5]= 0x12;
  data[6]= 0x0;
  data[7]= 0x62;
  CAN_send();   
}//end RPM Loop  

void send_temp() //ARBID 329,  Temperature, Throttle position
{ 
  //********************temp sense  *******************************
  //  The sensor and gauge are not linear. 
  //  data[1] values and correlating needle position:
  //  0x56 = First visible movment of needle starts
  //  0x5D = Begining of Blue section
  //  0x80 = End of Blue Section
  //  0xA9 = Begin Straight up
  //  0xC1 = Midpoint of needle straight up values
  //  0xDB = Needle begins to move from center
  //  0xE6 = Beginning of Red section
  //  0xFE = needle pegged to the right
  // MAP program statement: map(value, fromLow, fromHigh, toLow, toHigh)

  //Can bus data packet values to be sent
  id = 0x329;  //From ECU
  data[0]= 0xD9;
  data[1]= tempValue; //temp bit tdata  From OBDII
  data[2]= 0xB2;
  data[3]= 0x19;
  data[4]= 0x0;
  data[5]= 0xEE; //Throttle position currently just fixed value
  data[6]= 0x0;
  data[7]= 0x0;
  CAN_send();

}//end Temperature Loop

void send_fuel()//ARBID 545
//From ECU, MPG, MIL, overheat light, Cruise
// ErrorState variable controls:
//Check engine(binary 10), Cruise (1000), EML (10000)
//Temp light Variable controls temp light.  Hex 08 is on zero off
{

  //**************** set Error Lights & cruise light ******
/*  if(tempValue>0xE6){ // lights light if value is 229 (hot)
    tempLight = 8;  // hex 08 = Overheat light on
  }
  else {
    tempLight = 0; // hex 00 = overheat light off
  }*/

  // int z = 0x60; // + y;  higher value lower MPG
  if (LSBdata + MPG > 0xFF){ //if adding will go past max value
    MSBdata = MSBdata + 0x01;  //increment MSB
  }
  LSBdata=LSBdata + MPG; //increment values based on inj input

  //For debugging
  //   Serial.print(LSBdata);
  //   Serial.print(" ");
  //   Serial.println(MPG);

  //Can bus data packet values to be sent
  id = 0x545;  //From ECU, MPG, MIL, overheat light, Cruise
  data[0]= ErrorState;  //set error byte to error State variable
  data[1]= LSBdata;  //LSB Fuel consumption
  data[2]= MSBdata;  //MSB Fuel Consumption
  data[3]= tempLight;  //Overheat light
  data[4]= 0x7E;  //oil temp
  data[5]= b5; //0x10;  //unknown
  data[6]= b6; //0x00; //unknown
  data[7]= 0x18;  // unknown

  //Time captures for MPG gauge
  //  timeMPGp1 = micros();
  //  timeMPG = timeMPGp1 - timeMPGp2;
  //  timeMPGp2 = timeMPGp1;

  CAN_send();  //can send 545

} //end send_Fuel if








