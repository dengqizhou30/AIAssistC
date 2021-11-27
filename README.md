AIAssistC 是c++版本的AIAssist，原来c#的版本，由于使用的OpenCvSharp4不支持显卡CUDA加速，而游戏中检测速度是关键，所以再做有一个c++版本。<br>
目前已实现了图像检测、自动追踪、自动开火等主体功能。GPU加速已经开发成功，这块坑很多，有时间整理下相关内容。<br>
GPU加速的建议是不要尝试需要自己编译GPU功能的框架，非常浪费时间。直接选择提供预编译好支持GPU的运行库，如TensorFlow C API，可以节约大量时间。<br>
原来的c#版本不再更新，感兴趣的可以参考 https://github.com/dengqizhou30/AIAssist <br>
<br>

**一、项目说明：**<br>
AIAssistC是一个AI游戏助手，使用OpenCv、DNN、tensorflow c api、cppflow、ssd_mobilenet/efficientdet、hidriver、MFC等技术，截取游戏屏幕，使用AI模型进行对象识别，并实现自动瞄准/自动开枪等鼠标操作，提升玩家的游戏体验。<br>
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

**三、主要的运行库：**<br>
1、intel贡献的大神级图像处理框架OpenCv：<br>
https://opencv.org/ <br>

OpenCV DNN 模块目前支持多种AI对象检测模型推理运行：<br>
https://github.com/opencv/opencv/tree/master/samples/dnn <br>
https://github.com/opencv/opencv/wiki/TensorFlow-Object-Detection-API <br>
https://github.com/openvinotoolkit/open_model_zoo <br>

2、TensorFlow C API运行库，cppflow是TensorFlow C API的C++封装库：<br>
选择使用TensorFlow C API运行库的原因，是因为它提供编译好的支持GPU的运行库，而其他框架都需要自己编译。<br>
https://tensorflow.google.cn/install/lang_c <br>
https://github.com/serizba/cppflow <br>
https://serizba.github.io/cppflow/quickstart.html <br>

3、谷歌tensoflow的对象检测模型efficientdet-lite0、ssd_mobilenet_v3： <br>
实测gpu加速效果，efficientdet-lite模型比ssd_mobilenet好很多 <br>
https://hub.tensorflow.google.cn/tensorflow/efficientnet/lite0/classification/2 <br>
https://github.com/tensorflow/models <br>


**四、使用注意：**<br>
1、目前基于AI图像检测，只做到了人员识别，无法区分敌我。为避免游戏中自动追踪并射击队友的尴尬，有一个使用小技巧。
目前在工具中加入了逻辑控制，只有切换到主副武器时才会运行自动追踪和自动射击，切换到其他武器时不会自动追踪。
所以在捡枪或者跟队友跑时，切换到手枪或其他数字键，停止自动追踪。对敌时再切换到主副武器（数字键1或2），自动追踪才生效。
习惯这种操作方式后，这个工具使用体验相对好一些。<br>
2、使用windows api实现了鼠标键盘操作模拟，HIDDriver驱动程序不再是必须项。在绝地求生、逆战、穿越火线三个游戏上测试，windows api可以正常工作。<br>
3、如果要尝试HIDDriver驱动，需要手工安装。这个驱动目前还没有微软颁发的正式证书，只能在win 10测试模式下执行，详情参考子项目说明： <br>
https://github.com/dengqizhou30/AIAssistC/tree/main/HIDDriver <br>


**五、游戏截图：**<br>
穿越火线游戏截图：<br>
![blockchain](https://github.com/dengqizhou30/AIAssistC/blob/main/Data/img/chuanyuehuoxian.png)</br>
