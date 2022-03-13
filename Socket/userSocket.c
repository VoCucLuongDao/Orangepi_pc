#include "userSocket.h"

struct socket_server{
    struct sockaddr_in serv_addr;
    int sockfd;
    char* buff;
    int num_client;
};

struct socket_client{
    struct sockaddr_in client_addr;
    int sockfd;
    int index;
    socklen_t len;
    char* buff;
};

int server_create(int no_port, unsigned int backLog)
{
    int ret = 0;
    int timeout = 0;
    struct socket_server sock_sever;
    sock_sever.num_client = backLog;
    sock_sever.buff = (char *) malloc(BUF_SIZE);
    if(sock_sever.buff == 0){
        printf("no memory \n");
        goto done;
    }

    sock_sever.sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sock_sever.sockfd < 0){
        printf("ERROR opening socket");
        goto done;
     }

    timeout = 5*1000*100; //5
 //   memset (&serv_addr, 0, sizeof(serv_addr));
    sock_sever.serv_addr.sin_port = htons (no_port);                // short, network byte order
    sock_sever.serv_addr.sin_family = AF_INET;                      // host byte order
    sock_sever.serv_addr.sin_addr.s_addr = INADDR_ANY;              // auto-fill with my IP
    bzero(&sock_sever.serv_addr, sizeof(sock_sever.serv_addr));     // zero the rest of the struct

   ret = bind(sock_sever.sockfd,
              (struct sockaddr *)&sock_sever.serv_addr,
              sizeof(struct sockaddr) );
   if(ret < 0){
       printf("ERROR binding socket");
       goto done;
   }

   ret = listen(sock_sever.sockfd,
                sock_sever.num_client);
   if(ret < 0){
       printf("ERROR listen socket");
       goto done;
   }

   printf("Server started listenting on port %d ...........\n", no_port);
   struct socket_client* sock_client = (struct socket_client*)
           malloc(sock_sever.num_client*sizeof (struct socket_client));

   static int clientCount = 0;
   pthread_t *thread = (pthread_t*) malloc(sock_sever.num_client*sizeof (pthread_t));

   while(1){

       sock_client[clientCount].index = clientCount;
       sock_client[clientCount].len  = (socklen_t) sizeof(struct sockaddr);
       sock_client[clientCount].sockfd = accept(sock_sever.sockfd,
                                                (struct sockaddr*) & sock_client[clientCount].client_addr,
                                                &sock_client[clientCount].len );
       pthread_create(&thread[clientCount],
                      0,
                      doNetworking,
                      (void *) &sock_client[clientCount] );

       pthread_join(thread[clientCount],0);

       clientCount ++;

      }

    return  0;

done:
        if(ret < 0){
            printf("socketclose() failled (err_no = %d) \n", ret);
        }

     return -1;
}

void * doNetworking(void * ClientDetail)
{
    struct socket_client* sock_client = (struct socket_client*) ClientDetail;
    int index = sock_client -> index;
    int sockfd = sock_client -> sockfd;
    printf("Client %d connected.\n",index + 1);
    while(1){
        sock_client->buff = (char *) malloc(BUF_SIZE);
        int read = recv(sockfd,sock_client->buff,BUF_SIZE,0);
        sock_client->buff[read] = '\0';
        sleep(1);
        send(sockfd, "Hello da nhan duoc",BUF_SIZE,0);


    }
}

int client_create(int no_port)
{
    int ret = 0;
    struct socket_server sock_server;
    struct socket_client sock_client;

    sock_client.sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_client.sockfd < 0){
        printf("ERROR opening socket");
        goto done;
     }

    sock_server.serv_addr.sin_family = AF_INET;
    sock_server.serv_addr.sin_port = htons(no_port);
    sock_server.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = connect( sock_client.sockfd,
                   (struct sockaddr*) &sock_server.serv_addr,
                   sizeof(struct sockaddr) );
    if(ret < 0){
        printf("Cannot connect client - > server \n");
        goto done;
    }

    printf("Connection established ............\n");

    pthread_t thread;
    pthread_create(&thread, 0, doRecieving, (void *) &sock_client );

    send(sock_client.sockfd,"The client request to be connected \n", 1024, 0);

    while(1){

        char input[1024];
        scanf("%s",input);
        send(sock_client.sockfd, input, 1024, 0);
    }

    return 0;
 done:
        return -1;
}

void * doRecieving(void * ClientDetail)
{
    struct socket_client* sock_client = (struct socket_client*) ClientDetail;
    int sockfd = sock_client -> sockfd;
    char data[1024];

    while(1){

            int read = recv(sockfd, data, 1024, 0);
            data[read] = '\0';
            printf("%s\n",data);

        }
}
