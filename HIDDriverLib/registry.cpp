#include "pch.h"
#include "framework.h"

#include "registry.h"

RegistryService &RegistryService::get()
{
    static RegistryService instance;
    return instance;
}

int RegistryService::getMouseSensivity() const
{
    DWORD size = 3;
    char buffer[3];

    LONG getValueResult = RegGetValueA(HKEY_CURRENT_USER, "Control Panel\\Mouse", "MouseSensitivity", RRF_RT_REG_SZ, nullptr, &buffer, &size);
    if (ERROR_SUCCESS != getValueResult) {
        return -1;
    }

    return atoi(buffer);
}

int RegistryService::getMouseSpeed() const
{
    DWORD size = 2;
    char buffer[2];

    LONG getValueResult = RegGetValueA(HKEY_CURRENT_USER, "Control Panel\\Mouse", "MouseSpeed", RRF_RT_REG_SZ, nullptr, &buffer, &size);
    if (ERROR_SUCCESS != getValueResult) {
        return -1;
    }

    return atoi(buffer);
}
