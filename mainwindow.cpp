#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "temp.h"
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QXYSeries>
#include <QDateTime>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QColor>
#include <QGradient>
#include <QLinearGradient>
#include <QPen>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // INIT CHART
    chart->legend()->hide();
    chart->setTitle("Temperature");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->graph_layout->addWidget(chartView);

    // Set the options for the pen an background of the chart
    QPen pen(QRgb(0xFFFFFF));
    pen.setWidth(3);
    series->setPen(pen);

    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(1.0, QRgb(0x2980b9));
    backgroundGradient.setColorAt(0.5, QRgb(0x27ae60));
    backgroundGradient.setColorAt(0.0, QRgb(0xc0392b));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(backgroundGradient);

    axisX->setTickCount(10);
    axisX->setFormat("dd. HH:mm");
    axisX->setTitleText("Date");

    axisY->setLabelFormat("%d");
    axisY->setTitleText("Temperature");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);


    // GET AVAILABLE COM PORTS
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if(ports.length() > 0)
        ui->dd_ports->removeItem(0);
    for(int i = 0; i < ports.length(); i++)
        ui->dd_ports->addItem(ports[i].portName());

    // INIT SERIAL PORT
    serial = new QSerialPort(this);

    // SIGNALS_N_SLOTS
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleErrors(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

MainWindow::~MainWindow()
{
    if(serial->isOpen())
        serial->close();

    delete ui;
}

// MENUBAR
void MainWindow::on_actionLoad_triggered()
{

}

void MainWindow::on_actionSave_triggered()
{

}

// BTN
void MainWindow::on_btn_connect_clicked()
{
    if(!serial->isOpen()){
        serial->setPortName(ui->dd_ports->itemText(0));
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        bool isOpen = serial->open(QIODevice::ReadWrite);

        if(!isOpen)
            qDebug() << serial->error();
    } else
        serial->close();

    if(serial->isOpen()) {
        addToTerminal("<b>Connection made</b>");
        //ui->txt_terminal->append("<b>Connection made</b>\n");
        ui->btn_connect->setText("Disconnect");

    } else {
        addToTerminal("<b>Connection closed</b>");
        //ui->txt_terminal->append("<b>Connection closed</b>\n");
        ui->btn_connect->setText("Connect");
    }
}

void MainWindow::on_btn_update_clicked()
{
    // Send Settings to the Arduino
    if(serial->isWritable())
        serial->write(QString::number(ui->spin_interval->value()).toStdString().c_str());
}


// FUNC
void MainWindow::readData(){
    if(serial->canReadLine()) {
        QByteArray data = serial->readLine();
        QString str = data.trimmed();
        if(str.length() == 5) {
            double temp = str.toDouble();
            showTemp(temp);
            addDataToChart(temp);

            // store Temperature Values
            QDateTime momentInTime = QDateTime::currentDateTime();
            Temp temperatureObject(temp, momentInTime);
            tempData.append(temperatureObject);
            //qDebug() << tempData.length() << " - " << tempData.at(tempData.length()-1).temp << " - " << tempData.at(tempData.length()-1).timestamp.toString("HH:mm:ss");
        }
        //ui->txt_terminal->append(str);
        addToTerminal(str);
    }
}

void MainWindow::handleErrors(QSerialPort::SerialPortError error){
    if(error == QSerialPort::ResourceError){
        serial->close();
        //ui->txt_terminal->append("Error happened.");
        addToTerminal("<b>Error happened!</b>");
    }
}

void MainWindow::showTemp(double temp) {
    QString temp_str = "<font color='%1'>%2 °C</font>";

    // Create Color for the current temperatur label
    QColor color = QColor(0,0,0);
    int lowTemp = 15;
    int normalTemp = 20;
    int highTemp = 25;
    int colorHue = 120;
    if(temp < lowTemp)
        colorHue = 240;
    if(temp > highTemp)
        colorHue = 0;
    if(temp > lowTemp && temp < highTemp)
        colorHue = 120;
    color.setHsv(colorHue, 255, 200, 255);

    // Set new Text for the current temperature label
    ui->lbl_currentTemp->setText(temp_str.arg(color.name(), QString::number(temp)));
}

void MainWindow::addDataToChart(double temp) {
    //series->append(timestamp, temp);
    QDateTime momentInTime = QDateTime::currentDateTime();
    series->append(momentInTime.toMSecsSinceEpoch(), temp);

    if(chart->series().count() != 0)
        chart->removeSeries(series);

    //series->attachAxis(axisX);
    //series->attachAxis(axisY);
    chart->addSeries(series);


    chart->createDefaultAxes();
}

void MainWindow::addToTerminal(QString data) {
    QDateTime currentTime = QDateTime::currentDateTime();
    QString string = currentTime.toString() + ": " + data;
    ui->txt_terminal->append(string);
}
