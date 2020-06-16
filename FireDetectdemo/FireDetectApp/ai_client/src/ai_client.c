#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "time.h"
#include "ai_client.h"

#include "cJSON.h"
#include "Base64.h"
#define TIC                                                             \
            struct timeval time1;                                \
            gettimeofday(&time1, NULL);


#define TOC(tag)                                                             \
            struct timeval time2;                                \
            gettimeofday(&time2, NULL);                                      \
            double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       \
                        (time2.tv_usec - time1.tv_usec) / 1000.;                \
            printf("\033[1;35mElapsed time[%s]: %lf(ms)\033[0m\n", tag, elapsed_time);


static char access_token[1024] = {0};

long long time_get_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

static char * get_string_value(cJSON *Item, char *name)
{
	cJSON *node = cJSON_GetObjectItem(Item,name); 
	if(node)
	{
		printf("[%s] %s:%s\n",__FUNCTION__, name, node->valuestring);
		return node->valuestring;
	}
	else
	{
		printf("[%s] %s no value\n",__FUNCTION__, name);
		return NULL;
	}
}

static float get_float_value(cJSON *Item, char *name)
{
	cJSON *node = cJSON_GetObjectItem(Item,name); 
	if(node)
	{
		printf("%s:%f\n",name,node->valuedouble);
		return (float)node->valuedouble;
	}
	else
		return -1;
}

static int get_int_value(cJSON *Item, char *name)
{
	cJSON *node = cJSON_GetObjectItem(Item,name); 
	if(node)
		return node->valueint;
	else
	{
		printf("%s: %s no value\n",__FUNCTION__, name);
		return -1;
	}
}

int check_leave_event(cJSON *response, void *result)
{
	/*get_result 返回值有result字段则表示成功，*/
	if(!response)
		return -1;
	cJSON *Item = cJSON_GetObjectItem(response,"result");
	if(Item)
	{
		cJSON *node = cJSON_GetObjectItem(Item,"totalCount");
		if(node && node->valueint > 0)
		{
			cJSON *tNode = NULL,*dataArray=NULL,*data=NULL,*renderOutput=NULL,*Box=NULL;
			cJSON *parent=Item, *son=NULL;
			int datai=0;
			dataArray =cJSON_GetObjectItem(Item,"data"); 
			if(!dataArray)
				return -1;
			int dataArray_size =cJSON_GetArraySize(dataArray); 
			T_DetectResult *pstDetectResult_Array = (T_DetectResult*)result;
			for(datai=0; datai<dataArray_size; datai++)
			{
				if((data =cJSON_GetArrayItem(dataArray,datai)))
				{
					T_DetectResult *pstDetectResult = &pstDetectResult_Array[datai];
					if(renderOutput =cJSON_GetObjectItem(data,"renderOutput") )
					{
						cJSON *emptyContent = cJSON_GetObjectItem(renderOutput,"emptyContent"); 
						if(emptyContent && strlen(emptyContent->valuestring)>0) //no detect person
						{
							pstDetectResult->leaveInfo.isLeaveJob = 1;
							sprintf(pstDetectResult->leaveInfo.message,"LEAVEWORK_WARNNING!");
							//不画框 只打印提示语
							pstDetectResult->event_count = 1; 
							pstDetectResult->boxInfo[0].x_x =0; 
							pstDetectResult->boxInfo[0].y_y =0; 
							pstDetectResult->boxInfo[0].width =0; 
							pstDetectResult->boxInfo[0].height =0; 
							pstDetectResult->boxInfo[0].probability = 1;
							pstDetectResult->boxInfo[0].classname = CLASSNAME_LEAVE;//leave work; 
							sprintf(pstDetectResult->boxInfo[0].message,"LEAVEWORK_WARNNING!");
						}
						else //detect person but not the real worker
						{
							Box =cJSON_GetObjectItem(renderOutput,"bondingBoxes"); 
							if(Box)
								pstDetectResult->event_count =  cJSON_GetArraySize(Box); 
							else
								pstDetectResult->event_count =  0; 
							int boxi = 0;
							for(boxi=0; boxi< pstDetectResult->event_count; boxi++)
							{
								if((tNode = cJSON_GetArrayItem(Box,boxi)))
								{
									//location of box
									cJSON *location =cJSON_GetObjectItem(tNode,"location"); 
									node =cJSON_GetObjectItem(location,"left"); 
									pstDetectResult->boxInfo[boxi].x_x =node->valueint; 
									node =cJSON_GetObjectItem(location,"top"); 
									pstDetectResult->boxInfo[boxi].y_y =node->valueint; 
									node =cJSON_GetObjectItem(location,"width"); 
									pstDetectResult->boxInfo[boxi].width =node->valueint; 
									node =cJSON_GetObjectItem(location,"height"); 
									pstDetectResult->boxInfo[boxi].height =node->valueint; 


									//lobale of event
									cJSON *label =cJSON_GetObjectItem(tNode,"label"); 
									int key_count=cJSON_GetArraySize(tNode); 
									int labeli=0;
									cJSON *keyLabel;
									for(labeli=0; labeli<key_count; labeli++)
									{
										keyLabel=NULL;
										node = NULL;
										if((keyLabel = cJSON_GetArrayItem(label,labeli)))
										{
											node =cJSON_GetObjectItem(keyLabel,"key"); 
											if(strcmp(node->valuestring, "score") == 0)
											{
												node = NULL;
												node =cJSON_GetObjectItem(keyLabel,"value"); 
												if(node)
												{
													pstDetectResult->boxInfo[boxi].probability = node->valuedouble;
													pstDetectResult->boxInfo[boxi].classname =CLASSNAME_LEAVE;//leave work; 
													printf("i:%d classname  %d \n",boxi, pstDetectResult->boxInfo[boxi].classname);
													sprintf(pstDetectResult->boxInfo[boxi].message,"LEAVEWORK_WARNNING!");
												}
											}
										}
									}
								}
							}//box_array
						}
					}
				}
			}
		}
	}//result

	return 0;
}

