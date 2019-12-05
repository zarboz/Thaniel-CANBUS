//*********************** Can cpp  ***************************************
//Below is Can bus library and programming.  Not specific to E46
/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino. */


void CAN_CanMessage ()
{
  extended = 0;
  id = DEFAULT_CAN_ID;
  len = 0;
}
// **************************** setByteData *******************
void CAN_setByteData (byte b)
{
  len = 1;

  data[0] = b;
}

//*******************  setIntData  ***************************
void setIntData (int i)
{
  len = 2;

  /* Big-endian network byte ordering */
  data[0] = i >> 8;
  data[1] = i & 0xff;
}

//******************* setLongData  ***************************
void CAN_setLongData (long l)
{
  len = 4;

  /* Big-endian network byte ordering */
  data[0] = (l >> 24) & 0xff;
  data[1] = (l >> 16) & 0xff;
  data[2] = (l >>  8) & 0xff;
  data[3] = (l >>  0) & 0xff;
}

//*********************** setData  *****************************
void CAN_setData (const uint8_t *data, uint8_t len)
{
  byte i;

  //  this->len = len;
  len = len;
  for (i=0; i<len; i++) {
    //    this->data[i] = data[i];

  }
}

//*********************** setData  **************************
void CAN_setData (const char *data, uint8_t len)
{
  CAN_setData ((const uint8_t *)data, len);
}

//********************* send  ******************************
void CAN_send (int CS_pin)
{
  //mcp2515_set_msg (0, id, data, len, extended);
  // slaveselectpin = pin;
  uint8_t tx_buf = 0;
//  Serial.print(id,HEX);
  //Serial.print(" ");

  uint8_t buf[5];

  if (extended) {
    buf[0] = (uint8_t)(id >> 21);
    buf[1] = (uint8_t)(((id >> 13) & 0xE0) |
      (1 << EXIDE) |
      ((id >> 16) & 0x03));
    buf[2] = (uint8_t)(id >> 8);
    buf[3] = (uint8_t)(id);
  } 
  else {
    buf[0] = (uint8_t)(id >> 3);
    buf[1] = (uint8_t)(id << 5);
  }

  if (len > 8)
    len = 8;

  buf[4] = len << DLC0;

  mcp2515_write_regs (REG(TX, tx_buf, SIDH), buf, 5, CS_pin);
  mcp2515_write_regs (REG(TX, tx_buf, D0), data, len, CS_pin);

  // mcp2515_request_tx (0);

  mcp2515_bit_modify (REG(TX, tx_buf, CTRL), 1 << TXREQ, 1 << TXREQ, CS_pin);
  //Serial.println(CS_pin);
}

//****************** getByteFromData **********************

byte CAN_getByteFromData()
{
  return data[0];
}

//**************** getIntFromData ************************
int CAN_getIntFromData ()
{
  int val;

  val |= (uint16_t)data[0] << 8;
  val |= (uint16_t)data[1] << 0;

  return val;
}

//**************** getLongFromData
long CAN_getLongFromData ()
{
  long val;

  val |= (uint32_t)data[0] << 24;
  val |= (uint32_t)data[1] << 16;
  val |= (uint32_t)data[2] <<  8;
  val |= (uint32_t)data[3] <<  0;

  return val;
}

//********************** getData *******************
void CAN_getData (uint8_t *data)
{
  byte i;

  for (i=0; i<len; i++) {
    //    data[i] = this->data[i];
  }
}

//******************** getData ***********************
void CAN_getData (char *data)
{
  CAN_getData ((uint8_t *)data);
}

/*
 * CANClass
 */

