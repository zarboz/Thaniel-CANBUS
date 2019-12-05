

void setup()
{

  //ibus setup part
  //#define Kbus_SERIAL_DEBUG  //uncomment to see some ibus message
  //#define RPM_DEBUG //uncomment to have RPM print to screen, In sense RPM
  //#define RPM_DEBUG2 //uncomment to have RPM print to screen, In send RPM debug
//#define RPM_DEBUG3 //uncomment to have pulse time print to screen
  pinMode(8,OUTPUT); //Set pin for turn off signal
  digitalWrite(8,HIGH); //low is OFF requst, need tx pin low as well for off (high turns TX ENABLE)
  // start serial communication at the adapter defined baudrate
  Kbus.begin(9600, SERIAL_8E1); //Baud rate, Parity, stopbits (1 if blank))


  //end ibus setup   
  //set up serial port for debugging with data to PC screen
  Serial.begin(115200);  

  Serial.println("Started0");
  //init SPI
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  pinMode(AC_pin,OUTPUT);
  /* Enable internal pull-ups */
  digitalWrite(AC_pin, LOW); //set AC to default off (currently high)
  pinMode(CAN_CS,OUTPUT);
  pinMode(ADC_CS,OUTPUT);
  pinMode(MIL_pin,INPUT_PULLUP); 
  pinMode(2,INPUT_PULLUP);
  pinMode(BMS_C_pin,INPUT_PULLUP);
  pinMode(BMS_L_pin,INPUT_PULLUP);
  pinMode(SS,OUTPUT);
  digitalWrite(CAN_CS, HIGH);
  digitalWrite(ADC_CS, HIGH); 

  //  setup interupt for RPM and INJ capture.
  //  When pin Digital 2 (interupt 0) goes from low to high (rising)
  //  When pin Digital 3 (interupt 1) goes from low to high (rising)
  //  The program will call Void RPM_sense or INJ_sense
  attachInterrupt(0, RPM_sense, RISING); //LOW, CHANGE, RISING, FALLING
  attachInterrupt(1, INJ_sense, FALLING); //LOW, CHANGE, RISING, FALLING

    len= 8;  //length set at 8 for all messages sent
  extended = 0x0;  //set as not extended format for all messages sent

  // Can bus speeds  (E46 is 500 Kbps)
  // 500 Kbps       = MCP2515_SPEED_500000,
  CAN_begin(MCP2515_SPEED_500000,CAN_CS);  // set can baud rate
  CAN_setMode (CAN_MODE_NORMAL,CAN_CS); // set can mode

  Pulse_Count = p_REV; //set pulse count to max pulse to get timers set correctly


}

void loop()
{
  time2 = millis()-time_Kbus;
  //************************  kbus shut down if no cativity on the kbus *******************
  if(time2 > 60000) //1 min (60 mSec)of ibus inactivity turn off
  {

    //Serial.print("  ");
    //Serial.println(time_Kbus);
    Serial.println("Kbus shut down");

    digitalWrite(8,LOW); //low is OFF requst, need tx pin low as well for off (high turns TX ENABLE)
    Kbus.end();
    pinMode(Kbus_tx,OUTPUT);  
    digitalWrite(Kbus_tx,LOW); //TX signal low with wake pin low turns off linbus chip
    delay(1000);

    //  digitalWrite(8,HIGH); //reset to on (if using USB cable and didn't really go off)
    Kbus.begin(9600, SERIAL_8E1); //Baud rate, Parity, stopbits (1 if blank))
    while(!Kbus.available()) //wait here until kbus signals come  Should not get here if power off worked.
    {//just wait.
    }// end if available
    Serial.println("Kbus started");
  }// end ibus time out
  //end ibus part

  //The main loop calls subroutines to send the actual messages

  if (millis() - time_CAN > del) CAN_bus_send(CAN_CS); //send CAN bus messages at regular intervals
  if (millis() - time_TP > 200) get_TP(ADC_CS); //update temperature value based
  if (millis() - time_temp > 200) get_temp(ADC_CS); //update temperature value based
  else // if getting temp skip the rest to get to next bus send quickly.
  {
    if (CAN_available(CAN_CS))
    {
      CAN_getMessage(CAN_CS);
      if(id == 0x615) CAN_ProcessMessage();
    }

    //    if (millis() - time_debug > 100)  //update screen with values at intervals if not going to slow RPM
    //    {
    //      Serial.print(tempValue);  //debug
    //      Serial.print(" = tempValue(V) ,  "); //debug
    //      Serial.print(RPM); //debug
    //      Serial.print(" = RPM,  "); //debug
    //      Serial.print(MPG); 
    //      Serial.println(" = MPG,  "); //debug
    //    }
  } //End if/else
}// End of Main Void

