#include "StdAfx.h"
#include "CGHeartBeat.h"

UINT CGHeartBeatHandler::Execute( CGHeartBeat* pPacket, Player* pPlayer )
{
__ENTER_FUNCTION

	return PACKET_EXE_CONTINUE ;

__LEAVE_FUNCTION

	return PACKET_EXE_ERROR ;
}
