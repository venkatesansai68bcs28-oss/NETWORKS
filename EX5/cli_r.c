#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[1024];

    struct sockaddr_in server_addr;

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    inet_pton(AF_INET,
              argv[1],
              &server_addr.sin_addr);

    // 3. Connect to server
    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to server.\n");

    // 4. Get input from user
    printf("Enter a string: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    // 5. Reverse the string
    int len = strlen(buffer);

    for (int i = 0; i < len / 2; i++)
    {
        char temp = buffer[i];

        buffer[i] = buffer[len - i - 1];
        buffer[len - i - 1] = temp;
    }

    // 6. Display reversed string
    printf("Reversed string: %s\n", buffer);

    // 7. Send reversed string to server
    send(sockfd,
         buffer,
         strlen(buffer),
         0);

    // 8. Close socket
    close(sockfd);

    return 0;
}