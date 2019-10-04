#include "widget.h"
#include "ui_widget.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

#include <QFileDialog>
#include <QFileDevice>
#include <QByteArray>
#include <QBuffer>
#include <QFile>

#include <QHttpMultiPart>
#include <QHttpPart>
#include <QContextMenuEvent>
#include <QJsonObject>

#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QImage>

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //摄像头
    camera = new QCamera;
    cameraViewfinder = new QCameraViewfinder;
    cameraImageCapture = new QCameraImageCapture(camera);
    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->setViewfinder(ui->widget);
    camera->start();

    QTimer *time = new QTimer;
    connect(time,SIGNAL(timeout()),this,SLOT(gettime()));
    time->start(1);
    manager = new QNetworkAccessManager(this);
    request = "https://aip.baidubce.com/rest/2.0/face/v3/detect";
    connect(manager,SIGNAL(finished(QNetworkReply *)),SLOT(replyfinished(QNetworkReply *)));
    connect(cameraImageCapture,SIGNAL(imageCaptured(int,QImage)),this,SLOT(displayImage(int,QImage)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{

    QUrl url(request + "?access_token=" + "24.22b8eccd31eb56328f8a3954465e895b.2592000.1570089816.282335-17168916");
    QNetworkRequest requesturl(url);
    requesturl.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    QString pic = QFileDialog::getOpenFileName();
    qDebug()<< pic;

    QImage image(pic);
    QSize laSize=ui->label->size();
    image.scaled(laSize,Qt::IgnoreAspectRatio);//重新调整图像大小以适应窗口
    ui->label->setPixmap(QPixmap::fromImage(image));//图片显示

    QByteArray arry;
    QBuffer buff(&arry);
    buff.open(QIODevice::WriteOnly);
    image.save(&buff,"jpg");
    QJsonObject post_data;;
    QJsonDocument document;
    post_data.insert("image", QString(arry.toBase64()));
    post_data.insert("image_type", "BASE64");
    post_data.insert("face_field", "age,beauty,gender,expression");
    document.setObject(post_data);
    QByteArray post_param = document.toJson(QJsonDocument::Compact);
    manager->post(requesturl,post_param);

}
void Widget::replyfinished(QNetworkReply * reply)
{
    //解析数据
    QByteArray arry = reply->readAll();
    qDebug()<<arry<<endl;
    QJsonDocument doc = QJsonDocument::fromJson(arry);
    QJsonObject result = doc.object();
    if(result.contains("result"))
    {
         QJsonObject resultObj = result.take("result").toObject();
         if(resultObj.contains("face_list"))
         {
             QJsonArray faceArray = resultObj.take("face_list").toArray();
             for(int i = 0; i < faceArray.size(); i++)
             {
                 QJsonObject faceObj = faceArray.at(i).toObject();
                 if(faceObj.contains("gender"))
                 {
                     QJsonObject genderObj = faceObj.take("gender").toObject();
                     if(genderObj.contains("type"))
                     {
                         QString type = genderObj.take("type").toString();;
                         if(type == "male")
                         {
                             ui->lineEdit->setText("已经签到");
                             ui->label_4->setText("男");
                         }
                         else {
                             ui->label_4->setText("女");
                         }
                     }
                 }
                 if(faceObj.contains("age"))
                 {
                     int age = faceObj.take("age").toDouble();
                     qDebug()<<"查询到了年龄："<<age;
                     ui->label_5->setText(QString::number(age));
                 }
                 if(faceObj.contains("beauty"))
                 {
                     int beauty = faceObj.take("beauty").toDouble();;
                     qDebug()<<"查询到了颜值："<<beauty;
                     ui->label_9->setText(QString::number(beauty));
                 }
             if(faceObj.contains("expression"))
             {
                 QJsonObject expressionObj = faceObj.take("expression").toObject();
                 if(expressionObj.contains("type"))
                 {
                     QString type = expressionObj.take("type").toString();;
                     if(type == "smile")
                     {
                         ui->label_6->setText("微笑");
                     }
                     else if(type == "laugh")
                     {
                         ui->label_6->setText("大笑");
                     }
                     else
                     {
                         ui->label_6->setText("不笑");
                     }
                 }
             }
         }
       }
    }
}
void Widget::displayImage(int,QImage preview)
{

    QSize laSize=ui->label->size();
    QImage image=preview.scaled(laSize,Qt::IgnoreAspectRatio);//重新调整图像大小以适应窗口
    ui->label->setPixmap(QPixmap::fromImage(image));//图片显示

    QUrl url(request + "?access_token=" + "24.22b8eccd31eb56328f8a3954465e895b.2592000.1570089816.282335-17168916");
    QNetworkRequest requesturl(url);
    requesturl.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    QByteArray arry;
    QBuffer buff(&arry);
    buff.open(QIODevice::WriteOnly);
    preview.save(&buff,"jpg");
    QJsonObject post_data;;
    QJsonDocument document;
    post_data.insert("image", QString(arry.toBase64()));
    post_data.insert("image_type", "BASE64");
    post_data.insert("face_field", "age,beauty,gender,expression");
    document.setObject(post_data);
    QByteArray post_param = document.toJson(QJsonDocument::Compact);
    manager->post(requesturl,post_param);

}
//拍照
void Widget::on_pushButton_2_clicked()
{
    cameraImageCapture->capture();
}
