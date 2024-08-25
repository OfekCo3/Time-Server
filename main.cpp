#include "server.h"

int main()
{
    try
    {
        Server server;
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }
    catch (const string& e)
    {
        cout << e << endl;
    }

    return 0;
}