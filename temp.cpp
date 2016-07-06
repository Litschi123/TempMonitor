#include "temp.h"

Temp::Temp(double temperature, QDateTime time, int interval)
{
    temp = temperature;
    timestamp = time;
    interval = interval;
}
