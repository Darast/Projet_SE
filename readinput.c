#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define nlines 2 // Number of input lines

int inputpins[nlines];


void USART_Init( unsigned int ubrr)
{
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0A = 0;
    /*Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (0<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data ) // Transmit 5 to 8 bits 
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ;
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char USART_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
    ;
    /* Get and return received data from buffer */
    return UDR0;
}

void Input_Init( void )
{
    DDRD =  (1<<DDD5)|(1<<DDD4)|(1<<DDD3)|(1<<DDD2)|(1<<DDD1)|(1<<DDD0); // Define input pins as NOT 5 to 0
    PORTD = (1<<PD7)|(1<<PD6); // Assign pull-ups to pins 6 and 7

    inputpins[0] = PD6;
    inputpins[1] = PD7;
}

unsigned char Read_Input( unsigned char block, int line )
{
    /* Insert nop for synchronization*/
    //__no_operation();
    /* Read individual state of the given line */
    return block & _BV(inputpins[line]);
}

void WriteLine( char* string, int size)
{
    int index = 0;

    while (index < size)
    {
        USART_Transmit(string[index]);
        index++;
    }

    USART_Transmit('\r'); // Retour chariot
    USART_Transmit('\n'); // Descendre d'une ligne
}

void Interrupt_init(){
    // SREG |= _BV(7);
    sei();
    // PCICR = _BV(PCIE2);
    // PCMSK2 = _BV(PCINT22);  //PD6

    PCICR = _BV(PCIE2);
    PCMSK2 = _BV(PCINT23);  //PD7

}

ISR(PCINT2_vect){
    char pin7 = (PIND & _BV(PD7))!=0;
    char pin6 = (PIND & _BV(PD6))!=0;
    
    unsigned short oct = 0;
    oct |= (pin7<<1) | (pin6<<0);
    USART_Transmit(oct);

    // USART_Transmit('\r');
    // USART_Transmit('\n'); 
        
}

int main() {
    USART_Init(MYUBRR);
    Input_Init();
    Interrupt_init();

    unsigned char old_data[nlines]; // Instant memory of the former read values

    while(1){

        // for (curr_line = 0; curr_line < nlines; curr_line++){
        //     newdata = Read_Input(data_block, curr_line); // Read the value on the corresponding input pin
        //     USART_Transmit(newdata + 50); // Transmit the new data

            // if (data[curr_line] != newdata){ // If the read value is different from the former one
            //     data[curr_line] = newdata; // Update memory
            //     // USART_Transmit(newdata); // Transmit the new data
            // }
        // }       
    
                

        // Add space between values
        //USART_Transmit(' ');

        // char pin7 = (PIND & _BV(PD7))!=0;
        // if(old_data[1] != pin7){
        //     old_data[1] = pin7;
        //     USART_Transmit('0'+pin7);
        // }        
        // Return and restart line
        // USART_Transmit('\r');
        // USART_Transmit('\n'); 
    }
}