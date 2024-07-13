#ifndef GEONS_TYPE_H
#define GEONS_TYPE_H 1

#include <sqlite3.h>

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef int (*sqlite3_callback)(
   void*,    /* Data provided in the 4th argument of sqlite3_exec() */
   int,      /* The number of columns in row */
   char**,   /* An array of strings representing fields in the row */
   char**    /* An array of strings representing column names */
);

#endif // !GEONS_TYPE_H