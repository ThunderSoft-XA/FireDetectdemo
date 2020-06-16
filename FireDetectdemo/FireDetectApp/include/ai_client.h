#ifndef __BDAI_CLINET_H__
#define __BDAI_CLINET_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "client.h"
#include "res.h"

#define CONTENTTYPE_OF_JSON_UTF8 		"Content-Type: application/json;charset=utf-8"

//#define DEVICE_TOKEN				"KZBK-U8PW-8V8G-JB3W"
//#define ACCESS_TOKEN 				"access_token=24.e4f1f1cfef8b750b2e08d3a639224abd.2592000.1589279243.282335-19344028"
#define AUTHORIZATION 			 	"Authorization=bce-auth-v1/X5gK86mEoaCwKO7dcwjLddGZ/2020-04-12T10:25:03Z/43200/host/ebdf166cec30794318b4b373911a668628780b8c87b4851c9ec43d2c4cdec360"
#define DEVICE_ADD_IP 				"https://aip.baidubce.com/rpc/2.0/cvsaas/v2/device/add"
#define DEVICE_DEL_IP 				"https://aip.baidubce.com/rpc/2.0/cvsaas/v2/device/delete"
#define DEVICES_LIST_QURY_IP 		"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/device/list"
#define IMAGE_UPLOAD_IP 			"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/resource/upload"
#define EVENT_QURY_IP  				"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/event/list"

#define FACESET_GROUP_ADD_IP 		"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/faceset/group/add"
#define FACESET_USR_ADD_IP 			"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/faceset/user/add"
#define FACESET_FACE_ADD_IP			"https://aip.baidubce.com/rpc/2.0/cvsaas/v1/faceset/face/add"



#define MAX_BOX_NUM 8
#define MAX_DEV_NUM 2 
#define MAX_DETECT_IMAGE 1 


typedef struct __box_info
{
	int classname;//烟火类型 火 烟
	float probability; //概率 可信度
	char message[64];
	//Box 
	int x_x;
	int y_y;
	int width;
	int height;
}T_BoxInfo;

typedef struct __leave_info
{
	char name[32];
	int isLeaveJob; //1 leave 2 inspead
	float score;
	char message[64];
}T_LeaveJobInfo;


typedef struct __detect_result_t
{
	int skill_type; //skill id
	int event_count;
	T_BoxInfo boxInfo[MAX_BOX_NUM];
	T_LeaveJobInfo leaveInfo;
}T_DetectResult;

typedef struct __device_info
{
	char name[16];
	char device_token[32];
	int status;
	int skill_type;
}T_DeviceInfo;

typedef enum 
{
	TYPE_ADD_DEVICDE,
	TYPE_LIST_DEVICDE,
	TYPE_UPLOAD_IMAGE,
	TYPE_QURY_FIRE_EVENT,
	TYPE_QURY_LEAVE_EVENT,
	TYPE_QURY_FACE_EVENT,
	TYPE_FACESET_GROUP_ADD,
	TYPE_FACESET_USR_ADD,
	TYPE_FACESET_FACE_ADD
}E_POST_TYPE;

/*baidu ai easymonitor skill list*/
typedef enum
{
	SKILL_FIRE_DETECT=19,
	SKILL_LEAVER_WORK=14,
	SKILL_STRANGER_DETECT=12,
	SKILL_BUTT
}E_SKILLTYPE;

typedef enum
{
	CLASSNAME_FIRE = 1,
	CLASSNAME_SMOKE=2,
	CLASSNAME_LEAVE=3,
	CLASSNAME_BUTT
}E_CLASSNAME;

int bdai_add_device( char *name, char *device_token);
int bdai_query_devices();

int bdai_push_image(char *image, int size,long long requestTime, char *device_token);
int bdai_get_detect_result(int type, char *device_token, long requestTime_start, long requestTime_end, void *detect_result);
int bdai_detect(T_DeviceInfo *pstDeviceInfo, unsigned char *image, int size, T_DetectResult *pstDetectResult);
int get_device_token(char *name, char *token);

int bdai_group_add(char * groupId, char *groupName);
int bdai_usr_add(char *groupId,char *userName, char *userId,char *image, int size);
int bdai_face_add(char *groupId, char *userId, char *image, int size);

#ifdef __cplusplus
}
#endif
#endif
