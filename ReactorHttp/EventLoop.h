#pragma once
#include<stdbool.h>
#include"Dispatcher.h"
#include"ChannelMap.h"
#include<pthread.h>

extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

//����ýڵ��е�channel�ķ�ʽ
enum ElemType{ADD, DELETE, MODIFY};
//����������еĽڵ�
struct ChannelElement
{
	int type; //����channel�ķ�ʽ
	struct Channel* channel;
	struct ChannelElement* next;
};

struct Dispatcher;
struct EventLoop 
{
	bool isQuit;   //�Ƿ�������
	struct Dispatcher* dispatcher;
	void* dispatcherData;
	//�������
	struct ChannelElement* head;
	struct ChannelElement* tail;
	//map
	struct ChannelMap* channelMap;
	//�߳�id��name��mutex
	pthread_t threadID;
	char threadName[32];
	pthread_mutex_t mutex;
	int socketPair[2];    //�洢����ͨ�ŵ�fd ͨ��socketpair��ʼ��
};

//��ʼ��
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(const char* threadName);
//������Ӧ��ģ��
int eventLoopRun(struct EventLoop* evLoop);
//��������ļ�fd
int eventActivate(struct EventLoop* evLoop,int fd,int event);
//��������������
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);
//������������е�����
int eventLoopProcessTask(struct EventLoop* evLoop);

//����dispatcher�еĽڵ�
int eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopRemove(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopModify(struct EventLoop* evLoop, struct Channel* channel);
//�ͷ�channel
int destroyChannel(struct EventLoop* evLoop, struct Channel* channel);
