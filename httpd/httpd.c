#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int server_port = 8080;
  char *root_dir = strdup(".");

  int opt;
  int parsed = 0;
  while ((opt = getopt(argc, argv, "p:h")) != -1) {
    switch (opt) {
      case 'p':
        parsed = strtol(optarg, &(char *){0}, 10);
        if (parsed == 0)
          fprintf(stderr, "error reading port: %s\n", optarg);
        else
          server_port = parsed;
        break;
      case 'h':
      default:
        printf(
            "Usage:\n"
            "  [<directory>]  The directory to share.\n"
            "                 (default: current working directory)\n"
            "  [-p <port>]    Port number server listens to.\n"
            "                 (default: 8080)\n");
        return 0;
    }
  }

  int ret = 0;

  if (optind < argc) {
    ret = chdir(root_dir = strdup(argv[optind]));
    if (ret == -1) {
      perror("error changing directory");
      return -1;
    }
  }

  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1) {
    perror("error creating socket");
    return -2;
  }

  ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
  if (ret == -1) {
    perror("error setting address reuse");
    return -3;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(server_port);
  ret = bind(sock, (struct sockaddr *)&server, sizeof(server));
  if (ret == -1) {
    perror("error binding address");
    return -4;
  }

  ret = listen(sock, 24);
  if (ret == -1) {
    perror("error preparing to accept connection");
    return -5;
  }

  printf("listening at %s:%d\n", inet_ntoa(server.sin_addr), server_port);
  printf("serving %s\n", root_dir);

  for (;;) {
    struct sockaddr_in client;
    int csock =
        accept(sock, (struct sockaddr *)&client, &(socklen_t){sizeof(client)});
    if (csock == -1) {
      perror("error accepting connection");
      continue;
    }

    char buf[BUFSIZ];
    ret = recv(csock, buf, sizeof(buf), 0);
    if (ret == -1) {
      perror("error reading request");
      close(csock);
      continue;
    }
    char method[10];
    char path[256];
    sscanf(buf, "%s%s", method, path);
    printf("%s:%d %s %s ", inet_ntoa(client.sin_addr), client.sin_port, method,
           path);

    struct stat stbuf;
    FILE *fp;
    if (strcmp(method, "GET") != 0) {
      char *msg =
          "HTTP/1.0 403 Method Not Allowed\n"
          "Server: httpd\n"
          "\n"
          "Only GET is allowed.\n";
      ret = send(csock, msg, strlen(msg), 0);
      if (ret == -1) perror("error sending response");
      printf("-> 403\n");
    } else if (stat(path + 1, &stbuf) == -1) {
      char *msg =
          "HTTP/1.0 404 Not Found\n"
          "Server: httpd\n"
          "\n"
          "No such file or directory.\n";
      ret = send(csock, msg, strlen(msg), 0);
      if (ret == -1) perror("error sending response");
      printf("-> 404\n");
    } else if (S_ISDIR(stbuf.st_mode)) {
      char *msg =
          "HTTP/1.0 406 Not Acceptable\n"
          "Server: httpd\n"
          "\n"
          "Is a directory.\n";
      ret = send(csock, msg, strlen(msg), 0);
      if (ret == -1) perror("error sending response");
      printf("-> 403\n");
    } else if ((fp = fopen(path + 1, "rb")) == NULL) {
      perror("error opening file");
      char *msg =
          "HTTP/1.0 500 Internal Server Error\n"
          "Server: httpd\n"
          "\n"
          "Internal error occurred.\n";
      ret = send(csock, msg, strlen(msg), 0);
      if (ret == -1) perror("error sending response");
      printf("-> 500\n");
    } else {
      char *hdr =
          "HTTP/1.0 200 OK\n"
          "Server: httpd\n"
          "\n";
      ret = send(csock, hdr, strlen(hdr), 0);
      if (ret == -1) {
        perror("error sending header");
        close(csock);
        continue;
      }
      char cpbuf[BUFSIZ];
      int rbytes, wbytes;
      while ((rbytes = fread(cpbuf, 1, sizeof(cpbuf), fp)) != 0) {
        wbytes = send(csock, cpbuf, rbytes, 0);
        if (wbytes != rbytes) {
          fprintf(stderr, "error sending file\n");
          break;
        }
      }
      fclose(fp);
      printf("-> OK\n");
    }
    close(csock);
  }

  free(root_dir);

  return 0;
}