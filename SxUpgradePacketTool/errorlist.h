//-------------------------------------------------------------------
/** 
 \file errorlist.h 
 \brief error list
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------
#ifndef ERRORLIST_H
#define ERRORLIST_H

#include <QString>

typedef enum _ErrorTypeDef{
    ERR_NULL = 0,
    ERR_CSV_OPEN_FAILED,// CSV
    ERR_CSV_CLOSE_FAILED,
    ERR_CSV_NOT_COMPLETE,
    ERR_CSV_FORMAT_NO_TOTAL,
    ERR_CSV_FORMAT_NO_WRITE,
    ERR_CSV_WRITE_VALUE,
    ERR_CSV_BUF_LENGTH,
    ERR_CSV_NO_CRC,
    ERR_CSV_ROOTFS_NOT_COMPLETE,
    ERR_CSV_FORMAT_ERROR,
    ERR_CSV_VERSION_NOT_MATCH,
    ERR_CSV_NOT_OPEN,
    ERR_CSV_COLUMN_NUMBER,
    ERR_CSV_OUTOF_LINE,
    ERR_CSV_SIZE,
    ERR_CSV_NOT_EXIST,
    ERR_MD5_FAILED,
    WAR_MD5_EXIST,
    ERR_PACKAGE_FAILED,
    ERR_WRITE_VERSION_FAILED,
}ErrorTypeDef;

#define LOG_ERROR(error)      do{switch(error){
#define ADD_ERROR(error, msg) case (error): \
    if(QString(#error).split("_").at(0) == "ERR")\
    {\
        __logout_error_msg(QString(msg));\
    }\
    else if(QString(#error).split("_").at(0) == "WAR")\
    {\
        __logout_warning_msg(msg);\
    }\
    else\
    {\
        __logout_error_msg("unknown error code");\
    }\
    break
#define END_LOG_ERROR         default:break;}}while(0)
#endif // ERRORLIST_H