//***************************************************************************
void CAN_bus_send(int CS_pin) // Send CAN bus messages at regular intervals
{//rotates through 3 different messages.  Sends the RPM twice (every other one)

  if(CAN_flag == 0 || CAN_flag == 2) send_RPM(CS_pin); //ARBID: 316
  if(CAN_flag == 1) send_temp(CS_pin); //ARBID: 329 20ms
  //if(CAN_flag == 2) send_RPM(); //ARBID: 316
  if(CAN_flag == 3) send_fuel(CS_pin); //ARBID: 545

  CAN_flag ++; //increment flag
  if(CAN_flag > 3) CAN_flag = 0;  //if flag exceeds max reset to 0
  time_CAN = millis(); //reset timer for Can send
}

//******************************* ARBID 316 RPM *************
void send_RPM(int CS_pin) //ARBID 316, RPM
{

  if((micros() - timeRPM2) > 1200000 / p_REV) RPM = 0; //check to see if pulse has not been rec'd in x sec (50 RPM) Motor has stopped then set RPM to zero
  else  RPM = 60000000/timeRPM; // if timeRPM is 
  rMSB = RPM/40;  // MSB of RPM value
  rLSB = RPM-(rMSB*40); //LSB

#if defined(RPM_DEBUG2)  //debug
  Serial.print ("timeRPM_Send = ");
  Serial.print(timeRPM);
  Serial.print (" RPM = ");
  Serial.println(RPM);
#endif
  id = 0x316;  //From ECU
  data[1]= 0x07;  //was 05 but didn't show all the data
  data[2]= rLSB;  //RPM LSB0xFF
  data[3]= rMSB; //RPM MSB "0x0F" = 600 RPM0x4F
  data[4]= 0x65;
  data[5]= 0x12;
  data[6]= 0x0;
  data[7]= 0x62;
  CAN_send(CS_pin);   
}//end RPM Loop  

//********************temp sense  *******************************
void get_temp(int CS_pin) //read temperature sender voltage
{
  tempValue = readVoltage(0,CS_pin);
  //  Serial.print("tempvalue = ");
  //  Serial.println(tempValue);
  //  The sensor and gauge are not linear.  So if the sensed 
  //  Voltage is less than the mid point the Map function
  //  is used to map the input values to output values For the gauge
  //  output values (in decimal):
  //  86 = First visible movment of needle starts
  //  93 = Begining of Blue section
  //  128 = End of Blue Section
  //  169 = Begin Straight up
  //  193 = Midpoint of needle straight up values
  //  219 = Needle begins to move from center
  //  230 = Beginning of Red section
  //  254 = needle pegged to the right
  // MAP program statement: map(value, fromLow, fromHigh, toLow, toHigh)
#define Temp_Min_Volts 0 //voltage from sensor at min guage reading
#define Temp_Mid_Volts 2.5 // vlotage for guage to be straight up
#define Temp_Max_Volts 4.53 //Voltage from Sensor at Max voltage


  if(tempValue < 964){  //if pin voltage < mid point value 
    tempValue2= map(tempValue,1024/5*Temp_Min_Volts,1024/5*Temp_Mid_Volts,86,173); //Map lower half of range
  }
  else {
    tempValue2= map(tempValue,1024/5*Temp_Mid_Volts,1024/5*Temp_Max_Volts,219,254); //Map upper half of range
  }  
  time_temp = millis(); //reset timer for get_temp
  // Serial.println(tempValue,BIN);
}// end get_temp void