//********************** Begin ***********************
void CAN_begin(uint32_t bit_period, int CS_pin) {

  //mcp2515_init (bit_time);  mcp2515_init (uint32_t bit_period)
  uint8_t brp;
  uint8_t bit_width;
  uint8_t prop_seg;
  uint8_t phase_1_seg;
  uint8_t phase_2_seg;

  /* Calculate BRP and bit width */
  brp = calc_brp (bit_period, &bit_width);

  if (bit_width < QUANTUM_WIDTH_MIN)
    bit_width = QUANTUM_WIDTH_MIN;

  /* Calculate segment widths based on bit width.  This algorithm keeps
   * the segments as even as possible, favoring phase2, then phase1
   * to be bigger. */
  phase_2_seg = (bit_width + 1) / 3;
  bit_width -= phase_2_seg;

  phase_1_seg = bit_width >> 1;

  prop_seg = bit_width - phase_1_seg - 1;

  /* Set registers */
  mcp2515_write_reg (CNF1, (brp << BRP) | ((SYNC_JUMP_WIDTH - 1) << SJW), CS_pin);

  mcp2515_write_reg (CNF2, ((prop_seg - 1) << PRSEG) | ((phase_1_seg - 1) << PHSEG1) |
    (0 << SAM) |  /* Sample once */
  (1 << BTLMODE), CS_pin);  /* Phase 2 set by CNF3 */

  mcp2515_write_reg (CNF3, ((phase_2_seg - 1) << PHSEG2) | (0 << WAKFIL), CS_pin);

  mcp2515_write_reg (REG(RX, 0, CTRL), (0x0 << RXM) | (0 << BUKT) , CS_pin);

/*  old style. use new MCP only library.
  //Rx buffer 0    
  CAN_setMaskOrFilter(MASK_0,   0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin); //can this be replaced by mcp2515_set_rx_mask and mcp2515_set_rx_filter
  CAN_setMaskOrFilter(FILTER_0, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin);
  CAN_setMaskOrFilter(FILTER_1, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin);
  //Rx buffer 1
  //disable the second buffer
  CAN_setMaskOrFilter(MASK_1,   0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin); 
  CAN_setMaskOrFilter(FILTER_2, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin);
  CAN_setMaskOrFilter(FILTER_3, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin); 
  CAN_setMaskOrFilter(FILTER_4, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin);
  CAN_setMaskOrFilter(FILTER_5, 0b00000000, 0b00000000, 0b00000000, 0b00000000, CS_pin);
*/

  // using MCP only
  //RX buffer 0
  mcp2515_set_rx_filter(0, 0x00, extended, CS_pin);
  mcp2515_set_rx_filter(1, 0x00, extended, CS_pin);
  //Rx buffer 1
  mcp2515_set_rx_filter(2, 0x00, extended, CS_pin);
  mcp2515_set_rx_filter(3, 0x00, extended, CS_pin);
  mcp2515_set_rx_filter(4, 0x00, extended, CS_pin);
  mcp2515_set_rx_filter(5, 0x00, extended, CS_pin);

  //mcp2515_set_rx_filter (uint8_t filter_num, uint32_t filter, uint8_t extended, int CS_pin)
  mcp2515_set_rx_mask (0, 0x00, extended, CS_pin); //RX buffer 0
  mcp2515_set_rx_mask (1, 0x00, extended, CS_pin); //Rx buffer 1

  //mcp2515_set_rx_mask (uint8_t mask_num, uint32_t mask, uint8_t extended,int CS_pin)

}


//******************************** reset filters and Masks  ****************
// added to set filters -- from another program (not from Kevin Smith <faz@fazjaxton.net>)
/*void CAN_resetFiltersAndMasks(int CS_pin) {
 //disable first buffer
 CAN_setMaskOrFilter(MASK_0,   0b00000000, 0b00000000, 0b00000000, 0b00000000);
 CAN_setMaskOrFilter(FILTER_0, 0b00000000, 0b00000000, 0b00000000, 0b00000000);
 CAN_setMaskOrFilter(FILTER_1, 0b00000000, 0b00000000, 0b00000000, 0b00000000);
 
 //disable the second buffer
 CAN_setMaskOrFilter(MASK_1,   0b00000000, 0b00000000, 0b00000000, 0b00000000); 
 CAN_setMaskOrFilter(FILTER_2, 0b00000000, 0b00000000, 0b00000000, 0b00000000);
 CAN_setMaskOrFilter(FILTER_3, 0b00000000, 0b00000000, 0b00000000, 0b00000000); 
 CAN_setMaskOrFilter(FILTER_4, 0b00000000, 0b00000000, 0b00000000, 0b00000000);
 CAN_setMaskOrFilter(FILTER_5, 0b00000000, 0b00000000, 0b00000000, 0b00000000); 
 }*/

//***************************** set mask or FIlter **********************
void CAN_setMaskOrFilter(byte mask, byte b0, byte b1, byte b2, byte b3, int CS_pin) {
  CAN_setMode(CAN_MODE_CONFIG, CS_pin); //  1= Configuration, 2 = NORMAL, 3 = SLEEP, 4 = LISTEN, 5 = LOOPBACK 
  mcp2515_write_reg (mask, b0, CS_pin);
  mcp2515_write_reg (mask+1, b1, CS_pin);
  mcp2515_write_reg (mask+2, b2, CS_pin);
  mcp2515_write_reg (mask+3, b3, CS_pin);
  CAN_setMode(CAN_MODE_LISTEN_ONLY, CS_pin); // (was 1) 1= Configuration, 2 = NORMAL, 3 = SLEEP, 4 = LISTEN, 5 = LOOPBACK
}

//************************** Set register ******************************
/*void CAN_setRegister(byte reg, byte value, int CS_pin) {
 //  mcp2515_write_reg (reg, value); this should be instead to write the registers
 //static void mcp2515_write_reg (uint8_t addr, uint8_t buf)
 // used from other program.  Replaced by above 
 digitalWrite(slaveselectpin, LOW);
 delay(10);
 SPI.transfer(WRITE);
 SPI.transfer(reg);
 SPI.transfer(value);
 delay(10);
 digitalWrite(slaveselectpin, HIGH);
 delay(10);
 
 }
 // end from another program
 */


///*********************** end ***********************
void CAN_end() {
  SPI.end ();
}

