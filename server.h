#ifndef __SERVER_H
#define __SERVER_H

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // This will link the Winsock library
#include <string>
#include <time.h>

class Server
{
public:
    Server();
    ~Server();
    static const int m_port = 27015;
    static const int time_out = 180; //seconds

    enum eOption {
        GET_TIME = 1, GET_TIME_WITHOUT_DATE, GET_TIME_SINCE_EPOCH,
        GET_CLIENT_TO_SERVER_DELAY_ESTIMATION,
        MEASURE_RTT, GET_TIME_WITHOUT_DATE_OR_SECONDS,
        GET_YEAR, GET_MONTH_AND_DAY, GET_SECONDS_SINCE_BEGINING_OF_MONTH,
        GET_WEEK_OF_YEAR, GET_DAYLIGHT_SAVINGS,
        GET_TIME_WITHOUT_DATE_IN_CITY, MEASURE_TIME_LAP
    };

private:

    enum eMeasureStatus {
        FIRST_REQUEST = 1, SECOND_REQUEST, TIMEOUT
    };

    SOCKET m_socket;

    SOCKET initSocket();
    void bindSocket(SOCKET socket);
    void handleClientRequests();
    string handleTimeRequest(int userChoice, sockaddr client_addr, int client_addr_len);
    int receiveData(char* recvBuff, sockaddr& client_addr, int& client_addr_len);
    void sendData(char* sendBuff, const sockaddr& client_addr, int client_addr_len);
    void cleanup();
    int requestsCityFromClient(sockaddr client_addr, int client_addr_len);
    string handleCityRequest(sockaddr client_addr, int client_addr_len);
    string getClientToServerDelayEstimation();
    string measureTimeLap(eMeasureStatus measureState, time_t* lapStartTime, bool* isFirstLap);
    void checkTimeLapLastRequestTime(time_t* lastRequestTime, bool* isFirstLap);
};

#endif // !__SERVER_H
