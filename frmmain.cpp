#include "frmmain.h"
#include "ui_frmmain.h"
#include "api/myhelper.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QByteArray>
#include "qrencode/qrencode.h"
#include <QSerialPortInfo>
#include "Barcode.h"
#include <QtDebug>


frmMain::frmMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->InitStyle();
    this->InitForm();
    barcode = new BarCode(this);
    barcode->setVisible(false);
    typedef QPair<BarCode::BarcodeTypes, QString> BarcodeType;
    BarCode::BarcodeTypePairList typeList = BarCode::getTypeList();
    port_param_init();
    myHelper::FormInCenter(this);
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::InitStyle()
{
    this->max = false;
    this->location = this->geometry();
    this->setProperty("Form", true);
    this->setProperty("CanMove", true);
    this->setWindowTitle("缇铭科技量产工具V1.0");
}

bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void frmMain::InitForm()
{
   this->rencode_text = "8A8F298391E1";
   this->rencode_text_2="E56CEE2E0CB";
   QRcode_Encode(this->rencode_text);
   QRcode_Encode_2(this->rencode_text_2);
   //设置调试窗口的字体大小
   ui->plainTextEdit->setFont(QFont( "宋体" , 10 ,  QFont::Normal) );
   log_output(tr("开机启动..."));
   QFile file("macAdress.xlsx");
   if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
   {
        qDebug()<<"open file failure";
   }
}
//显示条形码
void frmMain::QBarcode_ts102(QByteArray &text)
{
    ui->rencode_lineEdit->setText(text);
    if (this->rencode_text.trimmed().isEmpty())
        return;
    QImage barcodeImage(ui->rencode_view->size(), QImage::Format_ARGB32);
    barcodeImage.fill(QColor(255, 255, 255, 0));
    QPainter painter(&barcodeImage);
    barcode->setBarcodeType((BarCode::BarcodeTypes)60);
    barcode->setValue(this->rencode_text);
    barcode->drawBarcode(&painter, 0, 0, barcodeImage.width(), barcodeImage.height());
    painter.end();
    ui->rencode_view->setPixmap(QPixmap::fromImage(barcodeImage));
}
void frmMain::on_btnMenu_Max_clicked()
{

}
void frmMain::QPcode( QPrinter *printer,QPainter *painter,QByteArray &text)
{
    int margin = 1;//设置图像的页边距大小
    ui->rencode_lineEdit->setText(text);
    this->foreground = QColor("black");
    this->background = QColor("white");
    QRcode *qrcode = QRcode_encodeString(text.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(NULL != qrcode) {
        painter->begin(printer);
        //打印文字
        QPen pen;
        pen.setColor(QColor("#ff00ff"));
        pen.setWidth(2);
        painter->setPen(pen);

        QFont font;
        font.setBold(false);
        font.setPointSize(10);//设置字体大小
        font.setFamily("新宋体");
        painter->setFont(font);

        painter->drawText(QRect(0,0,60,10),Qt::AlignBottom,tr("缇铭科技"));
        painter->save();//缓存当前的坐标状态
        //画二维码
        unsigned char *point = qrcode->data;
        painter->restore();//恢复到之前save的状态
        painter->translate(0, 10);//坐标平移，向右是X，向下是Y
        painter->setPen(Qt::NoPen);
        painter->setBrush(this->background);
        painter->drawRect(30, 30, TWODIMENSION_SIZE, TWODIMENSION_SIZE);
        double scale = (TWODIMENSION_SIZE - 2.0 * margin) / qrcode->width;
        painter->setBrush(this->foreground);
        for (int y = 0; y < qrcode->width; y ++) {
            for (int x = 0; x < qrcode->width; x ++) {
                if (*point & 1) {
                    QRectF r(margin + x * scale, margin + y * scale, scale, scale);
                    painter->drawRects(&r, 1);
                }
                point ++;
            }
        }
        //打印文字
        //QPen pen;
        pen.setColor(QColor("#ff00ff"));
        pen.setWidth(2);
        painter->setPen(pen);

        //QFont font;
        font.setBold(false);
        font.setPointSize(6);//设置字体大小
        font.setFamily("新宋体");
        painter->setFont(font);

        painter->drawText(QRect(0,30,60,10),Qt::AlignBottom,text);
        painter->end();
        point = NULL;
        //把mac地址保存到文件里面
        QFile file("macAdress.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream stream( &file );
            stream << text << "\r\n";
            file.close();
        }
        else
        {
            log_output(tr("打开文件失败..."));
        }
        QRcode_free(qrcode);
    }
}
//二维码显示
void frmMain::QRcode_Encode(QByteArray &text)
{
    int margin = MARGIN_VALUE;
    ui->rencode_lineEdit->setText(text);
    this->foreground = QColor("black");
    this->background = QColor("white");
    QRcode *qrcode = QRcode_encodeString(text.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(NULL != qrcode) {
        QPixmap pixmap(IMAGE_SIZE,IMAGE_SIZE);//ui->rencode_view->width(), ui->rencode_view->height());
        QPainter painter;
        painter.begin(&pixmap);
        unsigned char *point = qrcode->data;
        painter.setPen(Qt::NoPen);
        painter.setBrush(this->background);
        painter.drawRect(0, 0, IMAGE_SIZE, IMAGE_SIZE);
        double scale = (IMAGE_SIZE - 2.0 * margin) / qrcode->width;
        painter.setBrush(this->foreground);
        for (int y = 0; y < qrcode->width; y ++) {
            for (int x = 0; x < qrcode->width; x ++) {
                if (*point & 1) {
                    QRectF r(margin + x * scale, margin + y * scale, scale, scale);
                    painter.drawRects(&r, 1);
                }
                point ++;
            }
        }
        ui->rencode_view->setPixmap(pixmap);
        painter.end();
        point = NULL;
        QRcode_free(qrcode);
    }
}
//二维码显示
void frmMain::QRcode_Encode_2(QByteArray &text)
{
    int margin = MARGIN_VALUE;
    ui->rencode_lineEdit_2->setText(text);
    this->foreground = QColor("black");
    this->background = QColor("white");
    QRcode *qrcode = QRcode_encodeString(text.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(NULL != qrcode) {
        QPixmap pixmap(IMAGE_SIZE,IMAGE_SIZE);//ui->rencode_view->width(), ui->rencode_view->height());
        QPainter painter;
        painter.begin(&pixmap);
        unsigned char *point = qrcode->data;
        painter.setPen(Qt::NoPen);
        painter.setBrush(this->background);
        painter.drawRect(0, 0, IMAGE_SIZE, IMAGE_SIZE);
        double scale = (IMAGE_SIZE - 2.0 * margin) / qrcode->width;
        painter.setBrush(this->foreground);
        for (int y = 0; y < qrcode->width; y ++) {
            for (int x = 0; x < qrcode->width; x ++) {
                if (*point & 1) {
                    QRectF r(margin + x * scale, margin + y * scale, scale, scale);
                    painter.drawRects(&r, 1);
                }
                point ++;
            }
        }
        ui->rencode_view_2->setPixmap(pixmap);
        painter.end();
        point = NULL;
        QRcode_free(qrcode);
    }
}
void frmMain::port_param_init()
{
    //init com buadrate
    ui->buadrate_comboBox->addItem(QLatin1String("300"));
    ui->buadrate_comboBox->addItem(QLatin1String("600"));
    ui->buadrate_comboBox->addItem(QLatin1String("1200"));
    ui->buadrate_comboBox->addItem(QLatin1String("2400"));
    ui->buadrate_comboBox->addItem(QLatin1String("4800"));
    ui->buadrate_comboBox->addItem(QLatin1String("9600"));
    ui->buadrate_comboBox->addItem(QLatin1String("19200"));
    ui->buadrate_comboBox->addItem(QLatin1String("38400"));
    ui->buadrate_comboBox->addItem(QLatin1String("115200"));
    ui->buadrate_comboBox->setCurrentIndex(8);

    // fill data bits
    ui->databits_comboBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->databits_comboBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->databits_comboBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->databits_comboBox->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->databits_comboBox->setCurrentIndex(3);

     // fill parity
    ui->parity_comboBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->parity_comboBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->parity_comboBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->parity_comboBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->parity_comboBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);

    // fill stop bits
    ui->stopbits_comboBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopbits_comboBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopbits_comboBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);

   check_serial_port();


  this->serialport = new QSerialPort(this);
  QObject::connect(this->serialport,SIGNAL(readyRead()),this,SLOT(serialport_recv()));


  QObject::connect(ui->check_port_Button,SIGNAL(clicked()),this,SLOT(check_serial_port()));
}
void frmMain::check_serial_port()
{
    ui->port_comboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        ui->port_comboBox->addItem(list.first(), list);
    }
}
void frmMain::FrameParse(char c)
{
    switch (state) {
    case 0:
        if(c=='#')
         {
           this->recv_arr.clear();
           state++;
         }

        break;
    case 1:
        if(c=='#')
        {
            this->rencode_text.clear();
            this->rencode_text = this->recv_arr;
            if(rencode_text.isEmpty()==false)
            {
            QRcode_Encode(this->rencode_text);//显示二维码
            //QBarcode_ts102(this->rencode_text);//显示条形码
            log_output(tr("解析成功"));
            }
            state = 0;
        }else
        {
           this->recv_arr.append(c);
        }

        break;
    default:
        state = 0;
        break;
    }

}
void frmMain::serialport_recv()
{
    QByteArray temp = this->serialport->readAll();
    if(temp.size() > 0)
    {
        for(int i = 0;i< temp.size();i++)
        {
            FrameParse(temp.at(i));
        }
    }
}

