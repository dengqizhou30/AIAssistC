#pragma once
#include<string>
#include<iostream>
#include<windows.h>
#include<winuser.h>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// 配置数据模型类，和UI配置信息对应，单例模式太麻烦，直接定义一个静态变量
class AssistConfig
{
public:
    // 获取单实例
    static AssistConfig* GetInstance();

    //计算检测区域相关数据
    //计算实际检测区域，只检测瞄准中心的一小块区域，保障检测速度
    void ReCalDetectionRect();

    //获取屏幕缩放比例
    double static getScreensZoom();

    //配置数据模型
    //选择的游戏进程，不同的游戏，操作参数不一样
    string processNames[2] = { "逆战", "TslGame" };
    //string processName = "TslGame";
    string processName = "逆战"; 


    //是否启用图像检测
    bool detectImg = true;
    //用户配置的实际检测宽度和高度
    int detectWidth = 320, detectHeight = 320;
    //游戏进程占用的原始屏幕区域
    cv::Rect screenRect = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };
    //裁剪后的实际图像检查区域
    cv::Rect detectRect = { 0,0,detectWidth,detectHeight };
    //检测区域中心点
    int detectCentX = detectRect.x + detectRect.width / 2, detectCentY = detectRect.y + detectRect.height / 2;
    //计算屏幕缩放后的，裁剪后的实际图像检查区域
    cv::Rect detectZoomRect = { 0,0,detectWidth,detectHeight };


    //是否自动追踪
    bool autoTrace = false;
    //自动追踪类型，1是持续追踪、2是鼠标右键瞄准触发追踪
    double mouseMoveSlow = 0.5; //数据移动太快，用一个参数，要控制移动速度变慢,值要设置为小于1
    int autoTraceType = 1;
    int autoTraceTimeOut = 0; //鼠标右键瞄准触发追踪倒计时，缺省按30秒设置

    //是否自动开火
    bool autoFire = false;
    //枪械射击类型,1是单点、2是3连点、3是6连点、4是持续开枪
    int gunType = 1;
    //持续开枪时间
    int autoFireTime = 600;

    //是否自动压枪
    bool autoPush = false;


    //游戏相关属性
    //游戏玩家本人的图像所在位置
    int playerCentX = screenRect.width/2; 
    //游戏模型最大宽度
    //int maxModelWidth = detectWidth / 4;
    int maxModelWidth = 150;

private:
    AssistConfig();
    ~AssistConfig();
    void test();

private:
    // 唯一单实例对象
    static AssistConfig* m_config;
};

