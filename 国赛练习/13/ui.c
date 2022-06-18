#include "ui.h"

bit UIFreqDisplay = FREQ_HZ,  //Ƶ�ʵ�λ
    UIDistDisplay = DIST_CM;  //���뵥λ

u8 UICurrentMenu = MENU_FREQ,  //��ǰ�˵�
   UICurrentArg = 1,   //��ǰ��ʾ��������Ŀ
   UIDispHumidity = 11; //��ʾ��ʪ��

u16 UIDispDistance = 114, //��ʾ�ľ���
    UIDispFrequency = 14514;//��ʾ��Ƶ��

u8 opArgHumidity,    //ʪ�Ȳ���(%��10-60)
opArgDistance;    //�������(CM��10-120)

u16 opArgFrequency;  //Ƶ�ʲ���(Hz��1000-12000)

void UI_Update() {
    static u8 lastMenu = 0xff;

    if (lastMenu != UICurrentMenu) {
        Display_Clear();
        if (lastMenu == MENU_ARGU) {
            //�����ò˵��˳�����Ч����
            ArgFrequency = opArgFrequency;
            ArgHumidity = opArgHumidity;
            ArgDistance = opArgDistance;
        }
        lastMenu = UICurrentMenu;
        if (UICurrentMenu == MENU_ARGU) {
            //�������ò˵�����������
            opArgFrequency = ArgFrequency;
            opArgHumidity = ArgHumidity;
            opArgDistance = ArgDistance;
            UICurrentArg = 1;
        }
    }
    switch (UICurrentMenu) {
        case MENU_FREQ:
            DispBuf[0] = CHAR_F;

            if (UIFreqDisplay == FREQ_KHZ) {
                if (UIDispFrequency >= 100000)
                    DispBuf[4] = UIDispFrequency / 100000;
                else
                    DispBuf[4] = CHAR_NUL;

                if (UIDispFrequency >= 10000)
                    DispBuf[5] = UIDispFrequency % 100000 / 10000;
                else
                    DispBuf[5] = CHAR_NUL;

                DispBuf[6] = (UIDispFrequency % 10000 / 1000) + 0x10;
                DispBuf[7] = UIDispFrequency % 1000 / 100;
            } else {
                if (UIDispFrequency >= 100000)
                    DispBuf[2] = UIDispFrequency / 100000;
                else
                    DispBuf[2] = CHAR_NUL;

                if (UIDispFrequency >= 10000)
                    DispBuf[3] = UIDispFrequency % 100000 / 10000;
                else
                    DispBuf[3] = CHAR_NUL;

                if (UIDispFrequency >= 1000)
                    DispBuf[4] = UIDispFrequency % 10000 / 1000;
                else
                    DispBuf[4] = CHAR_NUL;

                if (UIDispFrequency >= 100)
                    DispBuf[5] = UIDispFrequency % 1000 / 100;
                else
                    DispBuf[5] = CHAR_NUL;

                if (UIDispFrequency >= 10)
                    DispBuf[6] = UIDispFrequency % 100 / 10;
                else
                    DispBuf[6] = CHAR_NUL;

                DispBuf[7] = UIDispFrequency % 10;
            }
            break;

        case MENU_HUMD:
            DispBuf[0] = CHAR_H;
            if(UIDispHumidity >= 99)
                UIDispHumidity = 99;
            DispBuf[6] = UIDispHumidity / 10;
            DispBuf[7] = UIDispHumidity % 10;
            break;

        case MENU_DIST:
            DispBuf[0] = CHAR_A;

            if (UIDistDisplay == DIST_M) {
                DispBuf[5] = UIDispDistance / 100 + 0x10;
                DispBuf[6] = UIDispDistance % 100 / 10;
                DispBuf[7] = UIDispDistance % 10;
            } else {
                if (UIDispDistance >= 100)
                    DispBuf[5] = UIDispDistance % 1000 / 100;
                else
                    DispBuf[5] = CHAR_NUL;

                if (UIDispDistance >= 10)
                    DispBuf[6] = UIDispDistance % 100 / 10;
                else
                    DispBuf[6] = CHAR_NUL;

                DispBuf[7] = UIDispDistance % 10;
            }
            break;

        case MENU_ARGU:
            DispBuf[0] = CHAR_P;
            DispBuf[1] = UICurrentArg;
            switch (UICurrentArg) {
                case 1:
                    if (opArgFrequency >= 10000)
                        DispBuf[5] = opArgFrequency % 100000 / 10000;
                    else
                        DispBuf[5] = CHAR_NUL;
                    DispBuf[6] = (opArgFrequency % 10000 / 1000) + 0x10;
                    DispBuf[7] = opArgFrequency % 1000 / 100;
                    break;

                case 2:
                    DispBuf[5] = CHAR_NUL;
                    DispBuf[6] = opArgHumidity / 10;
                    DispBuf[7] = opArgHumidity % 10;
                    break;

                case 3:     
                    DispBuf[5] = CHAR_NUL;
                    DispBuf[6] = opArgDistance / 100 + 0x10;
                    DispBuf[7] = opArgDistance % 100 / 10;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}
