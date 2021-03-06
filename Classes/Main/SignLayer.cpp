﻿#include "SignLayer.h"
#include "GameControl.h"
#include "GameDataSet.h"
#include "ClientSocket.h"
#include "LoginScene.h"
#include "LoginInfo.h"
#include "HallInfo.h"

SignLayer::SignLayer(){
	m_isopen = false;
	m_index = 0;
	m_curindex = 0;
	m_isupdate = false;
	GameControl::getIns()->setSignLayer(this);
}

SignLayer::~SignLayer(){
	RootRegister::getIns()->resetWidget(m_RootLayer);
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
	m_RootLayer =RootRegister::getIns()->getWidget("yaoqianshu.json");
	this->addChild(m_RootLayer);

	SEL_TouchEvent selector = toucheventselector(SignLayer::TouchEvent);
	GameDataSet::getButton(m_RootLayer, "close_btn", selector, this);
	m_btn=GameDataSet::getButton(m_RootLayer, "touchbtn", selector, this);
	GameDataSet::setText(m_RootLayer, "tip1","");
	m_light = GameDataSet::getLayout(m_RootLayer, "deng");
	m_point = GameDataSet::getLayout(m_RootLayer, "touchbtn");
	OpenRun(true);
	RunPoint(true);
	char buff[50];
	for (int i = 0; i < 12;i++){
		sprintf(buff,"p%d",i+1);
		Layout *ly = GameDataSet::getLayout(m_RootLayer,buff);
		ly->setOpacity(0);
		if (i < 2){
			sprintf(buff, "s%d", i + 1);
			Layout *ly = GameDataSet::getLayout(m_RootLayer, buff);
			ly->setOpacity(0);
			ly->setScale(0.1);
		}
	}
	Layout *point = GameDataSet::getLayout(m_RootLayer, "pointbg");
	if (point){
		point->setRotation(0);
	}
	HallInfo::getIns()->SendCSignList();
    return true;
}

void SignLayer::TouchEvent(CCObject *obj, TouchEventType type){
	Button *btn = (Button *)obj;
	string name = btn->getName();
	if (type == TOUCH_EVENT_ENDED){
		if (name.compare("close_btn") == 0){
			this->removeFromParentAndCleanup(true);
		}
		else if (name.compare("touchbtn") == 0){
			m_curindex = m_index;
			SSignList ssl = HallInfo::getIns()->getSSignList();
			int sign = ssl.sign();
			if (sign == 0){
				m_btn->setTouchEnabled(false);
				HallInfo::getIns()->SendCSign();
			}
			else{
				log("%s",XXIconv::GBK2UTF("签到次数已经用完").c_str());
			}
		}
	}
}

