#include "vpparser.h"
#include <QStringList>

vpParser::vpParser(QString source, QObject *parent) :
    QObject(parent)
{
    m_source = source;
    m_type          = ParserUnknown;
    m_dataInStrings = false;
}

//static
bool vpParser::exportCSV(QString filename, QVector<QString> fields, const QVector<QVector<double> > &data, bool overwrite_dest)
{
    if(!fields.count())
    {
        qDebug() << "[WARN]vpParser::exportCSV-> Empty fields!";
        return false;
    }
    if(!data.count())
    {
        qDebug() << "[WARN]vpParser::exportCSV-> Empty data!";
        return false;
    }
    QFile f(filename);
    if(f.exists() && !overwrite_dest)
    {
        qDebug() << "[Error]vpDataSet::exportToCSV-> Already exist:"<<filename<<" overwrite="<<overwrite_dest;
        return false;
    }
    if(!f.open(QIODevice::ReadWrite))
    {
        qDebug() << "[Error]vpDataSet::exportToCSV-> Cant write::"<<filename;
        return false;
    }
    QByteArray enc_data;
    enc_data.append(generateCSVHeader(fields));

    for(uint r = 1 ; r < data.count() ; r++)
    {

        enc_data.append(encodeCSVLine(data[r]));
    }
    qint64 written=f.write(enc_data);
    f.close();
    if(written!=enc_data.count())
    {
        qDebug() << "[ERROR]vpDataSet::exportToCSV-> data has "<<enc_data.count()<<"bytes but only "<<written<<"were written!";
        return false;
    }
    qDebug() << "[INFO]vpParser::exportToCSV-> written "<<enc_data.count()<<"bytes";
    return true;
}

bool vpParser::exportCSV(QString filename, QVector<QString> fields, const QVector<QVector<QString> > &data, bool overwrite_dest)
{
    if(!fields.count())
    {
        qDebug() << "[WARN]vpParser::exportCSV-> Empty fields!";
        return false;
    }
    if(!data.count())
    {
        qDebug() << "[WARN]vpParser::exportCSV-> Empty data!";
        return false;
    }
    QFile f(filename);
    if(f.exists() && !overwrite_dest)
    {
        qDebug() << "[Error]vpDataSet::exportToCSV-> Already exist:"<<filename<<" overwrite="<<overwrite_dest;
        return false;
    }
    if(!f.open(QIODevice::ReadWrite))
    {
        qDebug() << "[Error]vpDataSet::exportToCSV-> Cant write::"<<filename;
        return false;
    }
    QByteArray enc_data;
    enc_data.append(generateCSVHeader(fields));

    for(uint r = 1 ; r < data.count() ; r++)
    {

        enc_data.append(encodeCSVLine(data[r]));
    }
    qint64 written=f.write(enc_data);
    f.close();
    if(written!=enc_data.count())
    {
        qDebug() << "[ERROR]vpDataSet::exportToCSV-> data has "<<enc_data.count()<<"bytes but only "<<written<<"were written!";
        return false;
    }
    qDebug() << "[INFO]vpParser::exportToCSV-> written "<<enc_data.count()<<"bytes";
    return true;
}

QString vpParser::generateCSVHeader(QVector<QString> fields)
{
    QString header;
    for(int i = 0; i<fields.count()-1 ; i++)
    {
        header.append(fields[i]).append(",");
    }
    header.append(fields.last()).append("\n");
    return header;
}

QString vpParser::encodeCSVLine(QVector<double> data)
{
    QString line;
    for(int i = 0; i<data.count()-1 ; i++)
    {
        line.append(QString::number(data.at(i),'f',4).append(","));
    }
    line.append(QString::number(data.last(),'f',4)).append("\n");
    return line;
}

QString vpParser::encodeCSVLine(QVector<QString> data)
{
    QString line;
    for(int i = 0; i<data.count()-1 ; i++)
    {
        line.append(data.at(i)).append(",");
    }
    line.append(data.last()).append("\n");
    return line;
}

vpParser::csvType vpParser::guessCSVType(QString filename)
{
    QFile f(filename);
    if(!f.exists())                     return csvInvalid;
    if(!f.open(QIODevice::ReadOnly))    return csvInvalid;
    QString raw_data = f.read(10000);
    f.close();
    QStringList data_lines = raw_data.split("\n");
    if(! data_lines.count()>1)          return csvInvalid;
    QStringList fields   = data_lines[0].split(",");
    QStringList firstRow = data_lines[1].split(",");
    if(fields.count()!=firstRow.count())return csvInvalid;
    csvType ctype = csvPlainArray;
    bool hastime      =false;
    bool hasGpsFields =false;
    for(int i = 0 ; i < fields.count() ; i++)
    {
        if(fields[i].contains("time",Qt::CaseInsensitive))
        {
            hastime=true;
        }
        else if( fields[i].contains("latitude",Qt::CaseInsensitive) ||
                 fields[i].contains("longitude",Qt::CaseInsensitive) )
        {
            hasGpsFields=true;
        }
    }
    if(hastime)         ctype=csvTimedArray;
    if(hasGpsFields)    ctype=csvGpsArray;
    return ctype;
}

//slots

void vpParser::recordCSV(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadWrite))
    {
        qDebug()<<"[ERROR]vpParser::recordCSV-> cannot open!:"<<filename;
        return;
    }
    QByteArray data;
    data.append(generateCSVHeader(m_fields));
    f.write(data);
    m_logFile=filename;
    connect(this,SIGNAL(newData(QVector<double>)), this,SLOT(logData(QVector<double>)));
    connect(this,SIGNAL(newData(QVector<QString>)),this,SLOT(logData(QVector<QString>)));
}

void vpParser::logData(QVector<double> row)
{
    QFile f(m_logFile);
    if((!f.exists())||(!f.open(QIODevice::Append)))
    {
        emit error();
        return;
    }
    QByteArray data;
    data.append(encodeCSVLine(row));
    f.write(data);
}

void vpParser::logData(QVector<QString> row)
{
    QFile f(m_logFile);
    if((!f.exists())||(!f.open(QIODevice::Append)))
    {
        emit error();
        return;
    }
    QByteArray data;
    data.append(encodeCSVLine(row));
    f.write(data);
}

void vpParser::stopRecord()
{
    if(m_logFile.isEmpty())
    {
        qDebug()<<"[WARN]vpParser::stopRecord()->Nothing to stop!?";
        return;
    }
    disconnect(this,SIGNAL(lastData(QVector<double>)),this,SLOT(logData(QVector<double>)));
    disconnect(this,SIGNAL(lastData(QVector<QString>)),this,SLOT(logData(QVector<QString>)));
    m_logFile.clear();
}
