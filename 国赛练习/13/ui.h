#ifndef __UI_H_
#define __UI_H_

#include "main.h"
#include "display.h"

//-------------------------------------------------------
// ����Ϊ��ʾ������������λ��UI.C�ڲ�
extern
bit UIFreqDisplay,  //Ƶ�ʵ�λ
    UIDistDisplay;  //���뵥λ

extern
u8  UICurrentMenu,  //��ǰ�˵�
    UICurrentArg,   //��ǰ��ʾ��������Ŀ
    UIDispHumidity; //��ʾ��ʪ��

extern
u16 UIDispDistance, //��ʾ�ľ���
    UIDispFrequency;//��ʾ��Ƶ��

//-------------------------------------------------------
// ����ΪUI.C�йܵ��߼�������������λ��UI.C�ڲ�
extern
u8 opArgHumidity,    //�༭�е�ʪ�Ȳ���(%��10-60)
   opArgDistance;    //�༭�еľ������(CM��10-120)

extern
u16 opArgFrequency;  //�༭�е�Ƶ�ʲ���(Hz��1000-12000)

//-------------------------------------------------------
// ����Ϊ�߼�������������λ��UI.C�ⲿ
extern
u8 ArgHumidity,    //ʪ�Ȳ���(%��10-60)
   ArgDistance;    //�������(CM��10-120)

extern
u16 ArgFrequency;  //Ƶ�ʲ���(Hz��1000-12000) 


//-------------------------------------------------------
enum {
    MENU_FREQ = 0,
    MENU_HUMD,
    MENU_DIST,
    MENU_ARGU
};

enum {
    FREQ_HZ = 0,
    FREQ_KHZ
};

enum {
    DIST_CM = 0,
    DIST_M
};

void UI_Update();

#endif