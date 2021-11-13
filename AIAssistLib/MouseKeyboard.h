#pragma once

#include <iostream>
#include <string>
#include<math.h>

#include "mouse.h"
#include "keyboard.h"

#include "AssistConfig.h"
#include "ImageDetection.h"


enum MouseKeyboardType { MKTYPE_USBDEVICE, MKTYPE_HIDDRIVER, MKTYPE_WINDOWSEVENT };

// 鼠标键盘操作封装类
// 优先级顺序：硬件USB设备（暂未实现）、HIDDriver驱动、windows事件
class MouseKeyboard
{
public:
	MouseKeyboard();
	~MouseKeyboard();

	//程序使用的模拟鼠键类型
	MouseKeyboardType m_type = MKTYPE_USBDEVICE;

	//相对坐标移动鼠标
	//定制一个3D游戏移动鼠标的函数，x1/y1为游戏中心点坐标，x2/y2为检测到的人物中心点坐标，z为三维坐标的z轴距离
	//mouseMoveSlow鼠标变慢的倍数
	void MouseMove(LONG x1, LONG y1, LONG x2, LONG y2, double z, double mouseMoveSlow);
	//鼠标单击
	void MouseLBClick();

	//判断是否已经对准目标
	bool IsInTarget(DETECTRESULTS detectResult);
	//判断是否需要自动开火
	void AutoFire(DETECTRESULTS detectResult);
	//移动鼠标到检测对象坐标中心
	void AutoMove(DETECTRESULTS detectResult);

private:
	// 唯一单实例对象
	static AssistConfig* m_AssistConfig;

	//HIDDriver驱动的模拟鼠键对象
	Mouse m_hidMouse;
	Keyboard m_hidKeyboard;
};