int check_fire_event(cJSON *response, void *result)
{
	/*get_result 返回值有result字段则表示成功，*/
	if(!response)
		return -1;
	cJSON *Item = cJSON_GetObjectItem(response,"result");
	if(Item)
	{
		cJSON *node = cJSON_GetObjectItem(Item,"totalCount");
		if(node && node->valueint > 0)
		{
			cJSON *tNode = NULL,*dataArray=NULL,*data=NULL,*renderOutput=NULL,*Box=NULL;
			cJSON *parent=Item, *son=NULL;
			int datai=0;
			dataArray =cJSON_GetObjectItem(Item,"data"); 
			if(!dataArray)
				return -1;
			int dataArray_size =cJSON_GetArraySize(dataArray); 
			T_DetectResult *pstDetectResult_Array = (T_DetectResult*)result;
			for(datai=0; datai<dataArray_size; datai++)
			{
				if((data =cJSON_GetArrayItem(dataArray,datai)))
				{
					T_DetectResult *pstDetectResult = &pstDetectResult_Array[datai];
					if(renderOutput =cJSON_GetObjectItem(data,"renderOutput") )
					{
						Box =cJSON_GetObjectItem(renderOutput,"bondingBoxes"); 
						if(Box)
							pstDetectResult->event_count =  cJSON_GetArraySize(Box); 
						else
							pstDetectResult->event_count =  0; 
						int boxi = 0;
						for(boxi=0; boxi< pstDetectResult->event_count; boxi++)
						{
							if((tNode = cJSON_GetArrayItem(Box,boxi)))
							{
								//location of box
								cJSON *location =cJSON_GetObjectItem(tNode,"location"); 
								node =cJSON_GetObjectItem(location,"left"); 
								pstDetectResult->boxInfo[boxi].x_x =node->valueint; 
								node =cJSON_GetObjectItem(location,"top"); 
								pstDetectResult->boxInfo[boxi].y_y =node->valueint; 
								node =cJSON_GetObjectItem(location,"width"); 
								pstDetectResult->boxInfo[boxi].width =node->valueint; 
								node =cJSON_GetObjectItem(location,"height"); 
								pstDetectResult->boxInfo[boxi].height =node->valueint; 


								//lobale of event
								cJSON *label =cJSON_GetObjectItem(tNode,"label"); 
								int key_count=cJSON_GetArraySize(tNode); 
								int labeli=0;
								cJSON *keyLabel;
								for(labeli=0; labeli<key_count; labeli++)
								{
									keyLabel=NULL;
									node = NULL;
									if((keyLabel = cJSON_GetArrayItem(label,labeli)))
									{
										node =cJSON_GetObjectItem(keyLabel,"key"); 
										if(strcmp(node->valuestring, "classname") == 0)
										{
											node = NULL;
											node =cJSON_GetObjectItem(keyLabel,"value"); 
											if(strncmp(node->valuestring, "fire",strlen("fire")) == 0)
												pstDetectResult->boxInfo[boxi].classname =CLASSNAME_FIRE;//fire; 
											else if(strncmp(node->valuestring, "smoke",strlen("smoke")) == 0)
												pstDetectResult->boxInfo[boxi].classname =CLASSNAME_SMOKE;//smoke
											printf("i:%d classname %s %d \n",boxi,node->valuestring, pstDetectResult->boxInfo[boxi].classname);
										}
										else if(strncmp(node->valuestring, "probability",strlen("probability")) == 0)
										{
											node = NULL;
											node =cJSON_GetObjectItem(keyLabel,"value"); 
											pstDetectResult->boxInfo[boxi].probability =atof(node->valuestring);
											printf("i:%d probability %s %f  \n",boxi,node->valuestring, pstDetectResult->boxInfo[boxi].probability);
										}
									}
								}
							}
						}//box_array
					}
				}
			}
		}
	}//result

	return 0;
}

