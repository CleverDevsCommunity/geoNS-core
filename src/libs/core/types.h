#ifndef GEONS_TYPE_H
#define GEONS_TYPE_H 1

#include <sqlite3.h>
#include <pthread.h>

typedef unsigned short              ushort;
typedef unsigned char               uchar;
typedef unsigned long               ulong;
typedef pthread_t                   thread;
typedef void                        (ServerCallback)(int fd, uchar *request);

#endif // !GEONS_TYPE_H