//************************** Get TP *********************************
void get_TP(int CS_pin) //read temperature sender voltage
{
  TPValue = readVoltage(1,CS_pin);
#define TP_Voltage_Zero_throttle .7  // in volts
#define TP_Voltage_Full_throttle 3.8 // in volts (max = 1024 TP value)
  // MAP program statement: map(value, fromLow, fromHigh, toLow, toHigh)
  TPValue2= map(TPValue,1024/5*TP_Voltage_Zero_throttle,1024/5*TP_Voltage_Full_throttle,0,254); //Map 
  time_TP = millis(); //reset timer for get_temp
}// end get_temp void


//******************************* ARBID 329 temp *************
void send_temp(int CS_pin) //ARBID 329,  Temperature, Throttle position
{ 
  //Can bus data packet values to be sent
  id = 0x329;  //From ECU
  data[0]= 0x07;  //was D9
  data[1]= tempValue2; //temp bit tdata
  data[2]= 0xB2;
  data[3]= 0x19;
  data[4]= 0x0;
  data[5]= TPValue2; //Throttle position sensed from voltage
  data[6]= 0x0;
  data[7]= 0x0;
  CAN_send(CS_pin);

}//end Temperature Loop

//******************************* ARBID 545 MIL, MPG *************
void send_fuel(int CS_pin)//ARBID 545
//From ECU, MPG, MIL, overheat light, Cruise
// ErrorState variable controls:
//Check engine(binary 10), Cruise (1000), EML (10000)
//Temp light Variable controls temp light.  Hex 08 is on zero off
{

  //**************** set Error Lights & cruise light ******
  if(tempValue2>229){ // lights light if value is 229 (hot)
    tempLight = 8;  // hex 08 = Overheat light on
    //   Serial.println("templight on");
  }
  else {
    tempLight = 0; // hex 00 = overheat light off
    //  Serial.println("templight oFF");
  }

  //  //Check to see if Pin "Down" = 0 (set chec engine) or Temp Hot
  if(digitalRead(MIL_pin) == 0 )  
  {
    bitSet(ErrorState,4);//Set EML. Binary 00010000 
    bitSet(ErrorState,1); //Set check engine. Binary 0010 (hex 02)
  }
  else   
  {
    bitClear(ErrorState,1); // clear check engine
    bitClear(ErrorState,4);//clear EML. 
  }
  if(digitalRead(BMS_L_pin) == 0 )  bitSet(ErrorState2,0); //Set batery light. Binary 001
  else   bitClear(ErrorState2,0); // No error light (zero)

  if(digitalRead(BMS_C_pin) == 0 ) 
  { 
    bitSet(ErrorState,4);//Set EML. Binary 00010000 
    bitSet(ErrorState,6);//Set CHECK gas cap. Binary 001000000 
    bitSet(ErrorState2,0);//Set batery light. Binary 00000001
  }
  else   
  { 
    bitClear(ErrorState,4);//clear EML. Binary 00010000 
    bitClear(ErrorState,6);//clear CHECK gas cap. Binary 001000000 
    bitClear(ErrorState2,0);//clear batery light. Binary 00000001
  }
  //**************** set fuel consumption ******
  // int z = 0x60; // + y;  higher value lower MPG
  // (for micro seconds)/ time for 1 pulse (1.0123 is a correction factor for sensing error)
  MPG = (timeINJ / 1.0123) / MPG_correct;  //from inj sense routine
  if (LSBdata + MPG > 0xFF){ //if adding will go past max value
    MSBdata = MSBdata + 0x01;  //increment MSB
  }
  LSBdata=LSBdata + MPG; //increment values based on inj input, higher value lower MPG

    //Can bus data packet values to be sent
  id = 0x545;  //From ECU, MPG, MIL, overheat light, Cruise
  data[0]= ErrorState;  //set error byte to error State variable
  data[1]= LSBdata;  //LSB Fuel consumption
  data[2]= MSBdata;  //MSB Fuel Consumption
  data[3]= tempLight;  //Overheat light
  data[4]= 0x7E;
  data[5]= ErrorState2;
  data[6]= 0x00;
  data[7]= 0x18;

  CAN_send(CS_pin);

} //end send_Fuel if

