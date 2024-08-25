#ifndef __TIME_LOGIC_H
#define __TIME_LOGIC_H

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <string>
#include <time.h>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <windows.h>

class TimeLogic
{
public:
    enum eSupportedCities {
        DOHA = 1, PRAGUE, NEW_YORK, BERLIN, UTC
    };

    static string getTime();
    static string getTimeWithoutDate();
    static string getTimeSinceEpoch();
    static string getTimeWithoutDateOrSeconds();
    static string getYear();
    static string getMonthAndDay();
    static string getSecondsSinceBeginningOfMonth();
    static string getWeekOfYear();
    static string getDaylightSavings();
    static string getTimeWithoutDateInCity(eSupportedCities city);
    static string getTimeDifference(time_t* timeStamp);

private:
    static string getCurrentTime(const char* format);
    static int getCityTimezoneOffset(eSupportedCities city);
};


#endif // !__TIME_LOGIC_H
