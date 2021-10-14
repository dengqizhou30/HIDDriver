#include "mouse.h"

#include <stdexcept>

#include "registry.h"

Mouse::Mouse()
    //: Device{L"\\\\?\\HID#VARIABLE_6&Col02#1"}
    ////×¢ÒâÒª´óÐ´
    : Device{ L"\\\\?\\HID#HIDRIVER&Col02#1" }
{}

void Mouse::initialize()
{
    if (isInitialized()) throw std::runtime_error{"ERROR_DOUBLE_INITIALIZATION"};
    populateRangeSpeedVector();
    Device::initialize();
}

void Mouse::leftButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons |= BUTTON_LEFT;
    sendMouseReport(0, 0);
}

void Mouse::leftButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons &= ~BUTTON_LEFT;
    sendMouseReport(0, 0);
}

void Mouse::leftButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    leftButtonDown();
    leftButtonUp();
}

void Mouse::rightButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons |= BUTTON_RIGHT;
    sendMouseReport(0, 0);
}

void Mouse::rightButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons &= ~BUTTON_RIGHT;
    sendMouseReport(0, 0);
}

void Mouse::rightButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    rightButtonDown();
    rightButtonUp();
}

void Mouse::middleButtonDown()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons |= BUTTON_MIDDLE;
    sendMouseReport(0, 0);
}

void Mouse::middleButtonUp()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    m_buttons &= ~BUTTON_MIDDLE;
    sendMouseReport(0, 0);
}

void Mouse::middleButtonClick()
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    middleButtonDown();
    middleButtonUp();
}

void Mouse::moveCursor(POINT point)
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    moveCursor(point.x, point.y);
}

void Mouse::moveCursor(LONG x, LONG y)
{
    if (!isInitialized()) throw std::runtime_error{"ERROR_NOT_INITIALIZED"};
    while (!isAborted()) {
        DWORD getCurrentCursorPositionError = ERROR_SUCCESS;
        POINT currentPoint = getCurrentCursorPosition(&getCurrentCursorPositionError);
        if (ERROR_SUCCESS != getCurrentCursorPositionError) {
            throw std::runtime_error{"ERROR_CURSOR_PROCESSING"};
        }

        LONG distance = (LONG) sqrt(pow(x - currentPoint.x, 2) + pow(y - currentPoint.y, 2));
        if (distance <= DISTANCE_TOLERANCE) {
            return;
        }

        CHAR xSpeed = static_cast<CHAR>(getSpeedByRange(abs(x - currentPoint.x)));
        xSpeed = (x > currentPoint.x ? xSpeed : -xSpeed);

        CHAR ySpeed = static_cast<CHAR>(getSpeedByRange(abs(y - currentPoint.y)));
        ySpeed = (y > currentPoint.y ? ySpeed : -ySpeed);

        sendMouseReport(xSpeed, ySpeed);
        Sleep(1);
    }
}

void Mouse::moveCursorEx(LONG x, LONG y)
{
    if (!isInitialized()) throw std::runtime_error{ "ERROR_NOT_INITIALIZED" };
    for (int i = 0; i < x / 2; i++) {
        CHAR xSpeed = static_cast<CHAR>(getSpeedByRange(abs(x)));
    
        CHAR ySpeed = static_cast<CHAR>(getSpeedByRange(abs(y)));

        sendMouseReport(xSpeed, ySpeed);
        Sleep(1);
    }
}

void Mouse::abort()
{
    Device::abort();
}

void Mouse::populateRangeSpeedVector()
{
    m_rangeSpeedVector.clear();
    int rangeIndex = 0;
    for (int speed = 0; speed < MAX_ABSOLUTE_SPEED + 1; ++speed) {
        int range = getRangeBySpeed(speed);
        while (rangeIndex <= range) {
            m_rangeSpeedVector.push_back(speed);
            ++rangeIndex;
        }
    }
}

int Mouse::getSpeedByRange(int range) const
{
    if (m_rangeSpeedVector.size() == 0) {
        return static_cast<int>(sqrt(range));
    }
    if (range > m_rangeSpeedVector.size() - 1) {
        range = static_cast<int>(m_rangeSpeedVector.size()) - 1;
    }
    return m_rangeSpeedVector.at(range);
}

int Mouse::getRangeBySpeed(int speed)
{
    double sensitivityFactors[21] = {0, 0.03125, 0.625, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.25, 3.5};
    int sensitivity = RegistryService::get().getMouseSensivity();
    int eppSpeed = RegistryService::get().getMouseSpeed();

    if (   sensitivity  < SENSITIVITY_MIN
        || sensitivity  > SENSITIVITY_MAX
        || eppSpeed     < EPP_DISABLED
        || eppSpeed     > EPP_ENABLED)
    {
        return speed;
    }

    double sensitivityFactor = eppSpeed == EPP_ENABLED ? 1 : sensitivityFactors[sensitivity];
    double eppFactor = eppSpeed == EPP_ENABLED ? 0.25 * sensitivity : 1;
    double range = static_cast<double>(speed) * sensitivityFactor * eppFactor;
    return static_cast<int>(range);
}

POINT Mouse::getCurrentCursorPosition(LPDWORD error)
{
    POINT currentPoint{0, 0};
    BOOL getCursorPosResult = GetCursorPos(&currentPoint);
    if (!getCursorPosResult && nullptr != error) {
        *error = GetLastError();
    }
    return currentPoint;
}

void Mouse::sendMouseReport(CHAR xSpeed, CHAR ySpeed) {
    Report report;
    report.reportId             = REPORT_ID;
    report.buttons              = m_buttons;
    report.x                    = xSpeed;
    report.y                    = ySpeed;

    setOutputReport(&report, static_cast<DWORD>(sizeof(Report)));
}
