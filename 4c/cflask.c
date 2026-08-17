#include "civetweb.h"
#include "functionslist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUERY_SIZE 256
#define MAX_ROUTES 100
#define MAX_ROUTE_LENGTH 50

typedef struct {
	char path[MAX_ROUTE_LENGTH];
	int index;
} Route;

Route routes[MAX_ROUTES];
int route_count = 0;
void
load_routes(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (!file) {
		exit(1);
	}
	char line[128];
	while (fgets(line, sizeof(line), file)) {
		char path[MAX_ROUTE_LENGTH];
		int index;

		if (sscanf(line, "%s %d", path, &index) == 2) {
			strcpy(routes[route_count].path, path);
			routes[route_count].index = index;
			route_count++;
		}
	}
	fclose(file);
}

int
get_route_index(const char *request_path)
{
	for (int i = 0; i < route_count; i++) {
		if (strcmp(routes[i].path, request_path) == 0) {
			return routes[i].index;
		}
	}
	return -1;
}


static int
handle_request(struct mg_connection *conn)
{
	const char *uri = mg_get_request_info(conn)->uri;

	int result = 0;
	int index = get_route_index(uri);
	if (index != -1) {
		result = function_list[index](conn);
	} else {
		mg_printf(conn,
		          "HTTP/1.1 404 OK\r\nContent-Type: text/plain\r\n\r\n"
		          "Invalid Path\n");
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("usage: ./filename <port> <thread_count>\n");
		exit(1);
	}

	load_routes("functions.h");

	struct mg_context *ctx;
	struct mg_callbacks callbacks;

	printf("Starting server at %s port with %s threads\n", argv[1], argv[2]);
	const char *options[] = {
	    "listening_ports", argv[1], "num_threads", argv[2], NULL};

	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = handle_request;

	ctx = mg_start(&callbacks, NULL, options);

	if (ctx == NULL) {
		printf("Failed to start server\n");
		return 1;
	}

	getchar();

	mg_stop(ctx);
	return 0;
}