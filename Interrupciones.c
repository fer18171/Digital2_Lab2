/*
 * File:   Interrupciones.c
 * Author: santi
 *
 * Created on February 1, 2021, 11:27 AM
 */


#include <xc.h>
#include <stdint.h>
#include "ADC.h"
#include "display.h"
//******************************************************************************
//Configuration Bits
//******************************************************************************
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
//Variables
//******************************************************************************
uint8_t ADC_value;
uint8_t ADC_finish;
//******************************************************************************
//Prototipos de Funciones
//******************************************************************************
void setup(void);

//******************************************************************************
//Loop Principal
//******************************************************************************

void main(void) {
    setup();
    ADC_setup(3, 2, 0, 0);
    ADCON0bits.GO = 1;
    while (1) {
        if (ADC_finish == 1) {
            ADC_finish = 0;
            ADCON0bits.GO = 1;
        }
        PORTD=tabla((ADC_value & 0b00001111));
        PORTC=ADC_value;
    }
}


//******************************************************************************
//Setup
//******************************************************************************

void setup(void) {
    ANSEL = 0;
    ANSELH = 0;
    ANSELbits.ANS2 = 1;
    TRISD = 0;
    TRISC = 0;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 0;
    PORTD = 0;
    PORTC = 0;
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA3 = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PSA = 0; // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 0; // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    ADC_finish = 0;
}

void __interrupt() oli(void) {
    if (INTCONbits.RBIF == 1) {
        if (PORTBbits.RB0 == 1) {
            PORTC++;
        } else if (PORTBbits.RB1 == 1) {
            PORTC--;
        }
        INTCONbits.RBIF = 0;
    }
    if (INTCONbits.T0IF == 1) {
        ADC_finish = 1;
        INTCONbits.T0IF = 0;
        TMR0 = 236;
    }
    if (PIR1bits.ADIF == 1) {
        PIR1bits.ADIF = 0;
        ADC_value = ADRESH;
        INTCONbits.T0IF = 0;
        TMR0 = 236;
    }

}
