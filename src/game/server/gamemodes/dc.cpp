/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you miss that file, contact Pikotee, because he changed some stuff here ...			 */
/*	... and would like to be mentioned in credits in case of using his code					 */

#include <engine/shared/config.h>

#include <game/server/entities/character.h>
#include "game/server/entities/base.h"
#include <game/server/player.h>
#include "dc.h"

CGameControllerDC::CGameControllerDC(class CGameContext *pGameServer) : IGameController(pGameServer)
{
	m_pGameType = "DC";
	m_GameFlags = GAMEFLAG_TEAMS|GAMEFLAG_FLAGS;
}

bool CGameControllerDC::OnEntity(int Index, vec2 Pos)
{
	if(IGameController::OnEntity(Index, Pos))
		return true;

	int Team = -1;
	if(Index == ENTITY_FLAGSTAND_RED) Team = TEAM_RED;
	if(Index == ENTITY_FLAGSTAND_BLUE) Team = TEAM_BLUE;

	if(Team == -1)
		return false;

	new CBase(&GameServer()->m_World, Pos, Team);
	return true;
}

int CGameControllerDC::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	if(pVictim->GetPlayer()->m_IsDummy)
		return 0;

	IGameController::OnCharacterDeath(pVictim, pKiller, Weapon);

	if(Weapon != WEAPON_GAME)
	{
		// do team scoring
		if(pKiller != pVictim->GetPlayer() && (pKiller->GetTeam()^1) == pVictim->GetPlayer()->GetTeam())
			m_aTeamscore[pKiller->GetTeam()&1]++;
	}

	pVictim->GetPlayer()->m_RespawnTick = max(pVictim->GetPlayer()->m_RespawnTick, Server()->Tick()+Server()->TickSpeed()*g_Config.m_SvRespawnDelay);

	return 0;
}

void CGameControllerDC::Snap(int SnappingClient)
{
	IGameController::Snap(SnappingClient);

	CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
	if(!pGameDataObj)
		return;

	pGameDataObj->m_TeamscoreRed = m_aTeamscore[TEAM_RED];
	pGameDataObj->m_TeamscoreBlue = m_aTeamscore[TEAM_BLUE];

	int State;

	CCharacter *pChar = GameServer()->GetPlayerChar(g_Config.m_SvMaxClients-1);

	if(!pChar)
		State = DUMMY_DEAD;
	else
		Captured = -1;


	pGameDataObj->m_FlagCarrierRed = ((Captured == TEAM_RED)?CAPTURED:(State == DUMMY_DEAD?DUMMY_DEAD:(pChar->m_aMoveID[TEAM_RED] == -1?(g_Config.m_SvMaxClients-1):pChar->m_aMoveID[TEAM_RED])));
	pGameDataObj->m_FlagCarrierBlue = ((Captured == TEAM_BLUE)?CAPTURED:(State == DUMMY_DEAD?DUMMY_DEAD:(pChar->m_aMoveID[TEAM_BLUE] == -1?(g_Config.m_SvMaxClients-1):pChar->m_aMoveID[TEAM_BLUE])));
}

void CGameControllerDC::AddScore(int Team)
{
	const int DUMMY = g_Config.m_SvMaxClients-1;
	char aBuf[128];

	int MoveID = -1;

	CCharacter *pChar = GameServer()->GetPlayerChar(DUMMY);

	if(!pChar)
		return;

	if(pChar->m_aMoveID[Team] != -1)
	{
		MoveID = pChar->m_aMoveID[Team];

		if(!GameServer()->m_apPlayers[MoveID])
		{
			GameServer()->GetPlayerChar(DUMMY)->Die(DUMMY, WEAPON_GAME);
			return;
		}

		str_format(aBuf, sizeof(aBuf), "The dummy was captured by '%s' (%s)", Server()->ClientName(MoveID), GetTeamName(Team));
		GameServer()->m_apPlayers[MoveID]->m_Score += 3;
		GameServer()->GetPlayerChar(DUMMY)->Die(MoveID, WEAPON_SELF);
		GameServer()->m_apPlayers[DUMMY]->m_LastPos = vec2(0, 0);
		GameServer()->m_World.ResetBases();
		GameServer()->SendChat(-1, -2, aBuf);
		m_aTeamscore[Team] += 100;
		Captured = Team;
	}
	else if(pChar->m_aMoveID[Team^1] != -1)
	{
		MoveID = pChar->m_aMoveID[Team^1];
		
		if(GameServer()->m_apPlayers[MoveID])
		{
			GameServer()->GetPlayerChar(DUMMY)->Die(MoveID, WEAPON_NINJA);
			GameServer()->m_apPlayers[MoveID]->m_Score -= g_Config.m_SvWrongBox;
			str_format(aBuf, sizeof(aBuf), "Wrong box! [-%i]", g_Config.m_SvWrongBox);

			if(g_Config.m_SvWrongBox)
				GameServer()->SendChatTarget(MoveID, aBuf);
		}
		else
			GameServer()->GetPlayerChar(DUMMY)->Die(DUMMY, WEAPON_GAME);
	}
}

void CGameControllerDC::Tick()
{
	IGameController::Tick();

	const int DUMMY = g_Config.m_SvMaxClients-1;

	bool ResetDummy = true;
	for(int i = 0; i < MAX_CLIENTS-1; i++)
	{
		if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam()+1)
			ResetDummy = false;
	}

	CCharacter *pChar = GameServer()->GetPlayerChar(DUMMY);

	if(ResetDummy && pChar && pChar->IsAlive() && GameServer()->m_apPlayers[DUMMY]->m_Moved)
	{
		GameServer()->GetPlayerChar(DUMMY)->Die(DUMMY, WEAPON_GAME);
		GameServer()->m_apPlayers[DUMMY]->m_LastPos = vec2(0, 0);
		GameServer()->m_World.ResetBases();
	}
}
