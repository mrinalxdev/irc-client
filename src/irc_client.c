// irc_client.c
#include "irc_client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <time.h>

void init_client(irc_client_t *client, int sockfd, const char *nickname, const char *username)
{
    client->sockfd = sockfd;
    strncpy(client->nickname, nickname, sizeof(client->nickname) - 1);
    strncpy(client->username, username, sizeof(client->username) - 1);
    client->is_registered = false;
    client->in_channel = false;
    client->current_channel[0] = '\0';
}

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
        vfprintf(log_file, format, args); // Fixed: Changed vprintf to vfprintf
        va_end(args);
        fprintf(log_file, "\n");
        fclose(log_file);
    }
}

bool irc_login(irc_client_t *client)
{ // Fixed: Changed return type to bool
    char buffer[512];

    // Send NICK command
    snprintf(buffer, sizeof(buffer), "NICK %s\r\n", client->nickname);
    if (send(client->sockfd, buffer, strlen(buffer), 0) < 0)
    { // Fixed: Removed bufferm typo
        log_message("Failed to send NICK command");
        return false;
    }
    log_message("Sent NICK command: %s", client->nickname);

    // Send USER command
    snprintf(buffer, sizeof(buffer), "USER %s 0 * :%s\r\n",
             client->username, client->username);
    if (send(client->sockfd, buffer, strlen(buffer), 0) < 0)
    {
        log_message("Failed to send USER command");
        return false; // Fixed: Added semicolon
    }
    log_message("Sent USER command: %s", client->username);

    return true;
}

void handle_ping(irc_client_t *client, const char *ping_payload)
{
    char pong_response[512];
    snprintf(pong_response, sizeof(pong_response), "PONG :%s\r\n", ping_payload);
    send(client->sockfd, pong_response, strlen(pong_response), 0);
    log_message("Responded to PING with PONG");
}

void handle_nickname_in_use(irc_client_t *client)
{
    char new_nickname[64];
    // Fixed: Changed sprintf to snprintf
    snprintf(new_nickname, sizeof(new_nickname), "%s_", client->nickname);
    strncpy(client->nickname, new_nickname, sizeof(client->nickname) - 1);

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "NICK %s\r\n", client->nickname);
    send(client->sockfd, buffer, strlen(buffer), 0);
    log_message("Nickname in use, trying new nickname: %s", client->nickname);
}

bool handle_registration_response(irc_client_t *client)
{
    char buffer[512];
    ssize_t bytes_received = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0'; // Fixed: Changed string literal to character

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

bool join_channel(irc_client_t *client, const char *channel)
{
    if (!client->is_registered)
    {
        log_message("Cannot join channel - not registered");
        return false;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "JOIN %s\r\n", channel);
    if (send(client->sockfd, buffer, strlen(buffer), 0) < 0)
    {
        log_message("Failed to send JOIN command");
        return false;
    }

    strncpy(client->current_channel, channel, sizeof(client->current_channel) - 1);
    client->in_channel = true;
    log_message("Joined channel: %s", channel);
    return true;
}

void send_message(irc_client_t *client, const char *channel, const char *message)
{
    if (!client->is_registered)
    {
        log_message("Cannot send message - not registered");
        return;
    }

    if (!client->in_channel)
    {
        log_message("Cannot send message - not in channel");
        return;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "PRIVMSG %s :%s\r\n", channel, message);
    send(client->sockfd, buffer, strlen(buffer), 0);
    log_message("Sent message to %s: %s", channel, message);
}

void receive_messages(irc_client_t *client)
{
    char buffer[512];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
        {
            log_message("Connection closed or error");
            break;
        }

        buffer[bytes_received] = '\0';
        printf("%s", buffer);
        log_message("Received: %s", buffer);

        // Handle PING messages
        if (strstr(buffer, "PING :"))
        {
            char *ping_payload = strstr(buffer, "PING :") + 6;
            handle_ping(client, ping_payload);
        }
    }
}