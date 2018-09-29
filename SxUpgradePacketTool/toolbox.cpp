//--------------------------------------------------------------- 
/** 
\file toolbox.cpp 
\brief tools to process data
\version v1.1.1809.9251 
\date 2018-9-25 
\author Zhou Tong
<p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
Vision Technology Branch and all right reserved.</p> 
*/ 
//---------------------------------------------------------------


#include "toolbox.h"
#include "FileWithMD5.h"
#include <QDebug>
#include <QFile>
#include <unistd.h>

//---------------------------------------------
/**
 * \brief ToolBox::CRC
 * \param pui8Addr
 * \param ui32Size
 * \return
 */
//---------------------------------------------
unsigned short ToolBox::CRC(uint8_t* pui8Addr, unsigned int ui32Size)
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
//---------------------------------------------
/**
 * \brief ToolBox::getMD5
 * \param path
 * \param name
 * \return
 */
//---------------------------------------------
ErrorTypeDef ToolBox::setMD5(const QString &path, const QString &name)
{
    chdir(path.toStdString().data());
    CFileWithMD5 add_md5(name.toStdString());
    if(add_md5.is_check_md5_successful())
    {
        return WAR_MD5_EXIST;
    }
    else
    {
        QFile file(name);
        if(!file.open(QIODevice::ReadOnly))
        {
            return ERR_MD5_FAILED;
        }
        QByteArray datas = file.readAll();
        file.close();
        add_md5.write_data(datas.data(), datas.size());
    }
    return ERR_NULL;
}


