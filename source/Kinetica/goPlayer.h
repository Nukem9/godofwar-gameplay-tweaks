#pragma once

namespace Kinetica
{

class goSoldier;

class goPlayer
{
public:
	virtual ~goPlayer();

	goSoldier *m_Soldier;

	static goPlayer *GetLocalPlayer()
	{
		return *Offsets::ResolveID<"gLocalPlayerGO", goPlayer **>();
	}
};

}