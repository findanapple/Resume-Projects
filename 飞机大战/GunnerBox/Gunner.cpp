#include"Gunner.h"
#include"../GameConfig/GameConfig.h"

CGunner::CGunner()
{
	m_x=0;
	m_y = 0;
}

CGunner::~CGunner()
{
}

void CGunner::InitGunner(int x, int y){
	m_x=x;
	m_y=y;
	::loadimage(&m_gun,"./res/gunner.jpg");
}
void CGunner::ShowGunner() {
	::putimage(m_x, m_y, IMG_GUNNER_WIDTH, IMG_GUNNER_HEIGHT, &m_gun, IMG_GUNNER_WIDTH, 0, SRCPAINT);//œ»Ã˘∆¡±ŒÕº
	::putimage(m_x, m_y, IMG_GUNNER_WIDTH, IMG_GUNNER_HEIGHT, &m_gun, 0, 0, SRCAND);//Ã˘‘≠Õº
}
void CGunner::MoveGunner(int step) {
	m_y -= step;
}
