//*************************** PID_REC ****************************
// Extra commands to take ELM data and put into PID array
int PID_rec()
{
  int bn = 0; //bit counter
  char c;
  //  char buffer[String_size-2];
  int x = 398; //initialize x to no bytes


  while (ELM.available()>0){//If ELM has bytes to read
    c = ELM.read();
#if defined(_DEBUG)
    //Serial.print("C");//debug
    //    Serial.print(c);  //debug
    //Serial.print(" ");  //debug
#endif

    switch (c) {

    case ' ':  //if space discard  
      x = 303; //flag leaving on space or carriage return
      break;

    case '\r':  //if space discard
      x = 303; //flag leaving on space or carriage return
      break;

    case '>':  // if command prompt clear buffer and end both loops
      PID_SIZE = iE; //set string size variable to current length of message
      iE=0;
      PID_flag = 0; //flag set to OK to send another PID command
#if defined(_DEBUG)
      Serial.println(" ");
      Serial.println(" prompt recieved with message ");  //debug
#endif
      Decode(); //decode pid response
      return 300; // return good message


    default:
      //if not command prompt, space or return
      buffer[bn] = c;  //store bite in temporary array
      //    Serial.print(buffer[n]);
      //    Serial.print("n= ");
      //    Serial.print(bn);
      bn++; // increment counter for reading in
      if(bn > 1){//if we have a full byte, 2 characters
//        Serial.print("b1=");
//        Serial.print(buffer[0]);
//        Serial.print(" b2=");
//        Serial.print(buffer[1]);
//        Serial.print(" iE= ");
//        Serial.print(iE);
//        Serial.print(" ");

        dataPID[iE] = toHex(buffer[0],buffer[1]); //convert 2 characters read in to hex
        bn = 0; // reset bit counter 
        iE++;  //index for next PID byte
      }

      if(iE >10)
      {
        Serial.println("something has gone wrong with ELM");
        iE = 0;
        PID_flag = 0; //flag to ready to send PID command
        ELM.flush();
        return 401; //error
      } 
    } // end switch  
    x = 399; //byte recieved but not finished
  }//end ELM available

  return x;  //return 399 if partial message 398 if no bytes

}//end void get pid

//  for(i=0 ;i< PID_response_SIZE ;i++){  //clear PID's
//    dataPID[i]=0; //clear PID's
//  } //end for //clear PID's

//  for(i=0 ;i< OBD_RECV_BUF_SIZE ;i++){  //clear buffer's
//    buffer[i]=0; //clear PID's
//  } //end for //clear PID's

//******************** decode **************************
//Decode for temp and RPM are ready to use.  Other PID's need checked.
void Decode() //Decode the data recieved in PID_rec  Data is stored in dataPID[]
{
  float engine_data;
  String message;

#if defined(_DEBUG)
  for(i=0; i< PID_SIZE; i++){ //debug
    Serial.print(dataPID[i],HEX); //debug
    Serial.print(" "); //debug
  }//debug
  Serial.println(" = finished dataPID[i]"); //debug			
#endif

  // ************The switch ******
  switch(dataPID[1])
  {                              /* Details from http://en.wikipedia.org/wiki/OBD-II_PIDs */
  case ENGINE_RPM:  			//   ((A*256)+B)/4    [RPM]
    engine_data =  ((dataPID[2]*256) + dataPID[3])/4;
    //=DEC2HEX(HEX2DEC(G3)/4*6.42/256)
    //    data[3] = engine_data / 40 ;  //RPM BMW MSB RPM / 40
    rMSB = engine_data / 40;  //RPM BMW MSB RPM / 40 then to MSB hex (256)
    //    data[2] = engine_data - (data[2] / 6.4 * 256);  //RPM LSB
    rLSB = (engine_data - (rMSB * 40)) * 6.4;  //RPM LSB subtract MSB portion then comvert to BMW value 
    message = "ENGINE_RPM rpm = "; //debug
    break;

  case ENGINE_COOLANT_TEMP: 	// 	A-40			  [degree C]
    engine_data =  dataPID[2] - 40; //Calculate output value
    //   data[1] = (dataPID[3] + 8.373) / .75;  //convert to BMW scale
    tempValue = (dataPID[2] + 8.373) / .75;  //convert to BMW scale
    //    Serial.print("BMW temp value = hex ");/debug
    //   Serial.println(tempValue,HEX);//debug
    message ="engine coolant temp (degC) = ";//debug
    break;

  case VEHICLE_SPEED: 		// A				  [km]
    // currently not used
    engine_data =  dataPID[2];
    message ="VEHICLE_SPEED km ";//debug
    break;

  case MAF_SENSOR:   	// ((256*A)+B) / 100  [g/s]
    engine_data =  ((dataPID[2]*256) + dataPID[3])/100;
    message ="MAF_SENSOR g/s ";//debug
    break;

  case MIL_CODE:    	// A * 0.005   (B-128) * 100/128 (if B==0xFF, sensor is not used in trim calc)
    engine_data = bitRead(dataPID[2],7);

    if(engine_data != 0 || tempValue>229) { 
      ErrorState = 0x02; //Set check engine. Binary 0010 (hex 02)
    }
    else {
      ErrorState = 0x00; // No error light (zero)
    }  
    message ="MIL = ";//debug
    
    break;

  case THROTTLE:				// Throttle Position
    engine_data = (dataPID[2]*100)/255;
    message ="THROTTLE %% ";//debug
    break;

  } //End Switch case

#if defined(_DEBUG)  
  //  Serial.print("message, engine data");
  //  Serial.print(message);
  //  Serial.println(engine_data); //print output
#endif

  return;
  //  Serial.println(" end of subroutine ");

} //end decode void





// ********* converts 2 charachter ascii to equivalent hex *********
uint8_t toHex(char hi, char lo)
{
  uint8_t b;
  hi = toupper(hi);
  if( isxdigit(hi) ) {
    if( hi > '9' ) hi -= 7;      // software offset for A-F
    hi -= 0x30;                  // subtract ASCII offset
    b = hi<<4;
    lo = toupper(lo);
    if( isxdigit(lo) ) {
      if( lo > '9' ) lo -= 7;  // software offset for A-F
      lo -= 0x30;              // subtract ASCII offset
      b = b + lo;
      return b;
    } // else error
  }  // else error
  return 0;
}

//************* write data to ELM ******************
void ELM_send(const char* s)
{
  ELM.write(s);
  //  Serial.print("writing this (s): "); //Debug
  //  Serial.println(s); //Debug
}

//************* write data to ELM ******************
void ELM_send(const char c)
{
  ELM.write(c);  // send command over serial bus to ELM
  //  Serial.print("writing this (c): "); //Debug
  //  Serial.println(c); //Debug
}














