/*bref: analysis the json data and make a list array of T_DeviceInfo by result json 
 * param; response "result"json data result: T_DeviceInfo array MAX_DEV_NUM=2 
 * return ; 0， -1*/

int check_devices_list(cJSON *response, void *result)
{
	int ret = 0;
	/*get_result 返回值有result字段则表示成功，*/
	if(!response)
		return -1;
	cJSON *Item = cJSON_GetObjectItem(response,"result");
	if(Item)
	{
		T_DeviceInfo *DevInfo = result;
		//totalCount 
		cJSON *node = cJSON_GetObjectItem(Item,"totalCount");
		if(node && node->valueint > 0)
		{
			int i = 0, array_count = 0;
			cJSON *arrayData = NULL;
			cJSON *nameData = NULL;
			cJSON *devTokenData = NULL;
			cJSON *statusData = NULL;
			cJSON *data = cJSON_GetObjectItem(Item,"data"); 
			array_count = cJSON_GetArraySize(data);
			for(i=0; i<array_count; i++)
			{
				arrayData = cJSON_GetArrayItem(data,i);
				if(arrayData)
				{
					nameData = cJSON_GetObjectItem(arrayData, "deviceName");
					sprintf(DevInfo[i].name,nameData->valuestring,strlen(nameData->valuestring));
					devTokenData = cJSON_GetObjectItem(arrayData, "deviceToken");
					sprintf(DevInfo[i].device_token,devTokenData->valuestring,strlen(devTokenData->valuestring));
					statusData = cJSON_GetObjectItem(arrayData, "deviceStatus");
					DevInfo[i].status = statusData->valueint;
				}
			}
			ret =  node->valueint;
		}
		else
			ret = 0; 
	}
	else
		ret = -1;
	return ret;
}

