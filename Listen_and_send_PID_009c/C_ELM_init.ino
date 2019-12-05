//**************** ELM INIT **********************
void Init()
{
  boolean passive;
  unsigned char n;
  char prompted;


  for (i = 0; i < INIT_CMD_COUNT; i++) {  //Loop enough times to send all init comands
    if (!passive) {
     char cmd[MAX_CMD_LEN];
      strcpy_P(cmd, s_initcmd[i]); // copy first string from S_initcmd to cmd
       ELM_send(cmd);  // Send cmd string to ELM
 //    Serial.println(cmd); //debug
    }
   ELM_rec(OBD_RECV_BUF_SIZE); //call ELM recieve
  }  // end INIT command send loop
  return;
}  // end Init subroutine


//*************************** ELM_REC ****************************
// currently using Serial read to get data.  To read elm just swtich.
//buffer array and string size (max number of characters to catch)
void ELM_rec(int String_size)
{
  //  int String_size = 2; //# of characters per array value to be read
  // int OBD_RECV_BUF_SIZE = 6;  //max data bit pairs to be read
  long OBD_TIMEOUT_INIT = 100000;
  int n;
  char c;
  // char buffer[String_size-2];
  unsigned long currentMillis;
  unsigned long elapsed;
 
  Prompted =0; //set propmpt ">" recieved flag to zero
  n = 0;
  currentMillis = millis();
  while (n < String_size) { //keep looping until recieved # of characters (String length)
      if (ELM.available()) { //do nothing if no data avail
      c = ELM.read();
      Serial.write(c);
      // ---------------- switch ------------------------       
      switch (c) {

      case '>':  // if command prompt clear buffer and end both loops
        buffer[n] = 0;
        n = String_size;  //set n to max size to end loop
        Prompted = 1;
        break;

      case ' ':  //if space discard     
      case '\r':  //if space discard
        break;

      default: //if not command prompt
        buffer[n] = c;  //move c to array.  needed for conversion
        n++; // increment counter for reading in
      }  //end switch
    }   //end ELM available  
    else {  //check elapsed time in case elm read errors
      elapsed = millis() - currentMillis;
      if (elapsed > OBD_TIMEOUT_INIT) {
        // init timeout
        //WriteData("\r");
        buffer[n] = 0;
        n = String_size;
        i = OBD_RECV_BUF_SIZE;
        Serial.print(elapsed);  //debug
        elapsed = 0;
        Serial.println(" timeout ");  //debug    
      } //end if time out
    } //end if serial avilable else (time out check)     
  } // end read increment n loop
}//end void

