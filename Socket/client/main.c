#include "userSocket.h"

int main(int argc, char* argv[])
{
    int x = 1;
    if(x)
        server_create(8888, 5);
    else
        client_create(8888, "127.0.0.1");


    printf("Hello World!\n");
    return 0;
}
