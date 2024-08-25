#include "timeLogic.h"

string TimeLogic::getCurrentTime(const char* format)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_c);

    stringstream ss;
    ss << put_time(&local_tm, format);
    return ss.str();
}

string TimeLogic::getTime()
{
    return getCurrentTime("%Y-%m-%d %H:%M:%S");
}

string TimeLogic::getTimeWithoutDate()
{
    return getCurrentTime("%H:%M:%S");
}

string TimeLogic::getTimeSinceEpoch()
{
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return to_string(chrono::duration_cast<chrono::seconds>(duration).count());
}

string TimeLogic::getTimeWithoutDateOrSeconds()
{
    return getCurrentTime("%H:%M");
}

string TimeLogic::getYear()
{
    return getCurrentTime("%Y");
}

string TimeLogic::getMonthAndDay()
{
    return getCurrentTime("%m-%d");
}

string TimeLogic::getSecondsSinceBeginningOfMonth()
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_c);

    local_tm.tm_mday = 1;
    local_tm.tm_hour = 0;
    local_tm.tm_min = 0;
    local_tm.tm_sec = 0;
    time_t beginning_of_month = mktime(&local_tm);
    long seconds_since_beginning = static_cast<long>(difftime(now_c, beginning_of_month));

    return to_string(seconds_since_beginning);
}

string TimeLogic::getWeekOfYear()
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_c);

    int week = local_tm.tm_yday / 7 + 1;
    return to_string(week);
}

string TimeLogic::getDaylightSavings()
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_c);

    return local_tm.tm_isdst > 0 ? "1 - Daylight Saving Time is in effect." : "0 - Daylight Saving Time is not in effect.";
}

int TimeLogic::getCityTimezoneOffset(eSupportedCities city)
{
    // Check if Daylight Saving Time is in effect
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_c);

    switch (city) {
    case DOHA:
        if (local_tm.tm_isdst > 0)
        {
            return 2;   // Doha is UTC+2 during DST
        }
        else
        {
            return 3;   // Doha is UTC+3 otherwise
        }
    case PRAGUE:
        return 1;   // Prague is UTC+1
    case NEW_YORK:
        return -5;  // New York is UTC-5
    case BERLIN:
        return 1;   // Berlin is UTC+1
    case UTC:
        if (local_tm.tm_isdst > 0)
        {
            return -1;   
        }
        else
        {
            return 0;   
        }
    }
}

string TimeLogic::getTimeWithoutDateInCity(eSupportedCities city)
{
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm utc_tm;
    gmtime_s(&utc_tm, &now_c);
    utc_tm.tm_hour += getCityTimezoneOffset(city);
    mktime(&utc_tm);

    // Normalize tm struct after adding offset
    if (utc_tm.tm_hour < 0)
    {
        utc_tm.tm_hour += 24; 
    }
    else if (utc_tm.tm_hour >= 24)
    {
        utc_tm.tm_hour -= 24; 
    }

    stringstream ss;
    ss << put_time(&utc_tm, "%H:%M");
    return ss.str();
}

string TimeLogic::getTimeDifference(time_t * timeStamp)
{
    time_t currentTime;
    time(&currentTime);
    time_t time_elapsed = currentTime - *timeStamp;

    int minutes = time_elapsed / 60;
    int seconds = time_elapsed % 60;

    char timeString[9]; // MM:SS + '\0'
    snprintf(timeString, sizeof(timeString), "%02d:%02d", minutes, seconds);

    return timeString;
}
