## Qualcomm - FireDetectApp Developer documentation

#brief introduction
#This document mainly introduces the deployment and use of FireDetectApp environment。

#Environment configuration
#Compile environment, run the installation script aikit_ 845_ Env-1.0.sh, mainly including opencv, GStreamer, GCC / G + +,
 etc. please modify according to the error prompt if you encounter any problems during the installation process. Refer to FireDetectApp_Development Manual.doc for details FireDetectApp.
cd FireDetectApp
./aikit_845_env-1.0.sh

#Check whether the environment configuration is correct

cd FireDetectApp
export | grep PATH

#Compile the project and generate the executable file to the FireDetectApp/bin directory

（编译工程，生成可执行文件到FireDetectApp/bin目录）

cd FireDetectApp
mkdir out
cd out
cmake ..
make


#Run program

cd FireDetectApp/bin
./fire_detect_app



#Instructions for use of pyrotechnic detection algorithm(支持烟火检测算法使用说明)

#1、preparation（准备工作）
①　Get access_ Token is used for permission of cloud service interface(获取access_token用于云服务接口使用权限)
②　Register baidu account(注册百度账号）
③　Log in https://ai.baidu.com/easymonitor#/dashboard to get AK/SK（登录https://ai.baidu.com/easymonitor#/dashboard获取AK/SK)
④　AK / SK replace get_ accecctoken.sh AK / SK, and run the script to extract "access" from the result_ Value of the token field.(AK/SK替换get_accecctoken.sh里的AK/SK,并运行脚本，运行结果中提取出“access_token”字段的值。)

2、Modify local configuration(修改本地配置)
   Compile FireDetectApp / include / res.h, and set AK, SK, access_ Token replaces the corresponding field.(编译FireDetectApp/include/res.h，将AK、SK、access_token替换相应字段)
     Note: cloud in res.h_ DEV_ Name represents the name of the virtual device to be registered to the cloud server.(备注:res.h中的CLOUD_DEV_NAME代表要注册到云服务端的虚拟设备名称。)
3、Recompile projects supporting fireworks detection（重新编译支持烟火检测的工程）
①　cd FireDetectApp/out
④　cmake ../
⑤　Make

