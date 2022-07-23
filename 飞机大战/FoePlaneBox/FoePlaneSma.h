#pragma once

#pragma once
#include"FoePlane.h"
class CFoePlaneSma : public CFoePlane
{
public:
	CFoePlaneSma();
	~CFoePlaneSma();

public:
	virtual void InitFoe();
	virtual void ShowFoe();

	virtual  bool IsHitFoe(CGunner* pGun);
	virtual  bool IsHitPlayer(CPlayerPlane& pPlayer);

};

