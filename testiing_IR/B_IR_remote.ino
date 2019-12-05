#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

void sendNEC (unsigned long data,  int nbits)
{
  // Set IR carrier frequency
  enableIROut(38);

  // Header
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);

  // Data
  for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
    if (data & mask) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    } else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
  }

  // Footer
  mark(NEC_BIT_MARK);
  space(0);  // Always end with the LED off
}
void mark (unsigned int time)
{
  TIMER_ENABLE_PWM; // Enable pin 3 PWM output
  if (time > 0) custom_delay_usec(time);
}
void custom_delay_usec(unsigned long uSecs) {
  if (uSecs > 4) {
    unsigned long start = micros();
    unsigned long endMicros = start + uSecs - 4;
    if (endMicros < start) { // Check if overflow
      while ( micros() > start ) {} // wait until overflow
    }
    while ( micros() < endMicros ) {} // normal wait
  } 
  //else {
  //  __asm__("nop\n\t"); // must have or compiler optimizes out
  //}
}
//+=============================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
void space (unsigned int time)
{
  TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  if (time > 0)custom_delay_usec(time);
}


//+=============================================================================
// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// The IR output will be on pin 3 (OC2B).
// This routine is designed for 36-40KHz; if you use it for other values, it's up to you
// to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
// TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
// controlling the duty cycle.
// There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
// A few hours staring at the ATmega documentation and this will all make sense.
// See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
//
void  enableIROut (int khz)
{
// FIXME: implement ESP32 support, see IR_TIMER_USE_ESP32 in boarddefs.h
#ifndef ESP32
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt

  pinMode(TIMER_PWM_PIN, OUTPUT);
  digitalWrite(TIMER_PWM_PIN, LOW); // When not sending PWM, we want it low

  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2  = 000: no prescaling
  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
  TIMER_CONFIG_KHZ(khz);
#endif
}

/*
//+=============================================================================
// NECs have a repeat only 4 items long
//
bool  IRrecv::decodeNEC (decode_results *results)
{
  long  data   = 0;  // We decode in to here; Start with nothing
  int   offset = 1;  // Index in to results; Skip first entry!?

  // Check header "mark"
  if (!MATCH_MARK(results->rawbuf[offset], NEC_HDR_MARK))  return false ;
  offset++;

  // Check for repeat
  if ( (irparams.rawlen == 4)
      && MATCH_SPACE(results->rawbuf[offset  ], NEC_RPT_SPACE)
      && MATCH_MARK (results->rawbuf[offset+1], NEC_BIT_MARK )
     ) {
    results->bits        = 0;
    results->value       = REPEAT;
    results->decode_type = NEC;
    return true;
  }

  // Check we have enough data
  if (irparams.rawlen < (2 * NEC_BITS) + 4)  return false ;

  // Check header "space"
  if (!MATCH_SPACE(results->rawbuf[offset], NEC_HDR_SPACE))  return false ;
  offset++;

  // Build the data
  for (int i = 0;  i < NEC_BITS;  i++) {
    // Check data "mark"
    if (!MATCH_MARK(results->rawbuf[offset], NEC_BIT_MARK))  return false ;
    offset++;
        // Suppend this bit
    if      (MATCH_SPACE(results->rawbuf[offset], NEC_ONE_SPACE ))  data = (data << 1) | 1 ;
    else if (MATCH_SPACE(results->rawbuf[offset], NEC_ZERO_SPACE))  data = (data << 1) | 0 ;
    else                                                            return false ;
    offset++;
  }

  // Success
  results->bits        = NEC_BITS;
  results->value       = data;
  results->decode_type = NEC;

  return true;
}
*/

