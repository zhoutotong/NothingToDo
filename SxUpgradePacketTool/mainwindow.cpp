//--------------------------------------------------------------- 
/** 
\file  mainwindows.cpp
\brief UI controllor
\version v1.1.1809.9251 
\date 2018-9-25 
\author Zhou Tong
<p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
Vision Technology Branch and all right reserved.</p> 
*/ 

//---------------------------------------------------------------

#include "mainwindow.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QTime>
#include <QDebug>
#include <QFileDialog>

#include "errorlist.h"

#include "ccsvfileprocessor.h"
#include "cfilemanager.h"
//---------------------------------------------
/**
 * \brief MainWindow::MainWindow
 * \param parent
 */
//---------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(640, 320);
    QWidget* widget = new QWidget;
    this->setCentralWidget(widget);
    QGridLayout* layout = new QGridLayout;
    widget->setLayout(layout);

    // add text logout to layout
    m_text = new QTextEdit();
    m_text->setReadOnly(true);
    layout->addWidget(m_text, 0, 0);

    // setup path layout
    QGridLayout* path_layout = new QGridLayout;

    QLabel* input_label = new QLabel("input path:");
    m_input_line = new QLineEdit();
    m_input_line->setText(QDir().absolutePath());
    m_input_btn = new QPushButton("open");

    path_layout->addWidget(input_label, 0, 0);
    path_layout->addWidget(m_input_line, 0, 1);
    path_layout->addWidget(m_input_btn, 0, 2);

    QLabel* output_label = new QLabel("output path:");
    m_output_line = new QLineEdit();
    m_output_line->setText(QDir().absolutePath());
    m_output_btn = new QPushButton("open");
    path_layout->addWidget(output_label, 1, 0);
    path_layout->addWidget(m_output_line, 1, 1);
    path_layout->addWidget(m_output_btn, 1, 2);

    // add path layout to layout
    layout->addLayout(path_layout, 1, 0);

    // add start button to layout
    m_start_btn = new QPushButton("start");
    QLabel* space = new QLabel();
    space->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout* ctr_layout = new QHBoxLayout;
    ctr_layout->addWidget(space);
    ctr_layout->addWidget(m_start_btn);
    layout->addLayout(ctr_layout, 2, 0);

    connect(m_input_btn, &QPushButton::clicked, this, &MainWindow::get_input_path);
    connect(m_output_btn, &QPushButton::clicked, this, &MainWindow::get_output_path);
    connect(m_start_btn, &QPushButton::clicked, this, &MainWindow::start_transform);

    __create_menu();
    __add_to_menubar();

    connect(m_input_path_action, &QAction::triggered, this, &MainWindow::get_input_path);
    connect(m_output_path_action, &QAction::triggered, this, &MainWindow::get_output_path);

}
//---------------------------------------------
/**
 * \brief MainWindow::~MainWindow
 */
//---------------------------------------------
MainWindow::~MainWindow()
{
}

//---------------------------------------------
/**
  \brief  创建菜单中的QAction
  \param  void

  \return void
  */
//---------------------------------------------
void MainWindow::__create_menu()
{
    m_output_path_action = new QAction(tr("output"));
    m_input_path_action = new QAction(tr("input"));
}
//---------------------------------------------
/**
 * /brief MainWindow::__add_to_menubar
 */
//---------------------------------------------
void MainWindow::__add_to_menubar()
{
    m_file_menu = menuBar()->addMenu(tr("file"));
    m_file_menu->addAction(m_output_path_action);
    m_file_menu->addAction(m_input_path_action);
}
//---------------------------------------------
/**
 * @brief MainWindow::__set_widget_enable
 * @param flag
 */
//---------------------------------------------
void MainWindow::__set_widget_enable(bool flag)
{
     m_start_btn->setEnabled(flag);
     m_input_btn->setEnabled(flag);
     m_input_line->setEnabled(flag);
     m_input_path_action->setEnabled(flag);
     m_output_btn->setEnabled(flag);
     m_output_line->setEnabled(flag);
     m_output_path_action->setEnabled(flag);
}

//---------------------------------------------
/**
 * \brief MainWindow::error_collector
 * \param error
 *
 * \return void
 */
//---------------------------------------------

void MainWindow::error_collector(ErrorTypeDef error)
{
    LOG_ERROR((ErrorTypeDef)error)
    ADD_ERROR(ERR_CSV_NOT_COMPLETE,        "CSV file is not complete");
    ADD_ERROR(ERR_CSV_FORMAT_ERROR,        "CSV file format error");
    ADD_ERROR(ERR_CSV_FORMAT_NO_TOTAL,     "missing total value");
    ADD_ERROR(ERR_CSV_FORMAT_NO_WRITE,     "missing write value");
    ADD_ERROR(ERR_CSV_WRITE_VALUE,         "CSV file write value error");
    ADD_ERROR(ERR_CSV_BUF_LENGTH,          "CSV file write length error");
    ADD_ERROR(ERR_CSV_NO_CRC,              "CSV missing CRC");
    ADD_ERROR(ERR_CSV_ROOTFS_NOT_COMPLETE, "only CSV file or only RootFS file");
    ADD_ERROR(ERR_CSV_VERSION_NOT_MATCH,   "The version of CSV file is not match");
    ADD_ERROR(ERR_CSV_NOT_OPEN,            "CSV file is not open, please check");
    ADD_ERROR(ERR_MD5_FAILED,              "add MD5 header failed");
    ADD_ERROR(ERR_PACKAGE_FAILED,          "make package failed");
    ADD_ERROR(WAR_MD5_EXIST,               "file already has a MD5 header");
    ADD_ERROR(ERR_CSV_SIZE,                "csv file size is not correct");
    END_LOG_ERROR;
}
//---------------------------------------------
/**
 * \brief MainWindow::__logout_msg
 * \param msg
 */
