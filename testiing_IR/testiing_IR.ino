//#include <IRremote.h>
//#include <IRremoteInt.h>
//#include <ir_Lego_PF_BitStreamEncoder.h>


/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */
/*
Address Alpine RUE-4202: 0x61 0x4E 
Teams:
0x68 — MUTE
0x90 — POWER
0x70 — UP
0xS8 — LEFT
0x50 — SOURCE
0x48 — RIGHT
0xF0 — DOWN
0xA8 — VOL-
0x28 — VOL +

Found command:
0x00 turning on the radio, pressing range selection
0x40 select the disk, pressing: Play-Pause
0xA0 switch-off DEFEAT
0x61 radio range selection
0xD0 Disc EJECT
*/
// for mega uses pin 9
// for uno uses pin 3
//IRsend irsend;

void setup()
{
}

void loop() {
	sendNEC(makeLong(0x48),32);
	delay(5000); //5 second delay between each signal burst
}
unsigned long makeLong(byte command)
{
unsigned long r = (0x61<<32)|(0x4E<<16)|(command<<8)|~command;
return r;
}


