#pragma once
#include<easyx.h>
class CGunner
{
public:
	IMAGE m_gun;
	int m_x;
	int m_y;

	CGunner();
	~CGunner();

public:
	void InitGunner(int x, int y);
	void ShowGunner();
	void MoveGunner(int step);


};

