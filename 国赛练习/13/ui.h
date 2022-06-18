#ifndef __UI_H_
#define __UI_H_

#include "main.h"
#include "display.h"

//-------------------------------------------------------
// 以下为显示功能区变量，位于UI.C内部
extern
bit UIFreqDisplay,  //频率单位
    UIDistDisplay;  //距离单位

extern
u8  UICurrentMenu,  //当前菜单
    UICurrentArg,   //当前显示的设置项目
    UIDispHumidity; //显示的湿度

extern
u16 UIDispDistance, //显示的距离
    UIDispFrequency;//显示的频率

//-------------------------------------------------------
// 以下为UI.C托管的逻辑功能区变量，位于UI.C内部
extern
u8 opArgHumidity,    //编辑中的湿度参数(%，10-60)
   opArgDistance;    //编辑中的距离参数(CM，10-120)

extern
u16 opArgFrequency;  //编辑中的频率参数(Hz，1000-12000)

//-------------------------------------------------------
// 以下为逻辑功能区变量，位于UI.C外部
extern
u8 ArgHumidity,    //湿度参数(%，10-60)
   ArgDistance;    //距离参数(CM，10-120)

extern
u16 ArgFrequency;  //频率参数(Hz，1000-12000) 


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