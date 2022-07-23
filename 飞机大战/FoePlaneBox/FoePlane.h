#pragma once
#include<easyx.h>
#include"../PlayerPlane/PlayerPlane.h"

class CFoePlane
{
public:
	IMAGE m_imgFoe;
	int m_x;
	int m_y;
	int m_blood;
	int m_showid;


	CFoePlane();
	virtual ~CFoePlane();

public:
	
	void MoveFoe(int step);
	
	virtual void InitFoe()=0;
	virtual void ShowFoe()=0;

	virtual  bool IsHitFoe(CGunner* pGun)=0;
	virtual  bool IsHitPlayer(CPlayerPlane& pPlayer)=0;

};

