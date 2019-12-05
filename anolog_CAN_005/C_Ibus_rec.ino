//*****************************get Ibus data **********************
/*possible returns 
 399 - Bytes but not complete
 398 - no bytes at all
 300 - Message complete and good
 301 - Error in Check sum
 1,2,3,4 etc are button values
 
 Kdata[0] = from module
 Kdata[1] = Length of data bits + length and To
 Kdata[2] = to module
 Kdata[1] + 1 =  Length of data bits + 1 = last byte (checksum)
 */

void get_Kbus_data()
{
  byte CRC;



  // check if the Time between last message and now is > x then kb=0 (start new message) 
  //  if(kb > 0 && millis()-time_Kbus > 5 && bus_started == 0) //else check to see if last byte recieved was too long ago (only if buss not yet initialized) 
  //  {
  //    kb=0;
  //    Serial.println("new message based on time");
  //  }
  int r = Kbus.read(); //Move data from buffer to variable
  if(r == -1)return;

  Kdata[kb] = (byte)r;
  time_Kbus = millis(); //reset timer 
  Ibus_x=399; //set flag byte recieved (but not finished)

#if defined(Kbus_SERIAL_DEBUG)  //debug
  //  if(Kdata[0] == 0x68 || Kdata[0] == 0x50 || Kdata[0] == 0x80){ //print only radio and wheel messages
  Serial.print(Kdata[kb],HEX);  //debug
  Serial.print(" ");  //debug
  //  } // end if print only radio and wheel messages
#endif  //debug

  if(kb == 1 && Kdata[1] > 0x15) //dump data if bus not initialized and length byte looks too long
  {
    Serial.println(" ibus junk length during initializing so stopping");
    Kbus.flush();
    kb=0;
    return; //leave void and wait for next byte
  }
  if(kb == 1 && Kdata[1] == 0) //Dump if length byte is zero
  {
    Serial.println(" ibus zero length initializing so stopping");
    Kbus.flush();
    kb=0;
    return;//leave void and wait for next byte
  }

  kb++; //index counter to be ready for next read

  if (kb>(Kdata[1]+1))//have we received all bytes
  { //is next byte to be recieved over max length
    kb=0; //Reset counter for next message
    CRC = iso_checksum(Kdata,Kdata[1]+1);  //calculate the CRC value

    if(CRC == Kdata[Kdata[1]+1]) //Check if CRC in data matches calculated
    {// if CRC matches it is a good message

#if defined(Kbus_SERIAL_DEBUG)  //debug
      //     if(Kdata[0] == 0x68 || Kdata[0] == 0x50 || Kdata[0] == 0x80){ //print only radio, wheel messages, IKE
      Serial.println(" ibus good");//debug 
      //      } // end if print only radio and wheel messages
#endif  //debug

      if(bus_started == 0) //if bus wasn't yet initialized
      {
        Serial.println(" ibus now initialized ");//debug
      } //end if first time through bus initialization

        bus_started = 1; //bus initialization complete flag
      Ibus_x=300; //300 - Message complete and go

      //Check if message is AC on
      if(Kdata[0] == 0x5B && Kdata[1] == 0x05 && Kdata[2] == 0x80 && Kdata[3] == 0x83 && Kdata[4] != 0x00)
      {
        Serial.println("ibus message AC on");
        digitalWrite(AC_pin,HIGH); //set AC to on;
      }
      //check if message is AC off
      if(Kdata[0] == 0x5B && Kdata[1] == 0x05 && Kdata[2] == 0x80 && Kdata[3] == 0x83 && Kdata[4] == 0x00)
      {
        Serial.println("ibus message AC off");
        digitalWrite(AC_pin,LOW); //set AC to OFF; 
      }

      if(Kdata[0] == 0x00 && Kdata[1] == 0x04 && Kdata[2] == 0xBF && Kdata[3] == 0x72) //if remote button pressed (locking car)
      {
        Serial.println("car on/off");
      }//if car on/off

      if(Kdata[0] == 0x80 && Kdata[1] == 0x04 && Kdata[2] == 0xBF && Kdata[3] == 0x11 && Kdata[4] == 0x00)
        //if IKE turns off 
      {
        Serial.println("ike off");
      }//if IKE off
      time_Kbus = millis(); //capture time when message recieved (duplicate to ensure program process time doesn't cause the kick out)

      return; //return good message
    } //end CRC if 

    //if check sum failed (does not need to be an else as cannot pass here due to return)
    Kbus.flush(); //wipe out buffer when errors
    kb=0;
    Serial.print(" ibus CRC fail CRC= "); //debug
    Serial.println(CRC,HEX); //debug
    bus_started = 0; //ibus not initialized
    Ibus_x=301; //301 - Error in Check sum
    return; // return fail
    //cannot pass here must exit 300 or 301   
  } // End if have all bytes

  //only passes here if NOT all bytes rec'd  

  return; //return 399 if partial message 398 if no bytes
}// end get K bus


/*
//**************************************Wheel message***************************
 void wheel_message()
 {
 if(Kdata[2] == 0x68 && Kdata[3] == 0x32)//if it volume button
 {  
 return;
 }
 
 //
 if(Kdata[2] == 0xC8)//if it is a telephone button
 {
 if(Kdata[3] == 0x01) ButtonState = RT_press; //R/T pressed (execute)
 if(Kdata[4] == 0x80) ButtonState = dial; //Dial Pressed (menu)
 if(Kdata[4] == 0x90) ButtonState = FFWD; //Dial Held (execute?)
 if(Kdata[4] == 0xA0) rad_command = play; //pressed play **command //Dial Released
 }
 //act on messages
 }//end wheel message void
 */




