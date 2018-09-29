//-------------------------------------------------------------------
/** 
 \file cbinfileprocessor.h 
 \brief bin file processor
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------
#ifndef CBINFILEPROCESSOR_H
#define CBINFILEPROCESSOR_H

#include <QObject>
#include <QFile>
#include <QVector>

class CBinFileProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CBinFileProcessor(QObject *parent = 0, const QString &file = "");
    ~CBinFileProcessor();
    // 新建一个bin文件
    int new_bin(const QString &name);
    // 打开一个文件
    int open(const QString &name);
    // 在文件尾部添加数据
    int append_bytes(const uint8_t *bytes, const uint32_t &size);
    // 关闭文件
    int close();
private:
    QString m_file_name;   ///< 当前操作的文件名称
    QFile* m_file;         ///< 文件对象
signals:

public slots:
};

#endif // CBINFILEPROCESSOR_H
