//-------------------------------------------------------------------
/** 
 \file mainwindow.h 
 \brief UI controllor
 \version v1.1.1809.9251 
 \date 2018-9-25 
 \author Zhou Tong
 <p>Copyright (c) 2018-2020 China Daheng Group, Inc. Beijing Image 
 Vision Technology Branch and all right reserved.</p> 
 */ 
//-------------------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "cfilemanager.h"
#include "errorlist.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QTextEdit* m_text;
    QLineEdit* m_input_line;
    QPushButton* m_input_btn;
    QLineEdit* m_output_line;
    QPushButton* m_output_btn;
    QPushButton* m_start_btn;

    QMenu* m_file_menu;
    QAction* m_input_path_action;
    QAction* m_output_path_action;

    void __logout_msg(QString msg);
    void __logout_warning_msg(QString msg);
    void __logout_error_msg(QString msg);
    void __logout(QString str);

    void __create_menu();
    void __add_to_menubar();

    void __set_widget_enable(bool flag);

public slots:
    void get_input_path(void);
    void get_output_path(void);
    void start_transform(void);
    void error_collector(ErrorTypeDef error);
};

#endif // MAINWINDOW_H
