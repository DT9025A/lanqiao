#include "key.h"

bit K7, K6, K5, K4, K7LongPress, K6LongPress;
u16 K6Count = 0, K7Count = 0;

void Key_Scan() {
    static bit lK7 = 0, lK6 = 0, lK5 = 0, lK4 = 0;
    bit cK7 = 0, cK6 = 0, cK5 = 0, cK4 = 0;
    P3 |= 0x0f;

    if (P30 == 0) {
        cK7 = 1;
    }
    else if (P31 == 0) {
        cK6 = 1;
    }
    else if (P32 == 0) {
        cK5 = 1;
    }
    else if (P33 == 0) {
        cK4 = 1;
    }

    if (cK7 && !lK7) {
        K7 = 1;
    } else if(!cK7 && !lK7){
        K7 = 0;
        K7Count = 0;
        K7LongPress = 0;
    } else if(cK7 && lK7)
        K7Count++;

    if (cK6 && !lK6) {
        K6 = 1;
    } else if(!cK6 && !lK6){
        K6 = 0;
        K6Count = 0;
        K6LongPress = 0;
    } else if(cK6 && lK6)
        K6Count++;

    if (cK5 && !lK5)
        K5 = 1;
    else if(!cK5 && !lK5)
        K5 = 0;

    if (cK4 && !lK4)
        K4 = 1;
    else if(!cK4 && !lK4)
        K4 = 0;

    if (K7Count == 20) {
        K7LongPress = 1;
    }
    if (K6Count == 20) {
        K6LongPress = 1;
    }
    lK7 = cK7;
    lK6 = cK6;
    lK5 = cK5;
    lK4 = cK4;
}
