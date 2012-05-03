/*
Sets up msp UART according to best principles, and then reads serial every RX interrupt
toggles LED and replies.
*/
#include "msp430g2553.h"
char str[31];
unsigned int i;
void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop WDT
BCSCTL1 = CALBC1_1MHZ; // Set DCO
DCOCTL = CALDCO_1MHZ;

P1SEL = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
P1SEL2 = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
P1DIR = BIT6 + BIT0; // P1.6 and P1.0 outputs
P1OUT = BIT6+BIT0; // LEDs off
UCA0CTL1 |= UCSSEL_2; // SMCLK
UCA0BR0 = 104; // 1MHz 9600
UCA0BR1 = 0; // 1MHz 9600
UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

__bis_SR_register(LPM0_bits + GIE); // Enter LPM0, interrupts enabled
}


// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

if(UCA0RXBUF==0x030)
{
P1OUT=0;
UCA0TXBUF = 0x030;
}
else if (UCA0RXBUF==0x031)
{
P1OUT= BIT0;
UCA0TXBUF = 0x031;
}
else if (UCA0RXBUF==0x032)
{
P1OUT=BIT6;
UCA0TXBUF = 0x032;
}
else if (UCA0RXBUF==0x033)
{
P1OUT=BIT0+BIT6;
UCA0TXBUF = 0x033;
}

}

// USCI A0/B0 Transmit ISR
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
UCA0TXBUF = str[i++]; // TX next character
}