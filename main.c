#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, "8080", &hints, &res);

    int bind_res = bind(fd, res->ai_addr, res->ai_addrlen);
    printf("Bind result %d\n", bind_res);

    int listen_res = listen(fd, 20);
    if (listen_res < 0)
    {
        perror("Listen Error");
    }

    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    int new_fd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);

    int max_len = 1000;
    char http_req[max_len];
    memset(http_req, 0, max_len);
    int num_bytes = recv(new_fd, http_req, max_len, 0);

    puts(http_req);
    return 0;
}