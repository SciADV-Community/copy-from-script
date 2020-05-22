#include "controller.h"

#include <windows.h>
#include <xinput.h>
#include <QTimer>
#include <QSettings>
#include <QDebug>

Controller::Controller()
{
}

void Controller::readInput()
{
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(pollControllers()));
    timer->start(25);
}

void Controller::pollControllers()
{
    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        XINPUT_STATE controllerState;
        if(XInputGetState(i, &controllerState) == ERROR_SUCCESS) {
            if(currentPacket != controllerState.dwPacketNumber) {
                XINPUT_GAMEPAD *gamepad = &controllerState.Gamepad;
                bool A = (gamepad->wButtons & XINPUT_GAMEPAD_A);
                bool B = (gamepad->wButtons & XINPUT_GAMEPAD_B);
                bool DPAD_LEFT = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                if(settings.value("controllerFlipped").toBool()) {
                    qDebug() << "Should be flipped";
                    if(A) {
                        emit ButtonPressed();
                    }
                } else {
                    qDebug() << "Shouldn't";
                    if(B) {
                        emit ButtonPressed();
                    }
                }
                if(DPAD_LEFT) {
                    emit Paused();
                }

            }
            currentPacket = controllerState.dwPacketNumber;
        }
    }
}
