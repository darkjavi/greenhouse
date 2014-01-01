#include "vpparserserial.h"

vpParserSerial::vpParserSerial(QString source, QObject *parent) :
    vpParser(source,parent)
{
    m_errorCounter = 0;
    m_serialPort = new QSerialPort(this);
//    if(pollType==QextSerialPort::EventDriven)
//    {
      m_pollTimer=0;
      m_imOpening=false;
      connect(&m_timeoutTimer,SIGNAL(timeout()),this,SLOT(timeout()));
      connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(read_serialPort()));
      connect(m_serialPort,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serial_error(QSerialPort::SerialPortError)));
      connect(m_serialPort,SIGNAL(baudRateChanged(qint32,QSerialPort::Directions)),this,SLOT(serial_baudrate(qint32,QSerialPort::Directions)));
//    }
//    else
//    {
//        m_pollTimer=new QTimer(this);
//        connect(m_pollTimer,SIGNAL(timeout()),this,SLOT(read_serialPort()));
//    }
}

vpParserSerial::~vpParserSerial()
{
//    if(m_serialPort)m_serialPort->deleteLater();// Esto lo hace automaticamente qt al crearlos como (this) ??
//    if(m_pollTimer) m_pollTimer->deleteLater();
}

bool vpParserSerial::openSource(QString serialDevice)
{
    m_imOpening = true;
    m_source=serialDevice;
    m_serialPort->setPortName(serialDevice);
    qDebug()<<"[INFO]vpParserSerial::openSource->Opening:"<<m_serialPort->portName()<<m_serialPort->baudRate()<<"Bauds";
    m_serialPort->setBaudRate(m_baudRate);
    //m_serialPort->setTextModeEnabled(false);
    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        m_serialPort->setBaudRate(m_baudRate);
        qDebug()<< "[INFO]vpParserSerial::openSource->OPENED!" << m_baudRate << "/" <<m_serialPort->baudRate()<<"Bauds";
        if(m_pollTimer)m_pollTimer->start();
        m_imOpening = false;
        return true;
    }
    else
    {
        qDebug()<< "[ERROR]vpParserSerial::openSource->Can't open:" <<m_serialPort->portName();
        m_imOpening = false;
        this->deleteLater();
        return false;
    }
}

void vpParserSerial::read_serialPort()
{
    if(m_timeoutTimer.isActive())m_timeoutTimer.start(m_timeoutTimer.interval());
    QByteArray tmp = m_serialPort->readAll();
    m_rxBuffer.append(tmp);
//    QString debugstr;
//    for(int i = 0 ; i < tmp.count() ; i++)
//    {
//        debugstr.append("[");
//        debugstr.append(QString::number((quint8)tmp[i]));
//        debugstr.append("] ");
//    }
//    qDebug()<<debugstr;
    emit dataRx();
    //qDebug()<<"[INFO]:vpParserSerial::read_serialPort()-> Leidos "<<tmp.count()<<"bytes " <<m_rxBuffer.count() <<"en el buffer";
    parse_rxBuffer();
}

void vpParserSerial::write_serialPort(QByteArray &data)
{
    if(!m_serialPort->isWritable())
    {
        qDebug()<< "[ERROR]:vpParserSerial::write_serialPort-> Can't write to:"<<m_serialPort->portName();
        return;
    }
    emit dataTx();
    qint64 written=m_serialPort->write(data);
    if(written!=data.count())
    {
        qDebug()<< "[ERROR]:vpParserSerial::write_serialPort->"<<m_serialPort->portName()<<"Data to write have"<<data.count()<<"/"<< "written:"<<written;
    }
}

void vpParserSerial::closeSource()
{
    m_timeoutTimer.stop();
    if(m_serialPort->isOpen())
    {
        qDebug()<< "[INFO]:vpParserSerial::closeSource->closing"<<m_serialPort->portName();
        m_serialPort->close();
    }
}

void vpParserSerial::serial_error(QSerialPort::SerialPortError e)
{
    //    enum SerialPortError {
    //        NoError,
    //        DeviceNotFoundError,
    //        PermissionError,
    //        OpenError,
    //        ParityError,
    //        FramingError,
    //        BreakConditionError,
    //        WriteError,
    //        ReadError,
    //        ResourceError,
    //        UnsupportedOperationError,
    //        UnknownError
    //    };

        if(e==QSerialPort::NoError || m_imOpening || e==9) return;
        qDebug()<<"[WARN]lvSerial::serial_closed-> Error!"<<e<<m_serialPort->errorString();
        resetSerial();
}

void vpParserSerial::resetSerial()
{
    m_errorCounter++;
    m_imOpening=true;
    qDebug()<<"[WARN]lvSerial::resetSerial->"<< m_serialPort->portName();
    m_serialPort->close();
    Sleeper::sleep(2);
    m_serialPort->setBaudRate(m_baudRate);
    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        m_serialPort->setBaudRate(m_baudRate);
        qDebug()<< "[INFO]lvSerial::resetSerial->OPENED!" << m_baudRate << "/" << m_serialPort->baudRate()<<"Bauds";
        m_errorCounter=0;
    }
    else
    {
        qDebug()<< "[WARN]lvSerial::resetSerial-> CAN'T OPEN!" <<m_serialPort->baudRate()<<"Bauds";
        if(m_errorCounter < 3)
        {
            resetSerial();
        }else
        {
            qDebug()<<"[ERROR]lvSerial::resetSerial-> FAILED"<<m_errorCounter<<"TIMES! GIVING UP!!!!!";
            emit error();//Pensar como hacer para con el modo consola....
            closeSource();
            //this->deleteLater();
        }
    }
    m_imOpening=false;
}

void vpParserSerial::serial_baudrate(qint32 b, QSerialPort::Directions d)
{
    qDebug()<< "[WARN]vpParserSerial::serial_baudrate->" << b << d;
}

void vpParserSerial::timeout()
{
    if(m_imOpening) return;
    qDebug()<<"[WARN]vpParserSerial::timeout()->No data for "<<m_timeoutTimer.interval()<<"msecs!";
    resetSerial();
}

void vpParserSerial::setTimeout(uint msecs)
{
    m_timeoutTimer.start(msecs);
}

void vpParserSerial::setBaudRate(uint baudrate)
{
    m_baudRate = baudrate;
    m_serialPort->setBaudRate(baudrate);
}
