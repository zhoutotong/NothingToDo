//#include "stdafx.h"
//#include "GxConvertToBinTool.h"
#include "GxConvertToBinToolCopy.h"
#include <stdio.h>
#include <QDebug>
#include <string.h>
#include <QByteArray>

typedef uint8_t BYTE;

CGxConvertToBinToolCopy::CGxConvertToBinToolCopy()
{
}

//----------------------------------------------------
/**
brief     生成的二进制文件添加CRC校验码
\param   strFileName  输入文件地址

\return  void
*/
//----------------------------------------------------
void CGxConvertToBinToolCopy::BinFlieAddCRC(string strBinFile)
{
    uint16_t wCRC       = 0;        // 返回CRC校验码
    size_t   nLength    = 0;        // 文件长度
    size_t   nTotalSize = 0;        // 数据总长度
    size_t   nSize      = 0;        // 实际数据长度
    size_t   nTemp      = 0;        // 中间变量
    size_t   nReadSize  = 0;        // 读取文件的长度
    FILE     *pFile     = NULL;     // 读文件
    uint8_t  *pDataBuf  = NULL;     // 文件数据
    uint8_t  *pTempBuf  = NULL;

	// 打开文件
    pFile = fopen(strBinFile.c_str(), "rb+");
    if(pFile == NULL)
    {
        return;
    }

	// 获取文件大小
	fseek(pFile, 0, SEEK_END);
	nLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pDataBuf = new BYTE[nLength];
	if (pDataBuf == NULL)
	{
		return;
	}
	memset(pDataBuf, 0, nLength);

	// 获取文件的内容
//	nReadSize = fread_s(pDataBuf, nLength, 1, nLength, pFile);
    nReadSize = fread(pDataBuf, nLength, 1, pFile);
	if (nReadSize != nLength)
	{
		delete[]pDataBuf;
		pDataBuf = NULL;
		return;
	}

	pTempBuf = pDataBuf;
	do 
	{
        BYTE    *pLocalTempBuf  = NULL;
		pLocalTempBuf = pTempBuf;

		// 获取总长度大小
		memcpy(&nTotalSize, pTempBuf, DATASIZE_LENGTH);
		pTempBuf = pTempBuf + DATASIZE_LENGTH;    //偏移到实际大小存储地址
		
		// 获取数据实际长度(计算校验码的长度)
		memcpy(&nSize, pTempBuf, DATASIZE_LENGTH);
		pTempBuf = pTempBuf + DATASIZE_LENGTH;    //偏移到实际数据存储地址

		// 计算CRC校验码
		wCRC = GXCalculateCRC(pTempBuf, nSize);

		// 获取校验码位置
		pTempBuf = pTempBuf + nSize;             //偏移到CRC校验码存储地址

		// 修改校验码的值
		memcpy(pTempBuf, &wCRC, sizeof(wCRC));
 
		// 校验码结尾
 	 	pTempBuf = pLocalTempBuf + nTotalSize;
		nTemp    = nTemp + nTotalSize;

	} while (nTemp < nLength);

	// 修改后的数据放回源文件
	fseek(pFile, 0, SEEK_SET);
	fwrite(pDataBuf, 1, nLength, pFile);

	if (pDataBuf != NULL)
	{
		delete[]pDataBuf;
		pDataBuf = NULL;
	}
 	fclose(pFile);
 	pFile = NULL;
}


//----------------------------------------------------
/**
brief     计算CRC校验码
\param     pui32Addr    [IN] 数据首地址
\param     ui32Size     [IN] 数据尺寸

\return    CRC校验码值
*/
//----------------------------------------------------
unsigned short CGxConvertToBinToolCopy::GXCalculateCRC(unsigned char *pui8Addr, unsigned int ui32Size)
{
	unsigned int i;
	unsigned int j;
	unsigned int c;
	unsigned int ui32CRC = 0xFFFF;

	for(i = 0; i < ui32Size; i++)
	{
		c = *(pui8Addr + i) & 0x00FF;
		ui32CRC ^= c;
		for (j = 0; j < 8; j++)
		{
			 if (ui32CRC & 0x0001)
			 {
				 ui32CRC >>= 1;
				 ui32CRC ^= 0xA001;
			 }
			 else
			 {
				 ui32CRC >>= 1;
			 }
		}
   }
	ui32CRC = (ui32CRC >> 8) + (ui32CRC << 8);

	return(ui32CRC);
}

