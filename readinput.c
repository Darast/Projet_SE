#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define nlines 2 // Number of input lines

int inputpins[nlines];

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
    DDRD =  (1<<DDD5)|(1<<DDD4)|(1<<DDD3)|(1<<DDD2)|(1<<DDD1)|(1<<DDD0); // Define input pins as NOT 5 to 0
    PORTD = (1<<PD7)|(1<<PD6); // Assign pull-ups to pins 6 and 7

    inputpins[0] = PD6;
    inputpins[1] = PD7;
}

void Interrupt_Init(){
    sei(); // Enable interruptions globally
    // SREG |= _BV(7); // Equivalent to sei()

    PCICR = _BV(PCIE2);     // Enable Pin Change Interrupt on Pin Change Mask 2
    PCMSK2 = _BV(PCINT23);  // Choose PD7 as trigger for interrupt
    // PCMSK2 = _BV(PCINT22);  //PD6
}

ISR(PCINT2_vect){ // Pin Change Interruption
    char pin7 = (PIND & _BV(PD7))!=0; // Read states of each input pin
    char pin6 = (PIND & _BV(PD6))!=0;
    
    unsigned short oct = 0; // Concatenate as a byte
    oct |= (pin7<<1) | (pin6<<0);

    USART_Transmit(oct); // Transmit data byte through serial port 
}

int main() {
    USART_Init(MYUBRR); // Set serial communication
    Input_Init();       // Set pins which will be considered as input
    Interrupt_Init();   // Set interruptions

    //unsigned char old_data[nlines]; // Instant memory of the former read values

    while(1){
        // Empty loop ! YEAAAH !
    }
}