//-------------------------------------------------------------------
/** 
 \file cfilemanager.h 
 \brief file manager, transport csv files to bin file, make package
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------
#ifndef CFILEMANAGER_H
#define CFILEMANAGER_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QVector>
#include "errorlist.h"

//#define ONLY_CSV_FILE
#define COPY_USE_CODE
//#define OUTPUT_SINGLE_BIN

#define ADDR_SIZE (4)
#define SECTOR_SIZE (4096)

class CFileManager : public QObject
{
    Q_OBJECT
public:
    explicit CFileManager(QObject *parent = 0, const QString &input = "", const QString &output = "");

    bool check_file();
    int trans_csv_to_bin();
    int make_package();

private:

    // base addr define
    typedef enum
    {
        REG_INIT_SENSOR           = 0x00D10000,
        REG_INIT_IMPLEMENTED      = 0x00D11000,
        REG_INIT_INFO             = 0x00D12000,
        REG_INIT_SCALE            = 0x00D13000,
        REG_INIT_STEP             = 0x00D14000,
        REG_INIT_CONTROL          = 0x00D15000,
        REG_MANU_PARAM            = 0x00D20000,
        REG_MANU_FPGA_PARAM       = 0x00D21000,
        REG_MANU_SENSOR_PARAM     = 0x00D22000,
        REG_USERSET0_PARAM        = 0x00D30000,
        REG_USERSET0_FPGA_PARAM   = 0x00D31000,
        REG_USERSET0_SENSOR_PARAM = 0x00D32000,
        REG_IMAGE_PROC_PARAM      = 0x00D40000,
        REG_CFG_INFO              = 0x00D50000,
        REG_SN                    = 0x00D60000,
        REG_NETINFO               = 0x00D61000,
    }BLOCK_ADDR;

    const QStringList m_check_csv_list;    ///< collect all csv files' name that need to check
    QMap<QString, QString> m_csv_list;     ///< collect all csv files' name that is checked
    const QVector<uint32_t> m_base_addr_list;
    const QString m_config_name;           ///< config file name
    const QString m_cfgtl_name;
    const QString m_pallas_module_name;
    QString m_fpga_name;
    const QString m_dev_version_name;
    QString m_image_name;
    QString m_pallas_data_name;
    QString m_upgrade_name;

    const QString m_input_path;            ///< input path
    const QString m_output_path;           ///< output path

    bool __check_config_file();
    int __write_version();
    int __check_csv_file(const QDir &dir);
    bool __check_fpga_file(const QDir &dir);
    bool __check_image_file(const QDir &dir);
    bool __check_pallas_data_file(const QDir &dir);
    bool __check_upgrade_file(const QDir &dir);


    QString __get_package_name(const QString &input_path);

    QStringList __check_file_exist(const QStringList &name_list, const QRegExp &reg);
signals:
    void send_msg(QString msg);
    void send_error_msg(QString msg);
    void send_warning_msg(QString msg);
    void send_error(ErrorTypeDef error);

public slots:
};

#endif // CFILEMANAGER_H
