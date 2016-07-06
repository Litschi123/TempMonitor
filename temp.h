#ifndef TEMP_H
#define TEMP_H

#include <QDateTime>

/** A class for storing a dataset of a temperature value and a timestamp **/

class Temp
{
public:
    double temp;
    QDateTime timestamp;
    int interval;

    /**
     * @brief create a new Temp object for storing the temperature and time
     * @param temperature stores as a double. No unit is assoziated, but °C is used
     * @param time as QDateTime object
     * @param interval of measuring the tempearture in seconds
     */
    Temp(double temperature, QDateTime time, int interval);
};

#endif
