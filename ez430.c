/*
Sets up msp UART according to best principles, and then pushes out a new value every timer interrupt.

Waits for a reply to toggle LEDS with. 
*/
#include "msp430.h"
#include "in430.h"
#include "msp430F2274.h"
unsigned int i=0;
unsigned int j=0;
int main ( void )
{
//- timer example code
WDTCTL = WDTPW + WDTHOLD;
P1DIR |= 0x03; // initialize led pins

// basic clock system control register 3
BCSCTL3 |= LFXT1S_2; // VLOCLK
TACCTL0 = CCIE; // Capture compare interupt enable
TACCR0 = 2000; // Timer A capture register value
TACTL = MC_1+TASSEL_1; // Up mode: the timer counts up to TACCR0. Clock srce ACLK.

BCSCTL1 = CALBC1_1MHZ; // Set DCO
DCOCTL = CALDCO_1MHZ;
P3SEL = 0x30; // P3.4,5 = USCI_A0 TXD/RXD
UCA0CTL1 |= UCSSEL_2; // SMCLK
UCA0BR0 = 104; // 1MHz 9600
UCA0BR1 = 0; // 1MHz 9600
UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

__bis_SR_register (GIE+LPM3_bits );
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
if (i==0)
{
UCA0TXBUF=0x030;
i++;
}
else if (i==1)
{
UCA0TXBUF=0x031;

i++;
}
else if (i==2)
{
UCA0TXBUF=0x032;
i++;
}
else if (i==3)
{
UCA0TXBUF=0x033;

i=0;
}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
if(UCA0RXBUF==0x030)
{ P1OUT=0x00;
}
else if (UCA0RXBUF==0x031)
{ P1OUT= 0x01;
}
else if (UCA0RXBUF==0x032)
{ P1OUT=0x02;
}
else if (UCA0RXBUF==0x033)
{ P1OUT=0x03;
}
//UCA0TXBUF=UCA0RXBUF;
}

// USCI A0/B0 Transmit ISR
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
}