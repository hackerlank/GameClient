﻿
#ifndef __LoginInfo__
#define __LoginInfo__

#include "AppMacros.h"
#include "ccEvent.h"


class LoginInfo:public Object
{
public:
	LoginInfo();
	~LoginInfo();
	static LoginInfo* getIns();
	bool init();
public:
	void SendCLogin(string uid, string pwd);
	void HandlerSLoginHand(ccEvent *event);

	void SendCRegister(string uid,string pwd,string uname);
	void HandlerSRegister(ccEvent *event);

	UserBase getMyUserBase(){
		return m_myinfo;
	}
private:
	static LoginInfo *m_shareLoginInfo;
	UserBase m_myinfo;
};

#endif 
