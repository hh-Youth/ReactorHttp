#define _GNU_SOURCE
#include "Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>

struct Buffer* bufferInit(int size) {
	struct Buffer* buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	if (buffer != NULL) {
		buffer->data = (char*)malloc(size);
		buffer->capacity = size;
		buffer->readPos = buffer->writePos = 0;
		memset(buffer->data, 0, size);
	}
	return buffer;
}

void bufferDestroy(struct Buffer* buf)
{
	if (buf!=NULL) {
		if (buf->data!=NULL) {
			free(buf->data);
		}
	}
	free(buf);
}

void bufferExtendRoom(struct Buffer* buffer, int size)
{
	// 1.ʣ���д�ڴ湻�ã�����Ҫ����
	if (bufferWriteableSize(buffer)>=size) {
		return;
	}
	// 2.ʣ���д�ڴ治������Ҫ�ϲ��Ѷ������ڴ��С�Ź��ã�����Ҫ����   readPosһֱ��0��ʼ����
	else if (bufferWriteableSize(buffer)+buffer->readPos>=size) {
		//�õ�δ�����ڴ��С
		int readable = bufferReadableSize(buffer);
		//�ƶ��ڴ�
		memcpy(buffer->data, buffer->data+buffer->readPos,readable);
		//����λ��
		buffer->readPos = 0;
		buffer->writePos = readable;
	}
	// 3.�ڴ治���ã�����
	else {
		void* temp = realloc(buffer->data,buffer->capacity+size);
		if (temp==NULL) {
			return;
		}
		memset(temp + buffer->capacity, 0, size);
		//��������
		buffer->data = temp;
		buffer->capacity += size;
	}
}

int bufferWriteableSize(struct Buffer* buffer)
{
	return buffer->capacity-buffer->writePos;
}

int bufferReadableSize(struct Buffer* buffer)
{
	return buffer->writePos - buffer->readPos;
}

int bufferAppendData(struct Buffer* buffer, const char* data, int size)
{
	if (buffer == NULL || data == NULL || size <= 0)
	{
		return -1;
	}
	// ���ݣ���ʵ���Ǽ���ڴ����������������������ݣ�
	bufferExtendRoom(buffer, size);
	// ���ݿ���
	memcpy(buffer->data + buffer->writePos, data, size);
	buffer->writePos += size;
	return 0;
}

int bufferAppendString(struct Buffer* buffer, const char* data)
{
	int size = strlen(data);
	int ret = bufferAppendData(buffer, data, size);
	return ret;
}

int bufferSocketRead(struct Buffer* buffer, int fd)
{
	//read/recv/readv
	struct iovec vec[2];  //���ǽ��� readv��ȡ�ַ��Ľṹ������ //iovec�ṹ���ڲ���һ��char* iov_base��int iov_len��������
	//��ʼ������Ԫ��
	int writeable = bufferWriteableSize(buffer);   //ʣ���д�ڴ���������
	vec[0].iov_base = buffer->data + buffer->writePos;   //��д��λ��
	vec[0].iov_len = writeable;
	char* tmpbuf = (char*)malloc(40960);
	vec[1].iov_base = buffer->data + buffer->writePos;
	vec[1].iov_len = 40960;

	int result = readv(fd, vec, 2);  //��������ͨ�����ݣ�2�� struct iovec
	                                    //����������д��vec�ṹ�������ÿ��Ԫ��iovec��iov_base����һ������д������ʼд�ڶ�����������ȥ��
	if (result == -1)
	{
		return -1;
	}
	else if (result <= writeable)  //��һ������Ԫ�ص�iov_baseûд��
	{
		buffer->writePos += result;  //����buffer��д���±�writePos
	}
	else
	{   //��һ���Ѿ�д���ˣ�ʣ������д����vec�ڶ�������Ԫ��vec[1]��iov_base
		buffer->writePos = buffer->capacity; 
		bufferAppendData(buffer, tmpbuf, result - writeable);  //���ڶ���vec[1]��ŵĶ�������׷�ӵ�buffer��
	}
	free(tmpbuf);
	return result;
}

char* bufferFindCRLF(struct Buffer* buffer)
{
	//strstr  --> ���ַ�����ƥ�����ַ���������\0������ char* strstr(const char* haystack,const char* needle);
	//mememm  -->�����ݿ���ƥ�������ݿ飨��Ҫָ�����ݿ��С��
	//void *menmen(const void* haystack, size_t haystacklen,const void* needle,size_t needlelen);
	char* ptr = memmem(buffer->data + buffer->readPos, bufferReadableSize(buffer), "\r\n", 2);
	return ptr;
}

int bufferSendData(struct Buffer* buffer, int socket)
{
	// �ж���������
	int readable = bufferReadableSize(buffer);
	if (readable > 0)
	{
		int count = send(socket, buffer->data + buffer->readPos, readable, MSG_NOSIGNAL);
		if (count > 0)
		{
			buffer->readPos += count;
			usleep(1);
		}
		return count;
	}
	return 0;
}

