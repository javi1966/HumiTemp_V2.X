
#include "LCD.h"

#define _XTAL_FREQ  8000000
//bitmap nuevo caracter

BYTE b1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
BYTE b2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
BYTE b3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
BYTE b4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
BYTE b5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

BYTE const LCD_INIT_STRING[4] = {
    0x28, // | (lcd_type << 2), // Func set: 4-bit, 2 lines, 5x8 dots
    0xc, // Display on
    1, // Clear display
    6 // Increment cursor
};


//**********************************************************************

void LcdSendNibble(BYTE nibble) {

    LCD_DB4 = !!(nibble & 1);
    LCD_DB5 = !!(nibble & 2);
    LCD_DB6 = !!(nibble & 4);
    LCD_DB7 = !!(nibble & 8);

    __delay_ms(1);
    LCD_E = 1; // Enable
    __delay_ms(1);
    LCD_E = 0; // Disable


}

/* LcdSendByte
 *
 * Sends a 8-bit byte to the display.
 *
 * Parameters:
 *   BYTE theByte   The byte to send to the display
 *
 * Returns:
 *   nothing
 */
void LcdSendByte(BYTE theByte) {

    LcdSendNibble(theByte >> 4);
    LcdSendNibble(theByte & 0xF);

}

//*********************************************************************

void writeLCD(BYTE addr, char c) {

    /* while( busyLCD());
     PMPSetAddress( addr);       // select register
     PMPMasterWrite( c);         // initiate write sequence*/
    if (addr)
        LCD_RS = 1;
    else
        LCD_RS = 0;

    LcdSendByte(c);
}

//************************************************************************

void lcd_gotoxy(BYTE x, BYTE y) {
    BYTE address;

    if (y != 1)
        address = lcd_line_two;
    else
        address = 0;

    address += x - 1;
    cmdLCD(0x80 | address);
}

//***********************************************************************

void putsLCD(char *s) {

    while (*s) {
        switch (*s) {
            case '\n': // point to second line
                lcd_gotoxy(1, 2);
                break;
            case '\f':

                homeLCD();
                __delay_ms(2);

                break;
            case '\r': // home, point to first line

                clrLCD();
                break;
            default: // print character

                putLCD(*s);
                break;
        } //switch
        s++;
        __delay_ms(2);
    } //while

}


//***********************************************************************

void setCGRAM(BYTE addr, BYTE *bmp) {
    int j;


    setLCDG(addr * 8);

    for (j = 0; j < 8; j++)
        putLCD(bmp[j]);

    setLCDC(0x40);

}

//************************************************************************

void newBarTip(int i, int width) {

    char bar;


    setLCDG(i * 8);
    if (width > 4)
        width = 0;
    else
        width = 4 - width;

    for (bar = 0xFF; width > 0; width--)
        bar <<= 1;

    putLCD(bar);
    putLCD(bar);
    putLCD(bar);
    putLCD(bar);
    putLCD(bar);
    putLCD(bar);
    putLCD(bar);
    putLCD(bar);

    setLCDC(0x40);

}

//*************************************************************************

void drawProgressBar(int index, int imax, int size) {

    BYTE i;
    int pixelprogress;
    BYTE c;

    pixelprogress = ((index * (size * 5)) / imax);

    for (i = 0; i < size; i++) {
        if ((i * 5) + 5 > pixelprogress) {
            if (i * 5 > pixelprogress)
                c = 0;
            else
                c = pixelprogress % 5;
        } else
            c = 5;

        if (!c)break;

        putLCD(c);
    }

    for (i = 0; i < size; i++) //borra resto
        putLCD(' ');
}
//*************************************************************************

/* LcdInitialize
 *
 * Initialize the display.
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   nothing
 */
void LcdInitialize(void) {
    short i;

    TRIS_RS = 0;
    TRIS_E = 0;


    __delay_ms(30);
    LCD_RS = 0;
    LCD_E = 0;
    __delay_ms(15);
    for (i = 0; i < 3; i++) {
        LcdSendNibble(0x03);
        __delay_ms(5);
    }

    // Now, still in 8-bit mode, set the display to 4-bit mode
    LcdSendNibble(0x02);

    // We are now in 4-bit mode.
    // Do the rest of the init sequence.


    for (i = 0; i < sizeof (LCD_INIT_STRING); i++) {
        cmdLCD(LCD_INIT_STRING[i]);
        __delay_ms(5);
    }

    __delay_ms(5);
    putsLCD("\f");
    __delay_ms(5);

    LCD_RS = 1;


       setCGRAM(0, b1);
       setCGRAM(1, b2);
       setCGRAM(2, b3);
       setCGRAM(3, b4);
       setCGRAM(4, b5);


}


