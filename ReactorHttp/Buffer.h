#pragma once

struct Buffer
{
    // ָ���ڴ��ָ��
    char* data;
    int capacity;
    int readPos;
    int writePos;
};

// ��ʼ��
struct Buffer* bufferInit(int size);
void bufferDestroy(struct Buffer* buf);
// ����  size����˼Ϊ��������Ҫsize���ȵĿռ�д����
void bufferExtendRoom(struct Buffer* buffer, int size);
// �õ�ʣ��Ŀ�д���ڴ�����
int bufferWriteableSize(struct Buffer* buffer);
// �õ�ʣ��Ŀɶ����ڴ�����
int bufferReadableSize(struct Buffer* buffer);


/*
    д�ڴ� : 1 ֱ��д . 2 �����׽�������
*/ 
int bufferAppendData(struct Buffer* buffer, const char* data, int size);
// 1 ֱ��д
int bufferAppendString(struct Buffer* buffer, const char* data);
// 2 �����׽�������
int bufferSocketRead(struct Buffer* buffer, int fd);



// ����\r\nȡ��һ��, �ҵ��������ݿ��е�λ��, ���ظ�λ��
char* bufferFindCRLF(struct Buffer* buffer);
// ��������
int bufferSendData(struct Buffer* buffer, int socket);
