#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>
#include <QPainter>
#include <QSerialPort>
#include <QPrinter>

class BarCode;
namespace Ui
{
    class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    BarCode *barcode;
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_btnMenu_Max_clicked();
    void check_serial_port();
    void serialport_recv();
    void on_cboxStyle_currentIndexChanged(const QString &arg1);

    void on_open_port_Button_clicked();
    void on_prit_button_clicked();

    void on_print_button_clicked();
    void plotPic(QPrinter *printer);

private:
    Ui::frmMain *ui;

    void InitStyle();   //初始化无边框窗体
    void InitForm();    //初始化窗体数据
    void port_param_init();
    void open_serialport();
    void close_serialport();
    void QRcode_Encode(QByteArray &text);
    void QRcode_Encode_2(QByteArray &text);
    void QBarcode_ts102(QByteArray &text);//条形码显示
    void QPcode(QPrinter *printer,QPainter *painter,QByteArray &text);
    bool max;           //是否处于最大化状态
    QRect location;     //鼠标移动窗体后的坐标位置
    QColor foreground;
    QColor background;
    QByteArray rencode_text;//记录MAC地址
    QByteArray rencode_text_2;//记录第一个MAC地址
    quint8 state=0;
    QByteArray recv_arr;
    void log_output(QString info);
    void FrameParse(char c);
    QSerialPort *serialport;
    #define TWODIMENSION_SIZE 30 //定义打印二维码图像大小
    #define IMAGE_SIZE 200 //定义在上位机显示二维码大小
    #define MARGIN_VALUE 0
};

#endif // FRMMAIN_H