void frmMain::open_serialport()
{

        QString portName = ui->port_comboBox->currentText();
        qint32 baudRate = ui->buadrate_comboBox->currentText().toInt();
        QSerialPort::DataBits dataBits = static_cast<QSerialPort::DataBits>(ui->databits_comboBox->itemData(ui->databits_comboBox->currentIndex()).toInt());
        QSerialPort::StopBits stopBits = static_cast<QSerialPort::StopBits>(ui->stopbits_comboBox->itemData(ui->stopbits_comboBox->currentIndex()).toInt());
        QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(ui->parity_comboBox->itemData(ui->parity_comboBox->currentIndex()).toInt());
        this->serialport->setPortName(portName);
        this->serialport->setBaudRate(baudRate);
        this->serialport->setDataBits(dataBits);
        this->serialport->setStopBits(stopBits);
        this->serialport->setParity(parity);
        this->serialport->open(QIODevice::ReadWrite);
        ui->port_comboBox->setEnabled(false);
        ui->parity_comboBox->setEnabled(false);
        ui->stopbits_comboBox->setEnabled(false);
        ui->databits_comboBox->setEnabled(false);
        ui->buadrate_comboBox->setEnabled(false);


}
void frmMain::close_serialport()
{
        this->serialport->close();
        ui->port_comboBox->setEnabled(true);
        ui->parity_comboBox->setEnabled(true);
        ui->stopbits_comboBox->setEnabled(true);
        ui->databits_comboBox->setEnabled(true);
        ui->buadrate_comboBox->setEnabled(true);
}
void frmMain::on_open_port_Button_clicked()
{
    if(ui->open_port_Button->text()=="打开")
     {
        open_serialport();
        ui->open_port_Button->setText(tr("关闭"));
     }else
     {
        close_serialport();
        ui->open_port_Button->setText(tr("打开"));
    }
}
void frmMain::log_output(QString info)
{
    QString time= QDateTime::currentDateTime ().toString ("yyyy-MM-dd hh:mm:ss");
    ui->plainTextEdit->appendPlainText(tr("[%1] %2").arg (time).arg (info));
}
void frmMain::on_cboxStyle_currentIndexChanged(const QString &arg1)
{

}

void frmMain::plotPic(QPrinter *printer)
{
    log_output("开始打印...");
}

void frmMain::on_print_button_clicked()
{
    //二维码打印
    static int prinCount=0;
    QPrinter printer;
    QString printerName = printer.printerName();
    if(printerName.size()==0) return;
    QPrintDialog *dialog = new QPrintDialog(&printer);
    dialog->setWindowTitle("print Document");
    QPrintDialog dlg(&printer,this);
    QPainter painter;
    QPcode(&printer,&painter,this->rencode_text);
    prinCount++;
    log_output(tr("打印第 ")+QString::number(prinCount)+tr(" 次"));
    //条形码显示
//    QPrinter printer;
//    log_output("开始打印...");
//    if (this->rencode_text.trimmed().isEmpty())
//        return;
//    QPainter painter(&printer);
//    barcode->setBarcodeType((BarCode::BarcodeTypes)60);
//    barcode->setValue(this->rencode_text);
//    barcode->drawBarcode(&painter, 0, 0, 70,70);
//    painter.end();
}
void frmMain::on_prit_button_clicked()
{

}
