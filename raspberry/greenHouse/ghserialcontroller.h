#ifndef GHSERIALCONTROLLER_H
#define GHSERIALCONTROLLER_H

#include "vpparserserial.h"

class ghSerialController : public vpParserSerial
{
    Q_OBJECT
public:
    explicit ghSerialController(QObject *parent = 0);
protected:
    bool parse_rxBuffer();
signals:
    void dataIn(QByteArray data);
public slots:
    void dataOut(QByteArray data);

};

#endif // GHSERIALCONTROLLER_H
