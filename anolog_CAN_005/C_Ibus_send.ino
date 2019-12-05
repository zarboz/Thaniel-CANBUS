//********************************** new Kbus send text  *********
//>>>>>>>>> this is the only send we need?
//prepares message into BusMessage variable from Ipod data (ARTIST TITLE ETC) or Text string in quotes
// chops message into 2 chunks if over 11 bytes.
void set_BusMessage_ipod(char message[], int first) //send message over Kbus byte message[], first is if its the first of th 11 characters
{// use 0x19 for empty spaces,  BusMessage will be what we send
 
  //  byte BusMessage[18]={ //first 6 bytes are fixed (18 bytes max)
  //    0xC8,0x10,0x80,0x23,0x42,0x30                          };//message to send text header
  String rar= message;
  r2=rar.length();  //get length of string
  int r=r2;
  int i;
  int start_i;
  int end_i ;
  if (first == 0) 
  { //if first 11 start at 0
    start_i = 0;
    end_i = 11;
    space=rar.lastIndexOf(' ',11);
    if (r2 > 11) r = space;
  } 
  else { //if second 11 start at 11 or where the space was
    start_i = space+1;
    end_i = start_i + 11; 
  }

  for (i = start_i; i < end_i; i++) //11 characters max
  {
    //// Serial.print(i);//debug
    if(i<r) BusMessage[i+6-start_i] = message[i]; //forming bus Message for characters
    else    BusMessage[i+6-start_i] = 0x19; //fill empty space with 0x19
  }
  //  // Serial.println(' ');//debug
  BusMessage[17] = iso_checksum(BusMessage,17);  //calculate the CRC value and set to last bit

  Ibus_MSG_ready = 1;  //flag text is ready to send.

}// send of Kbus send text


//*************************** set_BusMessage_ipod version without 2 chunks **************
//this one sends first 11 characters of Message.  not tricks
void set_BusMessage_ipod(char message[]) //send message over ibus byte message[], first is if its the first of th 11 characters
{// use 0x19 for empty spaces,  BusMessage will be what we send

  //  byte BusMessage[18]={ //first 6 bytes are fixed (18 bytes max)
  //    0xC8,0x10,0x80,0x23,0x42,0x30                          };//message to send text header
  String rar= message;
  r2=rar.length();  //get length of string
  int i;
  int start_i = 0;
  int end_i = 11;

  for (i = start_i; i < end_i; i++) //11 characters max
  {
    //// Serial.print(i);//debug
    if(i<r2) BusMessage[i+6-start_i] = message[i]; //forming bus Message for characters
    else    BusMessage[i+6-start_i] = 0x19; //fill empty space with 0x19
  }
  //  // Serial.println(' ');//debug
  BusMessage[17] = iso_checksum(BusMessage,17);  //calculate the CRC value and set to last bit

  Ibus_MSG_ready = 1;  //flag text is ready to send.

}// send of Kbus send text


//********************************** Ibus_set_Status BusMessage *********
//used to set and Send cd status respnses into BusMessage
void Ibus_set_status_BusMessage(byte *Message) //send message over Kbus byte message[], first is if its the first of th 11 characters
{// used to send CD status responses
  int end_i = Message[1]+2 ;
  Message[end_i-1] = iso_checksum(Message,end_i-1);  //calculate the CRC value and set to last bit

  for (int i = 0; i < end_i; i++) //11 characters max
  {
    //   Serial.print(Message[i],HEX);
    BusStatusMessage[i] = Message[i];
  }

//  time_CD = millis(); //reset time_CD. status timer
//  Serial.print("time CD when setting CD message= ");//debug
//  Serial.print(time_CD);//debug

  Ibus_STATUS_ready = 1; //set flag to indicate message needs to be sent
}// send of Kbus send text


//********************************** Ibus_set_Satus2Message *********
void Ibus_set_status2_BusMessage(byte *Message)//set status into the "BusMessage" variable so two can be sent as one.
{
  int end_i = Message[1]+2 ;
  Message[end_i-1] = iso_checksum(Message,end_i-1);  //calculate the CRC value and set to last bit

  for (int i = 0; i < end_i; i++) //11 characters max
  {
    //   Serial.print(Message[i],HEX);
    BusStatus2Message[i] = Message[i];
  }
  Ibus_STATUS2_ready = 1;  //flag text is ready to send.
}


//********************************** Ibus_send_status BusMessage *********
//used to set and Send cd status respnses into BusMessage
void Ibus_send_status_BusMessage() //send message over Kbus byte message[], first is if its the first of th 11 characters
{// used to send CD status responses
  int end_i = BusStatusMessage[1]+2 ;
  Kbus.write(BusStatusMessage,end_i); //write bytes on the bus
  
  if (Ibus_STATUS2_ready == 1)
  {end_i = BusStatus2Message[1]+2 ;
  Kbus.write(BusStatus2Message,end_i); //write bytes on the bus
  Ibus_STATUS2_ready = 0;
  }//end send buss message 2

  if(Ibus_MSG_ready == 1) ibus_send_BusMessage(); //send 2nd message if one is ready without a break.

  Ibus_STATUS_ready = 0; //set flag to indicate message was sent 
//  Serial.print(" time message sent= ");//debug
//  Serial.println(millis());//debug

}// send of Kbus send text


//************************** ibus_send_BusMessage ****************
void ibus_send_BusMessage() //issue the prepared bus message (BusMessage) out on the ibus
{//call when bus has been quiet and Ibus_MSG_ready flag is 1
  int end_i = BusMessage[1]+2 ;
  Kbus.write(BusMessage,end_i);
//  Serial.println(" ibus message send  ");//debug
  Ibus_MSG_ready = 0;  //turn off flag that mesage has been sent
}


//****************************** Ibus check sum ****************************************
//checksum calculator for IBUS
byte iso_checksum(byte *data, byte len)//len is the number of bytes (not the # of last byte)
{
  byte crc=0;
  for(byte i=0; i<len; i++)
  {    
    crc=crc^data[i];
  }
  return crc;
}





















