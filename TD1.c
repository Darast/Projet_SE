#include <avr/io.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


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


// Sur 5 - 8 bits 
void USART_Transmit( unsigned char data )
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


int read_line(char* line){
    int index = 0;
    char current_char = USART_Receive();

    while(current_char != '\r'){
        line[index] = current_char;
        current_char = USART_Receive();
        index ++;
    }

    line[index] = '\r';
    index ++;

    line[index] = '\n';
    index ++;
    
    return index;
}

void send_line(char* line, int nb){
    int i = 0;
    while(i != nb){
        USART_Transmit(line[i]);
        i++;
    }
}


// Sur 9 bits
// void USART_Transmit( unsigned char data )
// {
//     /* Wait for empty transmit buffer */
//     while ( !( UCSR0A & (1<<UDRE0)) )
//     ;
//     /* Put data into buffer, sends the data */
//     UDR0 = data;
// }

// unsigned char USART_Receive( void )
// {
//     /* Wait for data to be received */
//     while ( !(UCSR0A & (1<<RXC0)) )
//     ;
//     /* Get and return received data from buffer */
//     return UDR0;
// }



int main() {
    DDRB |= _BV(PB5);
    PORTB |= _BV(PB5);

    USART_Init(MYUBRR);

    char* line;
    while(1){
        int nb_rcv = read_line(line);
        send_line(line, nb_rcv);        
    }
}



