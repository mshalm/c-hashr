#include "http_interface.h"
 
int connected = 0;
int work_avail = 1;
int init = 1;
//libcurl example function
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
	/* out of memory! */ 
	printf("not enough memory (realloc returned NULL)\n");
	return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}


char * get_ip_addr()
{
    //MC_SET_DEBUG(1);
    char * str = JSON_transfer("http://jsonip.com",NULL);
    if (!str)
	return NULL;
    //construct json object from get string
    json_t * new_obj = json_tokener_parse(str);
    
    
    //extract ip parameter
    json_t * addr_obj = json_object_object_get(new_obj, "ip");
    
    //extract value of ip parameter
    const char * addr = json_object_get_string(addr_obj);
    
    //create independent version of address
    char * ret = strdupe(addr);
    
    
    //free up memory
    free(str);
    json_object_put(new_obj);
    return ret;
}

//libcurl example code
char * JSON_transfer(const char * addr, const char * data)
{
    
    CURL *curl_handle;
    CURLcode res;
    
    struct MemoryStruct chunk;
    
    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
    
    char * ret;
    /* init the curl session */ 
    curl_handle = curl_easy_init();
    
    /* specify URL to get */ 
    curl_easy_setopt(curl_handle, CURLOPT_URL, addr);
    
    //send data if available
    if (data)
    {
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
	
    }
    /* send all data to this function  */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    
    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    
    /* some servers don't like requests that are made without a user-agent
	field, so we provide one */ 
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    /* get it! */ 
    res = curl_easy_perform(curl_handle);
    
    /* check for errors */ 
    if(res != CURLE_OK)
    {
	ret = NULL;
    }
    else
    {
	//construct c-string from data
	ret = realloc(chunk.memory,chunk.size+1);
	ret[chunk.size] = '\0';
    }
    
    //clean up memory
    curl_easy_cleanup(curl_handle);
    return ret;
    
}

/* get_string_load function:
 * retrieves a computational load from the servers
 */
getload_t * get_string_load()
{
    //HTTP GET a computation load
    char * load_str = JSON_transfer(GET_LOAD,NULL);
    if (!load_str)
    {
	if (connected||init)
	    fprintf(stderr,"Failed to connect to server!\n");
	init = 0;
	connected = 0;
	work_avail = 1;
	sleep(2);
	free(load_str);
	return NULL;
    }
    if (!connected)
	    printf("Connection established\n");
    connected = 1;
    getload_t * ret = NULL;
    //construct json object from string
    json_t * new_obj = json_tokener_parse(load_str);
    //detect if work is available
    int work = json_object_get_int(json_object_object_get(new_obj, "work"));
    if (!work)
    {
	//no work, will return null
	if (work_avail)
	    printf("No work available\n");
	work_avail = 0;
	sleep(2);
    }
    else
    {
	work_avail = 1;
	//work available, fill in data
	ret = malloc(sizeof(getload_t));
	ret->work = work;
	//extract string parameters
	ret->string = strdupe(json_object_get_string(json_object_object_get(new_obj, "string")));
	ret->hash = strdupe(json_object_get_string(json_object_object_get(new_obj, "hash")));
    }
    json_object_put(new_obj);
    free(load_str);
    return ret;
}

/* report function:
 * sends results of computation to server
 */
void report(report_t * rep)
{
    //construct HTTP query string from rep
    char post[1024];
    post[0] = '\0';
    strcat(post,"matched=");
    if (rep->matched)
	strcat(post,"1");
    else
	strcat(post,"0");
    strcat(post,"&matchString=");
    if (rep->matchString)
	strcat(post,rep->matchString);
    else
	strcat(post,"null");
    strcat(post,"&string=");
    strcat(post,rep->string);
    strcat(post,"&hash=");
    strcat(post,rep->hash);
    strcat(post,"&addr=");
    strcat(post,rep->addr);
    char * j = JSON_transfer(REPORT,post);
    if (j)
	free(j);
}




/* strdupe function:
 * returns independent copy of parameter string
 */
char * strdupe(const char * s)
{
    size_t length = strlen(s) + 1;
    char *ret = malloc(length);
    return memcpy(ret, s, length);
}