AIAssistC 是c++版本的AIAssist，原来c#的版本，由于使用的OpenCvSharp4不支持显卡cdma加速，而游戏中检测速度是关键，所以再做有一个c++版本。<br>
原来的c#版本不再更新，感兴趣的可以参考 https://github.com/dengqizhou30/AIAssist。<br>
<br>

**一、项目说明：**<br>
AIAssistC是一个AI游戏助手，使用OpenCv、DNN、ssd_mobilenet/efficientdet、hidriver、MFC等技术，截取游戏屏幕进行对象识别，使用虚拟鼠标键盘驱动实现自动瞄准/自动开枪等功能，提升玩家的游戏体验。<br>
<br>

**二、工程说明：**<br>
AIAssist：mfc前端UI子工程；<br>
AIAssistLib：AI助手静态库子工程；<br>
Data：存放模型文件及工具文件的子工程；<br>
HIDDriver：windows鼠标键盘驱动子工程；<br>
HIDDriverLib：鼠标键盘驱动调用库子工程；<br>
HIDDriverLibTest：鼠标键盘驱动调用库验证测试子工程；<br>
OpencvTest：openc功能验证测试子工程；<br>
<br>

**三、使用注意：**<br>
1、需要手工安装HIDDriver驱动程序，详情参考子项目说明：<br>
https://github.com/dengqizhou30/AIAssistC/tree/main/HIDDriver<br>
