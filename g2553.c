/*
Sets up msp UART according to best principles, and then reads serial every RX interrupt
toggles LED and replies.
*/
#include "msp430g2553.h"
#define countdown 0x05
unsigned int count, second, go_bit, newmsg, started, msgindex;
char str[31];
char rxbuff[10];
unsigned int i;
void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop WDT
BCSCTL1 = CALBC1_1MHZ; // Set DCO
DCOCTL = CALDCO_1MHZ;

P1SEL = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
P1SEL2 = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
P1DIR = BIT6 + BIT0; // P1.6 and P1.0 outputs
P1OUT = 0; // LEDs off
P2DIR = BIT3 + BIT4;
P2OUT = BIT3 + BIT4;
UCA0CTL1 |= UCSSEL_2; // SMCLK
UCA0BR0 = 104; // 1MHz 9600
UCA0BR1 = 0; // 1MHz 9600
UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
          CCTL0 = CCIE; // CCR0 interrupt 
          CCR0 = 125;	 // Set 1 count period - 125 @ 125khz == 1ms
          TACTL = TASSEL_2 + MC_1 + ID_3; // Timer_A is using 
        //SMCLK (1MHz)
        // Up mode -- the timer counts up to TACCR0
        // SMCLK divider of 2^3=8... f=125khz 
__bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled

while(1) {

          if(newmsg) {
    P1OUT^=BIT0;
  }
  
  
}
}


// USCI Receive ISR - Called when shift register has been transferred to RXBUF
// Indicates completion of TX/RX operation
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
//     253 as STARTCHAR and 255 as STOPCHAR
if(!started) {
	if(UCA0RXBUF == 0x46) {
				started = 1;
				newmsg = 0;
			}
}
		// In process of receiving a message		
        else if (started) {
          if(UCA0RXBUF != 0x0FF) {
				rxbuff[msgindex] = UCA0RXBUF;

				msgindex++;
        }
          // Stop char received or too much data received
		else if(UCA0RXBUF == 0x32) {	// Message completed
					newmsg = 1;
					rxbuff[msgindex] = 0xFF;	// "Null"-terminate the array
				
				started = 0;
				msgindex = 0;
                                }
        }
		
 }

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
         count = (count+1) % 1000;
	// Increment refresh count
  if (count == 0) {
  second = (second + 1) % countdown;
  if ((second==0) && (go_bit==1)){
      P1OUT ^= BIT6;
      P2OUT = BIT3
  }
  if ((second==0) & (go_bit==0)) {
      go_bit=1;
  }
         }

}