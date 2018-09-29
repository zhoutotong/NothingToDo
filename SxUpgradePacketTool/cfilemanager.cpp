//--------------------------------------------------------------- 
/** 
\file cfilemanager.cpp 
\brief file manager, transport csv file to bin file, make package
\version v1.1.1809.9251 
\date 2018-9-25 
\author Zhou Tong
<p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
Vision Technology Branch and all right reserved.</p> 
*/ 
//---------------------------------------------------------------

#include "cfilemanager.h"
#include <QDebug>
#include <QRegExp>
#include <QDataStream>
#include <QMap>
#include <QCoreApplication>

#include <sys/stat.h>
#include <sys/types.h>

#include "ccsvfileprocessor.h"
#include "cbinfileprocessor.h"
#include "FileWithMD5.h"
#include "toolbox.h"

#include "GxConvertToBinToolCopy.h"

#include <unistd.h>

//---------------------------------------------
/**
 * \brief 构造函数
 * \param parent    父指针
 */
//---------------------------------------------
CFileManager::CFileManager(QObject *parent, const QString &input, const QString &output)
    : QObject(parent)   
    , m_check_csv_list({"init", "img", "cfginfo", "factoryparam"})
    , m_base_addr_list({ REG_INIT_SENSOR, REG_INIT_IMPLEMENTED, REG_INIT_INFO, REG_INIT_SCALE, REG_INIT_STEP, REG_INIT_CONTROL\
                       , REG_IMAGE_PROC_PARAM\
                       , REG_CFG_INFO\
                       , REG_MANU_PARAM, REG_MANU_FPGA_PARAM, REG_MANU_SENSOR_PARAM\
                       , REG_USERSET0_PARAM, REG_USERSET0_FPGA_PARAM, REG_USERSET0_SENSOR_PARAM\
                       , REG_SN\
                       , REG_NETINFO})
    , m_config_name("config.csv")
    , m_cfgtl_name("cfgTL")
    , m_pallas_module_name("PallasModule")
    , m_fpga_name("fpga.bin")
    , m_dev_version_name("devversion")
    , m_image_name("image.ub")
    , m_pallas_data_name("pallas_data.tar.gz")
    , m_upgrade_name("upgrade.sh")
    , m_input_path(input)
    , m_output_path(output)
{

}
//---------------------------------------------
/**
 * \brief 检查文件是否存在
 * \return true, 所有文件存在    false, 一个或多个文件不存在
 */
//---------------------------------------------
bool CFileManager::check_file()
{
    QString path = m_input_path;
    if(!QDir(path).exists())
    {
        return false;
    }

    // check config.csv file
    bool check = __check_config_file();
    if(!check)
    {
        return false;
    }

    // check *.csv file and pallas_data file, those two files must exist at the same times
    // check *.csv file
    int csv_cnt = __check_csv_file(QDir(path));
#ifdef ONLY_CSV_FILE
    if(csv_cnt == m_check_csv_list.size())
    {
        return true;
    }
    else
    {
        emit send_error_msg("CSV file is not correct, please check");
        return false;
    }
#else


    bool pallas_data_flag = __check_pallas_data_file(QDir(path));
    if(csv_cnt == m_check_csv_list.size() && pallas_data_flag)
    {
    }
    else if(csv_cnt == 0 && !pallas_data_flag)
    {
    }
    else
    {
        emit send_error_msg("CSV file and rootfs file is not correct, please check");
        return false;
    }
    if(!__check_fpga_file(QDir(path)))
    {
        return false;
    }
    if(!__check_upgrade_file(QDir(path)))
    {
        return false;
    }
    __check_image_file(QDir(path));
    return true;
#endif
}
//--------------------------------------------
/**
 * \brief 检查config.csv文件是否存在
 * \param dir
 * \return
 */
