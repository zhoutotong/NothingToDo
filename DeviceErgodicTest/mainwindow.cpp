#include "mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setFixedSize(WIDTH, HEIGHT);
    QWidget *widget = new QWidget(this);
    this->setCentralWidget(widget);
    m_layout = new QVBoxLayout(widget);
    widget->setLayout(m_layout);

    m_table_header_list << "实际执行次数"\
                        << "执行成功次数"\
                        << "执行失败次数"\
                        << "最长执行时间"\
                        << "最短执行时间"\
                        << "平均执行时间"\
                        << "触发成功次数"\
                        << "触发失败次数"\
                        << "总采集帧数"\
                        << "异常帧数"\
                        << "残帧数"\
                        << "时间戳"\
                        << "时间戳差当前值"\
                        << "时间戳差最大值"\
                        << "时间戳差最小值"\
                        << "帧号"\
                        << "帧号异常"\
                        << "帧间隔当前值"\
                        << "帧间隔最大值"\
                        << "帧间隔最小值"\
                        << "Sensor型号"\
                        << "BlockID丢失个数"\
                        << "Buffer不足导致丢帧个数";

    setupUI();

    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    uint32_t nDeviceNum = 0;
    emStatus = GXUpdateAllDeviceList(&nDeviceNum, 500);
    qDebug() << "device num: " << nDeviceNum;
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUI()
{
    QVBoxLayout *img_show_layout = new QVBoxLayout();
    QVBoxLayout *ctr_layout      = new QVBoxLayout();
    QVBoxLayout *param_layout    = new QVBoxLayout();

    // setup image display ui
    m_show_img = new QLabel();
    m_show_img->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    img_show_layout->addWidget(m_show_img);
    QLabel *info_fps = new QLabel("当前采集帧率");
    m_fps_line = new QLineEdit();
    m_fps_line->setEnabled(false);
    QLabel *blank = new QLabel();
    blank->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_display_img_checkbox = new QCheckBox("是否显示图像");
    m_display_img_checkbox->setChecked(false);
    QHBoxLayout *img_info_layout = new QHBoxLayout();
    img_info_layout->addWidget(info_fps);
    img_info_layout->addWidget(m_fps_line);
    img_info_layout->addWidget(blank);
    img_info_layout->addWidget(m_display_img_checkbox);

    img_show_layout->addWidget(m_show_img);
    img_show_layout->addLayout(img_info_layout);



    // setup control ui
    // setup enum control ui
    QPushButton *enum_dev_btn  = new QPushButton("枚举设备");       ///< enum device button
    QPushButton *open_dev_btn  = new QPushButton("打开设备");       ///< open device button
    QPushButton *close_dev_btn = new QPushButton("关闭设备");      ///< close device button
    m_device_list_combobox = new QComboBox();
    QGridLayout *enum_ctr_layout = new QGridLayout();
    enum_ctr_layout->addWidget(enum_dev_btn, 0, 0);
    enum_ctr_layout->addWidget(open_dev_btn, 1, 0);
    enum_ctr_layout->addWidget(close_dev_btn, 1, 1);
    enum_ctr_layout->addWidget(m_device_list_combobox, 0, 1);
    ctr_layout->addLayout(enum_ctr_layout);

    // setup base test control ui
    QGroupBox *base_test_group = new QGroupBox("测试基础配置");
    QGridLayout *base_test_layout = new QGridLayout();
    base_test_group->setLayout(base_test_layout);
    m_is_trigger_signal = new QCheckBox("是否需要触发信号");
    QLabel *base_test_info_time = new QLabel("时钟周期");
    base_test_info_time->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_time_cycle = new QLineEdit();
    QLabel *base_test_info_acq_mode = new QLabel("采集模式");
    m_acquisition_mode = new QComboBox();
    m_is_image_detect = new QCheckBox("是否需要图像检测");
    m_is_stop_to_set = new QCheckBox("是否需要停止采集后设置");
    base_test_layout->addWidget(m_is_trigger_signal, 0, 0);
    base_test_layout->addWidget(base_test_info_time, 0, 1);
    base_test_layout->addWidget(m_time_cycle, 0, 2);

    base_test_layout->addWidget(base_test_info_acq_mode, 2, 0, Qt::AlignRight | Qt::AlignVCenter);
    base_test_layout->addWidget(m_acquisition_mode, 2, 1, 1, 2);

    base_test_layout->addWidget(m_is_image_detect, 3, 0, 1, 2);
    base_test_layout->addWidget(m_is_stop_to_set, 4, 0, 1, 2);

    ctr_layout->addWidget(base_test_group);


    // setup test item
    QGroupBox *test_item_group = new QGroupBox("测试项选择");
    QGridLayout *test_item_layout = new QGridLayout();
    test_item_group->setLayout(test_item_layout);
    QLabel *test_item_info_test = new QLabel("测试项");
    m_test_item = new QComboBox();
    QLabel *test_item_info_value = new QLabel("当前值/状态");
    m_value_status = new QLineEdit();
    test_item_layout->addWidget(test_item_info_test, 0, 0);
    test_item_layout->addWidget(m_test_item, 0, 1);
    test_item_layout->addWidget(test_item_info_value, 1, 0);
    test_item_layout->addWidget(m_value_status, 1, 1);

    ctr_layout->addWidget(test_item_group);

    // set test parameter
    QTabWidget *test_param_tab_widget = new QTabWidget;
    QWidget *ergodic_range_widget = new QWidget();
    test_param_tab_widget->addTab(ergodic_range_widget, "遍历范围");
    QGridLayout *ergodic_range_layout = new QGridLayout;
    ergodic_range_widget->setLayout(ergodic_range_layout);

    m_ergodic_max = new QLineEdit();
    m_ergodic_min = new QLineEdit();
    m_is_multiple_ralation = new QCheckBox("是否使用倍数关系");
    m_inc_mul = new QLineEdit();

    ergodic_range_layout->addWidget(new QLabel("Min"), 0, 0);
    ergodic_range_layout->addWidget(m_ergodic_min, 0, 1);
    ergodic_range_layout->addWidget(new QLabel("Max"), 1, 0);
    ergodic_range_layout->addWidget(m_ergodic_max, 1, 1);
    ergodic_range_layout->addWidget(m_is_multiple_ralation, 2, 0, 1, -1);
    ergodic_range_layout->addWidget(new QLabel("Inc/Mul"), 3, 0);
    ergodic_range_layout->addWidget(m_inc_mul, 3, 1);

    // set ergodic mode
    m_ascending_traversing_btn = new QRadioButton("升序遍历");
    m_decending_traversing_btn = new QRadioButton("降序遍历");
    m_random_traversing_btn = new QRadioButton("随机遍历");
    m_ergodic_mode_btn_group = new QButtonGroup(this);
    m_ergodic_mode_btn_group->addButton(m_ascending_traversing_btn);
    m_ergodic_mode_btn_group->addButton(m_decending_traversing_btn);
    m_ergodic_mode_btn_group->addButton(m_random_traversing_btn);
    m_ergodic_mode_btn_group->setExclusive(true);
    QWidget *ergodic_mode_widget = new QWidget();
    test_param_tab_widget->addTab(ergodic_mode_widget, "遍历方式");
    QVBoxLayout *ergodic_mode_layout = new QVBoxLayout();
    ergodic_mode_widget->setLayout(ergodic_mode_layout);
    ergodic_mode_layout->addWidget(m_ascending_traversing_btn);
    ergodic_mode_layout->addWidget(m_decending_traversing_btn);
    ergodic_mode_layout->addWidget(m_random_traversing_btn);

    // set traversing interval and times
    QWidget *traversing_interval_times = new QWidget();
    test_param_tab_widget->addTab(traversing_interval_times, "遍历间隔及次数");
    QGridLayout *traversing_interval_times_layout = new QGridLayout();
    traversing_interval_times->setLayout(traversing_interval_times_layout);
    m_is_random_interval = new QCheckBox("是否选择随机间隔");
    m_min_interval = new QLineEdit();
    m_max_interval = new QLineEdit();
    m_traversing_interval = new QLineEdit();
    m_traversing_times = new QLineEdit();
    traversing_interval_times_layout->addWidget(m_is_random_interval, 0, 0, 1, -1);
    traversing_interval_times_layout->addWidget(new QLabel("Min"), 1, 0);
    traversing_interval_times_layout->addWidget(m_min_interval, 1, 1);
    traversing_interval_times_layout->addWidget(new QLabel("Max"), 1, 2);
    traversing_interval_times_layout->addWidget(m_max_interval, 1, 3);
    traversing_interval_times_layout->addWidget(new QLabel("遍历间隔"), 2, 0);
    traversing_interval_times_layout->addWidget(m_traversing_interval, 2, 1);
    traversing_interval_times_layout->addWidget(new QLabel("遍历次数"), 3, 0);
    traversing_interval_times_layout->addWidget(m_traversing_times, 3, 1);

    ctr_layout->addWidget(test_param_tab_widget);

    // setup test control button
    QHBoxLayout *test_control_layout = new QHBoxLayout();
    ctr_layout->addLayout(test_control_layout);
    m_begin_test_btn = new QPushButton("测试开始");
    m_stop_test_btn = new QPushButton("测试结束");
    m_clear_count_info_btn = new QPushButton("清楚统计信息");
    test_control_layout->addWidget(m_begin_test_btn);
    test_control_layout->addWidget(m_stop_test_btn);
    test_control_layout->addWidget(m_clear_count_info_btn);

    // setup all layout to main layout
    QHBoxLayout *first_row_layout = new QHBoxLayout();
    first_row_layout->addLayout(img_show_layout);
    first_row_layout->addLayout(ctr_layout);
    QVBoxLayout *second_row_layout = new QVBoxLayout();
    second_row_layout->addLayout(param_layout);
    m_layout->addLayout(first_row_layout);


    m_test_data_table = new QTableWidget(5, m_table_header_list.size(), this);
    m_test_data_table->setHorizontalHeaderLabels(m_table_header_list);
    second_row_layout->addWidget(m_test_data_table);
    m_layout->addLayout(second_row_layout);
}
