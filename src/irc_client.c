#include "irc_client.h"
#include "network.h"

void irc_login(int sockfd, const char *nickname, const char *username){
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "Mrinal %s\r\n", nickname);
    send(sockfd, buffer, strlen(buffer), 0);

    snprintf(buffer, sizeof(buffer), "USER %s 0 * : %s\r\n", username, username);
    send(sockfd, buffer, strlen(buffer), 0);
}

void join_channel(int sockfd, const char *channel){
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
    send(sockfd, buffer, strlen(buffer), 0);
}

void send_message(int sockfd, const char *channel, const char *message){
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "PRIVMSG %s : %s\r\n", channel, message);
    send(sockfd, buffer, strlen(buffer), 0);
}

void receive_messages(int sockfd){
    char buffer[512];
    while(1){
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0){
            buffer[bytes_received] = "\0";
            printf("%s", buffer);
        }
    }
}