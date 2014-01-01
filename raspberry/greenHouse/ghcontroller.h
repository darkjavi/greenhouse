#ifndef GHCONTROLLER_H
#define GHCONTROLLER_H

#include "ghserialcontroller.h"
#include "ghtcpserver.h"


class ghController : public QObject
{
    Q_OBJECT
public:
    explicit ghController(QObject *parent = 0);
    bool enableServer(int port = 31416);
    bool openSerial(QString source = "/dev/ttyACM0");
    bool openNetwork(QString host = "greenhouse.local" , int port = 31416);
protected:

    ghSerialController  m_serial;
    ghTcpServer         m_tcpServer;
    ghTcpClient         m_tcpClient;


signals:
    void debugMsg(QString msg);
    void lampTemp(int temp);
    void lampFanSpeed(int pwm);
    void lampPower(int watts);
    void ambientTemp(int temp);
    void ambientHumidity(int humid);
    void powerConsumption(int watts);
    void dataOut(QByteArray data);
private slots:
    void dataIn(QByteArray data);
public slots:
    void init();
    void switchLampOn();
    void switchLampOff();
    void switchLedOn(int led);
    void switchLedOff(int led);
    void setLampFanSpeed(int pwm);
    void pump(int ml);
    void stopPump();
    void resetPumpCount();
};

#endif // GHCONTROLLER_H
