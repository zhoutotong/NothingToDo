//--------------------------------------------------------------- 
/** 
\file ccsvfileprocessor.cpp 
\brief csv file processor
\version v1.1.1809.9251 
\date 2018-9-25 
\author Zhou Tong
<p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
Vision Technology Branch and all right reserved.</p> 
*/ 
//---------------------------------------------------------------

#include "ccsvfileprocessor.h"
#include <QDebug>
#include "toolbox.h"
#include <QDataStream>

#define WRITE_TOTAL_LENGTH  (0)
#define WRITE_LENGTH        (1)
#define WRITE_CRC           (1)
#define WRITE_VALUE         (2)
#define WRITE_ADD_VALUE     (3)


//---------------------------------------------
/**
 * \brief 构造函数
 * \param parent    父指针
 * \param name      文件名称
 */
//---------------------------------------------
CCSVFileProcessor::CCSVFileProcessor(QObject *parent, QString name) : QObject(parent)
{
    m_file = new QFile(name);
    m_file->close();
    m_row.clear();
    uint32_t num = 0x12345678;
    if(BYTE(&num, 0) == 0x78)
    {
        // little endbian
    }
    else
    {
        // big endbian
    }
}
CCSVFileProcessor::~CCSVFileProcessor()
{
    delete m_file;
}
//---------------------------------------------
/**
 * \brief 打开文件
 * \return ERR_NULL，成功   ERR_CSV_OPEN_FAILED，失败
 */
//---------------------------------------------
ErrorTypeDef CCSVFileProcessor::open()
{
    if(!m_file->exists())
    {
        return ERR_CSV_NOT_EXIST;
    }
    if(!m_file->open(QIODevice::ReadOnly | QIODevice::WriteOnly | QIODevice::Text))
    {
        return ERR_CSV_OPEN_FAILED;
    }
    else
    {
        return ERR_NULL;
    }
}
//---------------------------------------------
/**
 * \brief 关闭文件
 * \return ERR_NULL，成功   ERR_CSV_CLOSE_FAILED，失败
 */
//---------------------------------------------
ErrorTypeDef CCSVFileProcessor::close()
{
    m_row.clear();
    m_file->close();
    return ERR_NULL;
}
//---------------------------------------------
/**
 * \brief 在CSV文件中读取一个两列的表格
 * \param table  读取得的表格数据
 * \return   ERR_NULL，成功   其它错误码，失败
 */
//---------------------------------------------
ErrorTypeDef CCSVFileProcessor::get_table(QMap<QString, QString> &table)
{
    m_row.clear();
    if(!m_file->isOpen())
    {
//        qDebug() << "csv can not open table file";
        return ERR_CSV_NOT_OPEN;
    }
    while(__read_row() != 0)
    {
        if(m_row.size() < 2)
        {
            return ERR_CSV_COLUMN_NUMBER;
        }
        table[QString(m_row.at(0)).toLower()] = m_row.at(1);
    }
    return ERR_NULL;
}
//---------------------------------------------
/**
 * \brief 向指定的位置填写数据
 * \param 要插入的数据
 * \param 插入的行
 * \param 插入的列
 * \return  ERR_NULL，成功  ERR_CSV_OUTOF_LINE，失败
 */
//---------------------------------------------
ErrorTypeDef CCSVFileProcessor::write_blank(const QString &str, const uint32_t &row, const uint32_t &column)
{
    m_file->seek(0);
    QByteArray data_bytes = m_file->readAll();
    QString data = QString(data_bytes.data());
    int mark_cnt = 0;
    for(uint32_t i = 0; i < row; i++)
    {
        mark_cnt = data.indexOf("\n", mark_cnt + 1);
    }
    int limit_size = data.indexOf("\n", mark_cnt + 1);

    for(uint32_t i = 0; i < column; i++)
    {
        mark_cnt = data.indexOf(",", mark_cnt + 1);
    }
    // check if outof line
    if(mark_cnt > limit_size || mark_cnt == -1)
    {
        return ERR_CSV_OUTOF_LINE;
    }
    mark_cnt++;
    int mark_end = data.indexOf(QRegExp("[,\n]{1}"), mark_cnt);

    if(mark_end == -1)
    {
        mark_end = limit_size;
    }

    data.remove(mark_cnt, mark_end - mark_cnt);
    data.insert(mark_cnt, str);
    m_file->resize(0);
    m_file->write(QByteArray::fromStdString(data.toStdString()));
    return ERR_NULL;
}

