#pragma once

#include "device.h"

#include <Windows.h>

#include "keys.h"

class Keyboard : public Device
{

public:
    explicit Keyboard();
    Keyboard(const Keyboard &) = delete;
    void operator =(const Keyboard &) = delete;
    virtual ~Keyboard() override = default;

    virtual void                        initialize() override;

    virtual void                        type(BYTE keyCode);

    virtual void                        abort() override;

protected:
    BYTE                                m_modifiers             {KEY_NONE};

    static const BYTE                   REPORT_ID               {0x04};

private:
#pragma pack(1)
    struct Report {
        BYTE                            reportId;
        BYTE                            modifiers;
        BYTE                            _reserved;
        BYTE                            keyCodes[6];
    };
#pragma pack()

    virtual void                        sendKeyboardReport(BYTE *keyCodes);
};
