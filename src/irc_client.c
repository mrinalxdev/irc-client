#include "irc_client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>     // For close()
#include <sys/socket.h> // For send() and recv()
#include <stdarg.h>
#include <time.h>

void init_client(irc_client_t *client, int sockfd, const char *nickname, const char *username)
{
    client->sockfd = sockfd;
    strncpy(client->nickname, nickname, sizeof(client->nickname) - 1);
    strncpy(client->usernamer, username, sizeof(client->username) - 1);
    client->is_registered = false;
    client->in_channel = false;
    client->current_channel[0] = '\0';
}

// logging message so that I can track each and every event performed by the client side !
// it is also necessary to register events to keep track on
void log_message(const char *format, ...)
{
    time_t now;
    time(&now);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    FILE *log_file = fopen("irc_client.log", "a");
    if (log_file)
    {
        fprintf(log_file, "[%s] ", timestamp);
        va_list args;
        va_start(args, format);
        vprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fclose(log_file);
    }
}

// todo:irc_login:65% completed
void irc_login(irc_client_t *client)
{
    char buffer[512];

    // sending the commands to perform specefic events
    // for this event we are considering, changing and logging the nickname
    snprintf(buffer, sizeof(buffer), "NICK %s/r/n", client->nickname);
    if (send(client->sockfd, buffer, strlen(buffer), 0) < 0)
    {
        log_message("Failed to send NICK command");
        return false;
    }
    log_message("Send NICK command : %s", client->nickname);
    snprintf(buffer, sizeof(buffer), "USER %s 0 * :%s\r\n", client->username, client->username);

    if (send(client->sockfd, bufferm strlen(buffer), 0) < 0)
    {
        log_message("Failed to send USER command");
        return false
    }
    log_message("Sent USER command : %s", client->username);

    return true;
}

void handle_ping(irc_client_t *client, const char *ping_payload)
{
    char pong_response[512];
    snprintf(pong_response, sizeof(pong_response), "PONG : %s\r\n", ping_payload);
    send(client->sockfd, pong_response, strlen(pong_response), 0);
    // handling ping pong messages during no contact between two clients
    // why ? -> so that when there is no message exchanging between two clients
    // the server will stay active
    log_message("Respond to PING with PONG");
}

void handle_nickname_in_use(irc_client_t *client)
{
    char new_nickname[64];
    snprintf(new_nickname, sizeof(new_nickname), "%s_", client->nickname);
    char buffer[512];
    sprintf(buffer, sizeof(buffer), "NICK %s_", client->nickname);
    send(client->sockfd, buffer, strlen(buffer), 0);
    log_message("Nickname in use, trying new nickname : %s", client->nickname);
}

bool handle_registration_response(irc_client_t *client)
{
    char buffer[512];
    ssize_t bytes_received = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';

        // Parse server response
        if (strstr(buffer, " 001 "))
        {
            client->is_registered = true;
            log_message("Registration successful");
            return true;
        }
        else if (strstr(buffer, " 433 "))
        {
            handle_nickname_in_use(client);
        }
        else if (strstr(buffer, "PING :"))
        {
            char *ping_payload = strstr(buffer, "PING :") + 6;
            handle_ping(client, ping_payload);
        }
    }

    return false;
}

bool join_channel(irc_client_t *client, const char *channel){
    if(!client->is_registered){
        log_message("Cannot join channel - not registered");
        return false;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
    if (send(client->sockfd, buffer, strlen(buffer), 0) < 0){
        log_message("Failed to send JOIN command");
        return false;
    }

    strncpy(client->current_channel, channel, sizeof(client->current_channel) - 1);
    client->in_channel = true;
    log_message("Joined channel : %s", channel);
    return true;
}

void send_message(irc_client_t *client, const char *channel, const char *message){
    if(!client->is_registered){
        log_message("Cannot send message - not registered");
        return;
    }
    if(!client->in_channel){
        log_message("Cannot send message - not in channel");
        return;
    }

    char buffer[512];
    sprintf(buffer, sizeof(buffer), "")
}

void receive_messages(irc_client_t *client){
    char buffer[512];
    while(1){
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_recieved = recv(client->sockfd, buffer, sizeof(buffer) -1, 0);

        if(bytes_recieved <= 0){
            log_message("Connection closed or error");
            break;
        }

        buffer[bytes_recieved] = "\0";
        printf("%s", buffer);
        log_message("Received : %s", buffer);

        //handling the ping message
        if(strstr(buffer, "PING :")){
            char *ping_payload = strstr(buffer, "PING :") + 6;
            handle_ping(client, ping_payload);
        }
    }
}