//--------------------------------------------
bool CFileManager::__check_config_file()
{
    // 尝试打开config.csv文件，如果打开失败则认为config.csv文件不存在
    CCSVFileProcessor csv_config(this, m_input_path + "/" + m_config_name);
    ErrorTypeDef error = csv_config.open();
    if(error != ERR_NULL)
    {
        emit send_error(error);
        emit send_error_msg("can not open " + m_config_name + " file, please check");
        return false;
    }
    csv_config.close();
    return true;
}
//---------------------------------------------
/**
 * \brief   写cfgTL, XML版本号到cfginfo.csv文件
 * \return  0, 成功  -1, 失败
 */
//---------------------------------------------
int CFileManager::__write_version()
{
    CCSVFileProcessor table(this, m_input_path + "/" + m_config_name);
    if(ERR_NULL == table.open())
    {
        QMap<QString, QString> t;
        table.get_table(t);
        table.close();
        QString cfg_name = m_input_path + "/" + m_csv_list["cfginfo"];
        CCSVFileProcessor cfginfo(this, cfg_name);
        if(ERR_NULL != cfginfo.open())
        {
            return -1;
        }
        if(t["xml"].isEmpty())
        {
            emit send_error_msg("there is no XML version in config.csv file");
            cfginfo.close();
            return -1;
        }
        if(t["cfgtl"].isEmpty())
        {
            emit send_error_msg("there is no cfgTL version in config.csv file");
            cfginfo.close();
            return -1;
        }
        emit send_error(cfginfo.write_blank(t["cfgtl"], 4, 2));
        emit send_error(cfginfo.write_blank(t["xml"], 5, 2));
        cfginfo.close();
    }
    else
    {
        emit send_error_msg("can not load config.csv file\nprogram will not load version info to cfginfo.csv");
        return -1;
    }
    return 0;
}
//---------------------------------------------
/**
 * \brief 将csv文件转换为bin
 * \param input_path    输入文件的路径
 * \param output_path   输出文件的路径
 * \return    处理的csv文件的数量
 */