/*bref:添加设备到百度公有云返回结果callback
 * param：reply_type 各种请求返回结果类型
 * 			data：数据 len: Date len
 * 			result:默认已分配bufi
 * return: 0 success, -1 failed, other 公有云返回error_code*/
static int result_of_request_bdai(int reply_type, const char *data, int len, void *result)
{
	int ret = 0;
	cJSON *objson_response = cJSON_Parse(data);
	printf("%s ....\n data %s ",__FUNCTION__,data);
	if(!objson_response)
		return -1;
	cJSON *Item_ErrCode = cJSON_GetObjectItem(objson_response,"error_code");
	if(0 != Item_ErrCode->valueint)
	{
		ret = Item_ErrCode->valueint;
		return ret;
	}
	char *response = cJSON_Print(objson_response);
	printf("response:\n %s \n",response);
	switch(reply_type)
	{
	case TYPE_ADD_DEVICDE:
		{
						/*upload 返回值有result字段则表示成功，*/
			cJSON *Item = cJSON_GetObjectItem(objson_response,"result");
			if(Item)
			{
				cJSON *node = cJSON_GetObjectItem(Item,"deviceToken");
				if(result)
					sprintf(result,"%s",node->valuestring);
			}
			else 
				ret = -1;
		}
		break;
	case TYPE_LIST_DEVICDE:
		{
			ret = check_devices_list(objson_response,result);
		}
		break;
	case TYPE_UPLOAD_IMAGE:
		{
		}
		break;
	case TYPE_QURY_FIRE_EVENT:
		{
		   ret = check_fire_event(objson_response,result);
		}
		break;
	case TYPE_QURY_LEAVE_EVENT:
		{
		   ret = check_leave_event(objson_response,result);
		}
		break;
	case TYPE_FACESET_GROUP_ADD:
		{
			
		}
		break;
	case TYPE_FACESET_USR_ADD:
		{
		}
		break;
	case TYPE_FACESET_FACE_ADD:
		{
		}
		break;
	}

	if(response)
	free(response);
	//if(objson_response)
	//cJSON_Delete(objson_response);
	printf("[%s] ret %x \n",__FUNCTION__,ret);
	return ret;
}


/*	{
	"deviceName":"XXXX",
	"address":"rtsp://admin:passwordn123456@172.0.0.1:8888/Streaming/Channels/1",
	"deviceType":0,
	"deviceStatus":2,
	"remark":"remark"
	}
	{
	"log_id": 1574922485819001,
	"error_code": 0,
	"result": {
	"deviceName":"XXXX",
	"address":"rtsp://admin:password@ip:port/Streaming/Channels/1",
	"deviceType": 0,
	"deviceStatus": 2,
	"remark": "remark",
	"deviceToken": "XXXX-7HQ4-2AWR-XXXX",
	"createTime": "2019-11-28 14:28:06"
		}
	}
 * */
int bdai_add_device( char *name, char *device_token)
{
	int deviceType = 1;
	int ret = 0;
	
	/*create json string*/
    cJSON * objson = cJSON_CreateObject();
	if(name)
		cJSON_AddStringToObject(objson, "deviceName",name);
	else
		cJSON_AddStringToObject(objson, "deviceName", "rtsp0");

	cJSON_AddNumberToObject(objson,"deviceType",deviceType);


	/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback =result_of_request_bdai;
	stClientOpt.post_type = TYPE_ADD_DEVICDE;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);

	//sprintf(stClientOpt.strUrl,"%s&%s&%s",DEVICE_ADD_IP,ACCESS_TOKEN,AUTHORIZATION);
	sprintf(stClientOpt.strUrl,"%s?%s",DEVICE_ADD_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, (void *)device_token);

	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);
	return ret;
}

