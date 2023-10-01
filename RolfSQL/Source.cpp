#include <iostream>
#include <vector>
#include <string>
#include "Server.hpp"

using namespace std;



int main() {
    WindowsServer* server = new WindowsServer(777,"127.0.0.1");
    return server->StartServer();
    




    return 0;

}
