//-------------------------------------------------------------------
/** 
 \file ccsvfileprocessor.h 
 \brief csv file processor
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------
#ifndef CCSVFILEPROCESSOR_H
#define CCSVFILEPROCESSOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QStringList>
#include <QMap>
#include "errorlist.h"

class CCSVFileProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CCSVFileProcessor(QObject *parent = 0, QString name = "");
    ~CCSVFileProcessor();
    // 打开一个CSV文件
    ErrorTypeDef open();
    // 关闭CSV文件
    ErrorTypeDef close();
    // 读取一个sector
    ErrorTypeDef get_sector(uint32_t &t_len, QVector<uint8_t> &data_buf);
    // 读取一个两列的表格
    ErrorTypeDef get_table(QMap<QString, QString> &table);
    // write data to csv file
    ErrorTypeDef write_blank(const QString &str, const uint32_t &row, const uint32_t &column);
    ErrorTypeDef write_blank(const QString &str, const QString &keywords);


    // 检查是否已达到文件尾部
    bool upto_end();
private:
    QFile* m_file;        ///< current file
    QStringList m_row;    ///< hold current row data
    QString m_base_addr;  ///< 

    // read a new row of csv file(m_file)
    int __read_row();
    // read a column from current row data
    QString __read_column(int n, bool* ok);
    // read a column from current row data then format QString to int
    long long __read_column_number(uint32_t n, bool* ok);



signals:

public slots:
};

#endif // CCSVFILEPROCESSOR_H
