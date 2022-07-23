#pragma once
#include"FoePlane.h"
class CFoePlaneBig: public CFoePlane
{
public:
	CFoePlaneBig();
	~CFoePlaneBig();

public:
	virtual void InitFoe();
	virtual void ShowFoe();

	virtual  bool IsHitFoe(CGunner* pGun)override;
	virtual  bool IsHitPlayer(CPlayerPlane& pPlayer)override;

};

