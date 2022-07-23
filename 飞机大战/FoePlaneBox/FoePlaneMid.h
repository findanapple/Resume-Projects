#pragma once
#pragma once
#include"FoePlane.h"
class CFoePlaneMid : public CFoePlane
{
public:
	CFoePlaneMid();
	~CFoePlaneMid();

public:
	virtual void InitFoe();
	virtual void ShowFoe();

	virtual  bool IsHitFoe(CGunner* pGun);
	virtual  bool IsHitPlayer(CPlayerPlane& pPlayer);

};

