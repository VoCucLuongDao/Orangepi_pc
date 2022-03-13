#include "userSocket.h"

int main(int argc, char* argv[])
{
    int x = 1;
    if(x)
        server_create(8088, 5);
    else
        client_create(8088);

    printf("Hello World!\n");
    return 0;
}
