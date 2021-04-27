#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "LCD.h"
#include "math.h"
#include "LCD4Bit.h"
#include <stdio.h>

#define TC74 0x48
#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define init 0x00

struct LCD theLCD;
//timer interrupt stuff
uint16_t ms=0;
uint16_t s=0;
double time;
//temp stuff
uint8_t *temp;
int i;
// heart stuff
uint8_t BPM;
uint8_t HB;
double time_i;
double time_f;
//functions
void compute();
void timer_callback();
void steup_LCD(char x [10],char y [10]);
void LCD_printplz(char x [10],char y [16]);
void BPM_ctr();


void main(void){
    SYSTEM_Initialize();
    INTERRUPT_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    I2C2_Initialize();
    TMR2_Initialize();
    IOCAF7_SetInterruptHandler(BPM_ctr);
    TMR2_SetInterruptHandler(timer_callback);  
    
    __delay_ms(200);
    steup_LCD("Heart Beat","Monitor");
    TMR2_Start();
    I2C2_WriteNBytes( TC74, CONFIG_REG, init);
    while(1){      
        *temp=I2C2_Read1ByteRegister(TC74, TEMP_REG);
        char str1[16];
        char str2[16];
//        compute();
        sprintf(str1, "temp= %dC", *temp);
        sprintf(str2, "HB=%d BPM=%d",HB,BPM);
        LCD_printplz(str1,str2);
    }
}

void BPM_ctr()
{
    if(IO_RA7_GetValue()==1)
    {
        time_i=time;
        HB++;
    }
    else if (IO_RA7_GetValue()==0)
    {
        time_f=time;
    }
    BPM=60/(time_f-time_i);
}

void compute()
{
    if (temp=255){
        i=temp-256;
        temp=i;
    }
}

void timer_callback()
{
    ms=ms++;
    if(ms >= 1000){
        ms=ms-1000;
        s=s++;
        time = (ms/1000.00) + s;
        IO_RA5_Toggle();
    }
}


void steup_LCD(char x [10],char y [10])
{
    // if error, then use the following code
    PORTD = 0;
    ANSELD = 0;
    TRISD = 0;
    LCD_initParallel(&theLCD, LCD_4BITMODE, &LATD, &LATD, 7, 6);
    LCD_begin(&theLCD, 16, 2, LCD_5x8DOTS);
    LCD_clear(&theLCD);
    LCD_home(&theLCD);
    LCD_setCursor(&theLCD, 0, 0);
    LCD_printString(&theLCD, x);
    LCD_setCursor(&theLCD, 0, 1);
    LCD_printString(&theLCD, y);
    __delay_ms(200);
}

void LCD_printplz(char x [10],char y [16])
{
    // if error, then use the following code
    LCD_clear(&theLCD);
    LCD_home(&theLCD);
    LCD_setCursor(&theLCD, 0, 0);
    LCD_printString(&theLCD, x);
    LCD_setCursor(&theLCD, 0, 1);
    LCD_printString(&theLCD, y);
}

