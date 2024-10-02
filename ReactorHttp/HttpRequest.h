#pragma once
#include"Buffer.h"
#include<stdbool.h>
#include "HttpResponse.h"

//请求头键值对
struct RequestHeader {
	char* key;
	char* value;
};

//当前解析的状态
enum HttpRequestState {
	ParseReqLine,   //请求行
	ParseReqHeaders,  //请求头
	ParseReqBody,    //请求体
	ParseReqDone     //解析完毕
};

//定义http请求结构体
struct HttpRequest {
	char* method;
	char* url;
	char* version;
	struct RequestHeader* reqHeaders;   //需要存储请求头中键值对的数组
	int reqHeadersNum;    //reqHeaders有效数据个数
	enum HttpRequestState curState;  //当前解析的状态
};


//初始化
struct HttpRequest* httpRequestInit();
//重置
void httpRequestReset(struct HttpRequest* req);
void httpRequestResetEx(struct HttpRequest* req);
void httpRequestDestory(struct HttpRequest* req);
//获取处理状态
enum HttpRequestState getHttpRequestState(struct HttpRequest* request);
// 添加请求头的键值对
void httpRequestAddHeader(struct HttpRequest* request, const char* key, const char* value);
//根据key得到请求头的value
char* httpRequestGetHeaderValue(struct HttpRequest* request, const char* key);


//解析请求行
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf);
//解析请求头
bool parseHttpRequestHeader(struct HttpRequest* request, struct Buffer* readBuf);

char* splitRequestLine(const char* start, const char* end, const char* sub, char** ptr);

// 解析http请求协议
bool parseHttpRequest(struct HttpRequest* request, struct Buffer* readBuf,
	struct HttpResponse* response, struct Buffer* sendBuf, int socket);
// 处理http请求协议，将一些数据存在response结构体里面
bool processHttpRequest(struct HttpRequest* request, struct HttpResponse* response);
// 解码字符串
void decodeMsg(char* to, char* from);
const char* getFileType(const char* name);
void sendDir(const char* dirName, struct Buffer* sendBuf, int cfd);
void sendFile(const char* fileName, struct Buffer* sendBuf, int cfd);
