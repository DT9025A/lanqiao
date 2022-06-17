#include "key.h"

void Delay10ms() {	//@12.000MHz
    unsigned char i, j;

    i = 117;
    j = 184;
    do {
        while (--j);
    } while (--i);
}

u8 Line_Scan() {
//    if (P30 == 0)
//        return 1;
//    if (P31 == 0)
//        return 2;
    if (P32 == 0)
        return 3;
    if (P33 == 0)
        return 4;
    return 0;
}

u8 Key_Scan() {
    static u8 last_key = 0;
    u8 current_key = 0;
    P3 = 0xFF;
    P42 = 1;
    P44 = 1;

    P34 = 0;
    if (Line_Scan()) {
        Delay10ms();
        switch (Line_Scan()) {
            case 1:
                current_key = 19;
            case 2:
                current_key = 18;
            case 3:
                current_key = 17;
            case 4:
                current_key = 16;
        }
    }

    P34 = 1;
    P35 = 0;
    if (Line_Scan()) {
        Delay10ms();
        switch (Line_Scan()) {
            case 1:
                current_key = 15;
                break;
            case 2:
                current_key = 14;
                break;
            case 3:
                current_key = 13;
                break;
            case 4:
                current_key = 12;
                break;
        }
    }

    P35 = 1;
    P42 = 0;
    if (Line_Scan()) {
        Delay10ms();
        switch (Line_Scan()) {
            case 1:
                current_key = 11; 
                break;
            case 2:
                current_key = 10;
                break;
            case 3:
                current_key = 9;
                break;
            case 4:
                current_key = 8;
                break;
        }
    }

    P42 = 1;
    P44 = 0;
    if (Line_Scan()) {
        Delay10ms();
        switch (Line_Scan()) {
            case 1:
                current_key = 7;
                break;
            case 2:
                current_key = 6;  
                break;
            case 3:
                current_key = 5; 
                break;
            case 4:
                current_key = 4;  
                break;
        }
    }

    if (current_key != last_key) {
        last_key = current_key;
        return current_key;
    }
    return 0;
}