#include <xc.h>
#include "Delays.h"
#include "bluetoothLib.h"

#define MAX_NAME_LEN 20
#define PIN_LEN 4

//UART EVENT HANDLER CALLBACK MEMBERS
typedef void (*CallbackFunction)(char);
CallbackFunction function;
short eventHandlerSet = 0;
void addUARTReceiveEventHandler(void* cf){
    function = (CallbackFunction) cf;
    eventHandlerSet = 1;
}

/* UART INTERRUPT MEMBERS */
char receivedChar;
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void) {
    _U1RXIF = 0;
    receivedChar = U1RXREG;
    
    if(eventHandlerSet){
        function(receivedChar);
    }
}

/* UART PARAMETERS SET FUNCTIONS */
void setName(char* newName) {
    char mes[MAX_NAME_LEN + 7] = "AT+NAME";

    int c;
    for (c = 0; c < MAX_NAME_LEN && newName[c]; c++) {
        mes[c + 7] = newName[c];
    }
    
    for (c = 0; c < MAX_NAME_LEN + 7 && mes[c]; c++) {
        U1TXREG = mes[c];
        while (!_U1TXIF);
        _U1TXIF = 0;
    }
    
    //Wait 1 Second
    for(c = 0; c < 1000; c++)
        wait_1_ms();
    
    return;
}
void setPin(char* newPin){
    //Queue message
    char mes[PIN_LEN+6] = "AT+PIN";
    int c;
    for(c = 0; c < PIN_LEN; c++){
        mes[c+6] = newPin[c];
    }
    //Send Message
    for(c = 0; c < PIN_LEN + 6 && mes[c]; c++){
        while(!_U1TXIF);
        _U1TXIF = 0;
        U1TXREG = mes[c];
    }
    //Wait 1 Second
    for(c = 0; c < 1000; c++)
        wait_1_ms();
    
    return;
}
void setBaud(char baud){
    //Queue message
    char mes[8] = "AT+BAUD4";
    int c;
    mes[7] = baud;
    //Send Message
    for(c = 0; c < 8 && mes[c]; c++){
        while(!_U1TXIF);
        _U1TXIF = 0;
        U1TXREG = mes[c];
    }
    
    //Wait 1 Second
    for(c = 0; c < 1000; c++)
        wait_1_ms();
    return;
}
void setParity(enum Parity parity){
    //Queue message
    char mes[5] = "AT+PN"; //by default no parity
    int c;
    if(parity ==  NONE)
        return;
    else if(parity == EVEN)
        mes[4] = 'E';
    else if(parity == ODD)
        mes[4] = 'O';
    //Send Message
    for(c = 0; c < 5 && mes[c]; c++){
        while(!_U1TXIF);
        _U1TXIF = 0;
        U1TXREG = mes[c];
    }
    
    //Wait 1 Second
    for(c = 0; c < 1000; c++)
        wait_1_ms();
    return;
}

/* UART SETUP METHOD */
void bluetoothSetup(void){
    //Setup UART1 Pins
    _TRISB5 = 0;  // U1TX output
    _TRISB4 = 1; // U1RX input
    
    //PPS UART1 Pins
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    _RP5R = 0x0003; //RB5->UART1:U1TX; See Table 10-3 on P109 of the datasheet
    RPINR18bits.U1RXR = 4;
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS 
    
    //Setup UART1 (need to parameterize these)
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1BRG = 416; // 9600 baud
    U1MODEbits.UEN = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXISEL = 0x00; //Interrupt when any character received.
    
    //Setup UART Interrupts
    _U1RXIE = 1;
    _U1RXIF = 0;
    _U1TXIF = 0;
}