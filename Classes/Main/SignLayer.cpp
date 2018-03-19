﻿#include "SignLayer.h"
#include "GameControl.h"
#include "GameDataSet.h"
#include "ClientSocket.h"
#include "LogoScene.h"
#include "LoginInfo.h"

SignLayer::SignLayer(){
	m_isopen = false;
	GameControl::getIns()->setSignLayer(this);
}

SignLayer::~SignLayer(){
	if (this == GameControl::getIns()->getSignLayer()){
		GameControl::getIns()->setSignLayer(NULL);
	}
}

SignLayer *SignLayer::create(){
	SignLayer *p = new SignLayer();
	if (p&&p->init()){
		p->autorelease();
	}
	else{
		CC_SAFE_DELETE(p);
	}
	return p;
}

bool SignLayer::init()
{             
	if (!Layer::init())
    {
        return false;
    }
	m_RootLayer = (Layout *)GUIReader::shareReader()->widgetFromJsonFile("yaoqianshu.json");
	this->addChild(m_RootLayer);

	SEL_TouchEvent selector = toucheventselector(SignLayer::TouchEvent);
	GameDataSet::getButton(m_RootLayer, "close_btn", selector, this);
	GameDataSet::setText(m_RootLayer, "tip1","");
	m_light = GameDataSet::getLayout(m_RootLayer, "deng");
	m_point = GameDataSet::getLayout(m_RootLayer, "touchbtn");
	OpenRun(true);
	RunPoint(true);
    return true;
}

void SignLayer::TouchEvent(CCObject *obj, TouchEventType type){
	Button *btn = (Button *)obj;
	string name = btn->getName();
	if (type == TOUCH_EVENT_ENDED){
		if (name.compare("close_btn") == 0){
			this->removeFromParentAndCleanup(true);
		}
	}
}

void SignLayer::RunLight(float dt){
	if (m_light){
		m_light->setVisible(true);
		int ro = m_light->getRotation();
		ro = ro % 360;
		m_light->setRotation(ro + 30);
	}
}

void SignLayer::ShowLight(bool isshow){
	if (m_light){
		m_light->setVisible(isshow);
		if (!isshow){
			OpenRun(false);
		}
	}
}

void SignLayer::OpenRun(bool isopen){
	Director *dir = Director::sharedDirector();
	SEL_SCHEDULE selector = schedule_selector(SignLayer::RunLight);
	if (isopen){
		if (!m_isopen){
			m_isopen = true;
			dir->getScheduler()->scheduleSelector(selector, this, 1.0, false);
		}
	}
	else{
		if (m_isopen){
			m_isopen = false;
			dir->getScheduler()->unscheduleSelector(selector, this);
		}
	}
}

void SignLayer::RunPoint(bool isrun){
	m_point->stopAllActions();
	m_point->setRotation(0);
	if (isrun){
		m_point->runAction(CCRepeatForever::create(CCSequence::create(CCRotateTo::create(1.0, -20), CCRotateTo::create(1.0, 25), NULL)));
		CallFun();
		m_point->runAction(CCRepeatForever::create(CCSequence::create(DelayTime::create(2.0), CallFunc::create(this, callfunc_selector(SignLayer::CallFun)), NULL)));
	}
}

void SignLayer::CallFun(){
	char buff[30];
	for (int i = 0; i < 5; i++){
		sprintf(buff, "runchild%d", i);
		ImageView*img = (ImageView *)m_point->getChildByName(buff);
		if (!img){
			img = ImageView::create("XYDCJ_btn_choujiang.png", Widget::TextureResType::PLIST);
			m_point->addChild(img);
			img->setName(buff);
			img->setPosition(m_point->getSize() / 2.0);
			img->setTag(i);
		}
		else{
			img->stopAllActions();
			img->setScale(1.0);
			img->setOpacity(255);
		}
		img->setOpacity(255);
		img->setScale(1.0);
		img->runAction(Sequence::create(DelayTime::create(i*0.1),
			Spawn::create(ScaleTo::create(0.5, 1.1), FadeOut::create(0.5), NULL),
			NULL)
			);
	}
	
}