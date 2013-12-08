#include "main.h"
data_t _data;
pthread_mutex_t print_mutex;

/* For now all nodes are homogenous */
int main(int argc, char** argv) {
  init_structs();
  start_repl();
  return 0;
}

/* Some info about initializing structs :
 *  Ideally, the node should initialize itself.
 *  Then it will initialize the other nodes in the network.
 *  This suggests that we know all the nodes in the network anyways. */
void init_structs() {
  if(pthread_mutex_init(&mutex, NULL)) {
    printf("Can't initialize mutex\n");
    exit(1);
  }
  _data.cluster_list = NULL;
  /* Populate some cluster data */
  for(int i = 0 ; i < 5 ; i++) 
    if(!list_insert(&_data.cluster_list, "127.0.0.1", BASE_PORT+i)) {
      printf("Failed to init\n");
      exit(1);
    }
  /* Launch server thread */
  if(0 != pthread_create(&_data.server_thread, NULL, server_listener, (int *)5000)) {
    printf("failed allocating a new pthread for the listener\n");
    exit(1);
  }
}

void start_repl() {
  while(1) {
    int sock_fd;
    char input_buf[1024];
    fgets(input_buf, 1024, stdin);
    printf("%s\n", input_buf);
    if(0 == strncmp("grep", input_buf, 4)) {
      send_grep(input_buf);
    }
    sock_fd = conn_socket("127.0.0.1", 5000);
  }
}

void* remote_exec(void *dat) {
  cmd_info_t* info = (cmd_info_t*) dat;
  int sock_fd = conn_socket(info->addr, info->port);
  if(sock_fd == -1) {
    printf("Failed to connect with remote host %s:%d\n", info->addr, info->port);
    return NULL;
  }
  
  /* Send Grep, Wait grep ack, send cmd, recv until end, grab lock, print after gettin spinlock*/
  return NULL;
}

void send_grep(const char* args) {
  if(args == NULL) return;
  /* TODO : connect socket ignores address argument */
  list_node_t *head = _data.cluster_list;
  while(head != NULL) {
    /* Free this in the thread */
    cmd_info_t *cmd = malloc(sizeof(cmd_info_t));
    if(cmd == NULL) {
      head = head->next;
      continue;
    }
    cmd->addr = strdup(head->addr);
    cmd->port = head->port;
    cmd->cmd = GREP;
    cmd->args = strdup(args+5);
    
    /* Spawn pthread to do the job */
    pthread_t thread;
    if(0 != pthread_create(&thread, NULL, remote_exec, &cmd)) {
      printf("Failed to launch a grep job\n");
      exit(1);
    }
  }

  return;
}


void* server_listener(void* listen_port) {
  //char buf[BUF_SIZE];
  int socket_fd = 0, conn_fd =0;
  struct sockaddr_in server_addr;
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1) 
    err_handler("Failed to get socket file descriptor");

  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((uint16_t)listen_port);
  printf("Listener started");

  if(bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    printf("Failed to bind port");
    return 0;
  }
  if(listen(socket_fd, 10) == -1) {
    printf("Failed to listen on port");
    return 0;
  }
  while(1) {
    conn_fd = accept(socket_fd, (struct sockaddr*) NULL, NULL);
    printf("ACCEPTED");
    close(conn_fd);
    /*while(recv(conn_fd, buf, BUF_SIZE, NO_FLAGS) > 0) 
        close(conn_fd);*/
  }

  return NULL;
}

