#include <SPI.h>
      int slaveSelectPin = 10;
#include <SoftwareSerial.h>  //Library file for software serial connenction (Used to comunicate with ELM327)
#include <avr/pgmspace.h>
#include <avr/power.h> //for power off mode
#include <avr/sleep.h>  //for power off mode

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

//variables for checking MPG time

unsigned long MPG;    //Calculated MPG step value based on input
long MPG_correct = 100;  //correction factor for MPG guage

//variables to capture RPM pulse.   
int rLSB;   //RPM Least significant pit from RPM calc
int rMSB;  //RPM most significant pit from RPM calc

long del;  //Delay time

//Variables for temperature light
int tempValue;
int tempLight;

//CanMessage message;
byte i;
int x;
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
