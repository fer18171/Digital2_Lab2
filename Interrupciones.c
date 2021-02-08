/*
 * File:   Interrupciones.c
 * Author: santi
 *
 * Created on February 1, 2021, 11:27 AM
 */


#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
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
uint8_t ADC_value; //Variable que toma el valor del ADC luego de la conversion
uint8_t ADC_finish; // Bandera que me sirve para saber si ya se hizo la conversion
uint8_t Multiplex; //Bandera que me sirve para hacer el multiplexado
//******************************************************************************
//Prototipos de Funciones
//******************************************************************************
void setup(void);

//******************************************************************************
//Loop Principal
//******************************************************************************

void main(void) {
    setup();
    ADC_setup(3, 2, 0, 0); //Aqui llamo a mi funcion de la libreria del ADC
    ADCON0bits.GO = 1; //Empiezo a hacer la primera conversion antes de entrar al loop
    while (1) {
        if (ADC_finish == 1) { //Reviso bandera pa ver si ya puedo empezar la siguiente conversion
            ADC_finish = 0;
            ADCON0bits.GO = 1;
        }
        if (Multiplex == 1) { //Primera iteracion con la bandera multiplex para mostrar los LSB del ADC_value
            PORTAbits.RA1 = 0;
            PORTD = tabla((ADC_value & 0b00001111));
            PORTAbits.RA0 = 1;
        } else if (Multiplex == 0) {//Segunda iteracion con la bandera multiplex para mostrar los MSB del ADC_value
            PORTAbits.RA0 = 0;
            PORTD = tabla(((ADC_value >> 4) & 0b00001111));
            /*Se hace un shift de 4 bits a la derecha para colocar los MSB del 
             * ADC_value en la posicion de los LSB y asi poder usarlos en la 
             * funcion de multiplexado de la libreria*/
            PORTAbits.RA1 = 1;
        }
        if (ADC_value > PORTC) { //Se compara el valor del ADC con el registro PORTC que es el del contador
            PORTAbits.RA3 = 1; //Se enciende la alarma si se cumple
        } else {
            PORTAbits.RA3 = 0; //De lo contrario se apaga la alarma
        }
    }
}



//******************************************************************************
//Setup
//******************************************************************************

void setup(void) {
    ANSEL = 0; //Canales diitales
    ANSELH = 0;
    ANSELbits.ANS2 = 1; //Solo el bit RA2 como analogico
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
    INTCONbits.GIE = 1; //Activamos las interrupciones de todo
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    IOCBbits.IOCB0 = 1; //Asignamos el interrupt-on-change a los bits RB0 y RB1
    IOCBbits.IOCB1 = 1;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PSA = 0; // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 0; // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    ADC_finish = 0; //Iniciamos nuestras variables y banderas en 0
    Multiplex = 0;
    TMR0 = 245; //Se inicia el timer0 en 245 para tener las interrupciones en el tiempo requerido
}

void __interrupt() oli(void) {
    if (INTCONbits.RBIF == 1) { //Interrupcion del interrupt-on-change para los botones
        if (PORTBbits.RB0 == 1) {
            PORTC++;
        } else if (PORTBbits.RB1 == 1) {
            PORTC--;
        }
        INTCONbits.RBIF = 0;
    }
    if (INTCONbits.T0IF == 1) {//Interrupcion del TMR0 para iteraciones del multiplexado
        ADC_finish = 1; //Enciendo la bandera del ADC para indicar que ya paso el aquisition time para hacer la siguiente conversion
        INTCONbits.T0IF = 0;
        TMR0 = 245;
        /*Literal si la bandera esta en 0 la pongo en 1, si esta en 1 la pongo en 0
         * y asi cada vez que este en 0 se enciende el primer display y si esta en
         * 1, se enciende el segundo display */
        if (Multiplex == 1) {
            Multiplex = 0;
        } else if (Multiplex == 0) {
            Multiplex = 1;
        }

    }
    if (PIR1bits.ADIF == 1) { //Interrupcion del ADC
        PIR1bits.ADIF = 0;
        ADC_value = ADRESH; //Se cargan los 8 MSB del resultado al registro
        INTCONbits.T0IF = 0; //Apago la bandera
        TMR0 = 245; //Cargo de nuevo el TMR0 ya que lo utilize como apoyo para el aquisition time
    }

}
