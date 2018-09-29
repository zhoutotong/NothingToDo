//--------------------------------------------------------------- 
/** 
\file cbinfileprocessor.cpp 
\brief bin file processor
\version v1.1.1809.9251 
\date 2018-9-25 
\author Zhou Tong
<p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
Vision Technology Branch and all right reserved.</p> 
*/ 
//---------------------------------------------------------------

#include "cbinfileprocessor.h"
#include <QFile>
#include <QDebug>
#include <QDataStream>

#include <stdlib.h>
#include "toolbox.h"
//---------------------------------------------
/**
 * \brief 构造函数
 * \param parent  父指针
 * \param file    文件名称
 */
//---------------------------------------------
CBinFileProcessor::CBinFileProcessor(QObject *parent, const QString &file) : QObject(parent)
{
    m_file_name = file;
    m_file = NULL;
}
CBinFileProcessor::~CBinFileProcessor()
{
    this->close();
}
//---------------------------------------------
/**
 * \brief 新建一个bin文件
 * \param name    新建文件的名称
 * \return    0，成功 -1，失败
 */
//---------------------------------------------
int CBinFileProcessor::new_bin(const QString &name)
{
    if(name.isEmpty())
    {
        return -1;
    }
    m_file_name = name;

    QFile file(name);
    if(file.open(QIODevice::WriteOnly))
    {
        file.close();
    }
    else
    {
        return -1;
    }
    return 0;
}
//---------------------------------------------
/**
 * \brief 读写打开一个文件
 * \param name 文件名称
 * \return  0，成功  -1，失败
 */
//---------------------------------------------
int CBinFileProcessor::open(const QString &name)
{
    if(name.isEmpty())
    {
        return -1;
    }
    m_file_name = name;
    m_file = new QFile(name);
    if(m_file->open(QIODevice::WriteOnly | QIODevice::ReadOnly))
    {

    }
    else
    {
        return -1;
    }
    return 0;
}
//---------------------------------------------
/**
 * \brief 关闭当前打开的文件
 * \return
 */
//---------------------------------------------
int CBinFileProcessor::close()
{
    if(m_file != NULL)
    {
        if(m_file->isOpen())
        {
            m_file->close();
            delete m_file;
            m_file = NULL;
            m_file_name = "";
            return 0;
        }
    }
    return -1;
}
//---------------------------------------------
/**
 * \brief 在文件尾部添加数据
 * \param bytes    待添加的数据
 * \param size     添加数据的字节数
 * \return    0，成功 -1，失败
 */
//---------------------------------------------
int CBinFileProcessor::append_bytes(const uint8_t *bytes, const uint32_t &size)
{
    QFile file(m_file_name);
    if(!file.exists())
    {
        return -1;
    }
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return -1;
    }
    QDataStream out(&file);
    for(uint32_t i = 0; i < size; i++)
    {
        out << bytes[i];
    }
    file.close();
    return 0;
}
