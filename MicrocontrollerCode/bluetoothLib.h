#ifndef BLUETOOTHLIB_H
#define	BLUETOOTHLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

enum Parity{
    NONE,
    EVEN,
    ODD
}; 

void addUARTReceiveEventHandler(void* cf);
void setBaud(char baud);
void setName(char* newName);
void setParity(enum Parity parity);
void setPin(char* newPin);
void bluetoothSetup(void);

//THESE DON'T BELONG IN A BLUETOOTH LIBRARY
//void uartInit(void);
//void displayMessage();
//void readMessage(int line);
//void storeChar(char nextChar);

#ifdef	__cplusplus
}
#endif

#endif	/* BLUETOOTHLIB_H */