//********************* setMode *********************
void CAN_setMode (uint8_t mode,int CS_pin)
{
  // mcp2515_set_mode (mode);
  mcp2515_bit_modify (CANCTRL, REQOP_MASK, mode << REQOP, CS_pin);
}

//******************** ready **********************
uint8_t CAN_ready (int CS_pin)
{
  //return mcp2515_msg_sent ();
  uint8_t byte;

  mcp2515_read_regs (REG(TX, 0, CTRL), &byte, 1, CS_pin);

  return (!(byte & (1 << TXREQ)));


}

//****************** Available *********************
boolean CAN_available (int CS_pin)
{
  //return (boolean)mcp2515_msg_received();
  uint8_t byte;
  mcp2515_read_regs (CANINTF, &byte, 1, CS_pin);

  return (byte & ((1 << RX1IF) | (1 << RX0IF)));



}

//***************** getMessage ***********************
void CAN_getMessage(int CS_pin) //rewrite to have both chips checked??
{
  uint8_t extended;
extended = mcp2515_get_msg (0, &id, data, &len,CS_pin);
}
//CANClass CAN;

/*
/**
 * A class representing a single CAN message.  The message can be built
 * using the send<Type>Data functions, or the bytes of the message can
 * be set directly by accessing the public data[] array.  This class is
 * also used to retrieve a message that has been received.  The data
 * can be read using the get<type>Data functions, or can be read directly
 * by accessing the public data[] array.
 */
//class CanMessage {
//    public:
/** A flag indicating whether this is an extended CAN message */
//        uint8_t extended;
/** The ID of the CAN message.  The ID is 29 bytes long if the
 * extended flag is set, or 11 bytes long if not set. */
//        uint32_t id;
/** The number of bytes in the data field (0-8) */
//        uint8_t len;
/** Array containing the bytes of the CAN message.  This array
 * may be accessed directly to set or read the CAN message.
 * This field can also be set by the setTypeData functions and
 * read by the getTypeData functions. */
//        uint8_t data[8];

//        CanMessage();

/**
 * Simple interface to set up a CAN message for sending a byte data
 * type.  When received, this message should be unpacked with the
 * getByteData function.  This interface only allows one byte to be
 * packed into a message.  To pack more data, access the data array
 * directly.
 * @param b - The byte to pack into the message.
 */
//        void setByteData (byte b);

/**
 * Simple interface to set up a CAN message for sending an int data
 * type.  When received, this message should be unpacked with the
 * getIntData function.  This interface only allows one int to be
 * packed into a message.  To pack more data, access the data array
 * directly.
 * @param i - The int to pack into the message.
 */
//       void setIntData (int i);

/**
 * Simple interface to set up a CAN message for sending a long data
 * type.  When received, this message should be unpacked with the
 * getLongData function.  This interface only allows one long to be
 * packed into a message.  To pack more data, access the data array
 * directly.
 * @param l - The long to pack into the message.
 */
//        void setLongData (long l);

/**
 * A convenience function for copying multiple bytes of data into
 * the message.
 * @param data - The data to be copied into the message
 * @param len  - The size of the data
 */
//        void setData (const uint8_t *data, uint8_t len);
//        void setData (const char *data, uint8_t len);

/**
 * Send the CAN message.  Once a message has been created, this
 * function sends it.
 */
//        void send();

/**
 * Simple interface to retrieve a byte from a CAN message.  This
 * should only be used on messages that were created using the
 * setByteData function on another node.
 * @return The byte contained in the message.
 */
//        byte getByteFromData ();

/**
 * Simple interface to retrieve an int from a CAN message.  This
 * should only be used on messages that were created using the
 * setIntData function on another node.
 * @return The int contained in the message.
 */
//        int getIntFromData ();

/**
 * Simple interface to retrieve a long from a CAN message.  This
 * should only be used on messages that were created using the
 * setLongData function on another node.
 * @return The long contained in the message.
 */
//        long getLongFromData ();

/**
 * A convenience function for copying multiple bytes out of a
 * CAN message.
 * @param data - The location to copy the data to.
 */
//        void getData (uint8_t *data);
//        void getData (char *data);
//};

//class CANClass {
//	public:
/**
 * Call before using any other CAN functions.
 * @param bit_time - Desired width of a single bit in nanoseconds.
 *                   The CAN_SPEED enumerated values are set to
 *                   the bit widths of some common frequencies.
 */
//		static void begin(uint32_t bit_time);

/** Call when all CAN functions are complete */
//		static void end();

/**
 * Set operational mode.
 * @param mode - One of the CAN_MODE enumerated values */
//		static void setMode(uint8_t mode);

/** Check whether a message may be sent */
//       static uint8_t ready ();

/**
 * Check whether received CAN data is available.
 * @return True if a message is available to be retrieved.
 */
//        static boolean available ();

/**
 * Retrieve a CAN message.
 * @return A CanMessage containing the retrieved message
 */
//        static CanMessage getMessage (); */












