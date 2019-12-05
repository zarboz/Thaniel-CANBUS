#include <SPI.h>

#define CAN_CS 10 //Chip select for can chips
#define ADC_CS 9 //Chip select for ADC chip
#define off_pin 8  //Pin used to put linbus transciever to sleep
#define AC_pin 7  //pin used to trigger AC on off circuit (output)
#define MIL_pin 6 //pin to ground for MIL to come on (input)
#define BMS_C_pin 5  //pin used to  (input)
#define BMS_L_pin 4 //pin to (input)
// pin 4 and 5 sitll availabe

#define p_REV 2  //number of pulses from engine for 1 rev
//#include <SoftwareSerial.h>  //Library file for software serial connenction (Used to comunicate with ELM327)
//#include <avr/pgmspace.h>


#define DEFAULT_CAN_ID	0x0555
// added for filter and can changes
#define MASK_0 0x20
#define MASK_1 0x24
#define FILTER_0 0x00
#define FILTER_1 0x04
#define FILTER_2 0x08
#define FILTER_3 0x10
#define FILTER_4 0x14
#define FILTER_5 0x18

#define WRITE 0x02 //read and write comands for SPI

/* Mcp2515 Operation Modes */
enum CAN_MODE {
  CAN_MODE_NORMAL,      /* Transmit and receive as normal */
  CAN_MODE_SLEEP,	/* Low power mode */
  CAN_MODE_LOOPBACK,	/* Test mode; anything "sent" appears in the receive buffer without external signaling */  							 
  CAN_MODE_LISTEN_ONLY, /* Receive only; do not transmit */
  CAN_MODE_CONFIG,	/* Default; Allows writing to config registers */
  CAN_MODE_COUNT
};


//CanMessage message;
uint8_t  message;

/** A flag indicating whether this is an extended CAN message */
uint8_t extended;

/** The ID of the CAN message.  The ID is 29 bytes long if the
 * extended flag is set, or 11 bytes long if not set. */
uint32_t id;

/** The number of bytes in the data field (0-8) */
uint8_t len;

/** Array containing the bytes of the CAN message.  This array
 * may be accessed directly to set or read the CAN message.
 * This field can also be set by the setTypeData functions and
 * read by the getTypeData functions. */
uint8_t data[8];
uint8_t LSBdata;
uint8_t MSBdata;
uint8_t ErrorState;
uint8_t ErrorState2;

//***************************************************our/can library variables***************************
//variables for checking MPG time
unsigned long timeRISING; //used to catch clock time for injector pulse
unsigned long timeINJp2; //used to save time of a injector pulse
unsigned long timeINJ; //used for time difference calculation
unsigned long MPG;    //Calculated MPG step value based on input from INJ_Sense in
long MPG_correct = 100;  //correction factor for MPG guage

//variables to capture RPM pulse.  RPM connect to pin D2 
unsigned long timePULSE;  //used to catch clock time for RPM pulse
unsigned long timeRPM2; //used to save time of a RPM pulse
long timeRPM = 600000000;  //used for time difference calculation
long RPM;    //Calculated RPM based on input from RPMin

int Pulse_Count; // number of pulses recieved
int rLSB;   //RPM Least significant pit from RPM calc
int rMSB;  //RPM most significant pit from RPM calc

int del = 10; // delay time between steps 10-15 works well.
// if using "delay" command it is milliseconds

long time_debug; // for displaying values for debugging
long time_temp; // Timer between temp sensing
long time_TP; // Timer between temp sensing
//Variables for temperature light
int tempValue;
int tempValue2;
int tempLight;

int TPValue;
int TPValue2;
//CanMessage message;
byte i;
int x;

int CAN_flag; //flag to indicate which can message to send next
int IGN_status; // ignition status
long time_CAN; // Timer between CAN bus outputs
long time_CAN_in; // Timer between CAN bus inputs

