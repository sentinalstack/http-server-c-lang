#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, "8080", &hints, &res);

    int bind_res = bind(fd, res->ai_addr, res->ai_addrlen);

    while (1)
    {
        printf("Waitng for request.....\n");
        sleep(1);
    }
    return 0;
}