//---------------------------------------------
int CFileManager::trans_csv_to_bin()
{
    QString input_path = m_input_path;

#ifdef ONLY_CSV_FILE
    QString output_path = m_output_path;
#else
    QString output_path = m_input_path;
#endif

    // set version info to cfginfo csv file

    if(-1 == __write_version())
    {
        emit send_error(ERR_WRITE_VERSION_FAILED);
        return 1;
    }

    // add base address to bin files, at the front of every sector
    // notice that: the oder of bin files must be same with 'm_base_addr_list'

    int addr_cnt = 0;
    int cnt = 0;
    CBinFileProcessor bin_file;
    bin_file.new_bin(output_path + "/" + m_cfgtl_name);
    QStringList trans_csv_list = m_check_csv_list;
    // copy user param from factory bin file, then set user param base address to user param bin file
    trans_csv_list.append("factoryparam");
    foreach(QString str, trans_csv_list)
    {
        send_msg("begin to transform " + m_csv_list[str]);
#ifdef COPY_USE_CODE
        QByteArray data_buf;
        CGxConvertToBinToolCopy::ConverttoBinFlie(QString(input_path + "/" + m_csv_list[str]).toStdString(), data_buf);
#ifdef OUTPUT_SINGLE_BIN
        QString single_name = m_csv_list[str];
        single_name.replace(QRegExp(".csv"), ".bin");
        CBinFileProcessor single_bin(this, output_path + "/" + single_name);
        single_bin.new_bin(output_path + "/" + single_name);
        single_bin.append_bytes((uint8_t*)data_buf.data(), data_buf.size());
#endif

        if(0 == data_buf.size() % SECTOR_SIZE)
        {
            for(int m = 0; m < data_buf.size() / SECTOR_SIZE; m++)
            {
                const uint32_t sector_size = SECTOR_SIZE;
                bin_file.append_bytes((uint8_t*)&m_base_addr_list[addr_cnt], ADDR_SIZE);
                addr_cnt++;
                bin_file.append_bytes((uint8_t*)&sector_size, ADDR_SIZE);
                bin_file.append_bytes((uint8_t*)&data_buf.data()[m * SECTOR_SIZE], SECTOR_SIZE);
            }
        }
        else
        {
            if(str == "cfginfo")
            {
                const uint32_t sector_size = data_buf.size();
                bin_file.append_bytes((uint8_t*)&m_base_addr_list[addr_cnt], ADDR_SIZE);
                addr_cnt++;
                bin_file.append_bytes((uint8_t*)&sector_size, ADDR_SIZE);
                bin_file.append_bytes((uint8_t*)data_buf.data(), data_buf.size());
            }
            else
            {
                emit send_error(ERR_CSV_SIZE);
                emit send_error_msg("file size is not correct: " + str);
                return 1;
            }
        }
#else
        CCSVFileProcessor csvproc(this, input_path + "/" + m_csv_list[str]);
        qDebug() << "csv: " << m_csv_list[str];
        csvproc.open();
        while(!csvproc.upto_end())
        {
            uint32_t len;
            QVector<uint8_t> buf;
            ErrorTypeDef error = csvproc.get_sector(len, buf);
            if(ERR_NULL == error)
            {
                bin_file.append_bytes((uint8_t*)&m_base_addr_list.at(addr_cnt), ADDR_SIZE);
                addr_cnt++;
                bin_file.append_bytes(buf.data(), buf.size());
                qDebug() << "data size: " << buf.size();
            }
            else
            {
                emit send_error(error);
                send_error_msg(m_csv_list[str] + ": failed");
                csvproc.close();
                return cnt;
            }
        }
        csvproc.close();
#endif
        send_msg(m_csv_list[str] + QString(": completed"));
        cnt++;
    }
#ifdef ONLY_CSV_FILE
    send_msg("add MD5 header to cfgTL");
    send_error(ToolBox::setMD5(output_path, m_cfgtl_name));
#endif
    return cnt;
}
//---------------------------------------------
/**
 * \brief 对指定的文件进行打包处理
 * \param path    文件所在的路径
 * \return
 */
//---------------------------------------------
int CFileManager::make_package()
{
    QStringList file_list;    ///< collect all file, that need to package
    file_list << m_fpga_name << m_image_name << m_pallas_data_name << m_upgrade_name << m_cfgtl_name;
    QString package_name = __get_package_name(m_input_path);
    package_name = package_name.replace(QRegExp(" "), "\\ ");
    // add MD5 header to all file in 'file_list', if a file already has MD5 header, not add again
    chdir(m_output_path.toStdString().data());
    mkdir(QString(m_output_path + "/temp").toStdString().data(), 0777);

    foreach(QString file, file_list)
    {
        emit send_msg("add MD5 header to: " + file);
        system(QString("cp " + m_input_path + "/" + file + " " + m_output_path + "/temp/").toStdString().data());
        emit send_error(ToolBox::setMD5(m_output_path + "/temp", file));
    }
    // package all files, then move the package to 'output_paht'
    chdir(QString(m_output_path + "/temp").toStdString().data());
    QString cmd = "tar -zcf " + package_name;
    foreach(QString file, file_list)
    {
        cmd += QString(" " + file);
    }
    if(0 != system(cmd.toStdString().data()))
    {
        emit send_error(ERR_PACKAGE_FAILED);
        return -1;
    }
    cmd = "mv " + package_name + " " + m_output_path;
    system(cmd.toStdString().data());
    cmd = "rm -rf " + m_output_path + "/temp";
    system(cmd.toStdString().data());
    return 0;
}
//---------------------------------------------
/**
 * \brief 生成打包文件的名称
 * \param input_path    待打包文件所在的路径
 * \return    生成的打包文件的名称
 */
