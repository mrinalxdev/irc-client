#include "irc_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6667); // Standard IRC port

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    // Login and join a channel
    irc_login(sockfd, "MyNickname", "MyUsername");
    join_channel(sockfd, "#mychannel");

    // Send a message
    send_message(sockfd, "#mychannel", "Hello, IRC!");

    // Receive messages (blocking)
    receive_messages(sockfd);

    // Close the socket
    close(sockfd);

    return 0;
}
