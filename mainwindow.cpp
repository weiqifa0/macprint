#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QByteArray>
#include "qrencode/qrencode.h"
#include <QSerialPortInfo>
#include "Barcode.h"
#include <QtDebug>
#include <QDateTime>
#include <QProcess>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->InitStyle();
    this->InitForm();
    barcode = new BarCode(this);
    barcode->setVisible(false);
    typedef QPair<BarCode::BarcodeTypes, QString> BarcodeType;
    BarCode::BarcodeTypePairList typeList = BarCode::getTypeList();
    port_param_init();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::InitStyle()
{
    this->location = this->geometry();
    this->setProperty("Form", true);
    this->setProperty("CanMove", true);
    this->setWindowTitle(EXE_NAME);
}


void MainWindow::InitForm()
{
   this->rencode_text  ="";
   this->rencode_text_2="";
   QRcode_Encode(this->rencode_text);
   QRcode_Encode_2(this->rencode_text_2);

   //设置调试窗口的字体大小
   ui->plainTextEdit->setFont(QFont( "宋体" , 10 ,  QFont::Normal) );
   log_output(tr("启动..."));
   QFile file("macAdress.txt");
   if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
   {
        qDebug()<<"open file failure";
   }
   if(ui->checkBox->isChecked())
   {
       ui->rencode_lineEdit_2->setVisible(true);
       ui->rencode_view_2->setVisible(true);
   }
   else
   {
       ui->rencode_lineEdit_2->setVisible(false);
       ui->rencode_view_2->setVisible(false);
   }
   ui->groupBox_2->setVisible(false);//隐藏串口设置控件
}
//显示条形码
void MainWindow::QBarcode_ts102(QByteArray &text)
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