//---------------------------------------------
QString CFileManager::__get_package_name(const QString &input_path)
{
    QString package_name = "PallasModule#fpga#image#pallas#csv#dev.tar.gz";
    CCSVFileProcessor ver_table(this, input_path + "/" + m_config_name);
    ver_table.open();
    QMap<QString, QString> table;
    if(ERR_NULL == ver_table.get_table(table))
    {
        package_name.replace(QRegExp("fpga"), table[m_fpga_name.toLower()]);
        package_name.replace(QRegExp("dev"), table[m_dev_version_name.toLower()]);
        package_name.replace(QRegExp("pallas"), table[m_pallas_data_name.toLower()]);
        package_name.replace(QRegExp("csv"), table[m_cfgtl_name.toLower()]);
        package_name.replace(QRegExp("image"), table[m_image_name.toLower()]);
        package_name.replace(QRegExp("PallasModule"), table[m_pallas_module_name.toLower()]);
    }
    else
    {
        emit send_error_msg("config.csv error");
        return "";
    }
    ver_table.close();
    return package_name;
}
//---------------------------------------------
/**
 * \brief 检查文件是否存在
 * \param name_list 待检查的文件名称列表
 * \param reg 正则表达式规则
 * \return 与正则表达式规则相匹配的文件名称列表
 */
//---------------------------------------------
QStringList CFileManager::__check_file_exist(const QStringList &name_list, const QRegExp &reg)
{
    QStringList names;
    foreach(QString name, name_list)
    {
        if(-1 != name.toLower().indexOf(reg))
        {
            names.append(name);
        }
    }
    return names;
}
//---------------------------------------------
/**
 * \brief 检查CSV文件是否合法
 * \param dir  文件路径
 * \return   合法CSV文件数量
 */
//---------------------------------------------
int CFileManager::__check_csv_file(const QDir &dir)
{
    m_csv_list.clear();
    QStringList filter;
    filter << "*.csv";
    QDir d = dir;
    d.setNameFilters(filter);
    QStringList csvlist = d.entryList();
    QString csv_ver = "";
    QRegExp reg("[Vv]{1}[0-9.]{1,}");

    if(csvlist.size() > m_check_csv_list.size())
    {
        emit send_warning_msg(QString("find more than %1 CSV files, that may cause an error").arg(m_check_csv_list.size()));
    }
    // check if file is correct
    foreach(QString str, m_check_csv_list)
    {
        QStringList finds = __check_file_exist(csvlist, QRegExp(str));
        if(finds.size() == 1)
        {
            emit send_msg("find correct " + str + " CSV file");
            csvlist.removeOne(finds.at(0));
            m_csv_list[str] = finds.at(0);
            int begain = m_csv_list[str].indexOf(reg);

            if(-1 == begain)
            {
                emit send_error_msg("csv file name is not correct");
                break;
            }

            int end = m_csv_list[str].indexOf(QRegExp(".csv"));
            csv_ver = QString::fromStdString(m_csv_list[str].toStdString().substr(begain, end - begain));

            csv_ver.remove(QRegExp("[Vv]{1}"));
            reg.setPattern(csv_ver);
        }
        else if(finds.size() == 0)
        {
            emit send_warning_msg("can not find " + str + " CSV file");
        }
        else
        {
            emit send_warning_msg("there are too many " + str + " CSV file");
        }
    }
    CCSVFileProcessor* wver = new CCSVFileProcessor(this, m_input_path + "/" + m_config_name);
    if(ERR_NULL == wver->open())
    {
        wver->write_blank(csv_ver, "cfgTL");
        wver->close();
    }
    delete wver;

    return m_csv_list.size();
}
//---------------------------------------------
/**
 * \brief 检查FPGA文件是否合法
 * \param 待检查目录
 * \return true，合法   false，不合法
 */
