#include "irc_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#define JOIN_TIMEOUT_SECONDS 30  // Timeout after 30 seconds

int wait_for_join_confirmation(irc_client_t *client, const char *channel) {
    struct timeval tv;
    fd_set readfds;
    time_t start_time = time(NULL);
    char buffer[512];

    while (time(NULL) - start_time < JOIN_TIMEOUT_SECONDS) {
        // Set up select() timeout for 1 second
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(client->sockfd, &readfds);

        int result = select(client->sockfd + 1, &readfds, NULL, NULL, &tv);
        
        if (result < 0) {
            if (errno == EINTR) continue; // Handle interrupted system call
            log_message("Select error: %s", strerror(errno));
            return 0;
        }
        
        if (result == 0) {
            // Timeout on select, continue loop
            continue;
        }

        // Data is available to read
        ssize_t bytes_received = recv(client->sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            log_message("Connection closed or error during join");
            return 0;
        }

        buffer[bytes_received] = '\0';
        log_message("Received while waiting for join: %s", buffer);

        // Handle PING during join wait
        if (strstr(buffer, "PING :")) {
            char *ping_payload = strstr(buffer, "PING :") + 6;
            handle_ping(client, ping_payload);
        }

        // Check for successful join confirmation
        // Example: ":nick!user@host JOIN :#channel" or ":server 366 nick #channel :End of /NAMES list"
        if ((strstr(buffer, "JOIN") && strstr(buffer, channel)) ||
            (strstr(buffer, "366") && strstr(buffer, channel))) {
            return 1;
        }

        // Check for common errors
        if (strstr(buffer, "473")) { // ERR_INVITEONLYCHAN
            log_message("Channel %s is invite-only", channel);
            return 0;
        }
        if (strstr(buffer, "475")) { // ERR_BADCHANNELKEY
            log_message("Channel %s requires a key", channel);
            return 0;
        }
    }

    log_message("Timeout waiting for join confirmation");
    return 0;
}

int main() {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6667); // Standard IRC port

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return 1;
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    // Initialize IRC client
    irc_client_t client;
    init_client(&client, sockfd, "MyNickname", "MyUsername");

    // Attempt login
    if (!irc_login(&client)) {
        close(sockfd);
        return 1;
    }

    // Wait for registration confirmation with timeout
    time_t reg_start_time = time(NULL);
    while (!client.is_registered && (time(NULL) - reg_start_time < 30)) {
        if (!handle_registration_response(&client)) {
            sleep(1);
            continue;
        }
    }

    if (!client.is_registered) {
        log_message("Timeout waiting for registration");
        close(sockfd);
        return 1;
    }

    // Join channel with timeout
    if (join_channel(&client, "#mychannel")) {
        if (wait_for_join_confirmation(&client, "#mychannel")) {
            // Successfully joined channel
            log_message("Successfully joined #mychannel");
            
            // Send initial message
            send_message(&client, "#mychannel", "Hello, IRC!");
            
            // Start receiving messages
            receive_messages(&client);
        } else {
            log_message("Failed to join channel within timeout period");
            close(sockfd);
            return 1;
        }
    } else {
        log_message("Failed to send join command");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}