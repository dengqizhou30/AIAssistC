#pragma once

#include <iostream>
#include <string>
#include<math.h>

#include "mouse.h"
#include "keyboard.h"

#include "AssistConfig.h"
#include "ImageDetection.h"


// 鼠标键盘操作封装类
class MouseKeyboard
{
public:
	MouseKeyboard();
	~MouseKeyboard();

	//判断是否已经对准目标
	bool IsInTarget(DETECTRESULTS detectResult);
	//判断是否需要自动开火
	void AutoFire(DETECTRESULTS detectResult);
	//移动鼠标到检测对象坐标中心
	void AutoMove(DETECTRESULTS detectResult);

private:
	// 唯一单实例对象
	static AssistConfig* m_AssistConfig;

	Mouse m_mouse;
	Keyboard m_keyboard;
};