void MainWindow::QPcode( QPrinter *printer,QPainter *painter,QByteArray &text)
{
    QPen pen;
    QFont font;
    int margin = 1;//设置图像的页边距大小
    ui->rencode_lineEdit->setText(text);
    this->foreground = QColor("black");
    this->background = QColor("white");

    QRcode *qrcode = QRcode_encodeString(text.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(NULL != qrcode) {
        painter->begin(printer);
        QRect pRect = painter->viewport();
        painter->setViewport(pRect.x(),pRect.y(),VIEWSIZE_WITH,VIEWWIZE_HIGHT);
        painter->setWindow(pRect.x(),pRect.y(),WINDONWSIZE_WITH,WINDONWSIZE_HIGHT);
        painter->translate(0, 0);//先回到坐标原点
        painter->save();
        //画二维码
        unsigned char *point = qrcode->data;
        painter->translate(S_LEFT_MARGIN, S_UP_MARGIN);//坐标平移，向右是X，向下是Y
        painter->setPen(Qt::NoPen);
        painter->setBrush(this->background);
        painter->drawRect(0, 0, S_TWODIMENSION_SIZE, S_TWODIMENSION_SIZE);
        double scale = (S_TWODIMENSION_SIZE - 2.0 * margin) / qrcode->width;
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
        painter->restore();
        painter->translate(S_LEFT_MARGIN_TEXT, S_TWODIMENSION_SIZE+S_UP_MARGIN+S_PITURE_B_TEXT);
        pen.setColor(QColor("#ff00ff"));
        pen.setWidth(1);
        painter->setPen(pen);
        font.setBold(false);
        font.setPointSize(S_TEXT_SIZE);//设置字体大小
        font.setFamily("新宋体");
        painter->setFont(font);
        painter->drawText(0,0,text);
        //结束打印
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
void MainWindow::QPcode_2( QPrinter *printer,QPainter *painter,QByteArray &text,QByteArray &text_2)
{
    int margin = D_MARGIN_VALUE;//设置图像的页边距大小
    ui->rencode_lineEdit->setText(text);
    this->foreground = QColor("black");
    this->background = QColor("white");
    QPen pen;
    QFont font;

    printer->logicalDpiX();

    QRcode *qrcode = QRcode_encodeString(text.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(NULL != qrcode) {
        painter->begin(printer);
        QRect pRect = painter->viewport();
        painter->setViewport(pRect.x(),pRect.y(),VIEWSIZE_WITH,VIEWWIZE_HIGHT);
        painter->setWindow(pRect.x(),pRect.y(),WINDONWSIZE_WITH,WINDONWSIZE_HIGHT);
        painter->translate(0,0);//移动到原点
        painter->save();//缓存当前的坐标状态
        qDebug("printer.x %d printer.y %d",printer->pageRect().x(),printer->pageRect().y());
        qDebug("printer.w %d printer.h %d",printer->pageRect().width(),printer->pageRect().height());
        //画二维码
        unsigned char *point = qrcode->data;
        painter->translate(D_LEFT_MARGIN,D_UP_MARGIN);//坐标平移，向右是X，向下是Y
        painter->setPen(Qt::NoPen);
        painter->setBrush(this->background);
        painter->drawRect(0, 0, D_TWODIMENSION_SIZE, D_TWODIMENSION_SIZE);
        double scale = (D_TWODIMENSION_SIZE - 2.0 * margin) / qrcode->width;
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
        painter->restore();//回到save的位置
        painter->save();//缓存当前的坐标状态
        painter->translate(D_LEFT_MARGIN,D_TWODIMENSION_SIZE+D_UP_MARGIN+D_PITURE_B_TEXT);
        pen.setColor(QColor("#ff00ff"));
        pen.setWidth(1);
        painter->setPen(pen);
        font.setBold(false);
        font.setPointSize(D_TEXT_SIZE);//设置字体大小
        font.setFamily("新宋体");
        painter->setFont(font);
        painter->drawText(0,0,text);

        qDebug("printer.x %d printer.y %d",printer->pageRect().x(),printer->pageRect().y());
        qDebug("printer.w %d printer.h %d",printer->pageRect().width(),printer->pageRect().height());
        //打印二维码
        painter->restore();//回到save的位置
        painter->save();//缓存当前的坐标状态
        painter->translate(D_LEFT_MARGIN+D_TWODIMENSION_SIZE+D_TWODIMENSION_INTERVAL,D_UP_MARGIN);//坐标平移，向右是X，向下是Y
        QRcode *qrcode2 = QRcode_encodeString(text_2.data(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
        if(NULL != qrcode2) {
            unsigned char *point = qrcode2->data;
            painter->setPen(Qt::NoPen);
            painter->setBrush(this->background);
            painter->drawRect(0, 0, D_TWODIMENSION_SIZE, D_TWODIMENSION_SIZE);
            double scale = (D_TWODIMENSION_SIZE - 2.0 * margin) / qrcode2->width;
            painter->setBrush(this->foreground);
            for (int y = 0; y < qrcode2->width; y ++) {
                for (int x = 0; x < qrcode2->width; x ++) {
                    if (*point & 1) {
                        QRectF r(margin + x * scale, margin + y * scale, scale, scale);
                        painter->drawRects(&r, 1);
                    }
                    point ++;
                }
            }
        }
        //打印文字
        painter->restore();//回到save的位置
        painter->save();//缓存当前的坐标状态
        painter->translate(D_LEFT_MARGIN+D_TWODIMENSION_SIZE+D_TWODIMENSION_INTERVAL,D_TWODIMENSION_SIZE+D_UP_MARGIN+D_PITURE_B_TEXT);
        pen.setColor(QColor("#ff00ff"));
        pen.setWidth(1);
        painter->setPen(pen);
        font.setBold(false);
        font.setPointSize(D_TEXT_SIZE);//设置字体大小
        font.setFamily("新宋体");
        painter->setFont(font);
        painter->drawText(0,0,text_2);
        //结束打印
        painter->end();
        point = NULL;
        //把mac地址保存到文件里面
        QFile file("macAdress.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream stream( &file );
            stream << text << "\r\n";
            stream << text_2 << "\r\n";
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
void MainWindow::QRcode_Encode(QByteArray &text)
{
    int margin = D_MARGIN_VALUE;
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
void MainWindow::port_param_init()
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
void MainWindow::check_serial_port()
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
void MainWindow::FrameParse(char c)
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
            if(ui->checkBox->isChecked())
            {
                static unsigned char dcount=0;
                if(dcount==0)
                {
                    dcount=1;
                    this->rencode_text.clear();
                    this->rencode_text = this->recv_arr;
                    if(rencode_text.isEmpty()==false)
                    {
                    QRcode_Encode(this->rencode_text);//显示二维码
                    log_output(tr("解析成功"));
                    }
                }
                else if(dcount==1)
                {
                    dcount=0;
                    this->rencode_text_2.clear();
                    this->rencode_text_2 = this->recv_arr;
                    if(rencode_text_2.isEmpty()==false)
                    {
                    QRcode_Encode_2(this->rencode_text_2);//显示二维码
                    log_output(tr("解析成功"));
                    }
                }
            }
            else
            {
                this->rencode_text.clear();
                this->rencode_text = this->recv_arr;
                if(rencode_text.isEmpty()==false)
                {
                QRcode_Encode(this->rencode_text);//显示二维码
                log_output(tr("解析成功"));
                }
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
void MainWindow::serialport_recv()
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

void MainWindow::open_serialport()
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
void MainWindow::close_serialport()
{
        this->serialport->close();
        ui->port_comboBox->setEnabled(true);
        ui->parity_comboBox->setEnabled(true);
        ui->stopbits_comboBox->setEnabled(true);
        ui->databits_comboBox->setEnabled(true);
        ui->buadrate_comboBox->setEnabled(true);
}
void MainWindow::on_open_port_Button_clicked()
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
void MainWindow::log_output(QString info)
{
    QString time= QDateTime::currentDateTime ().toString ("yyyy-MM-dd hh:mm:ss");
    ui->plainTextEdit->appendPlainText(tr("[%1]\r\n%2").arg(time).arg (info));
}


void MainWindow::plotPic(QPrinter *printer)
{
    log_output("开始打印...");
}
//函 数 名：HexToAsc()
//功能描述：把16进制转换为ASCII
char MainWindow::IntToStr(char aChar)
{
    char ss;
    printf("%s %d\n",__FUNCTION__,aChar);
    switch(aChar)
    {
        case 0: ss= '0';break;
        case 1: ss= '1';break;
        case 2: ss= '2';break;
        case 3: ss= '3';break;
        case 4: ss= '4';break;
        case 5: ss= '5';break;
        case 6: ss= '6';break;
        case 7: ss= '7';break;
        case 8: ss= '8';break;
        case 9: ss= '9';break;

        case 10: ss= 'A';break;
        case 11: ss= 'B';break;
        case 12: ss= 'C';break;
        case 13: ss= 'D';break;
        case 14: ss= 'E';break;
        case 15: ss= 'F';break;
        default:break;
    }
    printf("%s %c\n",__FUNCTION__,ss);
    return ss;
}
//函 数 名：StrToInt()
//功能描述：把字符转换成对应的数字，比如a转换成10
char MainWindow::StrToInt(char aChar)
{
    char ss;
    printf("%s %c\n",__FUNCTION__,aChar);
    switch(aChar)
    {
        case '0': ss= 0;break;
        case '1': ss= 1;break;
        case '2': ss= 2;break;
        case '3': ss= 3;break;
        case '4': ss= 4;break;
        case '5': ss= 5;break;
        case '6': ss= 6;break;
        case '7': ss= 7;break;
        case '8': ss= 8;break;
        case '9': ss= 9;break;

        case 'A': ss= 10;break;
        case 'B': ss= 11;break;
        case 'C': ss= 12;break;
        case 'D': ss= 13;break;
        case 'E': ss= 14;break;
        case 'F': ss= 15;break;
        default:break;
    }
    printf("%s %d\n",__FUNCTION__,ss);
    return ss;
}
//函 数 名：HexToAsc()
//功能描述：把16进制转换为ASCII
char IntToStr(unsigned char aChar){
    char ss;
    switch(aChar)
    {
        case 0: ss= '0';break;
        case 1: ss= '1';break;
        case 2: ss= '2';break;
        case 3: ss= '3';break;
        case 4: ss= '4';break;
        case 5: ss= '5';break;
        case 6: ss= '6';break;
        case 7: ss= '7';break;
        case 8: ss= '8';break;
        case 9: ss= '9';break;

        case 10: ss= 'A';break;
        case 11: ss= 'B';break;
        case 12: ss= 'C';break;
        case 13: ss= 'D';break;
        case 14: ss= 'E';break;
        case 15: ss= 'F';break;
        default:break;
    }
    //printf("%c\n",ss);
    return ss;
}
//char *str=(char*)malloc(4*2);
char * HexToString(char *str,unsigned char Hex[],unsigned char lenth)
{
    unsigned char i=0,j=0;
    unsigned char tema,temp;
    //char *str=(char*)malloc(lenth*2);
    for(i=0;i<lenth;i++)
    {
        tema=(Hex[i]>>4)&0x0F;
        str[j]=IntToStr(tema);
        j++;
        temp=Hex[i]&0x0F;
        str[j]=IntToStr(temp);
        j++;
    }
    str[j]='\0';
    return str;
}

void MainWindow::on_print_button_clicked()
{
   QByteArray readCmdMac="DD54";
   static int pTwoFlag=0;
   static int macValue=0x10000001;//初始值
   QString sValue=ui->textEdit->toPlainText();
   qDebug()<<sValue;
   //QByteArray iValue=sValue.toLatin1().toHex();//QString 转成QByteArray
   //qDebug()<<iValue;
   //readCmdMac.append(iValue);
   bool ok;
   macValue=sValue.toLong(&ok,16);
   qDebug()<<macValue;
   qDebug("%x\r\n",macValue);
   int imacStep=ui->textEdit_2->toPlainText().toInt();
   qDebug()<<imacStep;
   int imacCount=ui->textEdit_3->toPlainText().toInt();
   qDebug()<<imacCount;
   //return;
for(int imac=0;imac<imacCount;imac++)
{
    //第一次数据##########################################
    readCmdMac="DD54";
    qDebug()<<QString::number(macValue,16).toUpper();
    readCmdMac.append(QString::number(macValue,16).toUpper().toLatin1());
    qDebug()<<readCmdMac;

    ui->rencode_lineEdit->setText(readCmdMac);
    this->rencode_text=readCmdMac;
    //在PC界面显示二维码
    QRcode_Encode(this->rencode_text);
    log_output(tr("-->1:")+readCmdMac);
    ui->print_button->setText("再次读取一次，并打印");
    macValue+=imacStep;
    //第二次数据##########################################
    readCmdMac="DD54";
    readCmdMac.append(QString::number(macValue,16).toUpper().toLatin1());
    qDebug()<<readCmdMac;
    ui->rencode_lineEdit_2->setText(readCmdMac);
    this->rencode_text_2=readCmdMac;
    //在PC界面显示二维码
    QRcode_Encode_2(this->rencode_text_2);
    log_output(tr("-->:")+readCmdMac);
    ui->print_button->setText("打印");
    macValue+=imacStep;
    //二维码打印///////////////////////////////////////////////////
    static long prinCount=0;
    QPrinter printer;
    //设置纸张大小
    printer.setPageSize(QPagedPaintDevice::Custom);
    printer.setPaperSize(QSizeF(PAPER_WIDTH,PAPER_HIGHT),QPrinter::Millimeter);

    qDebug("printer.x %d printer.y %d",printer.pageRect().x(),printer.pageRect().y());
    qDebug("printer.w %d printer.h %d",printer.pageRect().width(),printer.pageRect().height());
    QString printerName = printer.printerName();
    if(printerName.size()==0)
    {
        log_output("请连接打印机!!!!!");
        return;
    }
    QPrintDialog *dialog = new QPrintDialog(&printer);
    dialog->setWindowTitle("print Document");
    QPrintDialog dlg(&printer,this);
    QPainter painter;

    if(ui->checkBox->isChecked())
    {
        prinCount++;
        log_output(tr("[两张]打印第 ")+QString::number(prinCount)+tr(" 次"));
        QPcode_2(&printer,&painter,this->rencode_text,this->rencode_text_2);
    }
    else
    {
        prinCount++;
        log_output(tr("[单张]打印第 ")+QString::number(prinCount)+tr(" 次"));
        QPcode(&printer,&painter,this->rencode_text);
    }
}
}
//二维码显示
void MainWindow::QRcode_Encode_2(QByteArray &text)
{
    int margin = D_MARGIN_VALUE;
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

void MainWindow::on_rencode_lineEdit_2_textChanged(const QString &arg1)
{
    //QRcode_Encode_2();
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked)
    {
        log_output("打印两张...");
        ui->rencode_lineEdit_2->setVisible(true);
        ui->rencode_view_2->setVisible(true);
    }
    else
    {
        log_output("打印单张...");
        ui->rencode_lineEdit_2->setVisible(false);
        ui->rencode_view_2->setVisible(false);
    }
}
