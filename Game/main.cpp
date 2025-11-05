#include <iostream>
#include <SFML/Graphics.hpp>
#include "client.hpp"


int main()
{
    Client client(5555, "127.0.0.1");
    client.run();
    return 0;
}