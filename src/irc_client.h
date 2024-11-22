#ifndef IRC_CLIENT_H
#define IRC_CLIENT_H


void irc_login(int sockfd, const char *nickname, const char *username);
void join_channel(int sockfd, const char *channel);
void send_message(int sockfd, const char *channel, const char *message);
void receive_messages(int sockfd);

#endif 