int bdai_delete_device(char *device_token)
{
	int ret = 0;
	
	/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "deviceToken",device_token);

	/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback =result_of_request_bdai;
	stClientOpt.post_type = TYPE_ADD_DEVICDE;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);

	//sprintf(stClientOpt.strUrl,"%s&%s&%s",DEVICE_ADD_IP,ACCESS_TOKEN,AUTHORIZATION);
	sprintf(stClientOpt.strUrl,"%s?%s",DEVICE_DEL_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, NULL);

	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);
	return ret;

}

int bdai_query_devices(T_DeviceInfo *DevInfo)
{
	int deviceType = 1;
	int ret = 0;
	
	/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback =result_of_request_bdai;
	stClientOpt.post_type = TYPE_LIST_DEVICDE;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);

	//sprintf(stClientOpt.strUrl,"%s&%s&%s",DEVICE_ADD_IP,ACCESS_TOKEN,AUTHORIZATION);
	sprintf(stClientOpt.strUrl,"%s?%s",DEVICES_LIST_QURY_IP,ACCESS_TOKEN);

	printf("[%s:%d] UIL:%s \n",__FUNCTION__,__LINE__,stClientOpt.strUrl);

	ret = client_start_connection(stClientOpt, (void *)DevInfo);

	/*must release json string*/
	free(stClientOpt.body);
	return ret;
	return 0;
}

int get_device_token(char *name, char *token)
{
	int add_flag = 0, i = 0;
	T_DeviceInfo DevInfo[MAX_DEV_NUM];
	memset(DevInfo, 0, sizeof(T_DeviceInfo)*MAX_DEV_NUM);
	int ret = bdai_query_devices(DevInfo);
	if(ret > 0)
	{
		for(i=0; i<ret; i++)
		{
			if(strncmp(name, DevInfo[i].name, strlen(name)) == 0)
			{
			printf("%s name%s token %s \n",__FUNCTION__,DevInfo[i].name,DevInfo[i].device_token);
				sprintf(token, "%s",DevInfo[i].device_token);
				return 0;
			}
		}
	}
	return  bdai_add_device(name,token);
}

/*bref:upload image Base64
 *{
 "resource":"XXXX",
 "resourceType":"base64",
 "deviceToken":"XXXX-XXXX-XXXX-XXXX",
 "correlation":"class_id_1",
 "requestTime":1566357401222
 }
 *param:
 */

int bdai_push_image(char *image, int size,long long requestTime, char *device_token)
{
	int ret = 0;
	char *base64_data = (char *)malloc(size*2);
	memset(base64_data, 0, size*2);
	base64_encode(image, size, base64_data);

/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "resource",base64_data);
	cJSON_AddStringToObject(objson,"resourceType","base64");
	cJSON_AddStringToObject(objson, "deviceToken",device_token);
	cJSON_AddNumberToObject(objson, "requestTime",requestTime);
/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback = result_of_request_bdai;
	stClientOpt.post_type = TYPE_UPLOAD_IMAGE;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);

	sprintf(stClientOpt.strUrl,"%s?%s",IMAGE_UPLOAD_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	printf("[%s:%d] UIL:%s \nbody: \n",__FUNCTION__,__LINE__,stClientOpt.strUrl/*,stClientOpt.body*/);

	ret = client_start_connection(stClientOpt, NULL);

	if(base64_data)
	free(base64_data);
	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);
	return ret;
}

/*bref; 获取检测结果
 * {
  "deviceToken": "XXXX-XXXX-XXXX-XXXX",
  "skillId": 10,
  "pageNo": 1,
  "pageSize": 20,
  "startTime":1576039837000,
  "endTime":1576039847000
}
* param; type 检测类型 百度ai技能id 
* 			detect_result: struct detect_result_t
* return 0, -1*/
int bdai_get_detect_result(int type, char *device_token, long requestTime_start, long requestTime_end, void *detect_result)
{
	int ret = 0;
	/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "deviceToken",device_token);
	cJSON_AddNumberToObject(objson, "skillId",type);
	cJSON_AddNumberToObject(objson, "startTime",requestTime_start);
	cJSON_AddNumberToObject(objson, "endTime",requestTime_end);
