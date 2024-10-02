#pragma once
#include"Buffer.h"
#include<stdbool.h>
#include "HttpResponse.h"

//����ͷ��ֵ��
struct RequestHeader {
	char* key;
	char* value;
};

//��ǰ������״̬
enum HttpRequestState {
	ParseReqLine,   //������
	ParseReqHeaders,  //����ͷ
	ParseReqBody,    //������
	ParseReqDone     //�������
};

//����http����ṹ��
struct HttpRequest {
	char* method;
	char* url;
	char* version;
	struct RequestHeader* reqHeaders;   //��Ҫ�洢����ͷ�м�ֵ�Ե�����
	int reqHeadersNum;    //reqHeaders��Ч���ݸ���
	enum HttpRequestState curState;  //��ǰ������״̬
};


//��ʼ��
struct HttpRequest* httpRequestInit();
//����
void httpRequestReset(struct HttpRequest* req);
void httpRequestResetEx(struct HttpRequest* req);
void httpRequestDestory(struct HttpRequest* req);
//��ȡ����״̬
enum HttpRequestState getHttpRequestState(struct HttpRequest* request);
// �������ͷ�ļ�ֵ��
void httpRequestAddHeader(struct HttpRequest* request, const char* key, const char* value);
//����key�õ�����ͷ��value
char* httpRequestGetHeaderValue(struct HttpRequest* request, const char* key);


//����������
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf);
//��������ͷ
bool parseHttpRequestHeader(struct HttpRequest* request, struct Buffer* readBuf);

char* splitRequestLine(const char* start, const char* end, const char* sub, char** ptr);

// ����http����Э��
bool parseHttpRequest(struct HttpRequest* request, struct Buffer* readBuf,
	struct HttpResponse* response, struct Buffer* sendBuf, int socket);
// ����http����Э�飬��һЩ���ݴ���response�ṹ������
bool processHttpRequest(struct HttpRequest* request, struct HttpResponse* response);
// �����ַ���
void decodeMsg(char* to, char* from);
const char* getFileType(const char* name);
void sendDir(const char* dirName, struct Buffer* sendBuf, int cfd);
void sendFile(const char* fileName, struct Buffer* sendBuf, int cfd);
