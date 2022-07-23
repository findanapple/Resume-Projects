#pragma once
#include"Gunner.h"
#include<list>
using namespace std;
class CGunnerBox
{
public:
	list<CGunner*>m_lstGun;
	CGunnerBox();
	~CGunnerBox();

public:
	void ShowAllGunner();
	void MoveAllGunner(int step);
	

};

