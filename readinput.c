#include <avr/io.h>

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
    DDRB =  (1<<DDB5)|(1<<DDB4)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0); // Define input pins as NOT 5 to 0
    PORTB = (1<<PB7)|(1<<PB6) // Assign pull-ups to pins 6 and 7

    inputpins[0] = PB6;
    inputpins[1] = PB7;
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

int main() {
    USART_Init(MYUBRR);

    int curr_line; // Index of the current line
    unsigned char data[nlines]; // Instant memory of the former read values
    unsigned char newdata; // Data just read from one input line
    unsigned char data_block; // Data read from all input lines at once

    while(1){
        WriteLine("Reading data block", 18);
        data_block = PINB; // Read every input lines' states
        USART_Transmit(data_block);

        for (curr_line = 0; curr_line < nlines; curr_line++){
            newdata = Read_Input(data_block, curr_line); // Read the value on the corresponding input pin

            if (data[curr_line] != newdata){ // If the read value is different from the former one
                data[curr_line] = newdata; // Update memory
                USART_Transmit(newdata); // Transmit the new data
            }
        }        
    }
}