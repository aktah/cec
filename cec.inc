#if defined _CEC_INCLUDE_included
	#endinput
#endif
#define _CEC_INCLUDE_included

#if !defined PAWNRAKNET_INC_
	#error "Please include Pawn.RakNet"
#endif

#if defined RPC_Create3DTextLabel
	#undef RPC_Create3DTextLabel
	#define RPC_Create3DTextLabel 36
#else
	#define RPC_Create3DTextLabel 36
#endif

#if defined RPC_DialogBox
	#undef RPC_DialogBox
	#define RPC_DialogBox 61
#else
	#define RPC_DialogBox 61
#endif

#if defined RPC_SendClientMessage
	#undef RPC_SendClientMessage
	#define RPC_SendClientMessage 93
#else
	#define RPC_SendClientMessage 93
#endif

#if defined RPC_CLIENT_JOIN
	#undef RPC_CLIENT_JOIN
	#define RPC_CLIENT_JOIN 25
#else
	#define RPC_CLIENT_JOIN 25
#endif

native CE_Convert(const input[], output[]);
native CE_Convert_Dialog(const input[], output[], style = 0);
native CE_CountTag(const input[]);
native CE_CountVowel(const input[]);

enum E_TEXT_STRUCT
{
	e_wLabelId,
	e_dwColor,
	Float:e_fPosX,
	Float:e_fPosY,
	Float:e_fPosZ,
	Float:e_fDrawDistance,
	e_byteTestLOS,
	e_playerId,
	e_vehicleId,
	e_szText[4096]
};

enum PR_Chat
{
	dwColor,
	szLength,
	szMessage[144]
};

enum E_DIALOG_STRUCT
{
	e_wDialogID, 			// UINT16
	e_bDialogStyle, 		// UINT8
	e_bTitleLength, 		// UINT8
	e_szTitle[64], 			// char
	e_bButton1Len, 			// UINT8
	e_szButton1[61],		// char
	e_bButton2Len, 			// UINT8
	e_szButton2[64], 		// char
	e_szInfo[4096]
};

enum E_CLIENT_KEY
{
	e_clientVersion,
	e_szAuthKey[44]
}

static const stock m_szClientVersions[][E_CLIENT_KEY] = {
	{70, "15121F6F18550C00AC4B4F8A167D0379BB0ACA99043"},
	{69, "161186A32578A6E62CBD2BB46D56F7F061EDB3A15B0"},	
	{47, "1031CA8429843C9B8C178B65F3C73602578440D17F8"}
};

enum E_CLIENT_STRUCT
{
	iVersion, 					// INT32 
	byteMod, 					// UINT8 
	byteNicknameLen, 			// UINT8 
	NickName[MAX_PLAYER_NAME],	// char  
	uiClientChallengeResponse,  // UINT32
	byteAuthKeyLen, 			// UINT8 
	auth_key[64], 				// char  
	iClientVerLen, 				// UINT8 
	ClientVersion[12]			// char  

};

static bool:playerDevice[MAX_PLAYERS char];

stock bool:io(playerId) { return playerDevice{playerId}; }
stock setDeviceInfo(playerId, deviceType)
{
	playerDevice{playerId} = !!deviceType;
	return 1;
}

#if defined _inc_y_hooks || defined _INC_y_hooks
	hook OnOutcomingRPC(playerid, rpcid, BitStream:bs)
#else
	public OnOutcomingRPC(playerid, rpcid, BitStream:bs)
