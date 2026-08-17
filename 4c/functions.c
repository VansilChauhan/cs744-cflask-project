#include "civetweb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
root(struct mg_connection *conn)
{
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "Hello world\n");
	return 1;
}

int
square(struct mg_connection *conn)
{
	int num;
	int result = 1;
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		if (sscanf(query, "num=%d", &num) == 1) {
			result = num * num;
		}
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%d\n",
	          result);
	return 1;
}

int
cube(struct mg_connection *conn)
{
	int num;
	int result = 1;
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		if (sscanf(query, "num=%d", &num) == 1) {
			result = num * num * num;
		}
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%d\n",
	          result);
	return 1;
}

int
helloworld(struct mg_connection *conn)
{
	int result = 1;
	char str[256] = "Hello";
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		char name[128];
		char result_str[256];
		if (sscanf(query, "str=%127s", name) == 1) {
			snprintf(result_str, sizeof(result_str), "Hello, %s", name);
			strcpy(str, result_str);
		}
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%s\n",
	          str);
	return result;
}


int
pingpong(struct mg_connection *conn)
{
	int result = 1;
	char str[256] = "PingPong";
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		sscanf(query, "str=%s", str);
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%s\n",
	          str);
	return result;
}


int
prime(struct mg_connection *conn)
{
	int num;
	char *result = "False";
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		if (sscanf(query, "num=%d", &num) == 1) {
			if (num >= 0) {
				int flag = 1;
				if (num <= 1) {
					flag = 0;
				}
				for (int i = 2; i * i <= num; i++) {
					if (num % i == 0) {
						flag = 0;
					}
				}
				if (flag) {
					result = "True";
				}
			}
		}
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%s\n",
	          result);
	return 1;
}

int
fibonacci(int n)
{
	if (n == 0) {
		return 0;
	} else if (n == 1) {
		return 1;
	}

	int a = 0, b = 1, c;

	for (int i = 2; i <= n; i++) {
		c = a + b;
		a = b;
		b = c;
	}

	return b;
}

int
fibo(struct mg_connection *conn)
{
	int num;
	int result = 1;
	const char *query = mg_get_request_info(conn)->query_string;
	if (query) {
		if (sscanf(query, "num=%d", &num) == 1) {
			if (num > 0) {
				result = fibonacci(num);
			}
		}
	}
	mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
	          "%d\n",
	          result);
	return 1;
}