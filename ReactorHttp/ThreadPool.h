#pragma once
#include "EventLoop.h"
#include <stdbool.h>
#include "WorkerThread.h"

// 定义线程池
struct ThreadPool
{
    // 主线程的反应堆模型
    struct EventLoop* mainLoop;    //主线程反应堆
    bool isStart;  //是否启动
    int threadNum;   //子线程数量
    struct WorkerThread* workerThreads;   //子线程数组
    int index;  //下标
};

// 初始化线程池
struct ThreadPool* threadPoolInit(struct EventLoop* mainLoop, int count);
// 启动线程池
void threadPoolRun(struct ThreadPool* pool);
// 取出线程池中的某个子线程的反应堆实例
struct EventLoop* takeWorkerEventLoop(struct ThreadPool* pool);

