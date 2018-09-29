//-------------------------------------------------------------------
/** 
 \file toolbox.h 
 \brief some tools to process data
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------

#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <QVector>
#include "MD5.h"
#include "errorlist.h"


#define BYTE(num, n)    ((uint8_t)*(((uint8_t*)num) + n))


namespace ToolBox{
    unsigned short CRC(uint8_t* pui8Addr, unsigned int ui32Size);
    ErrorTypeDef setMD5(const QString &path, const QString &name);
}

#endif // TOOLBOX_H
