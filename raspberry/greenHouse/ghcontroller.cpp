#include "ghcontroller.h"

ghController::ghController(QObject *parent) : QObject(parent)
{

}

bool ghController::enableServer(int port)
{

}

bool ghController::openSerial(QString source)
{
    if(m_serial.openSource(source))
    {
        connect(&m_serial,SIGNAL(dataIn(QByteArray)),this,SLOT(dataIn(QByteArray)));
        connect(this,SIGNAL(dataOut(QByteArray)),&m_serial,SLOT(dataOut(QByteArray)));
        return true;
    }
    return false;
}

bool ghController::openNetwork(QString host, int port)
{

}

void ghController::init()
{

}

void ghController::switchLampOn()
{

}

void ghController::switchLampOff()
{

}

void ghController::switchLedOn(int led)
{

}

void ghController::switchLedOff(int led)
{

}

void ghController::setLampFanSpeed(int pwm)
{

}

void ghController::pump(int ml)
{

}

void ghController::stopPump()
{

}

void ghController::resetPumpCount()
{

}

void ghController::dataIn(QByteArray data)
{

}