ErrorTypeDef CCSVFileProcessor::write_blank(const QString &str, const QString &keywords)
{
    m_file->seek(0);
    QByteArray data_bytes = m_file->readAll();
    QString data = QString(data_bytes.data());
    int mark_cnt = data.indexOf(QRegExp(keywords)) + keywords.size() + 1;

    if(mark_cnt == -1)
    {
        return ERR_CSV_OUTOF_LINE;
    }

    int mark_end = data.indexOf(QRegExp("[,\n]{1}"), mark_cnt);
    if(mark_end == -1)
    {
        mark_end = data.size();
    }
    data.remove(mark_cnt, mark_end - mark_cnt);
    data.insert(mark_cnt, str);
    m_file->resize(0);
    m_file->write(QByteArray::fromStdString(data.toStdString()));
    return ERR_NULL;
}
//---------------------------------------------
/**
 * \brief 在CSV文件中读取一个sector
 * \param t_len       数据总长度
 * \param data_buf    数据
 * \return    ERR_NULL，成功   其它错误码，失败
 */
//---------------------------------------------
ErrorTypeDef CCSVFileProcessor::get_sector(uint32_t &t_len, QVector<uint8_t> &data_buf)
{
    int label;
    // check if m_file is open
    if(!m_file->isOpen())
    {
        return ERR_CSV_NOT_OPEN;
    }
    // goto the first line of a sector
    do
    {
        bool ok;
        if(0 == __read_row())
        {
            return ERR_NULL;
        }
        if(QString(m_row.at(0)).isEmpty())
        {
            continue;
        }
        label = __read_column_number(0, &ok);
        if(!ok)
        {
            return ERR_CSV_FORMAT_NO_TOTAL;
        }
    }while(label != WRITE_TOTAL_LENGTH);

    // get total length of a sector
    bool ok;
    uint32_t total_length = (uint32_t)__read_column_number(1, &ok);
    t_len = total_length;
    if(!ok)
    {
        return ERR_CSV_FORMAT_NO_TOTAL;
    }

    for(int i = 0; i < 4; i++)
    {
        data_buf.append(BYTE(&total_length, i));
    }

    if(0 == __read_row())
    {
        return ERR_CSV_NOT_COMPLETE;
    }
    label = (uint32_t)__read_column_number(0,&ok);

    if(!ok)
    {
        return ERR_CSV_FORMAT_NO_WRITE;
    }

    if(label != WRITE_LENGTH)
    {
        return ERR_CSV_FORMAT_NO_WRITE;
    }

    uint32_t write_length = (uint32_t)__read_column_number(1, &ok);

    if(!ok)
    {
        return ERR_CSV_FORMAT_NO_WRITE;
    }

    for(int i = 0; i < 4; i++)
    {
        data_buf.append(BYTE(&write_length, i));
    }

    uint32_t write_cnt = 0;

    while(true)
    {
        if(0 == __read_row())
        {
            return ERR_CSV_NOT_COMPLETE;
        }
        label = (uint32_t)__read_column_number(0, &ok);
        if(!ok)
        {
            return ERR_CSV_FORMAT_ERROR;
        }

        if(WRITE_VALUE == label)
        {

        }
        else if(WRITE_ADD_VALUE == label)
        {

        }
        else if(WRITE_CRC == label)
        {
            // check if column 1 value is 0x55aa
            uint32_t crc = __read_column_number(1, &ok);
            if(!ok)
            {
                return ERR_CSV_NO_CRC;
            }
            if(crc != 0x55aa)
            {
                return ERR_CSV_NO_CRC;
            }
            // check is write size equal to cnt size
            if(data_buf.size() != (int)write_length + 8)
            {
                qDebug() << "data_buf: " << data_buf.size() << " write_length: " << write_length;
                return ERR_CSV_BUF_LENGTH;
            }
//            uint32_t checksum = (uint32_t)ToolBox::CRC((data_buf.data() + 4), data_buf.size() - 4);
//            for(int i = 0; i < 4; i++)
//            {
//                data_buf.append(BYTE(&checksum, i));
//            }
            // write 0xff to tail of data_buf
            for(int i = data_buf.size(); i < (int)total_length + 4; i++)
            {
                data_buf.append(0xff);
            }
            return ERR_NULL;
        }
        else
        {
            return ERR_CSV_NO_CRC;
        }

        uint32_t size = __read_column_number(1, &ok);
        if(!ok)
        {
            return ERR_CSV_WRITE_VALUE;
        }
        write_cnt += size;
        if(write_cnt > write_length)
        {
            qDebug() << "write error";
            return ERR_CSV_BUF_LENGTH;
        }

        QRegExp reg("^[0-9]{1,}|0[xX][0-9A-Fa-f]{1,}$");
        if(reg.exactMatch(QString(m_row.at(2))))
        {
            // write data to buf
            for(uint32_t i = 0; i < size / 4; i++)
            {
                uint32_t data = (uint32_t)__read_column_number(2 + i, &ok);
                for(int j = 0; j < 4; j++)
                {
                    data_buf.append(BYTE(&data, j));
                }
            }
        }
        else
        {
            // write string to buf
            QString str = __read_column(2, &ok);
            if(!ok)
            {
                return ERR_CSV_FORMAT_ERROR;
            }
            for(int i = 0; i < str.size(); i++)
            {
                data_buf.append((uint8_t)str.at(i).toLatin1());
            }
            for(size_t i = str.size(); i < size; i++)
            {
                data_buf.append(0x00);
            }
        }
    }

    return ERR_NULL;
}
//---------------------------------------------
/**
 * \brief 判断是否读到文件末尾
 * \return    false，未到末尾   true，已到文件末尾
 */