/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback = result_of_request_bdai;
	if(type == SKILL_FIRE_DETECT)
		stClientOpt.post_type = TYPE_QURY_FIRE_EVENT;
	else if(type == SKILL_LEAVER_WORK)
		stClientOpt.post_type = TYPE_QURY_LEAVE_EVENT;
	else if(type == SKILL_STRANGER_DETECT)
		stClientOpt.post_type = TYPE_QURY_FACE_EVENT;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);


	sprintf(stClientOpt.strUrl,"%s?%s",EVENT_QURY_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, detect_result);

			sleep(3);
	/*must release json string*/
	//free(stClientOpt.body);
    //cJSON_Delete(objson);
	return ret;
}

/*bref:
 * {
    "groupId":"groupId191201_1",
    "groupName":"groupName191201_1",
    "groupRemark":"groupRemark191201_1"
*/
int bdai_group_add(char * groupId, char *groupName)
{
	int ret = 0;
	/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "groupId",groupId);
	cJSON_AddStringToObject(objson, "groupName",groupName);
/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback = result_of_request_bdai;
	stClientOpt.post_type = TYPE_FACESET_GROUP_ADD;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);


	sprintf(stClientOpt.strUrl,"%s?%s",FACESET_GROUP_ADD_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, NULL);

	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);

	return ret; 
}


/*bref:
 * {
    "groupId":"groupId191201_1",
    "userId":"userId191201_1",
    "userName":"userName191201_1",
    "userImage":"/9j/4AAQSkZJRgABAQAAAQABAADs...",
    "userRemark":""
}
*/
int bdai_usr_add(char *groupId,char *userName, char *userId,char *image, int size)
{
	int ret = 0;
	char *base64_data = (char *)malloc(size*2);
	memset(base64_data, 0, size*2);
	base64_encode(image, size, base64_data);
	/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "groupId",groupId);
	cJSON_AddStringToObject(objson, "userId",userId);
	cJSON_AddStringToObject(objson, "userName",userName);
	cJSON_AddStringToObject(objson, "userImage",base64_data);
/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback = result_of_request_bdai;
	stClientOpt.post_type = TYPE_FACESET_GROUP_ADD;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);


	sprintf(stClientOpt.strUrl,"%s?%s",FACESET_USR_ADD_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	//printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, NULL);

	if(base64_data)
		free(base64_data);
	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);

	return ret;
}

/*bref:
 * {
    "groupId":"groupId191201_1",
    "userId":"userId191201_1",
    "image":"/9j/4AAQSkZJRgABAQAAAQABAADs..."
}
*/
int bdai_face_add(char *groupId, char *userId, char *image, int size)
{
	int ret = 0;
	char *base64_data = (char *)malloc(size*2);
	memset(base64_data, 0, size*2);
	base64_encode(image, size, base64_data);
	/*create json string*/
	cJSON * objson = cJSON_CreateObject();
	cJSON_AddStringToObject(objson, "groupId",groupId);
	cJSON_AddStringToObject(objson, "userId",userId);
	cJSON_AddStringToObject(objson, "image",base64_data);
/*create connect*/
	T_CurlClientOpt stClientOpt;
	memset(&stClientOpt,0, sizeof(T_CurlClientOpt));
	stClientOpt.response_callback = result_of_request_bdai;
	stClientOpt.post_type = TYPE_FACESET_GROUP_ADD;
	sprintf(stClientOpt.content_type, "%s",CONTENTTYPE_OF_JSON_UTF8);


	sprintf(stClientOpt.strUrl,"%s?%s",FACESET_FACE_ADD_IP,ACCESS_TOKEN);

	stClientOpt.body = cJSON_Print(objson);
	//printf("[%s:%d] UIL:%s \nbody: %s\n",__FUNCTION__,__LINE__,stClientOpt.strUrl,stClientOpt.body);

	ret = client_start_connection(stClientOpt, NULL);

	if(base64_data)
		free(base64_data);
	/*must release json string*/
	free(stClientOpt.body);
    cJSON_Delete(objson);

	return ret;
}

