// HIDDriverLibTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>

#include "mouse.h"
#include "keyboard.h"

int main()
{
    Mouse mouse;
    try {
        mouse.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("鼠标设备初始化失败: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Keyboard keyboard;
    try {
        keyboard.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("键盘设备初始化失败: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Sleep(5000);


    mouse.moveCursor(900, 900, 300, 600, 1,1);
    Sleep(5000);

    /**
    //mouse.moveCursor(136, 271);
    mouse.moveCursorEx(10, 0);
    mouse.leftButtonClick();
    Sleep(2000);
    //mouse.moveCursor(56, 315);
    mouse.moveCursorEx(-10, 0);
    mouse.leftButtonClick();
    Sleep(2000);
    //mouse.moveCursor(283, 350);
    mouse.moveCursorEx(10, 0);
    mouse.leftButtonClick();
    Sleep(2000);
    //mouse.moveCursor(275, 271);
    mouse.moveCursorEx(-10, 0);
    mouse.leftButtonClick();
    Sleep(2000);
    */

    return EXIT_SUCCESS;
}
