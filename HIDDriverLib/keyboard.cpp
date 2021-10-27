#include "pch.h"
#include "framework.h"

#include "keyboard.h"

#include <stdexcept>

Keyboard::Keyboard()
    //: Device{L"\\\\?\\HID#VARIABLE_6&Col04#1"}
    //×¢ÒâÒª´óÐ´
    : Device{ L"\\\\?\\HID#HIDRIVER&Col04#1" }
{}

void Keyboard::initialize()
{
    if (isInitialized()) throw std::runtime_error{"ERROR_DOUBLE_INITIALIZATION"};
    Device::initialize();
}

void Keyboard::type(BYTE keyCode)
{
    BYTE keyCodes[6] = {KEY_NONE};

    keyCodes[0] = keyCode;
    sendKeyboardReport(keyCodes);

    keyCodes[0] = KEY_NONE;
    sendKeyboardReport(keyCodes);
}

void Keyboard::abort()
{
    Device::abort();
}

void Keyboard::sendKeyboardReport(BYTE *keyCodes)
{
    Report report;
    report.reportId             = REPORT_ID;
    report.modifiers            = m_modifiers;
    report._reserved            = 0x00;
    std::memcpy(report.keyCodes, keyCodes, 6);

    setOutputReport(&report, static_cast<DWORD>(sizeof(Report)));
}
