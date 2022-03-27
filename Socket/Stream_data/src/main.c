#include "./../inc/main.h"

void* CreateSocket(void * data);

int main(){
	
    	pthread_t thread;
   	pthread_create(&thread, 0, CreateSocket, (void *) &CreateSocket );

	char str[50];
	while(1){
		printf("wait for exit!! \n");
		gets(str);
		if(strstr(str, "!exit")){
			
			break;
		}
	}

	printf("break!! \n");
	pthread_cancel(thread);

	return 0;
}

void* CreateSocket(void * data){

	if(pthread_detach(pthread_self())){
		pthread_exit(0);		
	} else {
		printf("socket is created %x !!! \n", pthread_self());

	}

	server_create(1234, "192.168.1.14", 5);

	while(1){
		printf("socket is running!!! \n");
		sleep(10);
	}

}