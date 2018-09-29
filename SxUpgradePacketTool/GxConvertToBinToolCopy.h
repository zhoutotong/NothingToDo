// GxConvertToBinToolDlg.h : 头文件
//

#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>   
#include <QByteArray>
//#include "mylist.h"
//#include "FileManage.h"
//#include "afxcmn.h"
using namespace std;

#define  ITEM_ID             0    ///< 插入的ID号
#define  ITEM_FILE_PATH      1    ///< 插入的文件名称

#define  WRITE_TOTAL_LENGTH   0   ///< 写参数的总长度
#define  WRITE_LENGTH         1   ///< 写参数实际长度
#define  WRITE_VALUE          2   ///< 写参数值
#define  WRITE_ADDR_VALUE     3   ///< 写地址和值

#define  VALUE_LENGTH         4   ///< 值长度
#define  ADDR_VALUE_LENGTH    8   ///< 地址+值长度
#define  DATASIZE_LENGTH      4   ///< 数据长度

// CGxConvertToBinToolDlg 对话框
class CGxConvertToBinToolCopy
{
// 构造
public:
    CGxConvertToBinToolCopy();	// 标准构造函数

	protected:

public:
	/// 转化为bin文件
    static void ConverttoBinFlie(string strInFileName, QByteArray &data_out);

    static void BinFlieAddCRC(string strBinFile);

	/// 计算CRC校验码
    static unsigned short GXCalculateCRC(unsigned char *pui8Addr, unsigned int ui32Size);

	/// 字符串转化为整型值
    static unsigned long StringToInt(const string& strhexStr);
};