int bdai_detect(T_DeviceInfo *pstDeviceInfo, unsigned char *image, int size, T_DetectResult *pstDetectResult)
{
	int error_code = 0;
	int i=0;
	long requestTime_begin = time_get_us()/1000 ;
	TIC;
	long requestTime = 0;
	for(i=0; i<2; i++)
	{
		requestTime = time_get_us()/1000;
		error_code = bdai_push_image(image,size, requestTime ,pstDeviceInfo->device_token);
		if(error_code == -1)
		{
			printf("%s:%d error_code %d ",__FUNCTION__,__LINE__,error_code);
			return -1;
		}
		usleep(1000);
	}

	sleep(5);
	long requestTime_end = time_get_us()/1000;
	error_code = bdai_get_detect_result(pstDeviceInfo->skill_type,pstDeviceInfo->device_token,requestTime_begin,requestTime_end,(void *)pstDetectResult);
	int k = 0;
	for(k=0; k<MAX_DETECT_IMAGE; k++)
	{
		T_DetectResult *pstDetectResultData = &pstDetectResult[k];
		int event_count =pstDetectResultData->event_count;
		printf("event_count %d skill %d \n",event_count,pstDetectResultData->skill_type);
		if(pstDetectResultData->skill_type == SKILL_FIRE_DETECT)
		{
			for(i=0; i<event_count; i++)
			{
				printf("classname: %d ",pstDetectResultData->boxInfo[i].classname);
				printf("probability: %f\n ",pstDetectResultData->boxInfo[i].probability);
				printf("x: %d",pstDetectResultData->boxInfo[i].x_x);
				printf("y: %d",pstDetectResultData->boxInfo[i].y_y);
				printf("w: %d",pstDetectResultData->boxInfo[i].width);
				printf("h: %d\n",pstDetectResultData->boxInfo[i].height);
			}
		}
		else if(pstDetectResultData->skill_type == SKILL_LEAVER_WORK)
		{
			for(i=0; i<event_count; i++)
			{
				printf("isLeaveJob: %d",pstDetectResultData->boxInfo[i].classname);
				printf("score: %f",pstDetectResultData->boxInfo[i].probability);
				printf("message: %s",pstDetectResultData->boxInfo[i].message);
			}
		}
	}
	requestTime_begin = requestTime_end -10000;
	TOC("detect");
	sleep(2);
	return 0;

}

int main()
{
	char name[32]="leavework_dev";
	char device_token[32] = {0};
	int error_code = 0;
	int skill_type = SKILL_LEAVER_WORK;
#if 1 
	char groupId[32] = "groupWork2020_4";
	char groupName[32] = "groupWork2020_4";
	//error_code = bdai_group_add(groupId, groupName);
	FILE *fp = fopen("./work.jpg","r");
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		int file_len = ftell(fp);
		if (file_len < 0) {
			printf("get file length error");
			return -1;
		}
		fseek(fp, 0, SEEK_SET);
		char *image = (char *)malloc(file_len);
		memset(image, 0, file_len);
		fread(image, 1, file_len, fp);
		fclose(fp);
		/*
		error_code = bdai_usr_add(groupId,"work1","work1",image,file_len);
		if(error_code == -1)
		{
			printf("%s:%d usr_add error_code %d ",__FUNCTION__,__LINE__,error_code);
			return -1;
		}
		*/
		error_code = bdai_face_add(groupId,"work1",image,file_len);
		if(error_code == -1)
		{
			printf("%s:%d face_add error_code %d ",__FUNCTION__,__LINE__,error_code);
			return -1;
		}
	}
	return 0;
#endif

