#ifndef TEMP_H
#define TEMP_H

#include <QDateTime>

/** A class for storing a dataset of a temperature value and a timestamp **/

class Temp
{
public:
    double temp;
    QDateTime timestamp;

    Temp(double t, QDateTime time);
};

#endif // TEMP_H
