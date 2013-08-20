

#include "DHT11.h"

#define _XTAL_FREQ  8000000

extern BYTE TOUT = 0, CheckSum;


void initDHT11(){

    T2CON = 0;
    TMR2IF =0;
    TMR2IE = 1;
    TMR2=0;
}

//***************************************************************

void StartSignal() {

    DATA_DIR = 0;
    DATA = 0;
    __delay_ms(25);
    DATA = 1;
    __delay_us(30);
    DATA_DIR = 1;


}

//***********************************************************************

BYTE CheckResponse() {


    TOUT = 0;
    TMR2 = 0;
    TMR2ON = 1;
    while (!DATA && !TOUT);
    if (TOUT) return 0;
    else {
        TMR2 = 0;
        while (DATA && !TOUT);
        if (TOUT) return 0;
        else {
            TMR2ON = 0;
            return 1;
        }

    }

}
//**************************************************************************

BYTE readByte(){

    BYTE num = 0, i;
    DATA_DIR = 1;


    for (i=0; i<8; i++){

        while(!DATA);
        TMR2 = 0;
        TMR2ON = 1;
        while(DATA);
        TMR2ON = 0;
        if(TMR2 > 80) num |= 1<<(7-i);


    }

    return num;

}