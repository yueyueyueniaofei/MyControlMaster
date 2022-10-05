#include "mastercontrol.h"
#include "ui_mastercontrol.h"

#include <QThread>
#include <QMap>

MasterControl::MasterControl(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MasterControl)
{
    ui->setupUi(this);

    _cdhd2Port = new QSerialPort();

    ui->baudrateComboBox->addItem("9600");
    ui->baudrateComboBox->addItem("19200");
    ui->baudrateComboBox->addItem("38400");
    ui->baudrateComboBox->addItem("115200");
    ui->baudrateComboBox->setCurrentIndex(3);
    on_refreshSerialNumber_clicked();
    connect(_cdhd2Port,SIGNAL(readyRead()),this,SLOT(processCdhdData()));

    /*******************/
    _modbusPort = new QSerialPort();
    ui->baudrateModbusComboBox->addItem("9600");
    ui->baudrateModbusComboBox->addItem("19200");
    ui->baudrateModbusComboBox->addItem("38400");
    ui->baudrateModbusComboBox->addItem("115200");
    ui->baudrateModbusComboBox->setCurrentIndex(0);

    ui->weightTypeComboBox->addItem("Rough Weight");
    ui->weightTypeComboBox->addItem("Net Weight");
    ui->weightTypeComboBox->addItem("Zero Weight");
    ui->weightTypeComboBox->setCurrentIndex(0);
    on_refreshSerialModbusNumber_clicked();
    connect(_cdhd2Port,SIGNAL(readyRead()),this,SLOT(processModbusData()));
}

MasterControl::~MasterControl()
{
    delete ui;
}

void MasterControl::on_refreshSerialNumber_clicked()
{
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    if(serialPortInfoList.isEmpty()) return;
    ui->serialNumberComboBox->clear();
    QList<QSerialPortInfo>::iterator iter = serialPortInfoList.begin();
    while(iter!=serialPortInfoList.end()) {
        ui->serialNumberComboBox->addItem(iter->portName());
        iter++;
    }
}

void MasterControl::on_refreshSerialModbusNumber_clicked()
{
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    if(serialPortInfoList.isEmpty()) return;
    ui->serialNumberModbusComboBox->clear();
    QList<QSerialPortInfo>::iterator iter = serialPortInfoList.begin();
    while(iter!=serialPortInfoList.end()) {
        ui->serialNumberModbusComboBox->addItem(iter->portName());
        iter++;
    }
}

void MasterControl::on_openPushButton_clicked()
{
    if(_cdhd2Port->isOpen()) {
        _cdhd2Port->clear();
        _cdhd2Port->close();
        ui->lightLabel->setStyleSheet("background-color:rgb(255,0,0);border-radius:12px;");
        ui->openPushButton->setText("Open");
    } else {
        _cdhd2Port->setPortName(ui->serialNumberComboBox->currentText());
        if (_cdhd2Port->open(QIODevice::ReadWrite)) {
            _cdhd2Port->setBaudRate(QSerialPort::Baud115200);
            _cdhd2Port->setDataBits(QSerialPort::Data8);
            _cdhd2Port->setParity(QSerialPort::NoParity);
            _cdhd2Port->setStopBits(QSerialPort::OneStop);
            _cdhd2Port->setFlowControl(QSerialPort::NoFlowControl);
            ui->lightLabel->setStyleSheet("background-color:rgb(0,255,0);border-radius:12px;");
            ui->openPushButton->setText("Close");

            initCdhd();
        }
    }
}

void MasterControl::on_openModbusPushButton_clicked()
{
    if(_modbusPort->isOpen()) {
        _modbusPort->clear();
        _modbusPort->close();
        ui->lightModbusLabel->setStyleSheet("background-color:rgb(255,0,0);border-radius:12px;");
        ui->openModbusPushButton->setText("Open");
    } else {
        _modbusPort->setPortName(ui->serialNumberComboBox->currentText());
        if (_modbusPort->open(QIODevice::ReadWrite)) {
            _modbusPort->setBaudRate(QSerialPort::Baud115200);
            _modbusPort->setDataBits(QSerialPort::Data8);
            _modbusPort->setParity(QSerialPort::NoParity);
            _modbusPort->setStopBits(QSerialPort::OneStop);
            _modbusPort->setFlowControl(QSerialPort::NoFlowControl);
            ui->lightModbusLabel->setStyleSheet("background-color:rgb(0,255,0);border-radius:12px;");
            ui->openModbusPushButton->setText("Close");
        }
    }
}

void MasterControl::on_movePushButton_clicked()
{
    QString msg = QString("moveabs ") + QString::number(ui->distanceSpinBox->value()) +
                QString(" ") + QString::number(ui->speedSpinBox->value()) + QString("\r");
    QByteArray messageSend;
    messageSend.append(msg);
    _cdhd2Port->write(messageSend);
}

void MasterControl::on_weightReadPushButton_clicked()
{
    enum WeightType { RoughWeight = 0, NetWeight, ZeroWeight };
    QMap<WeightType, QPair<int, int>> weightData;
    weightData[RoughWeight] = qMakePair(40081, 80);
    weightData[NetWeight] = qMakePair(40083, 82);
    weightData[ZeroWeight] = qMakePair(40085, 84);
    int curSelect = ui->weightTypeComboBox->currentIndex();



    int workingStorage = weightData.value(static_cast<WeightType>(curSelect)).first + 500 * 1 - 40000;

    char funcCode = 0x03;
    char count = 0x04;

//    QString msg = QString("moveabs ") + QString::number(ui->distanceSpinBox->value()) +
//                QString(" ") + QString::number(ui->speedSpinBox->value()) + QString("\r");
//    QByteArray messageSend;
//    messageSend.append(msg);
//    _cdhd2Port->write(messageSend);
}

void MasterControl::processCdhdData()
{
    int maxCount = 1000;
    while(maxCount--) {
        _cdhd2Port->readAll();
        _cdhd2Port->write("stopped\r");
        QByteArray x = _cdhd2Port->readLine();
        if (x.size() >0 && x[0] == '2')
            break;
        QThread::msleep(1);
    }
}

void MasterControl::processModbusData()
{

}

void MasterControl::initCdhd()
{
    _cdhd2Port->write("\\1\r");
    _cdhd2Port->write("en\r");
    _cdhd2Port->write("echo 1\r");
    _cdhd2Port->write("homecmd\r");
    _cdhd2Port->write("ready\r");
}

