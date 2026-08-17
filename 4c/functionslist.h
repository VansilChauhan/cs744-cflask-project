#include "civetweb.h"

extern int root(struct mg_connection *conn);
extern int square(struct mg_connection *conn);
extern int cube(struct mg_connection *conn);
extern int helloworld(struct mg_connection *conn);
extern int pingpong(struct mg_connection *conn);
extern int prime(struct mg_connection *conn);
extern int fibo(struct mg_connection *conn);

int (*function_list[])(struct mg_connection *conn) = {
    root,
    square,
    cube,
    helloworld,
    pingpong,
    prime,
    fibo,
};