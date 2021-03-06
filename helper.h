#ifndef HELPER_H
#define HELPER_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "consts.h"
void strip_newline(char* str) {
  if(str == NULL)
    return;

  while(*str != '\0')
    str++;
  str--;
  while(*str =='\n') {
    *str = '\0';
    str--;
  }
}

void to_file_name(char* str) {
  if(str == NULL)
    return;
  char *fast=str, *slow=str;
  while(*fast != '\0') {
    /* Filename addressing issues */
    if(*fast == ':')
      *fast = '_';
    if(*fast != ' ') 
      *slow++ = *fast;
    fast++;
  }
  *slow = '\0';
}
char* timestamp() {
  char* buf = malloc(sizeof(char)*BUF_SIZE);
  time_t ltime; /* calendar time */
  ltime=time(NULL); /* get current cal time */
  asctime_r(localtime(&ltime), buf);
  to_file_name(buf);
  return buf;
}

void err_handler(const char* msg) {
    if(msg)
        printf("%s\n", msg);
    exit(1);
}

/* acks are in some form of int */
char wait_ack(int sock_fd, int expected) {
  char buf[BUF_SIZE];
  memset(buf, '\0', BUF_SIZE);
  recv(sock_fd, buf, BUF_SIZE, NO_FLAGS);
  return atoi((char*)&buf) == expected;
} 

/* Returns a file descriptor corresponding to a connection to addr:port */
int conn_socket(const char* addr, const int port) {
  int sock_fd = 0;
  struct sockaddr_in serv_addr;

  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Cannot create socket\n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = inet_addr(addr);

  if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) {
    printf("Failed to connect socket\n");
    return -1;
  }

  return sock_fd;
}
#endif
