#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 1000000 // 1MBd
#define MYUBRR FOSC/16/BAUD-1
#define nlines 2 // Number of input lines

#define _CLI cli

int inputpins[nlines];
unsigned long timer_count = 0;


unsigned int TIM16_ReadTCNT1( void ){
    unsigned char sreg;
    unsigned int i;
    /* Save global interrupt flag */
    sreg = SREG;
    /* Disable interrupts */
    _CLI();
    /* Read TCNT1 into i */
    i = TCNT1;
    /* Restore global interrupt flag */
    SREG = sreg;
    return i;
}

void USART_Init( unsigned int ubrr){
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char) ubrr;
    UCSR0A = 0;
    /*Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (0<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data ){ // Transmit 5 to 8 bits through serial port
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ;
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char USART_Receive( void ){
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
    ;
    /* Get and return received data from buffer */
    return UDR0;
}

void Input_Init( void ){
    DDRD =  (1<<DDD1)|(1<<DDD0); // Define input pins as NOT 5 to 0

    inputpins[0] = PD6;
    inputpins[1] = PD7;
}

void Interrupt_Init(){
    sei();                      // Enable interruptions globally

    PCICR = _BV(PCIE2);         // Enable Pin Change Interrupt on Pin Change Mask 2
    PCMSK2 = 255;               // Set interrupt on all D port 
    PCMSK2 &= ~_BV(PCINT16);    // Disable interrupt on pin 0 (Rx)  
    PCMSK2 &= ~_BV(PCINT17);    // Disable interrupt on pin 1 (Tx)

}

void Timer_init(){
    //Timer interrupt every 10ms
    OCR1A = 20000;
    TIMSK1 |= _BV(OCIE1A);
    //Set interrupt on compare match
    TCCR1B = _BV(WGM12) | _BV(CS11);
    // Mode 4, CTC on OCR1A
    // set prescaler to 8 and start the timer
}

ISR(PCINT2_vect){ // Pin Change Interruption

    unsigned long timestamp = ((unsigned long)TIM16_ReadTCNT1()/2 + timer_count*10000);
    
    unsigned short t0 = (unsigned short) ((timestamp & 0xFF000000)>>24);
    unsigned short t1 = (unsigned short) ((timestamp & 0x00FF0000)>>16);
    unsigned short t2 = (unsigned short) ((timestamp & 0x0000FF00)>>8);
    unsigned short t3 = (unsigned short) ((timestamp & 0x000000FF));

    unsigned short portD = 0; 
    portD = PIND & 0xFC;        // Avoid to send Tx and Rx bits

    // Headers
    USART_Transmit(0xAA);
    USART_Transmit(0xFF);    

    // Timestamp
    USART_Transmit(t0); 
    USART_Transmit(t1); 
    USART_Transmit(t2); 
    USART_Transmit(t3); 

    // Data
    USART_Transmit(portD);  

    // Check sum
    unsigned short cs = t0 + t1 + t2 + t3 + portD;
    USART_Transmit(cs);
}

ISR(TIMER1_COMPA_vect){
    ++timer_count;
}

int main() {
    USART_Init(MYUBRR);         // Set serial communication
    Input_Init();               // Set pins which will be considered as input
    Interrupt_Init();           // Set interruptions
    Timer_init();               // Set timer
    
    while(1){
        // Empty loop ! YEAAAH !
    }
}