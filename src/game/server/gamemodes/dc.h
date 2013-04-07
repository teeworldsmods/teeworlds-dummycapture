/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you miss that file, contact Pikotee, because he changed some stuff here ...			 */
/*	... and would like to be mentioned in credits in case of using his code					 */

#ifndef GAME_SERVER_GAMEMODES_DC_H
#define GAME_SERVER_GAMEMODES_DC_H
#include <game/server/gamecontroller.h>

class CGameControllerDC : public IGameController
{
private:
	enum
	{
		DUMMY_DEAD=-3,
		CAPTURED=-1,
		DUMMY=15
	};

	int Captured;
public:
	CGameControllerDC(class CGameContext *pGameServer);

	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	virtual void Snap(int SnappingClient);
	virtual void Tick();

	// Dummy DC
	virtual void AddScore(int Team);
	virtual bool OnEntity(int Index, vec2 Pos);

};
#endif
