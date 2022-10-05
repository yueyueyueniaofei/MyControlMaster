#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MasterControl; }
QT_END_NAMESPACE

class MasterControl : public QMainWindow
{
    Q_OBJECT

public:
    MasterControl(QWidget *parent = nullptr);
    ~MasterControl();

private slots:
    void processCdhdData();
    void on_refreshSerialNumber_clicked();
    void on_openPushButton_clicked();
    void on_movePushButton_clicked();


    void processModbusData();
    void on_refreshSerialModbusNumber_clicked();
    void on_openModbusPushButton_clicked();
    void on_weightReadPushButton_clicked();


private:
    void initCdhd();

private:
    Ui::MasterControl *ui;

    QSerialPort* _cdhd2Port;
    QSerialPort* _modbusPort;
};
#endif // MASTERCONTROL_H
