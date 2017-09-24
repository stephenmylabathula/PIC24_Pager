#include <xc.h>
#include "Delays.h"

void lcd_cmd(char command) {
    I2C2CONbits.SEN = 1; //Initiate SEND
    while (I2C2CONbits.SEN); //Wait for SEND ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = 0b01111100; //Send Slave Address
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = 0b00000000; //Send Control Byte for Slave Memory Address
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = command; //Send Data Byte
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2CONbits.PEN = 1; //Initiate STOP
    while (I2C2CONbits.PEN); //Wait for STOP ACK 
    IFS3bits.MI2C2IF = 0; //Reset ACK
}
void lcd_init(void) {
    int i;
    for (i = 0; i < 50; i++)
        wait_1_ms(); //Wait for VDD to Stabalize ~50ms

    lcd_cmd(0b00111000); // function set, normal instruction mode
    wait_50us();
    lcd_cmd(0b00111001); // function set, extended instruction mode
    wait_50us();
    lcd_cmd(0b00010100); // interval osc
    wait_50us();
    lcd_cmd(0b01110000); // contrast C3-C0
    wait_50us();
    lcd_cmd(0b01010110); // C5-C4, Ion, Bon
    wait_50us();
    lcd_cmd(0b01101100); // follower control

    for (i = 0; i < 250; i++)
        wait_1_ms(); //Wait for VDD to Stabalize ~250ms

    lcd_cmd(0b00111000); // function set, normal instruction mode
    wait_50us();
    lcd_cmd(0b00001100); // Display On
    wait_50us();
    lcd_cmd(0b00000001); // Clear Display

    for (i = 0; i < 1000; i++)
        wait_1_ms(); //Wait for Initialization to End ~1s

}
void lcd_setCursor(char x, char y) {
    char encoded = 0x40 * y + x;
    char maksed = 0b10000000 | encoded;
    //lcd_cmd(0b11000101);
    lcd_cmd(maksed);
}
void lcd_printChar(char myChar) {
    I2C2CONbits.SEN = 1; //Initiate SEND
    while (I2C2CONbits.SEN); //Wait for SEND ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = 0b01111100; //Send Slave Address
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = 0b01000000; //Send Control Byte for DDRAM Write
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2TRN = myChar; //Send Data Byte
    while (!IFS3bits.MI2C2IF); //Wait for Slave ACK
    IFS3bits.MI2C2IF = 0; //Reset ACK

    I2C2CONbits.PEN = 1; //Initiate STOP
    while (I2C2CONbits.PEN); //Wait for STOP ACK 
    IFS3bits.MI2C2IF = 0; //Reset ACK
}
void lcd_printStr(const char* s, int line) {
    int pos = 0;
    lcd_setCursor(0, line);

    while (s[pos] != '\0') {
        lcd_printChar(s[pos++]);
    }
}
void lcd_animate(const char* s, int length) {
    int i = 0;
    int j;
    int pos = 7;

    while (1) {
        lcd_setCursor(pos--, 0);
        while (s[i] != '\0') {
            lcd_printChar(s[i++]);
            if (pos + i == length + 1) {
                lcd_setCursor(0, 0);
            }
        }
        i = 0;
        if (pos < 0)
            pos = length;
        for (j = 0; j < 500; j++)
            wait_1_ms();
        lcd_cmd(0b00000001);
        wait_1_ms();
    }
}
void lcd_setup(void) {
    I2C2CONbits.I2CEN = 0; //Disable I2C-2
    I2C2BRG = 157; //Set I2C-2 Frequency to 100kHz
    IFS3bits.MI2C2IF = 0; //Clear I2C-2 Interrupt Flag
    I2C2CONbits.I2CEN = 1; //Enable I2C-2
    lcd_init();
}