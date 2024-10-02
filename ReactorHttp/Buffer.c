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
	// 1.剩余可写内存够用，不需要扩容
	if (bufferWriteableSize(buffer)>=size) {
		return;
	}
	// 2.剩余可写内存不够，需要合并已读部分内存大小才够用，不需要扩容   readPos一直从0开始计算
	else if (bufferWriteableSize(buffer)+buffer->readPos>=size) {
		//得到未读的内存大小
		int readable = bufferReadableSize(buffer);
		//移动内存
		memcpy(buffer->data, buffer->data+buffer->readPos,readable);
		//更新位置
		buffer->readPos = 0;
		buffer->writePos = readable;
	}
	// 3.内存不够用，扩容
	else {
		void* temp = realloc(buffer->data,buffer->capacity+size);
		if (temp==NULL) {
			return;
		}
		memset(temp + buffer->capacity, 0, size);
		//更新数据
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
	// 扩容（其实就是检测内存容量够不够，不够会扩容）
	bufferExtendRoom(buffer, size);
	// 数据拷贝
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
	struct iovec vec[2];  //这是接收 readv读取字符的结构体数组 //iovec结构体内部有一个char* iov_base和int iov_len两个参数
	//初始化数组元素
	int writeable = bufferWriteableSize(buffer);   //剩余可写内存容量长度
	vec[0].iov_base = buffer->data + buffer->writePos;   //可写的位置
	vec[0].iov_len = writeable;
	char* tmpbuf = (char*)malloc(40960);
	vec[1].iov_base = buffer->data + buffer->writePos;
	vec[1].iov_len = 40960;

	int result = readv(fd, vec, 2);  //接收网络通信数据，2个 struct iovec
	                                    //将数据依次写入vec结构体数组的每个元素iovec的iov_base，第一个数组写满，开始写第二个，依次下去。
	if (result == -1)
	{
		return -1;
	}
	else if (result <= writeable)  //第一个数组元素的iov_base没写满
	{
		buffer->writePos += result;  //更新buffer的写入下标writePos
	}
	else
	{   //第一个已经写满了，剩余数据写到了vec第二个数组元素vec[1]的iov_base
		buffer->writePos = buffer->capacity; 
		bufferAppendData(buffer, tmpbuf, result - writeable);  //将第二个vec[1]存放的多余数据追加到buffer上
	}
	free(tmpbuf);
	return result;
}

char* bufferFindCRLF(struct Buffer* buffer)
{
	//strstr  --> 大字符串中匹配子字符串（遇到\0结束） char* strstr(const char* haystack,const char* needle);
	//mememm  -->大数据块中匹配子数据块（需要指定数据块大小）
	//void *menmen(const void* haystack, size_t haystacklen,const void* needle,size_t needlelen);
	char* ptr = memmem(buffer->data + buffer->readPos, bufferReadableSize(buffer), "\r\n", 2);
	return ptr;
}

int bufferSendData(struct Buffer* buffer, int socket)
{
	// 判断有无数据
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

