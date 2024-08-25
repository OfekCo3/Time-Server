#include "server.h"
#include "timeLogic.h"

using TL = TimeLogic;

Server::Server(): m_socket(INVALID_SOCKET)
{
    SOCKET socket = initSocket();
    bindSocket(socket);
    handleClientRequests();
    cleanup();
}

Server::~Server()
{
    cleanup();
}

SOCKET Server::initSocket()
{
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        throw exception("Time Server: Error at WSAStartup()");
    }

    SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (INVALID_SOCKET == m_socket)
    {
        int lastError = WSAGetLastError();
        WSACleanup();
        throw string("Time Server: Error at socket(): " + to_string(lastError));
    }

    return m_socket;
}

void Server::bindSocket(SOCKET socket)
{
    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(m_port);

    if (SOCKET_ERROR == bind(socket, (SOCKADDR*)&serverService, sizeof(serverService)))
    {
        int lastError = WSAGetLastError();
        closesocket(socket);
        WSACleanup();
        throw string("Time Server: Error at bind(): " + to_string(lastError));
    }
    this->m_socket = socket;
}

void Server::handleClientRequests()
{
    sockaddr client_addr;
    int client_addr_len = sizeof(client_addr);
    char sendBuff[255];
    char recvBuff[255];
    int userChoice;

    cout << "Time Server: Wait for clients' requests.\n";

    while (true)
    {
        userChoice = receiveData(recvBuff, client_addr, client_addr_len);
        string request = handleTimeRequest(userChoice, client_addr, client_addr_len);
        strcpy(sendBuff, request.c_str());
        sendData(sendBuff, client_addr, client_addr_len);
    }
}

string Server::handleTimeRequest(int userChoice, sockaddr client_addr, int client_addr_len)
{
    string serverResponse;
    int cityCode;
    static bool isFirstLapReq = true;
    static time_t timeLapStartTime = 0;

    if (timeLapStartTime != 0)
    {
        checkTimeLapLastRequestTime(&timeLapStartTime, &isFirstLapReq);
    }

    switch (userChoice)
    {
    case GET_TIME:
        serverResponse = TL::getTime();
        break;
    case GET_TIME_WITHOUT_DATE:
        serverResponse = TL::getTimeWithoutDate();
        break;
    case GET_TIME_SINCE_EPOCH:
        serverResponse = TL::getTimeSinceEpoch();
    case GET_CLIENT_TO_SERVER_DELAY_ESTIMATION:
        serverResponse = getClientToServerDelayEstimation();
        break;
    case MEASURE_RTT:
        serverResponse = "CLiet Measure RTT";
            break;
    case GET_TIME_WITHOUT_DATE_OR_SECONDS:
        serverResponse = TL::getTimeWithoutDateOrSeconds();
        break;
    case GET_YEAR:
        serverResponse = TL::getYear();
        break;
    case GET_MONTH_AND_DAY:
        serverResponse = TL::getMonthAndDay();
        break;
    case GET_SECONDS_SINCE_BEGINING_OF_MONTH:
        serverResponse = TL::getSecondsSinceBeginningOfMonth();
        break;
    case GET_WEEK_OF_YEAR:
        serverResponse = TL::getWeekOfYear();
        break;
    case GET_DAYLIGHT_SAVINGS:
        serverResponse = TL::getDaylightSavings();
        break;
    case GET_TIME_WITHOUT_DATE_IN_CITY:
        serverResponse = handleCityRequest(client_addr, client_addr_len);
        break;
    case MEASURE_TIME_LAP:
        if (isFirstLapReq)
        {
            time(&timeLapStartTime);
            isFirstLapReq = false;
            serverResponse = measureTimeLap(FIRST_REQUEST, &timeLapStartTime, &isFirstLapReq);
        }
        else
        {
            isFirstLapReq = true;
            serverResponse = measureTimeLap(SECOND_REQUEST, &timeLapStartTime, &isFirstLapReq);
            timeLapStartTime = 0;
        }
        break;
    default:
        serverResponse = "Invalid request";
        break;
        
        return serverResponse;
    }
}

int Server::receiveData(char* recvBuff, sockaddr& client_addr, int& client_addr_len)
{
    int bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
    if (SOCKET_ERROR == bytesRecv)
    {
        closesocket(m_socket);
        WSACleanup();
        int lastError = WSAGetLastError();
        throw string("Time Server: Error at recvfrom(): " + to_string(lastError));
    }

    int userChoice = static_cast<int>(recvBuff[0]);
    cout << "Time Server: Received: " << bytesRecv << " bytes of \"" << userChoice << "\" message.\n";
    return userChoice;
}

void Server::sendData(char* sendBuff, const sockaddr& client_addr, int client_addr_len)
{
    int bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
    if (SOCKET_ERROR == bytesSent)
    {
        cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
        closesocket(m_socket);
        WSACleanup();
        exit(1);
    }

    cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
}

void Server::cleanup()
{
    cout << "Time Server: Closing Connection.\n";
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    WSACleanup();
}

int Server::requestsCityFromClient(sockaddr client_addr, int client_addr_len)
{
    char recvBuff[255];
    int userChoice;
    userChoice = receiveData(recvBuff, client_addr, client_addr_len);
    return userChoice;
}

string Server::handleCityRequest(sockaddr client_addr, int client_addr_len)
{
    char recvBuff[255];
    char sendBuff[255];
    string response = "Time Server: Wait for clients' choice for a city.\n";
    int userChoice;

    strcpy(sendBuff, response.c_str());
    sendData(sendBuff, client_addr, client_addr_len);

    userChoice = requestsCityFromClient(client_addr, client_addr_len);
    response = TL::getTimeWithoutDateInCity((TL::eSupportedCities)userChoice);
    return response;
}

string Server::getClientToServerDelayEstimation()
{
    return to_string(GetTickCount());
}

string Server::measureTimeLap(eMeasureStatus measureState, time_t* lapStartTime, bool* isFirstLap)
{
    string res;

    switch (measureState)
    {
    case FIRST_REQUEST:
        res = "Measure time lap has been initilized.";
        break;
    case SECOND_REQUEST:
        res = TL::getTimeDifference(lapStartTime);
        break;
    case TIMEOUT:
        *lapStartTime = 0;
        *isFirstLap = true;
        break;
    }

    return res;
}

void Server::checkTimeLapLastRequestTime(time_t* lastRequestTime, bool* isFirstLap)
{
    const int timeOut = time_out; 
    time_t currTimer;
    time(&currTimer);

    int timeDifference = difftime(currTimer, *lastRequestTime);
    if (timeDifference >= timeOut)
    {
        measureTimeLap(TIMEOUT, lastRequestTime, isFirstLap);
    }
}

