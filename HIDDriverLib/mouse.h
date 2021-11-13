#pragma once

#include "device.h"

#include <Windows.h>
#include <vector>

class Mouse : public Device
{

public:
    explicit Mouse();
    Mouse(const Mouse &) = delete;
    void operator =(const Mouse &) = delete;
    virtual ~Mouse() override = default;

    virtual void                        initialize() override;
    virtual void                        abort() override;

    virtual void                        leftButtonDown();
    virtual void                        leftButtonUp();
    virtual void                        leftButtonClick();

    virtual void                        rightButtonDown();
    virtual void                        rightButtonUp();
    virtual void                        rightButtonClick();

    virtual void                        middleButtonDown();
    virtual void                        middleButtonUp();
    virtual void                        middleButtonClick();

    virtual void                        moveCursor(POINT);
    virtual void                        moveCursor(LONG x, LONG y);
    virtual void                        moveCursor(LONG x1, LONG y1, LONG x2, LONG y2, double z, double mouseMoveSlow);
    virtual void                        moveCursorEx(LONG x, LONG y);

    int                                 getSpeedByRange(int range) const;
    static int                          getRangeBySpeed(int speed);

    virtual void                        sendMouseReport(CHAR xSpeed, CHAR ySpeed);

protected:
    enum Button {
        BUTTON_NONE                     = 0x00,
        BUTTON_LEFT                     = 0x01,
        BUTTON_RIGHT                    = 0x02,
        BUTTON_MIDDLE                   = 0x04
    };

    void                                populateRangeSpeedVector();
    
    static POINT                        getCurrentCursorPosition(LPDWORD error);

    BYTE                                m_buttons               {BUTTON_NONE};
    std::vector<int>                    m_rangeSpeedVector;

    static const BYTE                   REPORT_ID               {0x02};
    static const LONG                   MAX_ABSOLUTE_SPEED      {127};
    static const int                    DISTANCE_TOLERANCE      {3};
    static const int                    EPP_DISABLED            {0};
    static const int                    EPP_ENABLED             {1};
    static const int                    SENSITIVITY_MIN         {1};
    static const int                    SENSITIVITY_MAX         {20};

private:
#pragma pack(1)
    struct Report {
        BYTE                            reportId;
        BYTE                            buttons;
        CHAR                            x;
        CHAR                            y;
    };
#pragma pack()

    
};
