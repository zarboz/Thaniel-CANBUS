
void set_testvalues()
{



  // RPM (put the hex number together then convert to DEC and divide by 6.4)
  // rLSB;  //RPM LSB0xFF
  // rMSB; //RPM MSB "0x0F" = 600, 0x19 = 1000 add 25 decimal to get next 1000, 7000 = 0xAF
  rLSB = 0x00;

  // *************RPM    

  if(test_up == 0)
    rMSB = rMSB + 1;
  if(test_up == 1)
    rMSB = rMSB - 1;

  if( rMSB < 0x01)
  {
    rMSB = rMSB + 2;  //set initial value for rMSB 
    test_up = 0; //set needle direction to increasing
  }     
  if(rMSB > 0xAF) // if beyond 7000 rpm change direction 
  {
    rMSB = rMSB - 2; 
    test_up = 1; //set needle direction to decreasing
  }
  Serial.print("RPM MSB = ");
  Serial.println(rMSB,HEX);

  if(test_flag > 2) //increment more slowly than RPM
  {
    // ************* temperature   
     //  tempValue; //temp bit tdata
  //  values and correlating needle position:
  //  0x56 = First visible movment of needle starts
  //  0x5D = Begining of Blue section
  //  0x80 = End of Blue Section
  //  0xA9 = Begin Straight up
  //  0xC1 = Midpoint of needle straight up values
  //  0xDB = Needle begins to move from center
  //  0xE6 = Beginning of Red section
  //  0xFE = needle pegged to the right 
/*    if(temp_up == 0)
      tempValue = tempValue + 1;
    if(temp_up == 1)
      tempValue = tempValue - 1;    

    if( tempValue < 0x56)
    {
      tempValue = tempValue + 2;  //set initial value for rMSB 
      temp_up = 0; //set needle direction to increasing
    }     
    if(tempValue > 0xFE) // if beyond 7000 rpm change direction 
    {
      tempValue = tempValue - 2; 
      temp_up = 1; //set needle direction to decreasing
    }*/
   test_flag = 0; 
  }
  
  
//******check engine lights  Check engine(binary 10,2), Cruise (1000,8), EML (10000,16), Check eng and EML (32)

  if(rMSB > 0x0D && rMSB < 0x19) //if RPM between 500 and 1000
{
ErrorState = 0x01; //Set B0 
tempLight = 0x01; //set B3
}
 
  if(rMSB > 0x19 && rMSB < 0x26) //if RPM between 1000 and 1500
{
ErrorState = 0x02; //Set B0 check engine. Binary 0010 (hex 02)
tempLight = 0x02; //set B3
}

  if(rMSB > 0x26 && rMSB < 0x32) //if RPM between 1500 and 2000
{
ErrorState = 0x04; //Set B0)
tempLight = 0x04; //set B3
}

if(rMSB > 0x32 && rMSB < 0x3F) //if RPM between 2000 and 2500
{
ErrorState = 0x08; //Set B0 Cruise
tempLight = 0x08; //set B3 Overheat
}

if(rMSB > 0x3F && rMSB < 0x4B) //if RPM between 2500 and 3000
{
ErrorState = 0x10; //Set B0 EML
tempLight = 0x10; //set B3
}

if(rMSB > 0x4B && rMSB < 0x58) //if RPM between 3000 and 3500
{
ErrorState = 0x20; //Set B0 
tempLight = 0x20; //set B3
}

if(rMSB > 0x58 && rMSB < 0x64) //if RPM between 3500 and 4000
{
ErrorState = 0x40; //Set B0 
tempLight = 0x40; //set B3
}

if(rMSB > 0x64 && rMSB < 0x71) //if RPM between 4000 and 4500
{
ErrorState = 0x80; //Set B0 
tempLight = 0x80; //set B3
}

if(rMSB > 0x71&& rMSB < 0x7D) //if RPM between 4500 and 5000
{
ErrorState = 0x100; //Set B0 
tempLight = 0x100; //set B3
}

//PAST 10 CHECKS
if(rMSB > 0x7D && rMSB < 0x8A) //if RPM between 5000 and 5500
{
ErrorState = 0x03; //Set B0 
tempLight = 0x00; //set B3
}

if(rMSB > 0x8A && rMSB < 0x96) //if RPM between 5500 and 6000
{
ErrorState = 0x06; //Set B0 
tempLight = 0x00; //set B3
}

if(rMSB > 0x96 && rMSB < 0xA3) //if RPM between 6000 and 6500
{
ErrorState = 0xFF; //Set B0 
tempLight = 0x00; //set B3
}

if(rMSB > 0xA3 && rMSB < 0xAF) //if RPM between 6050 and 7000
{
ErrorState = 0x00; //Set B0 
tempLight = 0xFF; //set B3
}



  test_flag++;//increment flag
//  if(test_flag==10) test_flag=0; //if past end of range reset
  time_test = millis(); //set timer 
}//end void


