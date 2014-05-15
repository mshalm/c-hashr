#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <json/json.h>
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#include "md5.h"
#include "string_handler.h"
#include "http_interface.h"
#endif
#define CUR_POS_LEN 20
#define QUERY_MAX_LEN 3

//char * hash = "594f803b380a41396ed63dca39503542";
//int x = 0; 
//char *p;
char * ip_addr;
void *hasher(void * t_id);
void handler(int sig);
int unfinished = 1;


char *str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}


 
int main(int argc, const char** argv)
{
    //assert correct usage of command
    if (argc != 2)
    {
	fprintf(stderr,"USAGE: ./md5 n\n");
	fprintf(stderr,"where n is the number of threads to execute concurrently.\n");
	fprintf(stderr,"Acceptable values for n are between 1 and 32\n");
	return 1;
    }
    int num_threads = atoi(argv[1]);
    if (num_threads < 1 || num_threads > 32)
    {
	fprintf(stderr,"Invalid number of threads!\n");
	return 1;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    ip_addr = get_ip_addr();
    
    
    //stop execution if unable to retrieve IP address
    if (!ip_addr)
    {
	fprintf(stderr,"Failed retrieve IP address!\n");
	free(ip_addr);
	curl_global_cleanup();
	return 1;
    }
    
    //setup SIGINT handler
    signal(SIGINT, handler);
    printf("Ready to hash\n");
    //create num_threads pthreads to each hash
    pthread_t hash_threads[num_threads];
    int i;
    for (i = 0; i < num_threads; i++)
    {
	pthread_create(&hash_threads[i], NULL, hasher, (void *) (long) (i+1));
        
    }
    
    for (i = 0; i < num_threads; i++)
    {
	pthread_join(hash_threads[i], NULL);
    }
    
    
    //cleanup
    curl_global_cleanup();
    free(ip_addr);
    return 0;
}


void *hasher(void * t_id)
{
    int thread_num = (long) t_id;
    //make sure SIGINT hasn't been called yet
    while (unfinished)
    {
	//retrieve computation load
	getload_t * load = get_string_load();
	if (load)
	{
	    printf("[thread %d]: Processing string group \"%s\" for hash %s\n", thread_num, load->string, load->hash);
	    report_t rep;
	    //prepare data to return to server;
	    rep.matched = 0;
	    rep.matchString = NULL;
	    rep.string = load->string;
	    rep.hash = load->hash;
	    rep.addr = ip_addr;
	    string_handler_t s;
	    string_handler_init(&s);
	    //start at the minimum value string of length QUERY_MAX_LEN to prevent overlap	
	    if (strcmp("",load->string))
	    {
		string_handler_set_init_len(&s, QUERY_MAX_LEN);
		
	    }
	    char * cur_hash;
	    char buf[1024];
	    //iterate through strings until s.cur_pos exceeds maximum allowed length
	    while((strlen(s.cur_pos)<=QUERY_MAX_LEN))
	    {
		buf[0] = '\0';
		//concatenate the load string with cur_pos in buf
		strcat(buf,load->string);
		strcat(buf,s.cur_pos);
		cur_hash = str2md5(buf,strlen(buf));
		if (!strcmp(cur_hash,load->hash))
		{
		    //hash of load->str + cur_pos matched, report to server
		    printf("[thread %d]: Match found for hash %s: %s\n",thread_num,load->hash,buf);
		    rep.matched = 1;
		    rep.matchString = buf;
		    break;
		}
		//incremet string
		string_handler_get_next(&s);
		if (cur_hash)
		{
		    free(cur_hash);
		    cur_hash = NULL;
		}
	    }
	    //send report
	    report(&rep);
	    free(load->string);
	    free(load->hash);
	    free(load);
	    if (cur_hash)
		{
		    free(cur_hash);
		    cur_hash = NULL;
		}
	}
    }
    return NULL;
}

/* SIGINT handler:
 * only action is to prevent threads from taking additional workloads
 */
void handler(int sig)
{
    unfinished = 0;  
}