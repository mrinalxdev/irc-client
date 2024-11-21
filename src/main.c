#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "irc_client.c"
#include "network.h"

void *recieve_thread(void *sockfd_ptr){
    int sockfd = *(int *)sockfd_ptr;
    receive_messages(sockfd);
    return NULL;
}

int main(){
    const char *server = "irc.libera.chat";
    int port = 6667;
    const char *nickname = "MyNickname";
    const char *username = "MyUser";
    const char *channel = "#mychannel";

    int sockfd = connect_to_server(server, port);
    irc_login(sockfd, nickname, username);
    join_channel(sockfd, channel);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recieve_thread, &sockfd);

    char input[512];
    while(1){
        fgets(input, sizeof(input), stdin);
        send_message(sockfd, channel, input);
    }

    close(sockfd);
    return 0;
}