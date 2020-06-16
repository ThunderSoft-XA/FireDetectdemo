#ifndef __CLIENT_H__
#define __CLIENT_H__

typedef int (*response_cb)(int type, const char *data, int size, void *result);
typedef struct __curl_client
{
    char server_ip[16];   //INET_ADDRSTRLEN这个宏系统默认定义 16
	int  server_port;
	char  strUrl[1024];
	int post_type;
 	char *header;	
	char *body;
	char content_type[1024];
	response_cb response_callback;
	void *private_data;
}T_CurlClientOpt;


int client_start_connection(T_CurlClientOpt stOpt, void * result);

int client_download(char* remote, char* file, char** received_data);

int client_download(char* remote, char* file, char** received_data);
int client_set_response_cb(response_cb sync_cb);

int client_init();

int client_deinit();
#endif
