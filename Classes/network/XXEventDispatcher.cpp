﻿#include "XXEventDispatcher.h"

XXEventDispatcher* XXEventDispatcher::m_ins = NULL;

XXEventDispatcher::XXEventDispatcher(){
	Director::sharedDirector()->getScheduler()->scheduleUpdate(this, 1, false);
}

XXEventDispatcher::~XXEventDispatcher(){
	Director::sharedDirector()->getScheduler()->unscheduleUpdate(this);
}

bool XXEventDispatcher::init()
{
	
    return true;
}

XXEventDispatcher *XXEventDispatcher::getIns(){
	if (!m_ins){
		m_ins = new XXEventDispatcher();
		m_ins->init();
	}
	return m_ins;
}

void XXEventDispatcher::addListener(int cmd, Object *target, EventHandler handler){
	CallList *clist = new CallList();
	clist->cmd = cmd;
	clist->obj = target;
	clist->handler = handler;

	
	CallList_Vec vec;
	if (m_eventLists.find(cmd) != m_eventLists.end()){
		vec = m_eventLists.at(cmd);
		vec.push_back(clist);
		m_eventLists.at(cmd) = vec;
	}
	else{
		vec.push_back(clist);
		m_eventLists.insert(make_pair(cmd,vec));
	}
}

void XXEventDispatcher::removeListener(int cmd, Object *target, EventHandler handler){
	CallList_Vec vec;
	if (m_eventLists.find(cmd) != m_eventLists.end()){
		vec = m_eventLists.at(cmd);
		CallList_Vec::iterator itr = vec.begin();
		while (itr!=vec.end()){
			CallList *clist = *itr;
			if(clist && clist->obj == target && clist->handler == handler){
				vec.erase(itr++);
				break;
			}
			else{
				itr++;
			}
		}
	}
	
}

void XXEventDispatcher::removeAllKistener(){
	std::map<int, CallList_Vec>::iterator eitr = m_eventLists.begin();
	while (eitr!=m_eventLists.end()){
		CallList_Vec vec = eitr->second;
		CallList_Vec::iterator itr = vec.begin();
		while (itr != vec.end()){
			vec.erase(itr++);
		}
		m_eventLists.erase(eitr++);
	}
}

void XXEventDispatcher::disEventDispatcher(ccEvent *event){
	if (event&&event->cmd > 0){
		m_Events.push_back(event);
	}
}

void XXEventDispatcher::update(float dt){
	EventPathch(m_Events);
}

void XXEventDispatcher::EventPathch(std::vector<ccEvent *> &ep){
	if (!ep.empty()){
		vector<ccEvent *>::iterator itr = ep.begin();
		if (itr != ep.end()){
			ccEvent *event = *itr;
			int cmd = event->cmd;
			CallList_Vec vec;
			if (m_eventLists.find(cmd) != m_eventLists.end()){
				vec = m_eventLists.at(cmd);
				CallList_Vec::iterator itr = vec.begin();
				while (itr != vec.end()){
					CallList *clist = *itr;
					if (clist && clist->obj && clist->handler){
						(clist->obj->*clist->handler)(event);
						itr++;
						break;
					}
					else{
						break;
					}
				}
			}

			delete event;
			ep.erase(itr);
		}
	}
}

void XXEventDispatcher::registerProto(int cmd, string tname){
	if (m_protos.find(cmd) == m_protos.end()){
		m_protos.insert(make_pair(cmd,tname));
	}
}

string XXEventDispatcher::getProtoName(int cmd){
	if (m_protos.find(cmd) != m_protos.end()){
		return m_protos.at(cmd);
	}
	return "";
}