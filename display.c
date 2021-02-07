#include "display.h"

uint8_t tabla(uint8_t valor) {
    uint8_t hex;
    switch (valor) {
        case 0x0:
            hex = 0b00111111;
            break;
        case 0x1:
            hex = 0b00000110;
            break;
        case 0x2:
            hex = 0b01011011;
            break;
        case 0x3:
            hex = 0b01001111;
            break;
        case 0x4:
            hex = 0b01100110;
            break;
        case 0x5:
            hex = 0b01101101;
            break;
        case 0x6:
            hex = 0b01111101;
            break;
        case 0x7:
            hex = 0b00000111;
            break;
        case 0x8:
            hex = 0b01111111;
            break;
        case 0x9:
            hex = 0b01101111;
            break;
        case 0xA:
            hex = 0b01110111;
            break;
        case 0xB:
            hex = 0b01111100;
            break;
        case 0xC:
            hex = 0b00111001;
            break;
        case 0xD:
            hex = 0b01011110;
            break;
        case 0xE:
            hex = 0b01111001;
            break;
        case 0xF:
            hex = 0b01110001;
            break;
    }
    return hex;
}
