 //******************************************mcp2515 declarations**************************************
 #include <stdint.h>

/** For each increment of the BRP, our time quanta goes up this many
  * nanoseconds (for our 16MHz crystal) */
#define TIME_QUANTUM_STEP            125

/** The minimum bit width in time quanta (1, 1, 1, 2) */
#define QUANTUM_WIDTH_MIN           5

/** The maximum bit width in time quanta (1, 8, 8, 8) */
#define QUANTUM_WIDTH_MAX           25

/** The highest possible BRP value */
#define BRP_MAX               63

/** The SYNC_JUMP_WIDTH is always 1 time quantum */
#define SYNC_JUMP_WIDTH 1


/* SPI Commands */
enum {
    MCP2515_CMD_RESET       = 0xC0,
    MCP2515_CMD_READ        = 0x03,
    MCP2515_CMD_WRITE       = 0x02,
    MCP2515_CMD_RTS         = 0x80,
    MCP2515_CMD_READ_STATUS = 0xA0,
    MCP2515_CMD_BIT_MODIFY  = 0x05,
};

#define FIELD_MASK(w)   ((1 << w) - 1)

/* Registers and bits */
#define RXM0SIDH    0x20
#define RXM1SIDH    0x24

#define CANSTAT     0x0E
#define OPMOD       5
#define ICOD        1

#define CANCTRL     0x0F
#define REQOP       5
#define REQOP_MASK  (FIELD_MASK(3) << REQOP)
#define ABAT        4
#define CLKEN       2
#define CLKPRE      0

#define CNF3        0x28
#define PHSEG2      0
#define WAKFIL      6

#define CNF2        0x29
#define PRSEG       0
#define PHSEG1      3
#define SAM         6
#define BTLMODE     7

#define CNF1        0x2A
#define BRP         0
#define SJW         6

#define CANINTF     0x2C
#define MERRF       7
#define WAKIF       6
#define ERRIF       5
#define TX2IF       4
#define TX1IF       3
#define TX0IF       2
#define RX1IF       1
#define RX0IF       0

/**
 * For registers that are duplicated across multiple RX and TX buffers,
 * use the REG macro.
 * @param dir - Direction, either "RX" or "TX"
 * @param num - Number of the buffer, 0 or 1 for RX; 0, 1, or 2 for TX.
 * @param reg - The register of the buffer, defined below.
 * @return The address of the register
 */
#define REG(dir,num,reg)  (((dir) + ((num) << 4)) | (reg))
#define TX          0x30
#define RX          0x60

#define CTRL        0x00
#define SIDH        0x01
#define SIDL        0x02
#define EID8        0x03
#define EID0        0x04
#define DLC         0x05
#define D0          0x06
#define D1          0x07
#define D2          0x08
#define D3          0x09
#define D4          0x0A
#define D5          0x0B
#define D6          0x0C
#define D7          0x0D

/* TX CTRL bits */
#define ABTF        6
#define MLOA        5
#define TXERR       4
#define TXREQ       3
#define TXP         0

/* TXB SIDL bits */
#define EXIDE       3

/* TX DLC bits */
#define RTR         6
#define DLC0        0

/* RX CTRL bits */
#define RXM         5
#define RXRTR       3
#define BUKT        2
#define BUKT1       1
#define FILHIT0     0

/* RXB SIDL bits */
#define IDE         3

/* RX DLC bits */
#define RTR         6
#define RB          4
// #define DLC0 0  // Duplicate of TX

/* Operation Modes */
#define MCP2515_MODE_NORMAL         0x00
#define MCP2515_MODE_SLEEP          0x01
#define MCP2515_MODE_LOOPBACK       0x02
#define MCP2515_MODE_LISTEN_ONLY    0x03
#define MCP2515_MODE_CONFIG         0x04

/**
 * CAN bus speeds.  These are the bit-times for common frequencies.
 */
enum {
    MCP2515_SPEED_500000 = 2000,
    MCP2515_SPEED_250000 = 4000,
    MCP2515_SPEED_125000 = 8000,
    MCP2515_SPEED_100000 = 10000,
    MCP2515_SPEED_62500  = 16000,
    MCP2515_SPEED_50000  = 20000,
    MCP2515_SPEED_31250  = 32000,
    MCP2515_SPEED_25000  = 40000,
    MCP2515_SPEED_20000  = 50000,
    MCP2515_SPEED_15625  = 64000
};