//********************* sense RPM from pin 2 ***************
//Called from interupt command
void RPM_sense() //Capture RPM time from pin 2
{ 
  // error checking begin
  timeRISING = micros(); //Captures current clock time for RPM 
  if ((timeRISING - timePULSE) > (8571 / p_REV)) // what about (timeRPM/ Pulse_Count) instead Is pulse valid: if pulse width big enough to be less than 7000 RPM (off the Tach)
  {   
    timePULSE = timeRISING; //Captures current clock time for injector pulse front edge

    if(Pulse_Count == p_REV)//if we are at the max pulses for rev (completed one rev)
    {
      Pulse_Count = 1;
      timeRPM = timePULSE - timeRPM2;  //Captures time it took to do one rev
      timeRPM2 = timePULSE;  //Save current clock time for next rev comparison 
#if defined(RPM_DEBUG)  //debug
      RPM = 60000000/timeRPM;  // add in p_REV
      Serial.print ("timeRPM_sense = ");
      Serial.print(timeRPM);
      Serial.print (" RPM = ");
      Serial.println(RPM);
#endif
    }
    else 
    {
      Pulse_Count ++; 
    } 
#if defined(RPM_DEBUG3)  //debug
    Serial.print ("timePULSE = ");
    Serial.print(timePULSE);
    Serial.print (" Pulse_Count = ");
    Serial.println(Pulse_Count);
#endif

  }//end if not error
}//end void
//********************* sense INJ from pin 3 ***************
//Called from interupt command
void INJ_sense() //Capture INJ time from pin 3
{  
  timeINJp2 = micros();  //Save current clock time for next time
  timeINJ = timeINJp2 - timeRISING;  //Captures pulse width time
  //how about averaging several or drop values over x past previous etc? 
}

//********************* look for messages in can stream ***************
void CAN_ProcessMessage()
{
  time_CAN_in= millis(); //set time for last time message recieved
  //  Serial.print("recd 0x615 ");  //debug
  //  Serial.print(data[0],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[1],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[2],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[3],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[4],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[5],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.print(data[6],HEX);  //debug
  //  Serial.print(" ");  //debug
  //  Serial.println(data[7],HEX);  //debug

  //----------------- look for Ignition on -----------------
  /*  if(bitRead(data[4],0) == 1) //look for IGN on bit
   {//IGN is on
   //do something
   IGN_status = 1; //set flag for IGN status
   //  Serial.println("IGN on");
   }
   else  //AC message from E46 is IGN off
   {
   IGN_status = 0; //set flag for IGN status
   //Stop ELM and stop sending can (Put elm and MCP's to sleep?)
   // Serial.println("IGN off");
   }
   
   */
  /*
  //----------------- look for AC on -----------------  Handled by Ibus sensing
   if(bitRead(data[0],7) == 1) //look for ac on bit
   {//AC message from E46 is AC on
   //set pin x low (AC on)
   digitalWrite(AC_pin,HIGH); //set AC to on;  this to be changed to send can message to LS3
   //    Serial.println("ac just turned on");
   }
   else  //AC message from E46 is AC off
   {
   //Set pin x high (AC off)
   digitalWrite(AC_pin,LOW); //set AC to OFF;  this to be changed to send can message to LS3
   //    Serial.println("ac just turned off");
   }
   */
} //end process message



word readVoltage(boolean channel,int CS_pin)
{
  byte cmd;
  byte MSB=0;
  byte LSB=0;
  byte c=0;
  byte s=0;
  word message;
  if(channel == 0)cmd =0x70;
  else cmd = 0x60;
  //cmd = cmd +(channel << 4);
  digitalWrite(CS_pin,LOW);
  MSB = SPI.transfer(cmd);
  LSB = SPI.transfer(0);
  digitalWrite(CS_pin,HIGH);
  //MSB=MSB&B111;
  //   Serial.print("msb = ");
  //   Serial.print(MSB,BIN);
  //   Serial.print(" LSB = ");
  //   Serial.println(LSB,BIN);
  //  Serial.print(" c = ");
  // Serial.println(c,BIN);
  //  Serial.print(" s = ");
  // Serial.println(s,BIN);
  // message = LSB + (MSB << 8);
  message = word(MSB,LSB);
  bitClear(message,10);
  bitClear(message,11);
  bitClear(message,12);
  bitClear(message,13);
  bitClear(message,14);
  bitClear(message,15);
  return message;
}












