#include "userSocket.h"

int main(int argc, char* argv[])
{
    int x = 1;
    int client_id = 0;
    if(x)
        server_create(2000, 15);
    else
       client_id = client_create(80, "127.168.1.25");

    char message[]= "hello cac ban \r\n";
    socketSend(client_id,message, sizeof (message));
    printf("Hello World!%d -  %d\n", client_id, sizeof(message));
    while(1);
    socketClose(client_id, 2);
    return 0;
}