#endif
	{
		if(rpcid == RPC_SendClientMessage)
		{	
			if(playerid == -1 || (playerid != -1 && io(playerid)))
			{
				return 1;
			}

			static rpcData[PR_Chat];

			BS_ReadUint32(bs, rpcData[dwColor]);
			BS_ReadUint32(bs, rpcData[szLength]);
			BS_ReadString(bs, rpcData[szMessage], rpcData[szLength]);

			static str[144];
			str[0] = '\0';
			CE_Convert(rpcData[szMessage], str);

			new BitStream:bsn = BS_New();

			BS_WriteUint32(bsn, rpcData[dwColor]);
			BS_WriteUint32(bsn, rpcData[szLength]);
			BS_WriteString(bsn, str);

			PR_SendRPC(bsn, playerid, rpcid);

			BS_Delete(bsn);

			return 0;
		}
		else if(rpcid == RPC_DialogBox)
		{
			if(playerid == -1 || (playerid != -1 && io(playerid)))
			{
				return 1;
			}

			static dataDialog[E_DIALOG_STRUCT];
			BS_ReadValue(
		        bs,
		        PR_UINT16,  dataDialog[e_wDialogID],
				PR_UINT8, dataDialog[e_bDialogStyle],
				PR_UINT8, dataDialog[e_bTitleLength],
				PR_STRING, dataDialog[e_szTitle], dataDialog[e_bTitleLength],
				PR_UINT8, dataDialog[e_bButton1Len],
				PR_STRING, dataDialog[e_szButton1], dataDialog[e_bButton1Len],
				PR_UINT8, dataDialog[e_bButton2Len],
				PR_STRING, dataDialog[e_szButton2], dataDialog[e_bButton2Len],
				PR_CSTRING, dataDialog[e_szInfo], sizeof(dataDialog[e_szInfo])
		    );

			new BitStream:bsn = BS_New();

			static captionOutput[256], infoOutput[4096];
			captionOutput[0] = '\0';
			infoOutput[0] = '\0';

			CE_Convert_Dialog(dataDialog[e_szTitle], captionOutput, DIALOG_STYLE_MSGBOX);
			CE_Convert_Dialog(dataDialog[e_szInfo], infoOutput, dataDialog[e_bDialogStyle]);

			BS_WriteValue(
			    bsn,
			    PR_UINT16, dataDialog[e_wDialogID],
				PR_UINT8, dataDialog[e_bDialogStyle],
				PR_UINT8, dataDialog[e_bTitleLength],
				PR_STRING, captionOutput,
				PR_UINT8, dataDialog[e_bButton1Len],
				PR_STRING, dataDialog[e_szButton1], 
				PR_UINT8, dataDialog[e_bButton2Len],
				PR_STRING, dataDialog[e_szButton2], 
				PR_CSTRING, infoOutput
			);

			PR_SendRPC(bsn, playerid, rpcid);

			BS_Delete(bsn);
			return 0;
		}
		else if(rpcid == RPC_Create3DTextLabel)
		{
			if(playerid == -1 || (playerid != -1 && io(playerid)))
			{
				return 1;
			}

			static textData[E_TEXT_STRUCT];

			BS_ReadUint16(bs, textData[e_wLabelId]);
			BS_ReadUint32(bs, textData[e_dwColor]);
			BS_ReadFloat(bs,textData[e_fPosX]);
			BS_ReadFloat(bs,textData[e_fPosY]);
			BS_ReadFloat(bs,textData[e_fPosZ]);
			BS_ReadFloat(bs, textData[e_fDrawDistance]);
			BS_ReadUint8(bs, textData[e_byteTestLOS]);
			BS_ReadUint16(bs, textData[e_playerId]);
			BS_ReadUint16(bs, textData[e_vehicleId]);
			BS_ReadCompressedString(bs, textData[e_szText], sizeof(textData[e_szText]));

			static str[4096];
			str[0] = '\0';
			CE_Convert(textData[e_szText], str);

			new BitStream:bsn = BS_New();

			BS_WriteUint16(bsn, textData[e_wLabelId]);
			BS_WriteUint32(bsn, textData[e_dwColor]);
			BS_WriteFloat(bsn,textData[e_fPosX]);
			BS_WriteFloat(bsn,textData[e_fPosY]);
			BS_WriteFloat(bsn,textData[e_fPosZ]);
			BS_WriteFloat(bsn, textData[e_fDrawDistance]);
			BS_WriteUint8(bsn, textData[e_byteTestLOS]);
			BS_WriteUint16(bsn, textData[e_playerId]);
			BS_WriteUint16(bsn, textData[e_vehicleId]);
			BS_WriteCompressedString(bsn, str);

			PR_SendRPC(bsn, playerid, rpcid);

			BS_Delete(bsn);
			return 0;
		}
		#if defined INCLUDE_OnOutcomingRPC
			return INCLUDE_OnOutcomingRPC(playerid, rpcid, BitStream:bs);
		#else
			return 1;
		#endif
	}
#if !defined _inc_y_hooks && !defined _INC_y_hooks
	#if defined _ALS_OnOutcomingRPC
		#undef OnOutcomingRPC
	#else
		#define _ALS_OnOutcomingRPC
	#endif
	#define OnOutcomingRPC INCLUDE_OnOutcomingRPC
	#if defined INCLUDE_OnOutcomingRPC
		forward INCLUDE_OnOutcomingRPC(playerid, rpcid, BitStream:bs);
	#endif
#endif

#if defined _inc_y_hooks || defined _INC_y_hooks
	hook OnIncomingRPC(playerid, rpcid, BitStream:bs)
#else
	public OnIncomingRPC(playerid, rpcid, BitStream:bs)
#endif
	{
		if(rpcid == RPC_CLIENT_JOIN)
		{
			new dataClient[E_CLIENT_STRUCT];
			BS_ReadValue(
				bs,
				PR_INT32, dataClient[iVersion],
				PR_UINT8, dataClient[byteMod],
				PR_UINT8, dataClient[byteNicknameLen],
				PR_STRING, dataClient[NickName], dataClient[byteNicknameLen],
				PR_UINT32, dataClient[uiClientChallengeResponse],
				PR_UINT8, dataClient[byteAuthKeyLen],
				PR_STRING, dataClient[auth_key], dataClient[byteAuthKeyLen],
				PR_UINT8, dataClient[iClientVerLen],
				PR_STRING, dataClient[ClientVersion], dataClient[iClientVerLen]
			);

			setDeviceInfo(playerid, 0);

			static iter = 0;
			for(iter = 0; iter < sizeof(m_szClientVersions); iter++)
			{
				if(!strcmp(dataClient[auth_key], m_szClientVersions[iter][e_szAuthKey]))
				{
					setDeviceInfo(playerid, m_szClientVersions[iter][e_clientVersion]);
					break;
				}
			}
		}

		#if defined INCLUDE_OnIncomingRPC
			return INCLUDE_OnIncomingRPC(playerid, rpcid, BitStream:bs);
		#else
			return 1;
		#endif
	}

#if !defined _inc_y_hooks && !defined _INC_y_hooks
	#if defined _ALS_OnIncomingRPC
		#undef OnIncomingRPC
	#else
		#define _ALS_OnIncomingRPC
	#endif

	#define OnIncomingRPC INCLUDE_OnIncomingRPC
	#if defined INCLUDE_OnIncomingRPC
		forward INCLUDE_OnIncomingRPC(playerid, rpcid, BitStream:bs);
	#endif
#endif 
