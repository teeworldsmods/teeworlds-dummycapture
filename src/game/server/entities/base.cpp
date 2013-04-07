/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you miss that file, contact Pikotee, because he changed some stuff here ...			 */
/*	... and would like to be mentioned in credits in case of using his code					 */

#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "base.h"

CBase::CBase(CGameWorld *pGameWorld, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_BASE)
{
	m_Pos = Pos;
	m_Type = Type;

	GameWorld()->InsertEntity(this);
	Reset();
}

void CBase::Reset()
{
	m_Score = 0;
	m_LastDistance = 0;
	m_MoveTicks = 0;
	//GameServer()->m_World.DestroyEntity(this);
}

void CBase::Tick()
{
	if(!GameServer()->m_apPlayers[15])
		return;

	CCharacter *pChr = GameServer()->GetPlayerChar(15);
	if(pChr && pChr->IsAlive())
	{
		if(pChr->Moving()) // useless now
			m_MoveTicks++;

		if(distance(m_Pos, pChr->m_Pos) < 16)
			GameServer()->m_pController->AddScore(m_Type);
		else
		{
			if(!m_LastDistance)
				m_LastDistance = distance(m_Pos, pChr->m_Pos);

			m_Score += m_LastDistance - distance(m_Pos, pChr->m_Pos);
			m_LastDistance = distance(m_Pos, pChr->m_Pos);
		}
	}

	for(int i = 0; i < MAX_CLIENTS-1; i++)
	{
		pChr = GameServer()->GetPlayerChar(i);

		if(pChr && pChr->IsAlive() && distance(m_Pos, pChr->m_Pos) < 16)
			pChr->Die(i, WEAPON_WORLD);

	}

}


void CBase::Snap(int SnappingClient)
{
	return;

	
}
