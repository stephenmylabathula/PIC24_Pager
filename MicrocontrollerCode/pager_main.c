#include <p24Fxxxx.h>
#include <xc.h>
#include "LCD.h"
#include "bluetoothLib.h"
#include "Delays.h"
//#include "buzzerLib.h"

// PIC24FJ64GA002 Configuration Bit Settings
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)

// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled. // Primary Oscillator refers to an external osc connected to the OSC1 and OSC2 pins)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

//Buffer defines
#define LINE_LENGTH             8
#define NUMBER_OF_LINES         2
#define BUFF_SIZE               32

volatile char msgTray[BUFF_SIZE][LINE_LENGTH];
volatile int readIdx = 0, writeIdx = 0, charIdx = 0;
int flag = 1;


void __attribute__((__interrupt__,__auto_psv__)) _INT0Interrupt(void) {
    readIdx+=NUMBER_OF_LINES;                            //Increase readIdx to the next even line
    if(readIdx>writeIdx+1){
        readIdx = writeIdx + (writeIdx%NUMBER_OF_LINES); //If readIdx exceeds writeIdx by more than one line, set readIdx to the next even line on/after writeIdx
    }
    readIdx&=(BUFF_SIZE-1);                              //Maintain circular buffer
    _INT0IF = 0;
}

void squeak(){    //Buzz active buzzer on RB6 for 100ms
    LATBbits.LATB6 = 1;
    int i;
    for(i = 0; i < 100; i++)
        wait_1_ms();
    LATBbits.LATB6 = 0;
}

char constr1[LINE_LENGTH];
char constr2[LINE_LENGTH];
void readMessage(){ 
    int i=0;
    for(i=0; i < LINE_LENGTH; i++)
    {
        if(msgTray[readIdx][i] != '\0')
        {
            constr1[i]=msgTray[readIdx][i];
        }
        else {
            break;
        }
    }
    for(i=0; i < LINE_LENGTH; i++)
    {
        if(msgTray[readIdx+1][i] != '\0')
        {
            constr2[i]=msgTray[readIdx+1][i];
        }
        else {
            break;
        }
    }
    
    lcd_printStr(constr1, 0);
    lcd_printStr(constr2, 1);
    
    for(i = 0; i < LINE_LENGTH; i++){
        constr1[i] = '\0';
        constr2[i] = '\0';
    }
        
    for(i = 0; i < 500; i++)
        wait_1_ms();
}
void displayMessage(void){
    lcd_cmd(0b00000001); // Clear Display  
    wait_1_ms();
    wait_1_ms();
    wait_1_ms();
    //int i;
    //for(i=0;i<NUMBER_OF_LINES;i++){
        readMessage();
    //}
}
void writeMessage(char input){//not yet tested
  //writeIdx = 0, charIdx = 0;//  
    if(((writeIdx+1)&(BUFF_SIZE-1)) != readIdx){//ignore messages on overflow
        if(!input){ //If end of message
            msgTray[writeIdx][charIdx] = '\0';
            writeIdx += NUMBER_OF_LINES - writeIdx % NUMBER_OF_LINES; //go to new message
            writeIdx &= BUFF_SIZE - 1;
            charIdx = 0;
            squeak();
        }
        else if(input == '\n'){
            msgTray[writeIdx][charIdx] = '\0';
            writeIdx++;
            writeIdx &= (BUFF_SIZE-1);
            charIdx = 0;
        }
        else{ //normal case*/
            msgTray[writeIdx][charIdx] = input;
            charIdx++;
            if(charIdx >= LINE_LENGTH){
              charIdx = 0;
              writeIdx++;
              writeIdx &= (BUFF_SIZE-1);
            }
        }     
    }
}

char buff[8];
int buffIdx = 0;
void bluetoothHandler(char c){
    writeMessage(c);
}
void buttonSetup(void){
    TRISBbits.TRISB7 = 1;   //RB7 = Input
    CNPU2bits.CN23PUE = 1;   //Pull-Up on RB7
    
    //Setup External Interrupt
    _INT0IE = 1; 
    _INT0EP = 1;    //Detect falling edge
    _INT0IF = 0;
}

void setup(void) {
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff; // First assume all digital
    
    //buzzerInit();
    TRISBbits.TRISB6 = 0;
    LATBbits.LATB6 = 0;
    
    buttonSetup();
    bluetoothSetup();
    addUARTReceiveEventHandler(&bluetoothHandler);
    lcd_setup();
}
int main(void) {
    setup();
    
    //Allow things to settle
    int i;
    for (i = 0; i < 1000; i++) 
        wait_1_ms();
    //msgTray[0][0] = '8';
    //msgTray[1][0] = '9';
    //setName("stephen");
    
    for (i = 0; i < 500; i++) 
            wait_1_ms();
    readIdx = 0;
    while (1){
        for (i = 0; i < 500; i++) 
            wait_1_ms();
        displayMessage();
    }
    return 0;
}