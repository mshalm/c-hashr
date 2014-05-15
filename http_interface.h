#ifndef __HTTP_INTERFACE_H__
#define __HTTP_INTERFACE_H__
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
#define json_t struct json_object
#define GET_LOAD "http://hashr.herokuapp.com/getload"
#define REPORT "http://hashr.herokuapp.com/report"
struct MemoryStruct {
  char *memory;
  size_t size;
};


typedef struct {
  int work;
  char * string;
  char * hash;
} getload_t;


typedef struct {
    int matched;
    char * matchString;
    char * string;
    char * hash;
    char * addr;
    
} report_t;



static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
char * get_ip_addr();
char * strdupe(const char * s);
char * JSON_transfer(const char * addr, const char * data);
getload_t * get_string_load();
void report(report_t * rep);
#endif