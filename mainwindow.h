#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "temp.h"
#include <QSerialPort>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QXYSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionLoad_triggered();
    void on_actionSave_triggered();

    void readData();
    void handleErrors(QSerialPort::SerialPortError error);
    void showTemp(double temp);
    void addDataToChart(double temp);
    void addToTerminal(QString data);

    void on_btn_connect_clicked();
    void on_btn_update_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    QtCharts::QChart *chart = new QtCharts::QChart();
    QtCharts::QDateTimeAxis *axisX = new QtCharts::QDateTimeAxis;
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
    QList<Temp> tempData;
};

#endif // MAINWINDOW_H
