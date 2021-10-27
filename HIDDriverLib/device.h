#pragma once

#include <stdlib.h>
#include <Windows.h>


class Device
{

public:
    explicit Device(PCWSTR deviceInterface);
    Device(const Device &) = delete;
    void operator =(const Device &) = delete;
    virtual ~Device() = default;

    virtual void                initialize();
    virtual void                abort();

    bool                        isInitialized() const;
    bool                        isAborted() const;

protected:
    void                        setOutputReport(PVOID data, DWORD size);

private:
    PCWSTR                      mp_deviceInterface  {nullptr};
    HANDLE                      mp_deviceHandle     {nullptr};
    bool                        m_isInitialized     {false};
    bool                        m_isAborted         {false};
};
