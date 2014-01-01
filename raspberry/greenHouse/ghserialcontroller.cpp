#include "ghserialcontroller.h"

ghSerialController::ghSerialController(QObject *parent) :
    vpParserSerial("/dev/ttyACM0",parent)
{
}

bool ghSerialController::parse_rxBuffer()
{
    if(m_rxBuffer.count() > 0)
    {
        emit dataIn(m_rxBuffer);
        m_rxBuffer.clear();
        return true;
    }
    return false;
}

void ghSerialController::dataOut(QByteArray data)
{
    write_serialPort(data);
}
