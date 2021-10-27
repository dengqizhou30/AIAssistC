#include "pch.h"
#include "MouseKeyboard.h"


//初始化静态成员变量
AssistConfig* MouseKeyboard::m_AssistConfig = AssistConfig::GetInstance();


MouseKeyboard::MouseKeyboard() {

    try {
        m_mouse.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("鼠标设备初始化失败: ") + e.what() << std::endl;
    }

    try {
        m_keyboard.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("键盘设备初始化失败: ") + e.what() << std::endl;
    }

	return;
}

MouseKeyboard::~MouseKeyboard() {

	return;
}


//判断是否已经对准目标
bool MouseKeyboard::IsInTarget(DETECTRESULTS detectResult) {
    bool ret = false;

    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //使用计算好的游戏屏幕中心坐标
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //计算人员的中心坐标，计算为靠上的位置，尽量打头
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width / 2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height / 4;

        //枪口移动到人员坐标指定位置后，自动开枪
        if ((abs(x2 - x1) < rect.width / 3) && (abs(y2 - y1) < rect.height / 4)) {
            ret = true;
        }
    }

    return ret;
}

//自动开火
void MouseKeyboard::AutoFire(DETECTRESULTS detectResult) {

    m_mouse.leftButtonClick();

    return;
}


//自动移动鼠标
void MouseKeyboard::AutoMove(DETECTRESULTS detectResult) {
    
    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //使用计算好的游戏屏幕中心坐标
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //计算人员的中心坐标，计算为靠上的位置，尽量打头
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width/2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height/ 4;

        //由于是3D游戏，位置是3维坐标，物体越远，移动距离要乘的系数就越大。
        //暂时没有好的方法通过图片检测计算3维坐标，先使用对象的大小初略计算z坐标，但是开镜后的大小暂时无法处理。
        //为了处理太远图片的问题，在按对数log计算一个倍数，实现位置越远倍数不能太大的效果。
        //另外这个倍数在移动鼠标时候要做除数，因为3维左边中，移动距离随着距离放大。
        double z = 1;
        if (m_AssistConfig->maxModelWidth > 0 && m_AssistConfig->maxModelWidth  > rect.width)
        {
            //log函数取值后数据移动还是偏多，改成线性计算
            //z = log2(m_AssistConfig->maxModelWidth) / (rect.width);
            z = m_AssistConfig->maxModelWidth / (rect.width);
        }
        

        //移动鼠标
        //3D游戏移动鼠标的函数，x1/y1为游戏中心点坐标，x2/y2为检测到的人物中心点坐标，z为三维坐标的z轴距离
        //mouseMoveSlow鼠标变慢的倍数
        m_mouse.moveCursor(x1, y1, x2, y2, z, m_AssistConfig->mouseMoveSlow);
    }

    return;
}