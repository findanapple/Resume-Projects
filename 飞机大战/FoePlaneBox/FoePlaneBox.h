#pragma once
#include<list>
#include"FoePlane.h"


using namespace std;

class CFoePlaneBox
{
public:
	list<CFoePlane*>m_lstFoe;
	list<CFoePlane*>m_lstFoeBoom;


	CFoePlaneBox();
	~CFoePlaneBox();
public:
	void ShowAllFoe();
	void MoveAllFoe();
	void ShowAllBoomFoe();
};

