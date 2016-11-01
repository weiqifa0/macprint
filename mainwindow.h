#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QSerialPort>
#include <QPrinter>

class BarCode;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    BarCode *barcode;
private slots:
    //void check_serial_port();
    //void serialport_recv();

    //void on_open_port_Button_clicked();

    void on_print_button_clicked();
    //void plotPic(QPrinter *printer);

    //void on_rencode_lineEdit_2_textChanged(const QString &arg1);

    //void on_checkBox_clicked(bool checked);

    //void on_textEdit_textChanged();

    void on_plainTextEdit_textChanged();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    void InitStyle();   //初始化无边框窗体
    void InitForm();    //初始化窗体数据
    void port_param_init();
    void open_serialport();
    void close_serialport();
    void QRcode_Encode(QByteArray &text);
    void QRcode_Encode_2(QByteArray &text);
    void QBarcode_ts102(QByteArray &text);//条形码显示
    void QPcode(QPrinter *printer,QPainter *painter,QByteArray &text);//单张二维码打印
    void QPcode_2(QPrinter *printer,QPainter *painter,QByteArray &text,QByteArray &text_2);//两张二维码打印
    char IntToStr(char aChar);
    char StrToInt(char aChar);
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
    #define PAPER_WIDTH 40
    #define PAPER_HIGHT 30
    #define IMAGE_SIZE 350 //定义在上位机显示二维码大小
    /***************************************
     * 设置viewsize windonwsize
     * ************************************/
    #define VIEWSIZE_WITH 40
    #define VIEWWIZE_HIGHT 30
    #define WINDONWSIZE_WITH 40
    #define WINDONWSIZE_HIGHT 30

    /***************************************
     * 两张规格
     * 上边距：1.89mm   左边距：2.29mm  二维码大小13mmx13mm
     * *************************************/
    #define D_MARGIN_VALUE 0
    #define D_DPI 25.5 //毫米和像素的转换
    #define D_PARAMETER 3.775//差值，乘以这个差值才是正确的mm
    #define D_TWODIMENSION_SIZE 12*D_PARAMETER //定义打印二维码图像大小
    #define D_LEFT_MARGIN 1.0*D_PARAMETER  //左边距2.29mm
    #define D_UP_MARGIN 0.5*D_PARAMETER //上边距
    #define D_TWODIMENSION_INTERVAL 5.0*D_PARAMETER //左右二维码的中间间隔
    #define D_PITURE_B_TEXT 1.7*D_PARAMETER //二维码和文字的距离
    #define D_TEXT_SIZE 4//二维码下面的字体大小
    /***************************************
     * 单张规格：
     * 上边距：1.86mm   左边距：1.67mm  二维码到文字的距离：1.7mm 二维码大小：12mmx12mm
     * *************************************/
    #define S_MARGIN_VALUE 0
    #define S_DPI 25.4
    #define S_PARAMETER 3.775
    #define S_UP_MARGIN 4.0*S_PARAMETER
    #define S_LEFT_MARGIN 10.0*S_PARAMETER   //图片的左边距
    #define S_LEFT_MARGIN_TEXT 10.0*S_PARAMETER //文字的左边距
    #define S_TEXT_TWODIMENSION 1.7*S_PARAMETER
    #define S_TWODIMENSION_SIZE 20*S_PARAMETER //定义二维码大小 20MM
    #define S_PITURE_B_TEXT 3.0*D_PARAMETER //二维码和文字的距离
    #define S_TEXT_SIZE 7 //字体大小

    #define READ_MAC_CMD "nrfjprog.exe --memrd 0x100000a3 --n 7"
    #define RESET_CMD "nrfjprog.exe -f NRF52 --reset"
    #define EXE_NAME "发货打包扫码打印工具V1.2"
    #define CMD_OFFSIZE 4
};

#endif // MAINWINDOW_H
