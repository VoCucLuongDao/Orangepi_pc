#include "userSocket.h"

int main(int argc, char* argv[])
{
<<<<<<< HEAD
    int x = 0;
    if(x)
        server_create(8888, 5);
    else
        client_create(8888, "127.168.1.25");
=======
    int x = 1;
    if(x)
        server_create(8888, 5);
    else
        client_create(8888, "127.0.0.1");
>>>>>>> 816e791a68d8b1dcc56a8a4f250545baa88f0779


    printf("Hello World!\n");
    return 0;
}
