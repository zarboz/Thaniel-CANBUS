//OBDII ****************************************
#define ENGINE_COOLANT_TEMP 0x05
#define ENGINE_RPM          0x0C
#define VEHICLE_SPEED       0x0D
#define MAF_SENSOR          0x10
#define O2_VOLTAGE          0x14
#define THROTTLE	    0x11
#define MIL_CODE            0x01
#define Engine_fuel_rate    0x5E // 	((A*256)+B)*0.05 in L/h.  Does not work on E46 or LS1

#define PID_REQUEST         0x7DF
#define PID_REPLY	    0x7E8

// initialization commands
#define INIT_CMD_COUNT 7  //number of init commands to send
#define MAX_CMD_LEN 7  //Longest initialization command strings.
#define OBD_RECV_BUF_SIZE 48 // maximum characters to be read into buffer
//Stings that will be sent during initialization
const char PROGMEM s_initcmd[INIT_CMD_COUNT][MAX_CMD_LEN] = {
  "ATZ\r","ATE0\r","ATL1\r","ATI\r","ATSP6\r","0100\r","ATDP\r"};
  // ATSPx\r where x is the protocol. Change that command for other protocols.
  // "ATSP2\r" is to initialze VPW OBDII comunicaiton (LS1).  
  // "ATSP6\r" is to initialize can bus OBDII comunicaiton (Newer LSx engines). 
  // "ATSP3\r" is to initialize ISO OBDII comunicaiton (E46 OBDII).   
/*
ATSPx\r where x is the protocol
1 SAE J1850 PWM (41.6 kbaud)
2 SAE J1850 VPW (10.4)
3 ISO 9141-2 (5 baud init)
4 iso 14230-4 KWP (5 baud init)
5 iso 14230-5 KWP (fast init)
6 ISO 15765-4 CAN (11 bit ID, 500 kbaud) *most common can bus and used in E46
7 ISO 15765-4 CAN (29 bit ID, 500 kbaud) *used on newer vehicles 
8 ISO 15765-4 CAN (11 bit ID, 250 kbaud)
9 ISO 15765-4 CAN (29 bit ID, 250 kbaud)

*/
char buffer[OBD_RECV_BUF_SIZE];
boolean Prompted;

/** Array containing the bytes of the CAN message.  This array
 * may be accessed directly to set or read the CAN message.
 * This field can also be set by the setTypeData functions and
 * read by the getTypeData functions. */
uint8_t dataPID[8];
int PID_flag; //flag to show if PID complete and rec'd
int PID_flag_o; //flag for which PID should be sent next
int CAN_flag; //flag to indicate which can message to send next
int PID_SIZE; //size of PID message recieved
int IGN_status; // ignition status
long time_PID; //timer between RPM sends
long time_PID_o; //Timere between non PID sends
long time_CAN; // Timer between CAN bus outputs
long time_CAN_in; // Timer between CAN bus inputs
long time_test; //debug
int test_up; //debug
int test_flag; //debug
int iE; //counter for dataPID[] in the PID_rec void so can keep it's place
//*******************************************************

