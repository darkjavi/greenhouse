#ifndef VPPARSER_H
#define VPPARSER_H

#include <QThread>
#include <QFile>
#include <QDebug>

#include "sleeper.h"
//#include "/home/darkjavi/Escritorio/carpc-new/hacks/sleeper.h"

class vpParser : public QObject
{
    Q_OBJECT
public:
    enum csvType
    {
        csvInvalid,
        csvPlainArray,
        csvTimedArray,
        csvGpsArray
    };
    enum ParserTypes
    {
        ParserUnknown,
        ParserGenericCSV,
        ParserTorqueCSV,
        ParserMindflexLive,
        ParserRawNMEA
    };

    explicit vpParser(QString source = "none", QObject *parent = 0);
    virtual bool     openSource(QString source)=0;
    virtual QVector<QVector<double> >   readDoubles(int start = 0, int count = 0)  {qDebug("[WARN]vpParser::read as doubles--> NOT IMPLEMENTED!");}
    virtual QVector<QVector<QString> >  readStrings(int start = 0, int count = 0)  {qDebug("[WARN]vpParser::read as strs--> NOT IMPLEMENTED!");}
    QVector<QString> fields()          {return m_fields;}
    QString          source()          {return m_source;}
    static  bool     exportCSV(QString filename,QVector<QString> fields,const QVector<QVector<QString> >& data, bool overwrite_dest=false);
    static  bool     exportCSV(QString filename,QVector<QString> fields,const QVector<QVector<double> >& data, bool overwrite_dest=false);
    static  vpParser::csvType guessCSVType(QString filename);
    virtual bool     isOpen() = 0;
    bool             isDataInStrings() {return m_dataInStrings;}
protected:
    QString          m_source;
    QVector<QString> m_fields;
    QVector<QString> m_auxFields;
    quint8           m_type;
    QString          m_logFile;
    bool             m_dataInStrings;
    QThread          m_thread;
    static  QString  generateCSVHeader(QVector<QString> fields);
    static  QString  encodeCSVLine(QVector<double> data);
    static  QString  encodeCSVLine(QVector<QString> data);
signals:
    void error();
    void progress(quint8 progress);
    void dataTx();
    void dataRx();
    void newData(QVector<double>);
    void newAuxData(QVector<double>);
    void newData(QVector<QString>);
    void newAuxData(QVector<QString>);
protected slots:
    void logData(QVector<double>);
    void logData(QVector<QString>);

public slots:
    virtual void    closeSource()=0;
    virtual void    recordCSV(QString filename);
    virtual void    stopRecord();
};
#endif // VPPARSER_H