//---------------------------------------------
bool CFileManager::__check_fpga_file(const QDir &dir)
{
    // m_fpga_name("fpga.bin")
    QDir d = dir;
    QStringList filter;
    filter << "*.bin";
    d.setNameFilters(filter);

    QStringList binlist = d.entryList();
    if(binlist.isEmpty())
    {
        emit send_error_msg("there are no FPGA.bin file, please check your path is correct!");
        return false;
    }
    QRegExp reg(m_fpga_name.toLower());
    QStringList finds = __check_file_exist(binlist, reg);

    if(finds.size() == 1)
    {
        m_fpga_name = finds.at(0);
        emit send_msg("find correct [ " + m_fpga_name + " ] file");

    }
    else if(finds.size() == 0)
    {
        emit send_error_msg("name of FPGA.bin file is not correct, e.g. FPGA#01.02.002.036#23.10_28.10_25.11.bin");
        return false;
    }
    else
    {
        emit send_error_msg("there are too many FPGA.bin file");
        return false;
    }

    return true;
}
//---------------------------------------------
/**
 * \brief 检查image文件是否合法
 * \param dir    待检查的目录
 * \return    true，合法  false，不合法
 */
//---------------------------------------------
bool CFileManager::__check_image_file(const QDir &dir)
{
    QDir d = dir;
    QStringList filter;
    filter << "*.ub";
    d.setNameFilters(filter);

    QStringList imagelist = d.entryList();
    if(imagelist.isEmpty())
    {
        emit send_warning_msg("there are no image file!");
        return false;
    }
    QRegExp reg(m_image_name.toLower());

    QStringList finds = __check_file_exist(imagelist, reg);
    if(finds.size() == 1)
    {
        m_image_name = finds.at(0);
        emit send_msg("find correct [ " + m_image_name + " ] file");
    }
    else if(finds.size() == 0)
    {
        emit send_warning_msg("name of image.ub file is not correct, please check!");
    }
    else
    {
        emit send_error_msg("there are too many image.ub file");
        return false;
    }
    return true;
}
//---------------------------------------------
/**
 * \brief 检查pallas_data文件是否合法
 * \param dir    文件目录
 * \return true，合法  false，不合法
 */
//---------------------------------------------
bool CFileManager::__check_pallas_data_file(const QDir &dir)
{
    QDir d = dir;
    QStringList filter;
    filter << "*.tar.gz";
    d.setNameFilters(filter);

    QStringList rootfslist = d.entryList();
    if(rootfslist.isEmpty())
    {
        emit send_warning_msg("there are no rootfs file!");
        return false;
    }
    //
    QRegExp reg(m_pallas_data_name.toLower());
    QStringList finds = __check_file_exist(rootfslist, reg);
    if(finds.size() == 1)
    {
        m_pallas_data_name = finds.at(0);
        emit send_msg("find correct [ " + m_pallas_data_name + " ] file!");
    }
    else if(finds.size() == 0)
    {
        emit send_warning_msg("name of rootfs.tar.gz file is not correct, please check!");
    }
    else
    {
        emit send_error_msg("there are too many rootfs.tar.gz file");
        return false;
    }
    return true;
}
//---------------------------------------------
/**
 * \brief 检查upgrade文件是否合法
 * \param dir   文件目录
 * \return true，合法    false，不合法
 */
//---------------------------------------------
bool CFileManager::__check_upgrade_file(const QDir &dir)
{
    QDir d = dir;
    QStringList filter;
    filter << "*.sh";
    d.setNameFilters(filter);

    QStringList installlist = d.entryList();
    if(installlist.isEmpty())
    {
        emit send_error_msg("there no upgrade.sh file, please check your path is correct!");
        return false;
    }
    QRegExp reg(m_upgrade_name.toLower());

    QStringList finds = __check_file_exist(installlist, reg);
    if(finds.size() == 1)
    {
        m_upgrade_name = finds.at(0);
        emit send_msg("find correct [ " + m_upgrade_name + " ] file");
    }
    else if(finds.size() == 0)
    {
        emit send_error_msg("name of upgrade.sh file is not correct, e.g. upgrade.sh");
        return false;
    }
    else
    {
        emit send_error_msg("there are too many upgrade.sh file");
        return false;
    }

    return true;
}
