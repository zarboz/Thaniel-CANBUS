
void mcp2515_read_regs (uint8_t addr, uint8_t* buf, uint8_t n)
{
    int i;

      digitalWrite (slaveSelectPin, LOW);
    SPI.transfer (MCP2515_CMD_READ);
    SPI.transfer (addr);
    for (i=0; i<n; i++)
        buf[i] = SPI.transfer (0);
    digitalWrite (slaveSelectPin, HIGH);
}

void mcp2515_write_regs (uint8_t addr, const uint8_t* buf, uint8_t n)
{
    int i;

      digitalWrite (slaveSelectPin, LOW);
    SPI.transfer (MCP2515_CMD_WRITE);
    SPI.transfer (addr);
    for (i=0; i<n; i++)
        SPI.transfer (buf[i]);
    digitalWrite (slaveSelectPin, HIGH);
}

static void mcp2515_write_reg (uint8_t addr, uint8_t buf)
{
      digitalWrite (slaveSelectPin, LOW);;
    SPI.transfer (MCP2515_CMD_WRITE);
    SPI.transfer (addr);
    SPI.transfer (buf);
    digitalWrite (slaveSelectPin, HIGH);
}

static void mcp2515_bit_modify (uint8_t addr, uint8_t mask, uint8_t bits)
{
      digitalWrite (slaveSelectPin, LOW);
    SPI.transfer (MCP2515_CMD_BIT_MODIFY);
    SPI.transfer (addr);
    SPI.transfer (mask);
    SPI.transfer (bits);
    digitalWrite (slaveSelectPin, HIGH);
}

/**
 * Find the best prescalar and bit width in time quanta for the given bit
 * period.  This algorithm favors lower prescalars and therefore higher
 * frequencies and more time quanta per bit.
 * @param bit_period - Length of bit period in nanoseconds
 * @param bit_width  - Pointer to the location to store the bit width
 * return - The best prescalar
 */
static uint8_t calc_brp (uint32_t bit_period, uint8_t *bit_width)
{
    uint32_t total_steps;
    uint16_t brp_min;
    uint16_t brp_max;
    uint8_t error;
    uint8_t best_width;
    uint8_t best_brp;
    uint8_t best_error = BRP_MAX;
    uint8_t i;

    /* Calculate the minimum BRP that can meet this rate */
    brp_min = bit_period / (QUANTUM_WIDTH_MAX * TIME_QUANTUM_STEP);
    /* Calculate the maximum BRP that can meet this rate */
    brp_max = bit_period / (QUANTUM_WIDTH_MIN * TIME_QUANTUM_STEP);

    /* Don't check outside the valid range */
    if (brp_min > BRP_MAX)
        brp_min = BRP_MAX;
    if (brp_max > BRP_MAX)
        brp_max = BRP_MAX;

    total_steps = bit_period / TIME_QUANTUM_STEP;

    for (i = brp_min; i <= brp_max; i++) {
        error = total_steps % (i + 1);

        /* If the number of quanta that make the correct bit rate at this
         * prescalar is an integer, this is an exact match. */
        if (error == 0) {
            best_brp = i;
            best_width = total_steps / (i + 1);
            break;
        }

        /* If rounding up, the error is the difference between the divisor
         * and the remainder */
        if ((i + 1) - error < error) {
            error = (i + 1) - error;
        }

        /* Store the settings at this BRP if they are the best yet */
        if (error < best_error) {
            best_error = error;
            best_width = (total_steps + ((i + 1) >> 1))/ (i + 1);
            best_brp = i;
        }
    }

    /* Return the best match */
    *bit_width = best_width;
    return best_brp;
}





/*
 * Set the operating mode of the MCP2515
 */
//void mcp2515_set_mode (uint8_t mode)
//{
//    mcp2515_bit_modify (CANCTRL, REQOP_MASK, mode << REQOP);
//}

/*
 * Reads a message from the receive buffer and marks it as read.
 */
uint8_t mcp2515_get_msg (uint8_t rx_buf, uint32_t *id,
                    uint8_t *data, uint8_t *len)
{
    uint8_t buf[5];
    uint8_t extended;

    mcp2515_read_regs (REG(RX, rx_buf, SIDH), buf, 5);
    *len = buf[4] & 0x0f;
    if (*len > 8)
        *len = 8;
    mcp2515_read_regs (REG(RX, rx_buf, D0), data, *len);

    extended = buf[1] & (1 << IDE);
    if (extended) {
        *id = ((uint32_t)(buf[0]) << 21) |
              ((uint32_t)(buf[1] & 0xE0) << 13) |
              ((uint32_t)(buf[1] & 0x03) << 16) |
              ((uint32_t)(buf[2]) << 8) |
              ((uint32_t)(buf[3]) << 0);
    } else {
        *id = ((uint32_t)buf[0] << 3) |
              ((uint32_t)buf[1] >> 5);
    }

    mcp2515_bit_modify (CANINTF, 1 << (RX0IF + rx_buf), 0);

    return extended;
}



void mcp2515_set_rx_mask (uint8_t mask_num, uint32_t mask, uint8_t extended)
{
    uint8_t reg;
    uint8_t buf[4];

    if (mask_num == 0)
        reg = RXM0SIDH;
    else
        reg = RXM1SIDH;

    if (extended) {
        buf[0] = (uint8_t)(mask >> 21);
        buf[1] = (uint8_t)(((mask >> 13) & 0xE0)
                         | ((mask >> 16) & 0x03));
        buf[2] = (uint8_t)(mask >> 8);
        buf[3] = (uint8_t)(mask >> 0);
    } else {
        buf[0] = (uint8_t)(mask >> 3);
        buf[1] = (uint8_t)(mask << 5);
        buf[2] = 0;
        buf[3] = 0;
    }

    mcp2515_write_regs (reg, buf, 4);
}

void mcp2515_set_rx_filter (uint8_t filter_num, uint32_t filter, uint8_t extended)
{
    uint8_t reg;
    uint8_t buf[4];

    /* Calculate correct register */
    reg = filter_num * 4;
    if (reg >= 12)
        reg += 4;

    if (extended) {
        buf[0] = (uint8_t)(filter >> 21);
        buf[1] = (uint8_t)(((filter >> 13) & 0xE0)
                         | ((filter >> 16) & 0x03)
                         | (1 << EXIDE));
        buf[2] = (uint8_t)(filter >> 8);
        buf[3] = (uint8_t)(filter >> 0);
    } else {
        buf[0] = (uint8_t)(filter >> 3);
        buf[1] = (uint8_t)(filter << 5);
        buf[2] = 0;
        buf[3] = 0;
    }

    mcp2515_write_regs (reg, buf, 4);
}
