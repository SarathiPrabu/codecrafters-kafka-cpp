#include "server.hpp"
#include <iostream>

#define PORT 9092

int main()
{
    try
    {
        Server server(PORT);
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
