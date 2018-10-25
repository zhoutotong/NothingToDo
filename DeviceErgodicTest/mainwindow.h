#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTableWidget>

#include "../include/GxIAPI.h"

#define WIDTH 960
#define HEIGHT 640

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QVBoxLayout *m_layout;

    // image show
    QLabel *m_show_img;
    QLineEdit *m_fps_line;
    QCheckBox *m_display_img_checkbox;

    // enum device control
    QComboBox *m_device_list_combobox; ///< list of device

    // base test control
    QCheckBox *m_is_trigger_signal;
    QLineEdit *m_time_cycle;
    QComboBox *m_acquisition_mode;
    QCheckBox *m_is_image_detect;
    QCheckBox *m_is_stop_to_set;

    // test item
    QComboBox *m_test_item;
    QLineEdit *m_value_status;

    // ergodic range control
    QLineEdit *m_ergodic_min;
    QLineEdit *m_ergodic_max;
    QCheckBox *m_is_multiple_ralation;
    QLineEdit *m_inc_mul;

    // ergodic mode control
    QRadioButton *m_ascending_traversing_btn;
    QRadioButton *m_decending_traversing_btn;
    QRadioButton *m_random_traversing_btn;
    QButtonGroup *m_ergodic_mode_btn_group;

    // traversing interval and times control
    QCheckBox *m_is_random_interval;
    QLineEdit *m_min_interval;
    QLineEdit *m_max_interval;
    QLineEdit *m_traversing_interval;
    QLineEdit *m_traversing_times;

    // test control button
    QPushButton *m_begin_test_btn;
    QPushButton *m_stop_test_btn;
    QPushButton *m_clear_count_info_btn;

    // test data info table widget
    QTableWidget *m_test_data_table;

    QStringList m_table_header_list;

    void setupUI();
};

#endif // MAINWINDOW_H
