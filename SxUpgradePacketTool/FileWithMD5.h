//-----------------------------------------------------------
/**
\file    FileWithMD5.h
\brief    interface of the CFileWithMD5
\version  v1.0.1807.9221
\date     2018-07-22
\author   Ma Qianjin
<p>Copyright (c) 2013-2014 China Daheng Group, Inc. Beijing
Image Vision Technology Branch and all right reserved.</p>
*/
//-----------------------------------------------------------
#ifndef C_FILE_WITH_MD5_H_
#define C_FILE_WITH_MD5_H_
#include <stdint.h>

#define FILE_MD5_HEADER_SIZE         256 
#define LEN_MD5_CODE              (0x10)

//文件头部添加的MD5信息结构体
typedef struct {
    char md5[16];
    unsigned int  data_len;
    unsigned char name[16];
    unsigned char reserve[0x100-16-16-4];
} md5_header_st;

//--------------------------------------------
/**
\brief    带MD5码的文件
\author   Maqianjin
*/
//---------------------------------------------
class CFileWithMD5
{
public:
    /// 构造函数
    CFileWithMD5(const std::string &file_name);

    /// 析构
    virtual ~CFileWithMD5();

    /// 读取数据
    std::uint32_t read_data(void* buffer, std::uint32_t buffer_size);

    /// 写入数据
    std::uint32_t write_data(void* buffer, std::uint32_t buffer_size);

    /// 获取DataSize 
    std::uint32_t get_data_size(){ return m_data_size; };

    /// 判断是否校验成功
    bool is_check_md5_successful(){ return m_data_size != 0; };

private:
    /// 获取MD5码
    void __GetMD5(void *buffer, std::uint32_t data_size, char md5[LEN_MD5_CODE]);

private:
    std::string      m_file_name;       ///< 文件名称
    std::uint32_t    m_data_size;       ///< 数据大小
    char*            m_data;            ///< 数据
};

#endif   //C_FILE_WITH_MD5_H_
