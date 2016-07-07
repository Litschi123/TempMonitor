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

/**
 * @brief Creates the ui and a chart, initializes the serial port.
*/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // INIT CHART
    chart->legend()->hide();
    chart->setTitle("Temperature");
    //chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->graph_layout->addWidget(chartView);

    // init chart mean line
    /*QPainter meanLine;
    meanLine.setPen(QPen(QRgb(0xFFFFFF)));
    meanLine.drawLine(0, 100, 0, 100);*/

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

    getComPorts();

    // INIT SERIAL PORT
    serial = new QSerialPort(this);

    // SIGNALS_N_SLOTS
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleErrors(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(ui->dd_ports, SIGNAL(highlighted(int)), this, SLOT(getComPorts()));
}

/**
 * @brief closes the port a deletes the ui
 */
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
        serial->open(QIODevice::ReadWrite);

        if(serial->error()) {
            if(serial->error() == QSerialPort::SerialPortError::DeviceNotFoundError)
                addToTerminal("<b>Error: Port not found</b>");
            else
                addToTerminal("<b>Error happend!</b>");
        }
    } else {
        serial->close();
        addToTerminal("<b>Connection closed</b>");
        ui->btn_connect->setText("Connect");
    }

    if(serial->isOpen()) {
        addToTerminal("<b>Connection made</b>");
        ui->btn_connect->setText("Disconnect");

        // set start Time
        startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
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
            // store Temperature Values
            QDateTime momentInTime = QDateTime::currentDateTime();
            Temp temperatureObject(temp, momentInTime, ui->spin_interval->value());
            tempData.append(temperatureObject);

            // update labels to show the data
            showTemp(temp);

            // add data to chart series
            addDataToChart(temp);
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
    QString hourTemp_str = "<font color='%1'>%2 °C</font>";
    QString dayTemp_str = "<font color='%1'>%2 °C</font>";

    double hourTemp = Avg(3600);
    double dayTemp = Avg(86400);

    // Create Color for the current temperatur label
    QColor color = QColor(0,0,0);
    QColor hourColor = QColor(0,0,0);
    QColor dayColor = QColor(0,0,0);

    int lowTemp = 15;
    //int normalTemp = 20;
    int highTemp = 25;

    int colorHue = 120;
    int hourColorHue = 120;
    int dayColorHue = 120;

    if(temp < lowTemp)
        colorHue = 240;
    if(temp > highTemp)
        colorHue = 0;
    if(temp > lowTemp && temp < highTemp)
        colorHue = 120;
    color.setHsv(colorHue, 255, 200, 255);

    if(hourTemp < lowTemp)
        hourColorHue = 240;
    if(hourTemp > highTemp)
        hourColorHue = 0;
    if(hourTemp > lowTemp && hourTemp < highTemp)
        hourColorHue = 120;
    hourColor.setHsv(hourColorHue, 255, 200, 255);

    if(dayTemp < lowTemp)
        dayColorHue = 240;
    if(dayTemp > highTemp)
        dayColorHue = 0;
    if(dayTemp > lowTemp && dayTemp < highTemp)
        dayColorHue = 120;
    dayColor.setHsv(dayColorHue, 255, 200, 255);

    // Set new Text for the temperature labels
    ui->lbl_currentTemp->setText(temp_str.arg(color.name(), QString::number(temp)));
    ui->lbl_hourAvg->setText(hourTemp_str.arg(hourColor.name(), QString::number(hourTemp,'f', 2)));
    ui->lbl_dayAvg->setText(dayTemp_str.arg(dayColor.name(), QString::number(dayTemp,'f', 2)));
}

void MainWindow::addDataToChart(double temp) {
    int momentInTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    series->append((momentInTime - startTime)/1000, temp);

    if(chart->series().count() != 0)
        chart->removeSeries(series);

    chart->addSeries(series);
    chart->createDefaultAxes();
}

void MainWindow::addToTerminal(QString data) {
    QDateTime currentTime = QDateTime::currentDateTime();
    QString string = currentTime.toString() + ": " + data;
    ui->txt_terminal->append(string);
}

double MainWindow::Avg(int seconds) {
    int totalSeconds = 0;
    double tempSum = 0;
    int totalSets = 0;
    for(int i=tempData.length()-1; i >= 0; i--) {
        totalSeconds += (tempData.at(i).interval * 5);
        tempSum += tempData.at(i).temp;
        totalSets++;

        if(totalSeconds >= seconds)
            break;
    }
    return tempSum / totalSets;
}

void MainWindow::getComPorts() {
    // GET AVAILABLE COM PORTS
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if(ports.length() > 0) {
        ui->dd_ports->removeItem(0);
        for(int i = 0; i < ports.length(); i++)
            ui->dd_ports->addItem(ports[i].portName());
    } else {
        addToTerminal("<b>No available ports found!</b>");
        for(int i = 0; i < ui->dd_ports->count(); i++){
            ui->dd_ports->removeItem(i);
        }
        ui->dd_ports->addItem("COM");
    }
}