//---------------------------------------------------------------------------------
/**
\brief   按行读文件
	
	     第一行：地址
	     1,(参数长度)
	     2,4,(参数值)
	     2,32/16,(字符串)
	     3,8,(地址+值)
\param   strFileName  输入文件地址
\param   strFileName  输出文件地址
	
\return  void
*/
//---------------------------------------------------------------------------------
void CGxConvertToBinToolCopy::ConverttoBinFlie(string strInFileName, QByteArray &data_out)
{
	if (strInFileName == "")
	{
//		MessageBox("请导入文件！");
        qDebug() << "file is not exist!";
		return;
	}

	ifstream objIfstream;               // 输入文件流
//	ofstream objOfstream;               // 输出文件流
	string   strOutFile = "";           // 输出文件名称
	string   strLine    = "";           // 每一行的字符串
	string   strAddr    = "";           // 输入的地址值
	string   strValue   = "";           // 每一行的内容
//	char     chFF       = 0;	        // 填充的二进制值()FF
	int      nParamNum  = 0;   
	int      nLength    = 0;            
	size_t   nPos       = 0;
	unsigned long ulValue = 0;
	int      nTotalLength = 0;          // 保存总长度
	int      nRealLength  = 0;          // 保存实际长度
	BYTE    *pDataBuffer  = NULL;
	BYTE    *pTempBuffer  = NULL;

	// 逗号的位置
	size_t uiFirstComma  = 0;
	size_t uiSecondComma = 0;
	size_t uiThirdComma  = 0;
	
	// 打开导入文件
	setlocale(LC_ALL,"Chinese-simplified");
	objIfstream.open(strInFileName.c_str());
	setlocale(LC_ALL,"C");
	if (!objIfstream)
	{
        qDebug() << "can not open file";
//		MessageBox("打开导入文件失败！");
		return;
	}

	// 参数组地址
	getline(objIfstream, strLine);
	uiFirstComma = strLine.find(',');
	strAddr = strLine.substr(0, uiFirstComma);

	// 导出文件的路径和名称
	nPos = strInFileName.find_last_of('.');
	strOutFile = strInFileName.substr(0, nPos) + ".bin";

	// 创建二进制文件进行写操作
	setlocale(LC_ALL,"Chinese-simplified");
//	objOfstream.open(strOutFile.c_str(), std::ofstream::binary|ios::trunc);
	setlocale(LC_ALL,"C");
    
	// 逐行读取文件信息
	while(getline(objIfstream, strLine))
	{
		// 读取单行的第一列值
		uiFirstComma = strLine.find(',');
		nParamNum	 = StringToInt(strLine.substr(0, uiFirstComma));

		// 获取单行的第二列值
		uiSecondComma = strLine.find(',', (uiFirstComma + 1));
		strValue = strLine.substr((uiFirstComma + 1), (uiSecondComma - uiFirstComma - 1));
		nLength	= StringToInt(strValue);

         // 写文件
        if (nParamNum == WRITE_TOTAL_LENGTH)
        {
			nTotalLength = nLength;
			if (pDataBuffer != NULL)
			{
				delete pDataBuffer;
				pDataBuffer = NULL;
			}

			pDataBuffer = new BYTE[nTotalLength];
			pTempBuffer = pDataBuffer;
			memset(pDataBuffer,0xFF,nTotalLength);
        }
	    else if(nParamNum == WRITE_LENGTH)           // 写实际长度
		{
			if (nLength == 0x55aa)
			{
				// 计算CRC校验码
//				BYTE *pCRCBuf = NULL;
//				pCRCBuf = pTempBuffer - nRealLength;
//				unsigned int  nCRC    = GXCalculateCRC(pCRCBuf,nRealLength);
//			    memcpy(pTempBuffer, &nCRC, sizeof(nCRC));
//				pTempBuffer = pTempBuffer + 4;
				
//				objOfstream.write((char *)pDataBuffer,nTotalLength);
                data_out.append((char *)pDataBuffer,nTotalLength);
			}
			else
			{
				nRealLength = nLength;
				memset(pDataBuffer,0,nRealLength + 4);
				memcpy(pTempBuffer,&nRealLength,sizeof(nLength));
				pTempBuffer = pTempBuffer + sizeof(nLength);
			}
		}
		else if (nParamNum == WRITE_VALUE)		    // 写值或字符串
		{
			// 获取单行的第三列值
			uiThirdComma = strLine.find(',', uiSecondComma + 1);
			strValue	 = strLine.substr((uiSecondComma + 1), (uiThirdComma - uiSecondComma - 1));
			
			if (nLength == VALUE_LENGTH)         // 写值
			{
				ulValue = StringToInt(strValue);
				memcpy(pTempBuffer,&ulValue,4);
				pTempBuffer = pTempBuffer + 4;
			}
			else if (nLength >= ADDR_VALUE_LENGTH)    // 写字符串
			{
				if (strValue == "0")	  // 空字符串	
				{
					pTempBuffer = pTempBuffer + nLength;
				}
				else                                          // 非空字符串
				{
					if (nLength - int(strValue.length()) < 0)
					{
                        qDebug() << "string is out of size";
//						MessageBox("输入的字符串超出范围！");
						break;
					}

					// 将字符串写入输出文件中
					memcpy(pTempBuffer,strValue.c_str(),strValue.length());
					pTempBuffer = pTempBuffer + nLength;
				}
			}
		}
		else if (nParamNum == WRITE_ADDR_VALUE)	    // 8字节，地址+值
		{
			// 获取单行的第三列值
			uiThirdComma = strLine.find(',', uiSecondComma + 1);
			strValue	 = strLine.substr((uiSecondComma + 1), (uiThirdComma - uiSecondComma - 1));

			// 判断地址和值是否是8字节
			if (nLength == ADDR_VALUE_LENGTH)
			{
				// 第三列，地址
				ulValue = StringToInt(strValue);
				memcpy(pTempBuffer,&ulValue,4);
				pTempBuffer = pTempBuffer + 4;;
				
				// 第四列，值
				strValue = strLine.substr((uiThirdComma + 1), strLine.length());
				ulValue	 = StringToInt(strValue);
				memcpy(pTempBuffer,&ulValue,4);
				pTempBuffer = pTempBuffer + 4;
			}
			else    
			{
//				MessageBox("地址+值输入的长度不为8!");
                qDebug() << "addr not equal to 8";
				break;
			}
		}
		else
		{
//			MessageBox("第一列必须为0，1，2，3!");
            qDebug() << "first line must be 0, 1, 2, 3!";
			break;
		}	
	}

	// 关闭文件
	objIfstream.close();
//	objOfstream.close();

	if (pDataBuffer != NULL)
	{
		delete pDataBuffer;
		pDataBuffer = NULL;
	}
}

//---------------------------------------------------------------------------------
/**
\brief   字符串转化为整型值
\param   hexStr      输入的字符串 
	
\return  返回的整型值
*/
//---------------------------------------------------------------------------------
unsigned long CGxConvertToBinToolCopy::StringToInt(const string& strhexStr)
{
	char *pOutPut;
	if(strhexStr.length() > 2)
	{
		if((strhexStr[0] == '0') && (strhexStr[1] == 'x'))
		{
			return strtoul(strhexStr.c_str(), &pOutPut, 16);
		}
	}
	return strtoul(strhexStr.c_str(), &pOutPut, 10);
}
