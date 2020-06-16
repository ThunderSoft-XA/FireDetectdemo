#include "curl/curl.h"
#include "curl/easy.h"
#include "string.h"
#include <sys/stat.h>
#include "buffer.h"
#include "client.h"

#define false (0) 
static int g_curl_init = 0;
 
size_t BufferWriterFunc(const char * data, size_t size, size_t nmemb, void * buffer)
{
    int result = 0;
	Buffer *response = (Buffer*)buffer;
    if (response != NULL)
    {
       int status = buffer_append(response, (char *)data, size*nmemb);
	   if(status != 0)
	   {
		   printf("buffer_append error\n");
		   return 0;
	   }
        result = size * nmemb;
    }
    return result;
}

int client_start_connection(T_CurlClientOpt stOpt, void *result)
{
	CURL *curl;
	CURLcode res;
	int ret = -1;

	Buffer *BufResponse = buffer_alloc(10*1024);
	

	struct curl_slist *chunk = NULL;

	/* get a curl handle*/
	curl = curl_easy_init();
	if (curl)
	{

		if(stOpt.body && strlen(stOpt.body) > 0)
		{
			curl_easy_setopt(curl, CURLOPT_POST,1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, stOpt.body);
			if(stOpt.content_type && strlen(stOpt.content_type)>0)
			{
				chunk = curl_slist_append(chunk, stOpt.content_type);
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER,chunk);
			}
		}
		/* set the URL with GET request*/
		curl_easy_setopt(curl, CURLOPT_URL, stOpt.strUrl);
		/* write response msg into strResponse*/
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriterFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)BufResponse);
		/*seconds transfer operation to take*/
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		
		/*perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* check for errors */
		if (res != CURLE_OK)
		{
			printf("curl_easy_perform() failed:res %d  type %d \n",res,stOpt.post_type);
			ret = -1;
		}
		else
		{
			printf("curl_easy_perform() success.\n");
			/*handle the response data*/
			if(stOpt.response_callback && BufResponse)
			{
				ret = stOpt.response_callback(stOpt.post_type, BufResponse->contents,BufResponse->bytes_used, result);
			}
		}

		if(chunk)
			curl_slist_free_all(chunk);
		/*always cleanup*/
		curl_easy_cleanup(curl);
	}

	if(BufResponse)
	buffer_free(BufResponse);

	return ret;
}

static int download_Progress = 0;
static int http_progress = 0;
/*bref;下载进度回调*/
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)  
{  
    if ( dltotal > -0.1 && dltotal < 0.1 )  
       return 0;  
    int nPos = (int) ( (dlnow/dltotal)*100 );
 
    //通知进度条更新下载进度  
    
	download_Progress = ((1<<16) | (nPos >> 1));
 
	http_progress = nPos;
	printf("Progress=========%d \n",nPos);
 
    return 0;  
}


/*bref: 下载到文件
 * param： remote  服务器请求路经 file 保存本地文件 received_data */