//---------------------------------------------
void MainWindow::__logout_msg(QString msg)
{
    msg = QTime::currentTime().toString("[hh:mm:ss.zzz]: ") + msg;
    QStringList lines = msg.split("\n");
    QString html;
    foreach(QString str, lines)
    {
        html += QString("<p style=\"margin:8pt;color:black;\">%1</p>").arg(str);
    }
    __logout(html);
}
//---------------------------------------------
/**
 * \brief MainWindow::__logout_warning_msg
 * \param msg
 */
//---------------------------------------------
void MainWindow::__logout_warning_msg(QString msg)
{
    msg = QTime::currentTime().toString("[hh:mm:ss.zzz]: ") + msg;
    QStringList lines = msg.split("\n");
    QString html;
    foreach(QString str, lines)
    {
        html += QString("<p style=\"margin:8pt;color:blue;\">%1</p>").arg(str);
    }

    __logout(html);
}
//---------------------------------------------
/**
 * \brief MainWindow::__logout_error_msg
 * \param msg
 */
//---------------------------------------------
void MainWindow::__logout_error_msg(QString msg)
{
    msg = QTime::currentTime().toString("[hh:mm:ss.zzz]: ") + msg;
    QStringList lines = msg.split("\n");
    QString html;
    foreach(QString str, lines)
    {
        html += QString("<p style=\"margin:8pt;color:red;\">%1</p>").arg(str);
    }

    __logout(html);
}
//---------------------------------------------
/**
 * \brief MainWindow::__logout
 * \param str
 */
//---------------------------------------------
void MainWindow::__logout(QString str)
{
    m_text->append(str);
}
//---------------------------------------------
/**
 * \brief MainWindow::get_input_path
 */
//---------------------------------------------
void MainWindow::get_input_path()
{
    QString path = m_input_line->text();
    if(!QDir(path).exists())
    {
        path = QDir().absolutePath();
    }
    path = QFileDialog::getExistingDirectory(this, "set input file path", path);
    if(!path.isEmpty())
    {
        m_input_line->setText(path);
    }
}
//---------------------------------------------
/**
 * \brief MainWindow::get_output_path
 */
//---------------------------------------------
void MainWindow::get_output_path()
{
    QString path = m_output_line->text();
    if(!QDir(path).exists())
    {
        path = QDir().absolutePath();
    }
    path = QFileDialog::getExistingDirectory(this, "set output file path", path);
    if(!path.isEmpty())
    {
        m_output_line->setText(path);
    }
}
//---------------------------------------------
/**
 * \brief MainWindow::start_transform
 */
//---------------------------------------------
void MainWindow::start_transform(void)
{
    __set_widget_enable(false);
    __logout_msg("check folder...");
    CFileManager* file = new CFileManager(this, m_input_line->text(), m_output_line->text());
    connect(file, &CFileManager::send_msg, this, &MainWindow::__logout_msg);
    connect(file, &CFileManager::send_error_msg, this, &MainWindow::__logout_error_msg);
    connect(file, &CFileManager::send_warning_msg, this, &MainWindow::__logout_warning_msg);
    connect(file, &CFileManager::send_error, this, &MainWindow::error_collector);

    if(!file->check_file())
    {
        __logout_error_msg("please check all files are correct!");
        __set_widget_enable(true);
        disconnect(file, 0, 0, 0);
        delete file;
        return;
    }
    __logout_msg("begin to transform CSV file(s) to Bin file(s)");
    int bin_num = file->trans_csv_to_bin();
    if(bin_num == 0)
    {
        __logout_msg("no csv file processed!");
    }
    else if(bin_num == 5)
    {
        __logout_msg("All csv file processed!");

    }
    else
    {
        __logout_error_msg("error during csv file processed!");
        __set_widget_enable(true);
        disconnect(file, 0, 0, 0);
        delete file;
        return;
    }
#ifndef ONLY_CSV_FILE
    __logout_msg("package all files...");
    if(-1 == file->make_package())
    {
        __logout_error_msg("package error");
    }
    else
    {
        __logout_msg("package done!");
    }

#endif
    __logout_msg("all done!");
    disconnect(file, 0, 0, 0);
    delete file;
    __set_widget_enable(true);
}
