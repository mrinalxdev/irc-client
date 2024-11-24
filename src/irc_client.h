#ifndef IRC_CLIENT_H
#define IRC_CLIENT_H

#include <stdbool.h>

// Response codes for the proper logging of IRC connections
#define RPL_WELCOME 001
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTREGISTERED 451

typedef struct {
    int sockfd;
    char nickname[64];
    char username[64];
    bool is_registered;
    bool in_channel;
    char current_channel[64];
} irc_client_t;

// intialising the client
void init_client(irc_client_t *client, int sockfd, const char *nickname, const char *username);

bool irc_login(irc_client_t *client);
bool handle_registration_response(irc_client_t *client);
bool join_channel(irc_client_t *client, const char *channel);
void send_message(irc_client_t *client, const char *channel, const char *message);
void handle_ping(irc_client_t *client, const char *ping_payload);
void handle_nickname_in_use(irc_client_t *client);
void receive_messages(irc_client_t *client);
void log_message(const char *format, ...);

#endif 
