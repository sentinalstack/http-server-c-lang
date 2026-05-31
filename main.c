#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define MAX_LEN 4096

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0)
            return -1;
        sent += n;
    }
    return 0;
}

int main(void)
{
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, "8080", &hints, &res) != 0)
    {
        perror("getaddrinfo");
        return 1;
    }

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    if (bind(fd, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    if (listen(fd, 20) < 0)
    {
        perror("listen");
        return 1;
    }
    printf("Listening on :8080\n");

    while (1)
    {
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof their_addr;
        int new_fd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd < 0)
        {
            perror("accept");
            continue;
        }

        char buf[MAX_LEN];
        memset(buf, 0, sizeof buf);
        ssize_t n = recv(new_fd, buf, sizeof buf - 1, 0);

        if (n > 3 && strncmp(buf, "GET", 3) == 0)
        {
            printf("GET request received\n");
            const char *resp =
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 13\r\n"
                "Content-Type: text/plain\r\n"
                "\r\n"
                "Hello, world!";
            send_all(new_fd, resp, strlen(resp));
        }
        else
        {
            printf("Non-GET request received\n");
        }

        close(new_fd);
    }

    close(fd);
    return 0;
}