void SignLayer::setSignData(){
	SSignList sl = HallInfo::getIns()->getSSignList();
	int sign = sl.sign();
	int count = sl.count();
	if (sign > 0){
		if (m_btn){
			m_btn->setTouchEnabled(false);
		}
	}
	GameDataSet::setTextBMFont(m_RootLayer, "BitmapLabel_sign", count);
	int sz = sl.reward_size();
	char buff[50];
	for (int i = 0; i < 8;i++){
		sprintf(buff,"w%d",i+1);
		Layout *img = (Layout *)GameDataSet::getLayout(m_RootLayer, buff);
		if (i < sz){
			SignAward awrad = sl.reward(i);
			Reward rd = awrad.reward();
			Prop prop = rd.prop();
			int id = prop.id();
			int number = rd.number();
			int day = awrad.day();
			sprintf(buff,XXIconv::GBK2UTF("%d天").c_str(),day);
			GameDataSet::setTextBMFont(img, "BitmapLabel_w1", buff);
			sprintf(buff, "x%d", number);
			GameDataSet::setTextBMFont(img, "BitmapLabel_w1_num", buff);
			if (id == 2){
				GameDataSet::setImageView(img, "icon1", "card1.png");
			}
		}
		else{
			img->setVisible(false);
		}
	}
	if (sign){
		GameDataSet::setTextBMFont(m_RootLayer, "BitmapLabel_shengyu",0);
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

void SignLayer::Run(){
	SSign ss = HallInfo::getIns()->getSSign();
	m_index = ss.index();
	Layout *point = GameDataSet::getLayout(m_RootLayer,"pointbg");
	if (point){
		OpenRun(false);
		RunCall(point);
		RunPoint(false);
	}
}

void SignLayer::RunCall(Node *node){
	char buff[50];
	sprintf(buff, "p%d", m_curindex % 12 + 1);
	ImageView *ly =(ImageView *) GameDataSet::getLayout(m_RootLayer, buff);
	ly->loadTexture("XYDCJ_JP_dkgx2.png", Widget::TextureResType::PLIST);
	ly->runAction(Sequence::create(FadeIn::create(0.12), FadeOut::create(0.27), NULL));
	m_curindex++;
	node->runAction(Sequence::create(RotateBy::create(0.04, 30), CCCallFuncN::create(this, callfuncN_selector(SignLayer::RunCall)), NULL));
	if (m_curindex>48&&m_curindex % 12 == m_index){
		RunEnd();
	}
}

void SignLayer::RunEnd(){
	Layout *point = GameDataSet::getLayout(m_RootLayer, "pointbg");
	OpenRun(false);
	point->stopAllActions();
	point->setRotation(m_curindex*30);

	char buff[50];
	int index1 = (m_index + 1 ) % 12+1; //6,5,4,3,2
	int index2 = (m_index - 1+12) % 12 +1;//12 11 10 9 8
	int index3 = (index1-1+12)%12;
	index3=index3 == 0 ? 12 : index3;
	float dr = 0.15;
	float dur = 0.2;
	//7
	sprintf(buff, "p%d", index3);
	Layout *ly2 = GameDataSet::getLayout(m_RootLayer, buff);
	ly2->runAction(Sequence::create(FadeIn::create(dur), FadeOut::create(dur), NULL));
	log("xxxxxxxxx:%d", index3);

	for (int i = 0; i < 5;i++){
		//6 5 4 3 2
		int j = ((index1 + 4 - i) % 12 + 12) % 12 == 0 ? 12 : ((index1 + 4 - i) % 12 + 12) % 12;
		sprintf(buff,"p%d",j);
		Layout *ly = GameDataSet::getLayout(m_RootLayer, buff);
		ly->runAction(Sequence::create(DelayTime::create((i + 1)*dr), FadeIn::create(dur), FadeOut::create(dur), NULL));
		log("index[%d]:%d",i,j);
		//8 9 10 11 12
		j = ((index2 - 4 + i) % 12 + 12) % 12 == 0 ? 12 : ((index2 - 4 + i) % 12 + 12) % 12;
		sprintf(buff, "p%d", j);
		Layout *ly1 = GameDataSet::getLayout(m_RootLayer, buff);
		ly1->runAction(Sequence::create(DelayTime::create((i + 1)*dr), FadeIn::create(dur), FadeOut::create(dur), NULL));
		log("nnnnnnn[%d]:%d", i, j);
		
	}
	sprintf(buff, "p%d", m_index+1);
	ImageView *ly =(ImageView *) GameDataSet::getLayout(m_RootLayer, buff);
	ly->loadTexture("XYDCJ_JP_dkgx.png" , Widget::TextureResType::PLIST);
	ly->runAction(Sequence::create(DelayTime::create(6*dr), Repeat::create(Sequence::create(FadeIn::create(0.03), FadeOut::create(0.03),NULL),10), 
		CCCallFunc::create(this, callfunc_selector(SignLayer::RunEndCall))
		,NULL));
	for (int i = 0; i < 2;i++){
		sprintf(buff, "s%d", i+1);
		Layout *ly = GameDataSet::getLayout(m_RootLayer,buff);
		ly->setOpacity(255);
		ly->setScale(0.0);
		ly->runAction(Sequence::create(DelayTime::create(6*dr+i*0.5),
			Spawn::create(ScaleTo::create(0.5,2.0),FadeOut::create(1.0),NULL)
			,NULL));
	}

}

void SignLayer::RunEndCall(){
	RunPoint(true);
	OpenRun(true);
	
	SSign ss = HallInfo::getIns()->getSSign();
	int count = ss.count();
	GameDataSet::setTextBMFont(m_RootLayer, "BitmapLabel_sign", count);
	m_btn->setTouchEnabled(true);

	SSignList ssl = HallInfo::getIns()->getSSignList();
	if (ssl.sign()){
		GameDataSet::setTextBMFont(m_RootLayer, "BitmapLabel_shengyu", 0);
	}

	RewardTipLayer *p = GameControl::getIns()->getRewardTipLayer();
	if (!p){
		RepeatedPtrField<SignAward > vecs;
		for (int i = 0; i < 2; i++){
			SignAward *sa = vecs.Add();
			sa->set_day(3);
			sa->set_id(i + 1);
			Reward *rd =sa->mutable_reward();
			rd->set_rid(i+1);
			Prop *pp=rd->mutable_prop();
			pp->set_id(i + 1);
			rd->set_number(i == 0 ? 2000 : 2);
			
		}
		p = RewardTipLayer::create(vecs);
		this->addChild(p);
	}
}