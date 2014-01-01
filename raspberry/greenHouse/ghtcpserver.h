#ifndef GHTCPSERVER_H
#define GHTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class ghTcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ghTcpClient(QObject *parent = 0)
    {

    }

signals:
    void dataIn(QByteArray data);
public slots:
    void dataOut(QByteArray data)
    {

    }
};


class ghTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ghTcpServer(QObject *parent = 0);
protected:
    QVector<ghTcpClient>    m_clients;

signals:
    void dataIn(QByteArray data);
public slots:
    void dataOut(QByteArray data)
    {

    }
};
#endif // GHTCPSERVER_H
