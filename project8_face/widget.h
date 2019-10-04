#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void on_pushButton_clicked();
    void replyfinished(QNetworkReply * reply);
    void displayImage(int,QImage);
    void on_pushButton_2_clicked();
private:
    Ui::Widget *ui;
    QNetworkAccessManager *manager;
    QString request;

    QCamera *camera ;
    QCameraViewfinder *cameraViewfinder ;
    QCameraImageCapture *cameraImageCapture ;
    QCameraImageCapture *qCameraImg;
};

#endif // WIDGET_H
