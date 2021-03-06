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
#include <QMessageBox>
#include <QCryptographicHash>

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
   ui->plainTextEdit->setFont(QFont( "宋体" , 9,  QFont::Normal) );
   //ui->textEdit->setFont(QFont( "微软雅黑" , 18 ,  QFont::Normal) );
   ui->labelcolour->setVisible(false);
   ui->groupBox_5->setVisible(false);//
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
    QPrinter printer;
    //设置纸张大小
    printer.setPageSize(QPagedPaintDevice::Custom);
    printer.setPaperSize(QSizeF(PAPER_WIDTH,PAPER_HIGHT),QPrinter::Millimeter);

    qDebug("printer.x %d printer.y %d",printer.pageRect().x(),printer.pageRect().y());
    qDebug("printer.w %d printer.h %d",printer.pageRect().width(),printer.pageRect().height());
    QString printerName = printer.printerName();
    if(printerName.size()==0)
    {
        //log_output("请连接打印机!!!!!");
        qDebug()<<"please connect printer...";
        return;
    }
    QPrintDialog *dialog = new QPrintDialog(&printer);
    dialog->setWindowTitle("print Document");
    QPrintDialog dlg(&printer,this);
    QPainter painter;
    if(this->rencode_text.length()<=0)
    {
        qDebug()<<"rencode_text is null...";
        //this->rencode_text="20-DD5411300012";
        ui->label->setText("请先扫描...");
        return;
    }
    qDebug()<<"print start...";
    QPcode(&printer,&painter,this->rencode_text);
    ui->lineEdit->setEnabled(true);
    ui->plainTextEdit->setEnabled(true);
    ui->label->setText("请继续扫描...");

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
QString md5Name01= QDateTime::currentDateTime ().toString ("yyyyMMddHHmmss");
QFile file2("Allts102_"+md5Name01+"_md5.txt");
void MainWindow::on_plainTextEdit_textChanged()
{
    QByteArray mactext = ui->plainTextEdit->toPlainText().toLatin1();
    qDebug()<<"Qbyte:"+mactext;
    int textLength=ui->plainTextEdit->toPlainText().trimmed().length();
    qDebug("length: %d",textLength);
    if(intCount==intBoxCount)
    {
        intCount=0;
        QString stringQrcode=stringArrayMacName;//QString::number(intBoxCount, 10)+"-"+macMd5;
        QByteArray byteQrcode=stringQrcode.toLatin1();
        this->rencode_text = stringQrcode.toLatin1();
        QRcode_Encode(byteQrcode);
        if(file2.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::Append))
        {
            QTextStream stream2( &file2 );
            stream2 << ui->plainTextEdit->toPlainText() << "\r\n";
            file2.flush();
            file2.close();
            ui->plainTextEdit->clear();
            ui->label->setText("请先打印...");
            ui->lineEdit->setEnabled(false);
            ui->plainTextEdit->setEnabled(false);
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

QString stringClour;
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QString StringMacText=ui->lineEdit->text();
    qDebug()<<"StringMacText:"+StringMacText;
    if((StringMacText.trimmed().length()>=PD_MAC_LENGTH)&&((StringMacText.trimmed().left(4)=="DD54")||(StringMacText.trimmed().left(4)=="dd54")))
    {
      QString stringMacText=ui->lineEdit->text();
      QString stringAllMacText =ui->plainTextEdit->toPlainText();
      if(stringAllMacText.contains(stringMacText))
      {
          ui->label->setText("重复录入");
          ui->lineEdit->clear();
          return;
      }
      if(intCount>1)
      {
          {
              if(ui->lineEdit->text().mid(C_MAC_STRING,1)!=stringClour)
              {
                  QPalette pa;
                  pa.setColor(QPalette::WindowText,Qt::red);
                  ui->labelcolour->setPalette(pa);
                  ui->labelcolour->setText("这个设备和这个箱子里的设备颜色不同!!!");
                  QMessageBox::critical(NULL, "出错", "这个设备和这个箱子里的设备颜色不同!!!", QMessageBox::Yes, QMessageBox::Yes);
                  ui->lineEdit->clear();
                  return;
              }
          }
      }
      //提示颜色
      if(ui->lineEdit->text().mid(C_MAC_STRING,1)=="3")
      {
          ui->labelcolour->setPixmap(QPixmap("image\\blue1.png"));
          ui->labelcolour->setVisible(true);
          ui->groupBox_5->setVisible(true);//
          ui->plainTextEdit->appendPlainText("###################[  蓝色  ]########################");
      }
      else if(ui->lineEdit->text().mid(C_MAC_STRING,1)=="1")
      {
          ui->labelcolour->setPixmap(QPixmap("image\\pink1.png"));
          ui->labelcolour->setVisible(true);
          ui->groupBox_5->setVisible(true);//
          ui->plainTextEdit->appendPlainText("###################[  粉色  ]########################");
      }
      else if(ui->lineEdit->text().mid(C_MAC_STRING,1)=="2")
      {
          ui->labelcolour->setPixmap(QPixmap("image\\yellow1.png"));
          ui->labelcolour->setVisible(true);
          ui->groupBox_5->setVisible(true);//
          ui->plainTextEdit->appendPlainText("###################[  黄色  ]########################");
      }
      else
      {
          QPalette pa;
          pa.setColor(QPalette::WindowText,Qt::red);
          ui->labelcolour->setPalette(pa);
          ui->labelcolour->setText("不识别该颜色");
          ui->plainTextEdit->appendPlainText("###################[  不识别该颜色  ]########################");
          return;
      }

      intCount++;
      qDebug()<<intCount;
      if(intCount==1)
      {//唯一标识码
          QString md5;
          QString pwd=ui->lineEdit->text();
          QByteArray bb;
          stringClour=ui->lineEdit->text().mid(C_MAC_STRING,1);//保存颜色值
          bb = QCryptographicHash::hash(pwd.toLatin1(),QCryptographicHash::Md5);
          md5.append(bb.toHex());
          stringArrayMacName=(QString::number(intBoxCount, 10)+"-"+md5).toUpper();//toUpper()
          qDebug()<<stringArrayMacName;
      }
      ui->label->setText( QString::number(intCount, 10));
      qDebug()<<"stringArrayMacName"+stringArrayMacName;
      if((StringMacText.indexOf("DD54")&&StringMacText.indexOf("dd54"))>=0)
      {
          qDebug("success ....%d",StringMacText.indexOf("DD54"));
      }
      else
      {
          qDebug("error ....%d",StringMacText.indexOf("DD54"));
      }
      ui->plainTextEdit->appendPlainText(StringMacText+","+stringArrayMacName);
      ui->lineEdit->clear();
    }
    else if(StringMacText.trimmed().length()>=PD_MAC_LENGTH)
    {
        ui->lineEdit->clear();
        ui->label->setText("出错!!!");
    }
}