#if 0
	error_code = bdai_group_add();
	FILE *fp = fopen("./face.jpg","r");
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		int file_len = ftell(fp);
		if (file_len < 0) {
			printf("get file length error");
			return -1;
		}
		fseek(fp, 0, SEEK_SET);
		char *image = (char *)malloc(file_len);
		memset(image, 0, file_len);
		fread(image, 1, file_len, fp);
		fclose(fp);
		error_code = bdai_usr_add("123","123",image,file_len);
		if(error_code == -1)
		{
			printf("%s:%d usr_add error_code %d ",__FUNCTION__,__LINE__,error_code);
			return -1;
		}
		error_code = bdai_face_add("123",image,file_len);
		if(error_code == -1)
		{
			printf("%s:%d face_add error_code %d ",__FUNCTION__,__LINE__,error_code);
			return -1;
		}
	}
#endif
#if 1 
	int ret = 0;
#if 1 
	ret = get_device_token(name,device_token);
	//sleep(10);
#else

	sprintf(device_token, "%s",DEVICE_TOKEN);
#endif
	long requestTime = 1586860156000;
#if 1 
	requestTime = time_get_us()/1000;
	if(ret == 0)
	{
		FILE *fp = fopen("./test.jpg","r");
		if(fp)
		{
			fseek(fp, 0, SEEK_END);
			int file_len = ftell(fp);
			if (file_len < 0) {
				printf("get file length error");
				return -1;
			}
			fseek(fp, 0, SEEK_SET);
			char *image = (char *)malloc(file_len);
			memset(image, 0, file_len);
			fread(image, 1, file_len, fp);
			fclose(fp);
			T_DeviceInfo stDeviceInfo={"fire_dev2",NULL,skill_type};
			sprintf(stDeviceInfo.device_token, device_token,strlen(device_token));
			stDeviceInfo.skill_type =skill_type; 
			T_DetectResult *pstDetectResult = (T_DetectResult*)malloc(sizeof(T_DetectResult)*MAX_DETECT_IMAGE);
			memset(pstDetectResult, 0, sizeof(T_DetectResult)*MAX_DETECT_IMAGE);
			int i = 0;
			for(i=0; i<MAX_DETECT_IMAGE; i++)
				pstDetectResult[i].skill_type = stDeviceInfo.skill_type;
			//	error_code = bdai_push_image(image,file_len, requestTime ,device_token);
			bdai_detect(&stDeviceInfo,(unsigned char *)image,file_len,pstDetectResult);
			if(error_code == -1)
			{
				printf("%s:%d error_code %d ",__FUNCTION__,__LINE__,error_code);
				return -1;
			}
		}
	}
	sleep(10);
#endif
#if 0 
long requestTime_start = requestTime;
long requestTime_end = time_get_us()/1000;
	stDetectResult.skill_type = skill_type;
	error_code = bdai_get_detect_result(skill_type,device_token,requestTime_start,requestTime_end,(void *)&stDetectResult);
	int i = 0;
	int event_count =stDetectResult.event_count;
	if(stDetectResult.skill_type == SKILL_FIRE_DETECT)
	{
		for(i=0; i<event_count; i++)
		{
			printf("classname: %d ",stDetectResult.boxInfo[i].classname);
			printf("probability: %f ",stDetectResult.boxInfo[i].probability);
			printf("x: %d",stDetectResult.boxInfo[i].x_x);
			printf("y: %d",stDetectResult.boxInfo[i].y_y);
			printf("w: %d",stDetectResult.boxInfo[i].width);
			printf("h: %d\n",stDetectResult.boxInfo[i].height);
		}
	}
	else if(stDetectResult.skill_type == SKILL_LEAVER_WORK)
	{
		for(i=0; i<event_count; i++)
		{
			printf("isLeaveJob: %d",stDetectResult.leaveInfo[i].isLeaveJob);
			printf("score: %d",stDetectResult.leaveInfo[i].score);
		}
	}
#endif
#endif


	return 0;
}
