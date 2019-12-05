
//#define IPOD Serial1 //rename serial to IPOD for Uno it'll be Serial
#define Kbus Serial2 //rename serial to IPOD for Uno it'll be Serial
#define timeoutlen 2
#define get_Kbus_data serialEvent2 //calls get_Kbus_data when there is activity on Serial 2

#define Kbus_rx 17  //rx pin for Kbus, Mega is 17 (tx =16),Uno and Leonardo rx 0, tx 1
#define Kbus_tx 16

#define CD_No   0x7
#define Track 0x1
//ours
byte Kdata[140]; //ibus data
//byte* Kbuf[138]={
//  &Kdata[2],&Kdata[3],&Kdata[4],&Kdata[5],&Kdata[6],&Kdata[7],&Kdata[8],&Kdata[9],&Kdata[10],&Kdata[11],&Kdata[12],&Kdata[13],&Kdata[14],&Kdata[15],
//  &Kdata[16],&Kdata[17],&Kdata[18],&Kdata[19],&Kdata[20]};//pointer for reading Kbus with read bytes w/o overwriting first 2 bits refences kdata bytes 2-20;

char text[11] = {
  0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19}; //text to send out on Kbus
byte BusMessage[18]={ //first 6 bytes are fixed (18 bytes max)
  0xC8,0x10,0x80,0x23,0x42,0x30,0x44,0x61,0x75,0x67,0x68,0x74,0x72,0x79,0x19,0x19,0x19,0x30};//message default
byte BusStatusMessage[18]={0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, CD_No, Track, 0xFF};//CD status Stop, pause default
byte BusStatus2Message[18]={0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, CD_No, Track, 0xFF};//CD status Stop, pause default
int Ibus_STATUS_ready;
int Ibus_STATUS2_ready;

long debugtime;
int ButtonState;
int Ibus_x; //store reciving status
long time_Kbus; //time waiting to see if messages stopped
long time2;

boolean Ibus_MSG_ready; //is there text ready to send over ibus
int bus_started =0;
int kb; //index counter for Kbus

boolean rar;
int r2; //auto set to length of text currently being output on Kbus
int space;
void KbusSendText2(String message,int first = 0); //message to send, where break should be, first or second sending
//button setup definitions
/*#define one    1
#define two    2
#define three  3
#define four   4
#define five   5
#define six    6
#define cd_stop 7
#define right  8
#define left   9
#define play   10
#define select 11
#define cd_status 12
#define random_on 13
#define random_off 14
#define scan_on 15
#define scan_off 16
#define FRWD    17
#define FFWD    18
#define cd_pause 19
#define cd_stop_pause 20
#define RT_press 21
#define dial 22
#define menu_display 99


//defines for CD status bit 4
#define B4_stop 0x00
#define B4_pause 0x01
#define B4_play 0x02
#define B4_ffwd 0x03
#define B4_frwd 0x04

//defines for CD statuts bit 5
#define B5_stop 0x00
#define B5_pause 0x02
#define B5_play 0x09
//#define Rad_seek 0x10
int B5_status; //play status (stop, pause or play)
int B5_seek; //0x10 makes B5 seek
#define Rad_random 0x20
int B5_random; //0x20 makes B5 Random
*/






