// MSP430FR5969 HC-SR04 Distance Measurement (polling style, stable)
// Author: Fayyaz Shaikh
// Email: fayyaz.shaikh7862005@gmail.com

#include <msp430.h>

#define TRIGGER BIT3   // P4.3
#define ECHO    BIT4   // P2.4
#define LED     BIT0   // P1.0

#define TIMEOUT 30000      // Max pulse width to consider (30ms)
#define DIST_MIN 2         // Minimum reliable distance (cm)
#define DIST_MAX 400       // Maximum reliable distance (cm)

// ---------- UART Functions ----------
void uart_init(void) {
    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= BIT0 | BIT1;      // P2.0=TX, P2.1=RX
    UCA0CTLW0 = UCSWRST;        // Reset eUSCI
    UCA0CTLW0 |= UCSSEL__SMCLK; // SMCLK
    UCA0BR0 = 8;                 // 1 MHz / 115200
    UCA0BR1 = 0;
    UCA0MCTLW = 0xD600;
    UCA0CTLW0 &= ~UCSWRST;      // Release from reset
}

void uart_putc(char c){
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = c;
}

void uart_puts(const char *str){
    while(*str) uart_putc(*str++);
}

// Microsecond delay (~1us per cycle at 1MHz)
void delay_us(unsigned int us){
    while(us--) __delay_cycles(1);
}

// pulseIn equivalent 
unsigned int pulseIn() {
    unsigned int timeout = TIMEOUT;

    // Wait for HIGH
    while((P2IN & ECHO) == 0 && timeout--) __no_operation();
    if(timeout == 0) return 0;

    unsigned int start = TA0R;

    // Wait for LOW
    timeout = TIMEOUT;
    while((P2IN & ECHO) != 0 && timeout--) __no_operation();
    unsigned int stop = TA0R;

    if(timeout == 0) return 0;

    // Handle timer overflow
    if(stop >= start) return stop - start;
    else return 0xFFFF - start + stop + 1;
}

// Convert integer to string 
void int_to_str(unsigned int num, char *str){
    int i=0;
    if(num==0){ str[i++]='0'; str[i]='\0'; return; }

    char temp[10];
    int j=0;
    while(num>0){ temp[j++] = (num%10)+'0'; num/=10; }

    int k;
    for(k=j-1;k>=0;k--) str[i++] = temp[k];
    str[i]='\0';
}

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog

    // Clock to 1 MHz
    CSCTL0_H = CSKEY_H;        // Unlock CS registers
    CSCTL1 = DCOFSEL_0;        
    CSCTL2 = SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVS__1 | DIVM__1;
    CSCTL0_H = 0;              // Lock CS

    //   GPIO 
    P1DIR |= LED; P1OUT &= ~LED;
    P4DIR |= TRIGGER; P4OUT &= ~TRIGGER;
    P2DIR &= ~ECHO; P2REN &= ~ECHO; // Echo input
    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIOs

    // UART ---
    uart_init();
    uart_puts("HC-SR04 MSP430FR5969 Stable Polling Demo\r\n");

    // --- Timer0_A ---
    TA0CTL = TASSEL_2 | MC_2 | TACLR; 

    while(1){
        // Trigger pulse
        P4OUT &= ~TRIGGER; delay_us(2);
        P4OUT |= TRIGGER;  delay_us(10);
        P4OUT &= ~TRIGGER;

        // Measure pulse
        unsigned int pulse_width = pulseIn();
        unsigned int distance_cm = pulse_width / 58;

        // Filter invalid readings
        if(distance_cm < DIST_MIN || distance_cm > DIST_MAX) distance_cm = 0;

        // LED indication
        if(distance_cm) P1OUT |= LED; else P1OUT &= ~LED;

        // UART print
        char buffer[20];
        int_to_str(distance_cm, buffer);
        uart_puts("Distance: ");
        uart_puts(buffer);
        uart_puts(" cm\r\n");

        __delay_cycles(16000); // ~0.16s delay
    }
}
