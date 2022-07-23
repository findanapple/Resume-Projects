#include "FoePlaneBox.h"
#include<typeinfo>
#include"FoePlaneBig.h"
#include"FoePlaneMid.h"
#include"FoePlaneSma.h"
#include"../GameConfig/GameConfig.h"

#define DELETE_FOE_LIST(MLIST)\
	list<CFoePlane*>::iterator ITE##MLIST = MLIST.begin();\
	while (ITE##MLIST != MLIST.end()) {\
	if (*ITE##MLIST) {\
		delete* ITE##MLIST;\
		* ITE##MLIST = nullptr;\
	}ITE##MLIST++;}\
	MLIST.clear();//清空链表节点


CFoePlaneBox::CFoePlaneBox() {

}
CFoePlaneBox::~CFoePlaneBox() {
	DELETE_FOE_LIST(m_lstFoe)
	DELETE_FOE_LIST(m_lstFoeBoom)
}

void CFoePlaneBox::ShowAllFoe() {
	list<CFoePlane*>::iterator ite = m_lstFoe.begin();
	while (ite != m_lstFoe.end()) {
		if (*ite) {
			(*ite)->ShowFoe();
		}
		ite++;
	}			
}
void CFoePlaneBox::MoveAllFoe() {
	list<CFoePlane*>::iterator ite = m_lstFoe.begin();
	while (ite != m_lstFoe.end()) {
		if (*ite) {
			//运行时类型识别

			if (typeid(**ite) == typeid(CFoePlaneBig)) {
				(*ite)->MoveFoe(TIMER_FOEBIG_MOVE_STEP);
			}
			else if (typeid(**ite) == typeid(CFoePlaneMid)) {
				(*ite)->MoveFoe(TIMER_FOEMID_MOVE_STEP);
			}
			else if (typeid(**ite) == typeid(CFoePlaneSma)) {
				(*ite)->MoveFoe(TIMER_FOESMA_MOVE_STEP);
			}
			//判断敌人飞机是否出界
			if ((*ite)->m_y > IMG_BACKGROUND_HEIGHT)
			{
				//删除敌人飞机 
				delete* ite;
				*ite = nullptr;
				ite=m_lstFoe.erase(ite);	
				continue;
			}
				
		}
		ite++;
	}
}
void CFoePlaneBox::ShowAllBoomFoe(){
	
	list<CFoePlane*>::iterator ite = m_lstFoeBoom.begin();
	while (ite != m_lstFoeBoom.end()) {
		if (*ite) {
			(*ite)->ShowFoe();
		}
		ite++;
	}
}