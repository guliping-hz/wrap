﻿#ifndef THREADINFORMER__H__
#define THREADINFORMER__H__
/*
	注释添加以及修改于 2014-4-4

	前前人留下了Udp的消息循环，前人留下了Windows静默窗口的消息循环，
	被我处理掉了因为这个不能跨平台

	我提供了一个新的消息循环队列ProcessIdle来处理业务。
	具体业务处理在私有函数dealMessageInner里面。

	CAvaratData 封装了一个机器人的相关信息，但是被裁掉了。。先保留着。
	*/
#include "client_socket.h"
#include "thread_mgr.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace Wrap{

	enum eMsgInformer
	{
		MSG_SEND_DATA = MSG_INVALID + 1,
	};

	struct MSGINFO{

		int server;
		ClientSocket* con;
		int		cmd;
		void*	v;
		int		len;
		bool	back;
		int		wseq;

		MSGINFO& operator=(const MSGINFO& other){
			if (this != &other)
			{
				this->server = other.server;
				this->con = other.con;
				this->cmd = other.cmd;
				this->v = other.v;
				this->len = other.len;
				this->wseq = other.wseq;
				this->back = other.back;
			}
			return *this;
		}
	};

	class ReserveData;

	struct MessageCenter{
		virtual int postMessage(int serverId, ClientSocket *conn, int cmd, void *v, int len, int seq,bool back = true) = 0;
		virtual int getMessage(MSGINFO& msg) = 0;
		virtual int sendToSvr(ClientSocket* pSvr, const char* buf, int len) = 0;
		virtual void addTimeout(int seq, ReserveData* data) = 0;
		virtual void delTimeout(int seq) = 0;
		virtual void onTimeout(ReserveData* data) = 0;
	};

	class ReserveData
	{
		DISABLE_COPY_CTOR(ReserveData);
	public:
		enum eType{
			TYPE_TIMEOUT,
			TYPE_REQFAILED
		};

		ReserveData(MessageCenter* center) :msgCenter(center){
			assert(msgCenter != nullptr);
		}
		virtual ~ReserveData(){}

		//网络线程
		void OnTimeOut(){
			msgCenter->onTimeout(this);
		}

		void setTimeout(eType value){
			type = value;
			if (type == TYPE_TIMEOUT){
				timeout = 30;//30秒后回调到上层
			}
			else{
				timeout = 3;//3秒后回调到上层
			}
		}

		eType type;
		int serverid;
		int seq;
		time_t t;
		int timeout;
		MessageCenter* msgCenter;
	};

	class ThreadInformer : public UdpSocket, public ThreadMgr
	{
	public:
#ifndef TEST_UDP
		ThreadInformer(MessageCenter* center);
#else
		ThreadInformer(NetworkUtil::Reactor *pReactor,const char *host = "127.0.0.1",short port = 27876);
#endif
		virtual ~ThreadInformer();
		//通知消息队列处理消息,,,这里消息只是充当事件的角色
		void inform();
		//初始化
		virtual int init();
		virtual bool unInit();
		//UdpSocket
		virtual void onFDRead();
		//CThreadMgr
		virtual void processMessage(int id, void* pData){};
		virtual void processIdle();

	protected:
		//处理 > MSG_SEND_DATA 的消息 msg.v 无需做处理
		virtual void dealCustomMsg(MSGINFO* msg){}
	private:
		void dealMessageInner();
	private:
		static bool sIsInit;
		MessageCenter* mMsgCenter;
		EventWrapper* mtEventInform;
	};

}

#endif//THREADINFORMER__H__
