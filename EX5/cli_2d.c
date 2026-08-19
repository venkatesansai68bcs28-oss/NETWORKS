#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_ROWS 10
#define COLS 7

int getParity(int count)
{
    return count % 2;
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;

    char data[MAX_ROWS][COLS + 1];
    int rowParity[MAX_ROWS];
    int colParity[COLS];

    int rows;

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    // -------------------------------
    // 1. Create TCP socket
    // -------------------------------
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // -------------------------------
    // 2. Server address
    // -------------------------------
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    // -------------------------------
    // 3. Connect to server
    // -------------------------------
    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to server.\n\n");

    // -------------------------------
    // 4. Get number of rows
    // -------------------------------
    printf("Enter number of 7-bit binary rows: ");
    scanf("%d", &rows);

    if (rows <= 0 || rows > MAX_ROWS)
    {
        printf("Invalid number of rows.\n");
        close(sockfd);
        return 1;
    }

    // -------------------------------
    // 5. Get binary input
    // -------------------------------
    for (int i = 0; i < rows; i++)
    {
        while (1)
        {
            printf("Enter Row %d (7 bits): ", i + 1);

            scanf("%7s", data[i]);

            if (strlen(data[i]) != 7)
            {
                printf("Please enter exactly 7 bits.\n");
                continue;
            }

            int valid = 1;

            for (int j = 0; j < COLS; j++)
            {
                if (data[i][j] != '0' && data[i][j] != '1')
                {
                    valid = 0;
                    break;
                }
            }

            if (!valid)
            {
                printf("Only 0 and 1 are allowed.\n");
                continue;
            }

            break;
        }
    }

    // -------------------------------
    // 6. Calculate Row Parity
    // -------------------------------
    for (int i = 0; i < rows; i++)
    {
        int count = 0;

        for (int j = 0; j < COLS; j++)
        {
            if (data[i][j] == '1')
                count++;
        }

        rowParity[i] = getParity(count);
    }

    // -------------------------------
    // 7. Calculate Column Parity
    // -------------------------------
    for (int j = 0; j < COLS; j++)
    {
        int count = 0;

        for (int i = 0; i < rows; i++)
        {
            if (data[i][j] == '1')
                count++;
        }

        colParity[j] = getParity(count);
    }

    // -------------------------------
    // 8. Display 2D Parity Matrix
    // -------------------------------
    printf("\n========== 2D PARITY MATRIX ==========\n");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("%c ", data[i][j]);
        }

        printf("| %d\n", rowParity[i]);
    }

    printf("-------------------------------\n");

    for (int j = 0; j < COLS; j++)
    {
        printf("%d ", colParity[j]);
    }

    printf("\n");

    // -------------------------------
    // 9. Send number of rows
    // -------------------------------
    send(sockfd, &rows, sizeof(rows), 0);

    // -------------------------------
    // 10. Send binary data
    // -------------------------------
    send(sockfd,
         data,
         sizeof(data),
         0);

    // -------------------------------
    // 11. Send row parity
    // -------------------------------
    send(sockfd,
         rowParity,
         sizeof(rowParity),
         0);

    // -------------------------------
    // 12. Send column parity
    // -------------------------------
    send(sockfd,
         colParity,
         sizeof(colParity),
         0);

    printf("\n2D parity data sent to server.\n");

    // -------------------------------
    // 13. Close socket
    // -------------------------------
    close(sockfd);

    return 0;
}