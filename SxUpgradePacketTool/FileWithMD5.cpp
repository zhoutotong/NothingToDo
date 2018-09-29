#include "Common.h"
#include "MD5.h"
#include "FileWithMD5.h"
#include <string.h>
#include <QDebug>

//*******************************************************************
/*
\brief  计算指定数据的MD5值

\param  buffer[in]                  数据地址
\param  data_size[in]               数据大小
\param  md5[out]                    计算的 md5值，大小为16字节

\return
*/
//*******************************************************************
void CFileWithMD5::__GetMD5(void *buffer, std::uint32_t data_size, char md5[LEN_MD5_CODE])
{
    MD5_CTX md5_st;
    memset(&md5_st, 0, sizeof(md5_st));

    MD5Init(&md5_st);
    MD5Update(&md5_st, (unsigned char *)buffer, data_size);
    MD5Final(&md5_st, (unsigned char *)md5);
}

//---------------------------------------------------------------------
/**
\brief  构造函数
\param  file_name　　         文件名

\return
*/
//----------------------------------------------------------------------
CFileWithMD5::CFileWithMD5(const std::string &file_name)
{
    FILE *fp = NULL;
    size_t ret = 0;  
    char md5_results[LEN_MD5_CODE] = {0};       //MD5计算结果
    struct stat statbuff;                 //文件状态信息结构体
    md5_header_st md5_info;               //MD5头信息


    memset(&statbuff, 0, sizeof(statbuff));
    memset(&md5_info, 0, sizeof(md5_header_st));
    this->m_file_name = file_name;
    this->m_data_size = 0;
    this->m_data = NULL;
    
    fp = fopen(file_name.data(), "rb");
    if(fp == NULL)
    {
        //文件打开失败或者文件不存在
        return;
    }

    //获取文件状态信息，里面包括文件大小
    stat(file_name.data(), &statbuff);

    //读取MD5头信息
    ret = fread(&md5_info, FILE_MD5_HEADER_SIZE, 1, fp);
    if (ret != 1)
    {
        //获取Md5头信息失败
        fclose(fp);
        return;
    }

    if ((md5_info.data_len == 0) ||(md5_info.data_len != (statbuff.st_size - FILE_MD5_HEADER_SIZE)))
    {
        //文件数据长度不匹配 或数据长度为0
        fclose(fp);
        return;
    }

    //读取所有的数据
    char *data_buff = new char[md5_info.data_len];
    ret = fread(data_buff, md5_info.data_len, 1, fp);
    if (ret != 1)
    {
        //获取数据失败
        fclose(fp);
        delete[] data_buff;
        return;
    }

    //计算数据对应的MD5值
    this->__GetMD5(data_buff, md5_info.data_len, md5_results);

    //比较MD5值是否正确
    int memcmp_result = memcmp(md5_results, md5_info.md5, LEN_MD5_CODE);
    if (memcmp_result == 0)
    {
        this->m_data_size = md5_info.data_len;
        this->m_data = data_buff;
    }

    fclose(fp);
}

//析够函数
CFileWithMD5::~CFileWithMD5()
{
    //释放内放
    if (this->m_data != NULL)
    {
        delete[] this->m_data;
        this->m_data = NULL;
    }
}


//---------------------------------------------------------------------
/**
\brief  获取指定长度的数据
\param  buffer[out]             数据buf
\param  buffer_sizep[in]       读取数据的长度 

\return 实际读取数据的长度
*/
//----------------------------------------------------------------------
std::uint32_t CFileWithMD5::read_data(void* buffer, std::uint32_t buffer_size)
{
    std::uint32_t copy_size = 0;

    if ((this->m_data == NULL) || (buffer == NULL))
    {
        return 0;
    }

    if (buffer_size > this->m_data_size)
    {
        copy_size = this->m_data_size;
    }
    else
    {
        copy_size = buffer_size;
    }

    memcpy(buffer, this->m_data, copy_size);
    return copy_size;
}


//---------------------------------------------------------------------
/**
\brief  对写入的数据添加MD5头信息，写入到文件
\param  buffer[in]             数据buf
\param  buffer_sizep[in]       读取数据的长度 

\return 实际写入的数据长度
*/
//----------------------------------------------------------------------
std::uint32_t CFileWithMD5::write_data(void* buffer, std::uint32_t buffer_size)
{
    size_t write_num = 0;
    md5_header_st md5_info;               //MD5头信息

    if ((buffer == NULL) || (buffer_size == 0))
    {
        return 0;
    }

    if (buffer_size > this->m_data_size)
    {
        if (this->m_data != NULL)
        {
            delete[] this->m_data;
            this->m_data = NULL;
        }

        this->m_data = new char[buffer_size];
    }

    //数据拷贝到私有变量中
    this->m_data_size = buffer_size;
    memcpy(this->m_data, buffer, this->m_data_size);

    //计算数据的MD5头信息
    memset(&md5_info, 0, sizeof(md5_info));
    md5_info.data_len = this->m_data_size;

    if(m_file_name.size() < sizeof(md5_info.name))
    {
        memcpy(md5_info.name, this->m_file_name.data(), m_file_name.size());
    }
    else
    {
        memcpy(md5_info.name, this->m_file_name.data(), sizeof(md5_info.name));
    }


    this->__GetMD5(this->m_data, md5_info.data_len, md5_info.md5);

    FILE *fp = fopen(this->m_file_name.data(), "w");
    if (fp == NULL)
    {
        delete[] this->m_data;
        this->m_data = NULL;
        this->m_data_size = 0;
        return 0;
    }
    write_num = fwrite(&md5_info, sizeof(md5_info), 1, fp);
    if (write_num != 1)
    {
        delete[] this->m_data;
        this->m_data = NULL;
        this->m_data_size = 0;
        fclose(fp);
        return 0;
    }

    write_num = fwrite(this->m_data, this->m_data_size, 1, fp);
    if (write_num != 1)
    {
        delete[] this->m_data;
        this->m_data = NULL;
        this->m_data_size = 0;
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return this->m_data_size;
}
