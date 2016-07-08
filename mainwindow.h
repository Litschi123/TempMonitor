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

    /**
     * @brief Avarages the the temperatures of the last specified seconds
     * @param seconds from now
     * @return The avarage of the temperatures of the Temp Object List
     */
    double Avg(int seconds);

private slots:
    void readData();
    void handleErrors(QSerialPort::SerialPortError error);

    void on_btn_connect_clicked();
    void on_btn_update_clicked();
    void getComPorts();

private:
    void showTemp(double temp);
    void addDataToChart(double temp);
    void addToTerminal(QString data);

    Ui::MainWindow *ui;
    QSerialPort *serial;
    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
    QtCharts::QChart *chart = new QtCharts::QChart();
    QList<Temp> tempData;
    int startTime;
};

#endif // MAINWINDOW_H
