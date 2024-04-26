#include <msp430.h>
#include <stdint.h>

// Helper method to output messages to serial
void ser_output(char *str){
    while(*str != 0) {
        while (!(IFG2 & UCA0TXIFG)); //while the TX buffer is ready
        UCA0TXBUF = *str++; //Set the transmit buffer
        }
}

void main(void)
{
    // Watchdog timer setup
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Pin Setup
    P1DIR |= BIT6 + BIT0;       // Set up P1.6 for PWM and P1.0 for alarm output
    P1SEL |= BIT6 + BIT0;
    P2DIR |= BIT1;              // Set up P2.1 for PWM
    P2SEL |= BIT1;

    // UART Setup
    WDTCTL = WDT_ADLY_1000;
    IE1 |= WDTIE;                 // WDT interrupt
    P1SEL |= BIT1 + BIT2;         // Set up P1.1 and P1.2 for UART communication
    P1SEL2 |= BIT1 + BIT2;

    UCA0CTL1 |= UCSWRST+UCSSEL_2; //Set the SMCLK
    UCA0BR0 = 104;  //settings for 9600 baud, compatible with bluetooth module
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST; //enable reset to initialize the state machine for USCI
    UC0IE |= UCA0RXIE;    // enable interrupt USCI

    __bis_SR_register(LPM1_bits + GIE);

    // ADC Setup
    ADC10CTL0 |= SREF_0 + ADC10SHT_2 + MSC + ADC10ON; // enable multi channel conversion
    ADC10CTL1 |= INCH_4 + SHS_0 + ADC10DIV_0 + ADC10SSEL_0 + CONSEQ_1;
    ADC10AE0 |= BIT3 + BIT4;  // P1.3 and P1.4
    ADC10CTL0 |= ENC;         // enable ADC

    volatile unsigned int adc[2]; // array for multichannel ADC values

    ADC10DTC1 = 0x02; // speicfy two channels

    // PWM for P1.6 controlling pan movement of the camera
    TA0CCR0 = 20000;
    TA0CCTL1 = OUTMOD_7;  //CCR1 selection reset-set
    TA0CTL = TASSEL_2|MC_1;   //SMCLK submain clock,upmode

    // PWM for P2.1 controlling tilt movement of the camera
    TA1CCR0 = 20000;
    TA1CCTL1 = OUTMOD_7;  //CCR1 selection reset-set
    TA1CTL = TASSEL_2|MC_1;   //SMCLK submain clock,upmode

    unsigned int x;
    unsigned int y;

    while(1){
        ADC10CTL0 |= ENC + ADC10SC;
        while(ADC10CTL1 & ADC10BUSY);   //while ADC10 is not BUSY
        ADC10SA = (unsigned int) adc;
        x = adc[0];                     // first channel X direction
        y = adc[1];                     // first channel Y direction

        TA0CCR1 = (int)(350 + (1.95*x)); // set PWM for pan movement with X direction of joystick
        TA1CCR1 = (int)(350 + (1.95*y)); // set PWM for tilt movement with X direction of joystick

        __bis_SR_register(LPM1_bits + GIE); // low power mode
        __delay_cycles(10000);
    }
}

// Watchdog timer service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM1_bits);     // Clear LPM3 bits from 0(SR)
}

// UART RX intterupt service routine
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    // when data is 1 alarm turned on, when data is 2 alarm turned off
    char data = UCA0RXBUF;
    while (!(IFG2&UCA0RXIFG)); // USCI_A0 TX buffer ready?
    if (data == '1') {
        P1OUT |= BIT0;  // Turn on alarm
        ser_output('alarm turned on');
    }
    else if (data == '2') {
        P1OUT &= ~BIT0;  // Turn off alarm
        ser_output('alarm turned off');
    }
}





