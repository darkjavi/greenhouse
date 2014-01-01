#ifndef VPPARSERSERIAL_H
#define VPPARSERSERIAL_H
#include "vpparser.h"
#include "qserialport.h"

#include <QTimer>
class vpParserSerial : public vpParser
{
    Q_OBJECT
public:
    explicit vpParserSerial(QString source, QObject *parent = 0);
    ~vpParserSerial();
    virtual bool openSource(QString serialDevice);
protected:
    bool            m_imOpening;
    quint8          m_errorCounter;
    uint            m_baudRate;
    QByteArray      m_rxBuffer;
    QSerialPort*    m_serialPort;
    QTimer*         m_pollTimer;
    QString         m_recordFile;
    QTimer          m_timeoutTimer;

    bool isOpen(){return m_serialPort->isOpen();}
    virtual bool parse_rxBuffer()=0;
signals:
    
protected slots:
    void serial_error(QSerialPort::SerialPortError);
    void serial_baudrate(qint32,QSerialPort::Directions);
    void read_serialPort();
    void write_serialPort(QByteArray &data);
    void resetSerial();
    void timeout();
public slots:
    void setBaudRate(uint baudrate);
    void setTimeout(uint msecs);
    virtual void closeSource();
};

#endif // VPPARSERSERIAL_H