int client_download(char* remote, char* file, char** received_data)
{
    CURL *curl;
    CURLcode res;
    FILE* fp = NULL;
    long code = 0;
    struct stat buf;
	double downloadFileLenth = -1;
	double receivedFileLenth = -2;
	
    curl = curl_easy_init();
    if(curl)
    {
        fp = fopen(file, "w");
        if(fp)
        {
            curl_easy_setopt(curl, CURLOPT_URL, remote);
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			
				//设置进度回调函数	
		    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);  
					 //set timeout
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);//10000L);                                                   
    		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60L);  
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);	
			
			res = curl_easy_perform(curl);
			
			if (res != CURLE_OK)  
			{
                printf("curl_easy_perform() failed: %d\n",res);
				downloadFileLenth = -1;
            }
			else 
			{  
				curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);  
				printf("~~~~~~~~~~~~http_download curl_easy_getinfo CURLINFO_CONTENT_LENGTH_DOWNLOAD downloadFileLenth:%lf\n",downloadFileLenth);
			}
            fclose(fp);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        }
        curl_easy_cleanup(curl);
		curl = NULL;
 
        if(code == 200)
        {
        	if(stat(file, &buf) == 0)
			{
				receivedFileLenth = buf.st_size;
			}
 
			if(receivedFileLenth != downloadFileLenth || downloadFileLenth <= 0 || receivedFileLenth <= 0)
			{
				printf("~~~~~~~~~~~~http_download downloadFileLenth:%lf receivedFileLenth:%lf, not equal~~~~~~~~~~~~~~\n", downloadFileLenth, receivedFileLenth);
				return -1;
			}
            return 0;
        }
        else
        {
            if(stat(file, &buf) == 0)
            {
                fp = fopen(file, "r");
                if(fp)
                {
                    *received_data = malloc(buf.st_size);
                    if(*received_data)
                    {
                        if(fread(*received_data, 1, buf.st_size, fp) == buf.st_size)
                        {
                            fclose(fp);
                            remove(file);
                            return -1;
                        }
                        free(*received_data);
                        *received_data = NULL;
                    }
                    fclose(fp);
                }
                remove(file);
            }
            return -1;
        }
    }
 
    return -1;
 
}

int client_init()
{
	if(!g_curl_init)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		g_curl_init = 1;
	}
	return 0;
}
int client_deinit()
{
	if(g_curl_init)
	{
		curl_global_cleanup();
		g_curl_init = 0;
	}
	return 0;
}
 
int main_test()
{
    CURL *curl;
    CURLcode res;
     char  strUrl[1024] = "http://10.0.36.93:8080/sys/face/sync/employee/data?";
    char *strResponse = (char *)malloc(1024); 
	if(!strResponse)
	{
		printf("[%s] malloc strResponse error\n",__FUNCTION__);
		return -1;
	}
	Buffer * response;
 
    response = buffer_alloc(1024*1024);
	printf("strResponse %p response %p \n",strResponse,response);
    curl_global_init(CURL_GLOBAL_DEFAULT);
 
    // get a curl handle
    curl = curl_easy_init();
    if (curl)
    {
		char body[1024] = "modified=201911111620292906";
        // set the URL with GET request
        curl_easy_setopt(curl, CURLOPT_URL, strUrl);
 
        // write response msg into strResponse
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriterFunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, strResponse);
 
		curl_easy_setopt(curl, CURLOPT_POST,1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        // perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // check for errors
        if (res != CURLE_OK)
        {
            printf("curl_easy_perform() failed: %s", curl_easy_strerror(res));
        }
        else
        {
            printf("curl_easy_perform() success.");
            //printf("strResponse is:%s ",strResponse);
        }
			printf("response:%s",response->contents);

 
        // always cleanup
        curl_easy_cleanup(curl);
    }
 
    curl_global_cleanup();
 
    return 0;
}
#if 0
#include <iostream>
#include <curl/curl.h>
#include <string>
 
using namespace std;
 
int BufferWriterFunc(char * data, size_t size, size_t nmemb, string * buffer)
{
    int result = 0;
    if (buffer != NULL)
    {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}
 
int main()
{
    CURL *curl;
    CURLcode res;
    string strResponse;
 
    curl_global_init(CURL_GLOBAL_DEFAULT);
 
    // get a curl handle
    curl = curl_easy_init();
 
    if (curl)
    {
        // set the URL that is about to receive our POST
        curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
 
        // specify the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "param1=value1&param2=value2");
        
        // write response msg into strResponse
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriterFunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResponse);
 
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK)
        {
            cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else
        {
            cout << "curl_easy_perform() success." << endl;
            cout << "strResponse is: " << strResponse << endl;
        }
 
        // always cleanup
        curl_easy_cleanup(curl);
    }
 
    curl_global_cleanup();
 
    return 0;
}
#endif