//---------------------------------------------
bool CCSVFileProcessor::upto_end()
{
    return m_file->atEnd();
}
//--------------------------------------------
/**
 * \brief   从文件中读取一行数据，并拆分
 * \return  0，数据为空  1，读取到数据
 *
 *
 * \notice before this function make sure csv file is opened
 */
//--------------------------------------------
int CCSVFileProcessor::__read_row()
{
    QStringList strlist;
    m_row.clear();
    if(m_file->atEnd())
    {
        return 0;
    }
    QByteArray array = m_file->readLine();
    array.remove(array.size() - 1, 1);
    QString line = QString(array.toStdString().data());
    strlist = line.split(",");
    m_row = strlist;
    return 1;
}
//---------------------------------------------
/**
 * \brief 从行数据中读取指定列，并将字符串转换为数字
 * \param n   指定的列位置
 * \param ok  字符串转换成功标识，true，成功  false，失败
 * \return 0
 */
//---------------------------------------------
long long CCSVFileProcessor::__read_column_number(uint32_t n, bool *ok)
{
    if(n < (uint32_t)m_row.size())
    {
        QRegExp reg("^0[xX]{1}[0-9a-fA-F]{1,}$");
        QString num(m_row.at(n));
        if(reg.exactMatch(num))
        {
            num.remove(0, 2);
            return (uint32_t)num.toLongLong(ok, 16);
        }
        else
        {
//            return QString(m_row.at(n)).toInt(ok);
            return (uint32_t)QString(m_row.at(n)).toLongLong(ok);
        }
    }
    *ok = false;
    return 0;
}
//---------------------------------------------
/**
 * \brief 从行数据中读取指定列
 * \param n    指定的列位置
 * \param ok   读取成功标识符， true，成功  false，失败
 * \return   读取到的列内容
 */
//---------------------------------------------
QString CCSVFileProcessor::__read_column(int n, bool *ok)
{
    if(n < m_row.size())
    {
        *ok = true;
        return m_row.at(n);
    }
    *ok = false;
    return "";
}



