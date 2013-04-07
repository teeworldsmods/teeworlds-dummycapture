/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you miss that file, contact Pikotee, because he changed some stuff here ...			 */
/*	... and would like to be mentioned in credits in case of using his code					 */

#ifndef GAME_SERVER_ENTITIES_BASE_H
#define GAME_SERVER_ENTITIES_BASE_H

class CBase : public CEntity
{
public:
	CBase(CGameWorld *pGameWorld, vec2 Pos, int Type);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);


private:
	int m_Type;
	float m_Score;
	float m_LastDistance;
	int64 m_MoveTicks;
};

#endif
