/* 
 * File:   main.c
 * Author: Javier
 *
 * Created on 8 de julio de 2013, 18:21
 */
//#include <pic16f819.h>
#include <xc.h>
#include "LCD.h"
#include "DHT11.h"

#pragma config FOSC=INTOSCIO,BOREN=OFF,CPD=OFF,MCLRE=OFF,LVP=OFF,CCPMX=RB2,\
               WDTE=OFF,CP=OFF

#define _XTAL_FREQ  8000000

#define	AD_CONVERT()	{ ADCON0bits.GO_nDONE = 1; while(ADCON0bits.GO_nDONE); }

#define BAT_LIMIT   1800    //aprox 1.8 Vol,5.8 v

#define LED         RA2
#define TRIS_LED    TRISA2
#define BAT_VOLT    RA0
#define TRIS_BAT    TRISA0

extern BYTE TOUT;
BOOL bFlagMedida = FALSE;
WORD ad_res;
BYTE temp;

int main(int argc, char** argv) {

    BYTE check = 0;
    BYTE T_Byte1, T_Byte2, RH_Byte1, RH_Byte2;
    BYTE CheckSum;

    char message1[] = "Temp = 00.0 C   ";
    char message2[] = "RH   = 00.0 %   ";

    TRIS_LED = 0;

    TRISB = 0x00;

    OSCCON = 0b01110000; // 8Mhz

    LED = 1; //led off


    //Tension de bateria

    ADCON0bits.ADON = 0; //AD on*/
    ADCON0bits.CHS = 0b0000; //AN0
    TRIS_BAT = 1;
    ADCON1bits.PCFG = 0b1110; //todo digital,excepto RA0
    ADCON1bits.ADFM = 1;
    ADCON0bits.ADON = 1; //AD on*/

    LcdInitialize();


    putsLCD("\f    HumiTemp   \n");
    putsLCD("Javier Espinosa");

    __delay_ms(3000);



    initDHT11();

    T1CON = 0b00110001; //div 8,0.5us *
    TMR1IF = 0;
    TMR1IE = 1;
    TMR1 = 0;


    GIE = 1;
    PEIE = 1;

    while (TRUE) {


        StartSignal();

        check = CheckResponse();

        if (!check) {
            putsLCD("\fSin Respuesta\n");
            putsLCD("desde DHT11    ");
        } else {
            RH_Byte1 = readByte();
            RH_Byte2 = readByte();
            T_Byte1 = readByte();
            T_Byte2 = readByte();
            CheckSum = readByte();

            if (CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)) {
                message1[7] = T_Byte1 / 10 + 48;
                message1[8] = T_Byte1 % 10 + 48;
                message1[10] = T_Byte2 / 10 + 48;
                message2[7] = RH_Byte1 / 10 + 48;
                message2[8] = RH_Byte1 % 10 + 48;
                message2[10] = RH_Byte2 / 10 + 48;
                message1[11] = 223;

                // clrLCD();
                lcd_gotoxy(1, 1);
                putsLCD(message1);
                lcd_gotoxy(1, 2);
                putsLCD(message2);

            } else {
                lcd_gotoxy(1, 1);
                putsLCD(" Error Checksum!");
                lcd_gotoxy(1, 2);
                putsLCD(".... Vuelve ....");
            }



        }

        if (bFlagMedida) {

            ad_res = 0;
            for (temp = 0; temp < 4; temp++) {
                AD_CONVERT();
                ad_res += ADRESL;
                ad_res += (ADRESH << 8);
            }

            if (ad_res < BAT_LIMIT)
                LED = 0;
            else
                LED = 1;

            bFlagMedida = FALSE;
        }
        /*  lcd_gotoxy(1, 1);
          drawProgressBar(valor++, 100, 12);
          lcd_gotoxy(1, 2);
          drawProgressBar(valor++, 100, 12);*/

        __delay_ms(1500);
    }

    return (1);
}

//*********************************************************************

void interrupt isr(void) {


    static BYTE num = 0;

    if (TMR2IF) {
        TOUT = 1;
        TMR2ON = 0;
        TMR2IF = 0;

    } else if (TMR1IF) {

        if (++num > 20) {    //aprox. 5 seg.
            // LED ^= 1;
            bFlagMedida = TRUE;
            num = 0;
        }
        TMR1IF = 0;
    }

}

