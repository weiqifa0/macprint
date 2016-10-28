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

int intCount=0;
int intBoxCount=20;
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

}
void MainWindow::QPcode( QPrinter *printer,QPainter *painter,QByteArray &text)
{
    QPen pen;
    QFont font;
    int margin = 1;//设置图像的页边距大小
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
    this->foreground = QColor("black");
    this->background = QColor("white");
    ui->rencode_lineEdit_2->setText(text);
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
            if(true)
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

void MainWindow::close_serialport()
{
        this->serialport->close();
}

void MainWindow::log_output(QString info)
{
    QString time= QDateTime::currentDateTime ().toString ("yyyy-MM-dd hh:mm:ss");
    ui->plainTextEdit->appendPlainText(tr("[%1]\r\n%2").arg(time).arg (info));
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
   //return;
   int imacCount;
for(int imac=0;imac<imacCount;imac++)
{
    //第一次数据##########################################
    readCmdMac="DD54";
    qDebug()<<QString::number(macValue,16).toUpper();
    readCmdMac.append(QString::number(macValue,16).toUpper().toLatin1());
    qDebug()<<readCmdMac;
    this->rencode_text=readCmdMac;
    //在PC界面显示二维码
    QRcode_Encode(this->rencode_text);
    log_output(tr("-->1:")+readCmdMac);
    ui->print_button->setText("再次读取一次，并打印");
    int imacStep;
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
//        log_output("请连接打印机!!!!!");
        return;
    }
    QPrintDialog *dialog = new QPrintDialog(&printer);
    dialog->setWindowTitle("print Document");
    QPrintDialog dlg(&printer,this);
    QPainter painter;

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
        painter.end();
        point = NULL;
        QRcode_free(qrcode);
    }
}
QString stringArrayMacName;
void MainWindow::on_textEdit_textChanged()
{
    QString StringMacText=ui->textEdit->toPlainText();
    qDebug()<<"StringMacText:"+StringMacText;
    if((StringMacText.length()>=12)&&((StringMacText.left(4)=="DD54")||(StringMacText.left(4)=="DD54")))
    {
      intCount++;
      qDebug()<<intCount;
      //qDebug()<<intBoxCount;
      //qDebug()<<QString::number(intBoxCount, 10);
      if(intCount==1) stringArrayMacName=QString::number(intBoxCount, 10)+"-"+ui->textEdit->toPlainText();
      ui->label->setText( QString::number(intCount, 10));
      qDebug()<<"stringArrayMacName"+stringArrayMacName;
      if((StringMacText.indexOf("DD54")&&StringMacText.indexOf("dd54"))>=0)
      {
          //qDebug("success ....%d",StringMacText.indexOf("DD54"));
      }
      else
      {
          //qDebug("error ....%d",StringMacText.indexOf("DD54"));
      }
      ui->plainTextEdit->appendPlainText(StringMacText+","+stringArrayMacName);
      ui->textEdit->clear();
    }
}
QFile file2("ts102_md5.txt");
void MainWindow::on_plainTextEdit_textChanged()
{
    //qDebug()<<"text:"+ui->plainTextEdit->toPlainText();

    QByteArray mactext = ui->plainTextEdit->toPlainText().toLatin1();
    //qDebug()<<"Qbyte:"+mactext;
    int textLength=ui->plainTextEdit->toPlainText().trimmed().length();
    // qDebug("length: %d",textLength);
    if(intCount==intBoxCount)
    {
        intCount=0;
        QString macMd5=ui->plainTextEdit->toPlainText().left(12);
        ui->textEdit->setText(QString::number(intBoxCount, 10)+"-"+macMd5);
        QString stringQrcode=QString::number(intBoxCount, 10)+"-"+macMd5;
        QByteArray byteQrcode=stringQrcode.toLatin1();
        QRcode_Encode(byteQrcode);

        QString md5Name= QDateTime::currentDateTime ().toString ("yyyyMMddHHmmss");
        QFile file("ts102_"+md5Name+"_md5.txt");

        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream( &file );
            stream << ui->plainTextEdit->toPlainText() << "\r\n";
            file.flush();
            file.close();
            if(file2.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::Append))
            {
                QTextStream stream2( &file2 );
                stream2 << ui->plainTextEdit->toPlainText() << "\r\n";
                file2.flush();
                file2.close();
                ui->plainTextEdit->clear();
            }
        }
        else
        {
            log_output(tr("打开文件失败..."));
        }
    }
}

void MainWindow::on_radioButton_2_clicked()
{
    intBoxCount=20;
    intCount=0;
    ui->plainTextEdit->clear();
    ui->label->setText("请扫描20次");
}

void MainWindow::on_radioButton_clicked()
{
    intBoxCount=50;
    intCount=0;
    ui->plainTextEdit->clear();
    ui->label->setText("请扫描50次");
}
