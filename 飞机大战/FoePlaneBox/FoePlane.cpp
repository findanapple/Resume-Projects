#include"FoePlane.h"



CFoePlane::CFoePlane()
{
	
	int m_x=0;
	int m_y=0;
	int m_blood=0;
	int m_showid=0;
}

CFoePlane::~CFoePlane()
{
}


void CFoePlane::MoveFoe(int step)
{
	m_y += step;
}