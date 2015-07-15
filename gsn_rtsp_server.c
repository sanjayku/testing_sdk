/*****************************************************************
*
*               COPYRIGHT (c) 2009-2013 GainSpan Corporation
*                         All Rights Reserved
*
* The source code contained or described herein and all documents
* related to the source code ("Material") are owned by GainSpan
* Corporation or its licensors.  Title to the Material remains
* with GainSpan Corporation or its suppliers and licensors.
*
* The Material is protected by worldwide copyright and trade secret
* laws and treaty provisions. No part of the Material may be used,
* copied, reproduced, modified, published, uploaded, posted, transmitted,
* distributed, or disclosed in any way except in accordance with the
* applicable license agreement.
*
* No license under any patent, copyright, trade secret or other
* intellectual property right is granted to or conferred upon you by
* disclosure or delivery of the Materials, either expressly, by
* implication, inducement, estoppel, except in accordance with the
* applicable license agreement.
*
* Unless otherwise agreed by GainSpan in writing, you may not remove or
* alter this notice or any other notice embedded in Materials by GainSpan
* or GainSpan's suppliers or licensors in any way.
*
* $RCSfile: gsn_rtsp_server.c,v $
*
*****************************************************************/
#include "time.h"
#include "gsn_includes.h"
#include "trcommon.h"
#include "modules\rtp\gsn_rtp.h"
#include "modules\rtsp\gsn_rtsp_server.h"
#include "app_debug.h"

//#define GSN_RTSP_DEBUG
#define GSN_RTSP_DEBUG_FAIL
#define GSN_RTSP_DEBUG_IMP


#ifdef GSN_RTSP_DEBUG
#define printf_rtspdebug AppDbg_Printf
#else
#define printf_rtspdebug(...)
#endif

#ifdef GSN_RTSP_DEBUG_FAIL
#define printf_rtspdebug_fail AppDbg_Printf
#else
#define printf_rtspdebug_fail(...)
#endif

#ifdef GSN_RTSP_DEBUG_IMP
#define printf_rtspdebug_imp AppDbg_Printf
#else
#define printf_rtspdebug_imp(...)
#endif



/* RTSP Context structures */
GSN_RTSP_SERVER_TCB rtspServerTcb;
GSN_RTSP_SERVER_CTX rtspServerCtx;

/* RTSP Protocol Methods and values */
static INT8 const* allowedCommandNames = "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY";
static INT8 const* videoStreamName = "live.h264";

/* RTP Port on which streaming happens */
extern UINT16 serverRTPPort;
extern TX_TIMER rtsp_timer;

/* RTSP DESCRIBE Method response: SDP Parameters  */
INT8 const* const sdpDescription_v = "v=0\r\n";
INT8 const* const sdpDescription_o = "o=- 1 1 IN IP4 "; //Add IP Address here
INT8 const* const sdpDescription_s = "s=/live.h264\r\n";
INT8 const* const sdpDescription_c = "c=IN IP4 ";		//Add IP Address here

INT8 const* const sdpDescription_t 	= "t=0 0\r\n";
INT8 const* const sdpDescription_a1 = "a=range:npt=0- \r\n";
INT8 const* const sdpDescription_m 	= "m=video "; //Add RTP Port here
INT8 const* const sdpDescription_m_remain 	= "/1 RTP/AVP 96\r\n";

INT8 const* const sdpDescription_a2 = "a=rtpmap:96 H264/90000\r\n";
INT8 const* const sdpDescription_a3 = "a=fmtp:96 packetization-mode=1;profile-level-id=4D400C;\r\n";

INT8 const* const sdpDescription_EOL = "\r\n";

/* RTP Context Structure */
extern PUBLIC GSN_RTP_TCB rtpCtx;
extern void AppVideo_RTSP_Comm_ThreadStop(void);
extern int rtsp_sd;
extern int sock_flag;

PRIVATE VOID
GsnRtsp_RstReqBuffer(VOID);


PRIVATE BOOL
GsnRtsp_ParseReqString(INT8 const* reqStr,
			       UINT32 	reqStrSize,
			       INT8* 	resultCmdName,
			       UINT32 	resultCmdNameMaxSize,
			       INT8* 	resultURLPreSuffix,
			       UINT32 	resultURLPreSuffixMaxSize,
			       INT8* 	resultURLSuffix,
			       UINT32 	resultURLSuffixMaxSize,
			       INT8* 	resultCSeq,
			       UINT32 	resultCSeqMaxSize,
			       UINT32* 	contentLength);

PRIVATE VOID
GsnRtsp_ParseTransportHdr(INT8 const* buf,
				 GSN_RTSP_STREAMING_MODE* streamingMode,
				 INT8* 		streamingModeString,
				 INT8* 		destinationAddressStr,
				 UINT8* 	destinationTTL,
				 UINT16* 	clientRTPPortNum, // if UDP
				 UINT16* 	clientRTCPPortNum // if UDP
				 );

PRIVATE INT8 const* 
GsnRtsp_DateHeader();


PRIVATE INT8* 
GsnRtsp_URL(INT32 sock);

PRIVATE VOID
GsnRtsp_Describe(INT8 const* cseq,
		     INT8 const* urlPreSuffix, INT8 const* urlSuffix,
		     INT8 const* fullRequestStr);

PRIVATE VOID
GsnRtsp_Setup(INT8 const* cseq,
		   INT8 const* urlPreSuffix, INT8 const* urlSuffix,
		   INT8 const* fullRequestStr);

PRIVATE VOID
GsnRtsp_ProcessCommand(INT8 const* cmdName,
			  INT8 const* urlPreSuffix, INT8 const* urlSuffix,
			  INT8 const* cseq, INT8 const* fullRequestStr);

PRIVATE VOID
GsnRtsp_Teardown( INT8 const* cseq);

PRIVATE VOID
GsnRtsp_Play( INT8 const* cseq,
		   INT8 const* fullRequestStr);


PRIVATE VOID
GsnRtsp_Setup(INT8 const* cseq,
		   INT8 const* urlPreSuffix, INT8 const* urlSuffix,
		   INT8 const* fullRequestStr);



/**
 *******************************************************************************
 * @ingroup GsnRtsp
 * @brief GsnRtsp_ClientSessionDeInit: De-Initialize RTSP Client Session
 *
 * This function de-initializes the ongoing RTSP client session
 * 
 * @param VOID
 * @retval VOID
 ******************************************************************************/
PUBLIC VOID
GsnRtsp_ClientSessionDeInit(VOID)
{
    tfClose(rtspServerCtx.RTSPClientSession.RtspDataSocketId);
}


PUBLIC VOID
GsnRtsp_Init(INT32 sock, UINT16 port)
{
    rtspServerCtx.RTSPServerSocket = sock; 
	rtspServerCtx.RTSPServerPort = port;   
}

PUBLIC VOID
GsnRtsp_DeInit(VOID) 
{
    GsnRtsp_ClientSessionDeInit();
    tfClose(rtspServerCtx.RTSPServerSocket);
}


PUBLIC VOID
GsnRtsp_ClientSessionInit(INT32 clientSocket, struct sockaddr_in clientAddr)
{
    rtspServerCtx.RTSPClientSession.RtspSessionId = 1;                       

    rtspServerCtx.RTSPClientSession.RtspDataSocketId = clientSocket;
    AppDbg_Printf("\r\nGsnRtsp_ClientSessionInit:clientSocket:%d\r\n",clientSocket);
    rtspServerCtx.RTSPClientSession.RtspClientAddress = clientAddr;          

    rtspServerCtx.RTSPClientSession.RtspSessionActive  = FALSE;
       
    GsnRtsp_RstReqBuffer();
  
}


PRIVATE VOID
GsnRtsp_RstReqBuffer(VOID) 
{
	UINT8 index = 0;

    rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd = 0;                                 
    rtspServerCtx.RTSPClientSession.RtspReqBuffRcvdMaxL = GSN_RTSP_BUFFER_SIZE;               
    rtspServerCtx.RTSPClientSession.RtspCrLf = &rtspServerCtx.RTSPClientSession.RtspReqBuff[index-3]; 

}


PUBLIC VOID
GsnRtsp_HndlReqBytes(VOID)
{
    INT32 newBytesRead = 0;
	INT32 numBytesRemaining = 0;
	INT32 status = 0;

    BOOL endOfMsg;
    UINT8* ptr;
	UINT8 *tmpPtr;

    INT8 cmdName[GSN_RTSP_PARAM_STRING_MAX];
    INT8 urlPreSuffix[GSN_RTSP_PARAM_STRING_MAX];
    INT8 urlSuffix[GSN_RTSP_PARAM_STRING_MAX];
    INT8 cseq[GSN_RTSP_PARAM_STRING_MAX];
    UINT32 contentLength;
	BOOL parseSucceeded;

	UINT32 requestSize;

	memset(rtspServerCtx.RTSPClientSession.RtspReqBuff, 0 , GSN_RTSP_BUFFER_SIZE);
	memset(rtspServerCtx.RTSPClientSession.RtspRespBuff, 0 , GSN_RTSP_BUFFER_SIZE);

RECV_RTSP_REQ:
    newBytesRead = recv(rtspServerCtx.RTSPClientSession.RtspDataSocketId, 
		                (char *)&rtspServerCtx.RTSPClientSession.RtspReqBuff[rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd], 
		                rtspServerCtx.RTSPClientSession.RtspReqBuffRcvdMaxL, 
		                MSG_DONTWAIT
		               );
	
    printf_rtspdebug_fail("\r\nRTSP: Length of request received:%d\r\n%s", newBytesRead,&rtspServerCtx.RTSPClientSession.RtspReqBuff[rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd]);

	if(newBytesRead > 0) {
		unsigned char *tPtr = &rtspServerCtx.RTSPClientSession.RtspReqBuff[rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd];
		UINT16 rtcpPktLen = (UINT16)(tPtr[2] << 8 | tPtr[3]);
		if((*tPtr == '$') && rtcpPktLen) {
			char dummyBuf[128] = "";
			recv(rtspServerCtx.RTSPClientSession.RtspDataSocketId, dummyBuf, rtcpPktLen, MSG_DONTWAIT);
			goto RECV_RTSP_REQ;
		}
	}
	  
  	do 
	{
		if (newBytesRead <= 0 || (UINT32)newBytesRead > rtspServerCtx.RTSPClientSession.RtspReqBuffRcvdMaxL) 
		{
			printf_rtspdebug_fail("\r\nRTSP: Data Read = %d Some thing fishy", newBytesRead);

			//rtspServerCtx.RTSPClientSession.RtspSessionActive = FALSE;
			break;
		}

		endOfMsg = FALSE;
		ptr = &rtspServerCtx.RTSPClientSession.RtspReqBuff[rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd];

		tmpPtr = rtspServerCtx.RTSPClientSession.RtspCrLf + 2;
		if (tmpPtr < rtspServerCtx.RTSPClientSession.RtspReqBuff) tmpPtr = rtspServerCtx.RTSPClientSession.RtspReqBuff;

		while (tmpPtr < &ptr[newBytesRead-1]) 
		{
			if (*tmpPtr == '\r' && *(tmpPtr+1) == '\n') 
			{
				if (tmpPtr - rtspServerCtx.RTSPClientSession.RtspCrLf == 2) 
				{ 
					endOfMsg = TRUE;
					printf_rtspdebug("\r\nRTSP: -------Received End of Request------");
					break;
				}
				rtspServerCtx.RTSPClientSession.RtspCrLf = tmpPtr;
			}
			++tmpPtr;
		}

		rtspServerCtx.RTSPClientSession.RtspReqBuffRcvdMaxL -= newBytesRead;
		rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd += newBytesRead;

		if (!endOfMsg) 
		{
			printf_rtspdebug_fail("\r\nRTSP: End of Request not found--------");
			break; 
		}

		rtspServerCtx.RTSPClientSession.RtspReqBuff[rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd] = '\0';

		*rtspServerCtx.RTSPClientSession.RtspCrLf = '\0'; 

		parseSucceeded = GsnRtsp_ParseReqString(
			                    (INT8*)rtspServerCtx.RTSPClientSession.RtspReqBuff, 
			                    rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd,
							    cmdName, sizeof(cmdName),
							    urlPreSuffix, sizeof(urlPreSuffix),
							    urlSuffix, sizeof(urlSuffix),
							    cseq, sizeof(cseq),
							    &contentLength
							   );

		*rtspServerCtx.RTSPClientSession.RtspCrLf = '\r';

		if (parseSucceeded) 
		{
			
			printf_rtspdebug("\r\nRTSP: cmdName: \"%s\"", cmdName);
			printf_rtspdebug("\r\nRTSP: urlPreSuffix \"%s\"", urlPreSuffix);
			printf_rtspdebug("\r\nRTSP: urlSuffix \"%s\"", urlSuffix);
			printf_rtspdebug("\r\nRTSP: CSeq \"%s\"", cseq);
			printf_rtspdebug("\r\nRTSP: Content-Length %d", contentLength);
			printf_rtspdebug("\r\nRTSP: Message Length: %d", ptr + newBytesRead - (tmpPtr + 2));

			if (ptr + newBytesRead < tmpPtr + 2 + contentLength) 
			{
				printf_rtspdebug_fail("\r\nRTSP: Incomplete Data");
				break; 
			}

			if (strcmp(cmdName, "OPTIONS") == 0) 
			{
				printf_rtspdebug_imp("\r\nRTSP: OPTIONS Received");
				
				snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
			     sizeof(rtspServerCtx.RTSPClientSession.RtspRespBuff),
			     "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n",
			     cseq,
			     GsnRtsp_DateHeader(),
			     allowedCommandNames
			    );
			} 
			else if (strcmp(cmdName, "DESCRIBE") == 0) 
			{
				GsnRtsp_Describe(cseq, urlPreSuffix, urlSuffix, (INT8 const*)rtspServerCtx.RTSPClientSession.RtspReqBuff);
//                                AppDbg_Printf("\r\nDESCRIBE RECEIVED\r\n");
                        } 
			else if (strcmp(cmdName, "SETUP") == 0) 
			{
				GsnRtsp_Setup(cseq, urlPreSuffix, urlSuffix, (INT8 const*)rtspServerCtx.RTSPClientSession.RtspReqBuff);
//                                AppDbg_Printf("\r\nSETUP RECEIVED\r\n");			
                        } 
			else if (strcmp(cmdName, "TEARDOWN") == 0 || strcmp(cmdName, "PLAY") == 0)
			{
				GsnRtsp_ProcessCommand(cmdName, urlPreSuffix, urlSuffix, cseq, (INT8 const*)rtspServerCtx.RTSPClientSession.RtspReqBuff);
                                
                        } 
			else 
			{
				printf_rtspdebug_imp("\r\nRTSP: Method not Allowed - %s Received", cmdName);
				
				snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
			     sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
			     "RTSP/1.0 405 Method Not Allowed\r\nCSeq: %s\r\n%sAllow: %s\r\n\r\n",
			     cseq, 
			     GsnRtsp_DateHeader(), 
			     allowedCommandNames
			    );
			}
		} 
		else 
		{
			snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
	         sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
		     GsnRtsp_DateHeader(), 
		     allowedCommandNames
		    );
		}

		status = send(rtspServerCtx.RTSPClientSession.RtspDataSocketId, 
			 (INT8 const*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
			 strlen((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff), 
			 0);

		if(-1 == status)
		{
			printf_rtspdebug_fail("\r\nRTSP: Socket Send Error = %d \r\n", tfGetSocketError(rtspServerCtx.RTSPClientSession.RtspDataSocketId));
		}

		requestSize = (rtspServerCtx.RTSPClientSession.RtspCrLf+4-rtspServerCtx.RTSPClientSession.RtspReqBuff) + contentLength;
		numBytesRemaining = rtspServerCtx.RTSPClientSession.RtspReqBuffRcvd - requestSize;
		GsnRtsp_RstReqBuffer(); 

		if (numBytesRemaining > 0) 
		{
			memmove(rtspServerCtx.RTSPClientSession.RtspReqBuff, &rtspServerCtx.RTSPClientSession.RtspReqBuff[requestSize], numBytesRemaining);
			newBytesRead = numBytesRemaining;
		}

		if (0 == strcmp(cmdName, "SETUP"))
		{
			if (GSN_FAILURE == GsnRtp_CtxInit(&rtspServerCtx))
			{
				printf_rtspdebug_fail("\r\nRTSP: RTP Init Fail. May be Already Initialized");
			}
			else
			{
				printf_rtspdebug_imp("\r\nRTSP: RTP Initialized");
			}
		}
		else if (0 == strcmp(cmdName, "PLAY"))
		{

			if(NULL != rtpCtx.rtpTask_Notifier)
			{
				printf_rtspdebug_imp("\r\nRTSP: Message Posted to RTP Task to start Streaming\r\n");
				rtpCtx.rtpTask_Notifier(RTP_REQRECV_EVENT, &rtpCtx );
			}
			else
			{
				printf_rtspdebug_fail("\r\nRTSP: RTP Notifier Function NUll !!!");
			}
		}
		else if (0 == strcmp(cmdName, "TEARDOWN"))
		{
                        GsnRtp_CtxDeInit(&rtspServerCtx);
                         rtsp_sd=-1;
		}


	} while (numBytesRemaining > 0);
}

PRIVATE BOOL
GsnRtsp_ParseReqString(INT8 const* reqStr,
			       UINT32 reqStrSize,
			       INT8* resultCmdName,
			       UINT32 resultCmdNameMaxSize,
			       INT8* resultURLPreSuffix,
			       UINT32 resultURLPreSuffixMaxSize,
			       INT8* resultURLSuffix,
			       UINT32 resultURLSuffixMaxSize,
			       INT8* resultCSeq,
			       UINT32 resultCSeqMaxSize,
			       UINT32* contentLength)
{

	BOOL parseSucceeded;
		
	UINT32 index,subindex, urlindex;
	UINT32 k,k1,k2;
	UINT32 scannum;
	INT8 byte;

	*contentLength = 0; 
	parseSucceeded = FALSE;
	  
	for (index = 0; index < resultCmdNameMaxSize-1 && index < reqStrSize; ++index) 
	{
		byte = reqStr[index];
		if (byte == ' ' || byte == '\t') 
		{
			parseSucceeded = TRUE;
			printf_rtspdebug("\r\nRTSP: Parsing of RTSP Command Succeded");
			break;
		}

		resultCmdName[index] = byte;
	}
	
	resultCmdName[index] = '\0';
	if (!parseSucceeded) 
	{
		printf_rtspdebug_fail("\r\nRTSP: Parsing of RTSP command failed");
		return FALSE;
	}
	
	subindex = index+1;
	while (subindex < reqStrSize && (reqStr[subindex] == ' ' || reqStr[subindex] == '\t')) ++subindex; 
	for (; (INT32)subindex < (INT32)(reqStrSize-8); ++subindex) 
	{
		if ((reqStr[subindex] == 'r' || reqStr[subindex] == 'R')
		&& (reqStr[subindex+1] == 't' || reqStr[subindex+1] == 'T')
		&& (reqStr[subindex+2] == 's' || reqStr[subindex+2] == 'S')
		&& (reqStr[subindex+3] == 'p' || reqStr[subindex+3] == 'P')
		&& reqStr[subindex+4] == ':' && reqStr[subindex+5] == '/') 
		{
			subindex += 6;
			if (reqStr[subindex] == '/') 
			{
				++subindex;
				while (subindex < reqStrSize && reqStr[subindex] != '/' && reqStr[subindex] != ' ') ++subindex;
			} else 
			{
				--subindex;
			}
			index = subindex;
			break;
		}
	}
	

	parseSucceeded = FALSE;
	for (k = index+1; (INT32)k < (INT32)(reqStrSize-5); ++k) 
	{
		if (reqStr[k] == 'R' && reqStr[k+1] == 'T' &&
		reqStr[k+2] == 'S' && reqStr[k+3] == 'P' && reqStr[k+4] == '/') 
		{
			while (--k >= index && reqStr[k] == ' ') {} 
			k1 = k;
			while (k1 > index && reqStr[k1] != '/') --k1;

			if (k - k1 + 1 > resultURLSuffixMaxSize) 
			{
				printf_rtspdebug_fail("\r\nRTSP: There is no room to copy the URL suffix");
				return FALSE;
			}

			urlindex = 0;
			k2 = k1+1;

			while (k2 <= k) resultURLSuffix[urlindex++] = reqStr[k2++];
			resultURLSuffix[urlindex] = '\0';
			if (k1 - index > resultURLPreSuffixMaxSize) 
			{
				printf_rtspdebug_fail("\r\nRTSP: There is no room to copy the URL Prefix");
				return FALSE; 
			}
			urlindex = 0; k2 = index + 1;
			while (k2 <= k1 - 1) resultURLPreSuffix[urlindex++] = reqStr[k2++];
			resultURLPreSuffix[urlindex] = '\0';

			index = k + 7;
			parseSucceeded = TRUE;
			printf_rtspdebug("\r\nRTSP: Parsing of RTSP URL suffix Succeded");
			break;
		}
	}

	  
	if (!parseSucceeded) 
	{
		printf_rtspdebug_fail("\r\nRTSP: Parsing of RTSP URL suffix failed");
		return FALSE;
	}
	
	parseSucceeded = FALSE;
	for (subindex = index; (INT32)subindex < (INT32)(reqStrSize-5); ++subindex) 
	{
		if (reqStr[subindex] == 'C' && reqStr[subindex+1] == 'S' && reqStr[subindex+2] == 'e' &&
		reqStr[subindex+3] == 'q' && reqStr[subindex+4] == ':') 
		{
			subindex += 5;
			while (subindex < reqStrSize && (reqStr[subindex] ==  ' ' || reqStr[subindex] == '\t')) ++subindex;
			for (urlindex = 0; urlindex < resultCSeqMaxSize-1 && subindex < reqStrSize; ++urlindex,++subindex) 
			{
				byte = reqStr[subindex];
				if (byte == '\r' || byte == '\n') 
				{
					parseSucceeded = TRUE;
					printf_rtspdebug("\r\nRTSP: Parsing of RTSP Cseq Succeded");
					break;
				}

				resultCSeq[urlindex] = byte;
			}
			resultCSeq[urlindex] = '\0';
			break;
		}
	}
	  
	if (!parseSucceeded) 
	{
		printf_rtspdebug_fail("\r\nRTSP: Parsing of RTSP Cseq failed");
		return FALSE;
	}
	
	for (subindex = index; (INT32)subindex < (INT32)(reqStrSize-15); ++subindex) 
	{
		if (reqStr[subindex] == 'C' && reqStr[subindex+1] == 'o' && reqStr[subindex+2] == 'n' && reqStr[subindex+3] == 't' && reqStr[subindex+4] == 'e' &&
		reqStr[subindex+5] == 'n' && reqStr[subindex+6] == 't' && reqStr[subindex+7] == '-' && (reqStr[subindex+8] == 'L' || reqStr[subindex+8] == 'l') &&
		reqStr[subindex+9] == 'e' && reqStr[subindex+10] == 'n' && reqStr[subindex+11] == 'g' && reqStr[subindex+12] == 't' && reqStr[subindex+13] == 'h' &&
		reqStr[subindex+14] == ':') 
		{
			subindex += 15;
			while (subindex < reqStrSize && (reqStr[subindex] ==  ' ' || reqStr[subindex] == '\t')) ++subindex;
	
			if (sscanf(&reqStr[subindex], "%u", &scannum) == 1) 
			{
				*contentLength = scannum;
			}
		}
	}
	
	return TRUE;
}


PRIVATE INT8 const* 
GsnRtsp_DateHeader() 
{
	static INT8 buf[GSN_RTSP_PARAM_STRING_MAX];

	sprintf((INT8*)buf,"Date: Mon, 29 April 2013 00:00:00 GMT\r\n");

	return buf;
}


PRIVATE INT8* 
GsnRtsp_URL(INT32 sock) 
{

	static INT8 resultURL[GSN_RTSP_PARAM_STRING_MAX];  
	struct sockaddr_in ourAddress;
	UINT32 ipaddr;
	INT32 namelen = sizeof (ourAddress);

	getsockname(sock,(struct sockaddr*)&ourAddress, &namelen);
	ipaddr = ntohl(ourAddress.sin_addr.s_addr);

	sprintf(resultURL, "rtsp://%d.%d.%d.%d/%s", ((ipaddr >> 24) & 0xff),((ipaddr >> 16) & 0xff),((ipaddr >> 8) & 0xff),(ipaddr & 0xff), videoStreamName);

	
	sprintf((char *)rtspServerCtx.ipAddress, "%d.%d.%d.%d",((ipaddr >> 24) & 0xff),((ipaddr >> 16) & 0xff),((ipaddr >> 8) & 0xff),(ipaddr & 0xff));
	printf_rtspdebug("\r\nRTSP: IP Address = %s", rtspServerCtx.ipAddress);
	sprintf((char *)rtspServerCtx.rtp_port,"%d", serverRTPPort);
	printf_rtspdebug("\r\nRTSP: Port = %s", rtspServerCtx.rtp_port);

	return resultURL;
}


PRIVATE VOID
GsnRtsp_ParseTransportHdr(INT8 const* buf,
				 GSN_RTSP_STREAMING_MODE* streamingMode,
				 INT8* streamingModeString,
				 INT8* destinationAddressStr,
				 UINT8* destinationTTL,
				 UINT16* clientRTPPortNum, 
				 UINT16* clientRTCPPortNum
				 ) 
{

	UINT16 p1, p2;
	UINT32 ttl;

	INT8 const* fields;
	INT8* field[GSN_RTSP_PARAM_STRING_MAX*2];


//	*streamingMode = GSN_RTSP_MODE_RTP_UDP;
	*streamingMode = GSN_RTSP_MODE_RTP_TCP;
	*clientRTPPortNum = 0;
	*clientRTCPPortNum = 1;

	while (1) 
	{
		if (*buf == '\0') 
		{
		    printf_rtspdebug_fail("\r\nRTSP: Buff is NULL");
		    return; 
		}
		if (*buf == '\r' && *(buf+1) == '\n' && *(buf+2) == '\r') 
		{
		    printf_rtspdebug_fail("\r\nRTSP: End of the headers not found");
		    return; 
		}
		if (strncasecmp(buf, "Transport: ", 11) == 0) 
		{
			 printf_rtspdebug("\r\nRTSP: Found the Transport header");
			 break;
		}
		++buf;
	}
	
	fields = buf + 11;
	

	
	while (sscanf(fields, "%[^;\r\n]", field) == 1) 
	{
		if (strcmp((const char*)field, "RTP/AVP/TCP") == 0) 
		{
			*streamingMode = GSN_RTSP_MODE_RTP_TCP;
			printf_rtspdebug("\r\nRTSP: streamingMode = GSN_RTSP_MODE_RTP_TCP");
		} 
		else if (strcmp((const char*)field, "RAW/RAW/UDP") == 0 ||
			 strcmp((const char*)field, "MP2T/H2221/UDP") == 0) 
		{
			*streamingMode = GSN_RTSP_MODE_RAW_UDP;
			printf_rtspdebug("\r\nRTSP: streamingMode = GSN_RTSP_MODE_RAW_UDP");
			
		} 
		else if (strncasecmp((const char*)field, "destination=", 12) == 0) 
		{
			
		} 
		else if (sscanf((const char*)field, "ttl%u", &ttl) == 1) 
		{
			
		} 
		else if (sscanf((const char*)field, "client_port=%hu-%hu", &p1, &p2) == 2) 
		{
			*clientRTPPortNum = p1;
			*clientRTCPPortNum = (*streamingMode == GSN_RTSP_MODE_RAW_UDP) ? 0 : p2; 
		} 
		else if (sscanf((const char*)field, "client_port=%hu", &p1) == 1) 
		{
			*clientRTPPortNum = p1;
			*clientRTCPPortNum = (*streamingMode == GSN_RTSP_MODE_RAW_UDP) ? 0 : p1 + 1;
		} 
		

		fields += strlen((const char*)field);
		while (*fields == ';') ++fields; 
		if (*fields == '\0' || *fields == '\r' || *fields == '\n') break;
	}
}


PRIVATE VOID
GsnRtsp_Describe(INT8 const* cseq,
		     INT8 const* urlPreSuffix, INT8 const* urlSuffix,
		     INT8 const* fullRequestStr) 
{



	INT8* rtspURL_ptr = NULL;
	INT8 urlTotalSuffix[GSN_RTSP_PARAM_STRING_MAX];
	UINT32 sdpDescriptionSize;

	printf_rtspdebug_imp("\r\nRTSP: DESCRIBE Received");

	do 
	{

		if (strlen(urlPreSuffix) + strlen(urlSuffix) + 2 > sizeof urlTotalSuffix) 
		{
			snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
	         sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
		     GsnRtsp_DateHeader(), 
		     allowedCommandNames
		    );
			printf_rtspdebug_fail("\r\nRTSP: DESCRIBE Length Mismatch...Exiting");
			break;
		}

		urlTotalSuffix[0] = '\0';
		if (urlPreSuffix[0] != '\0') 
		{
			strcat(urlTotalSuffix, urlPreSuffix);
			strcat(urlTotalSuffix, "/");
		}
		strcat(urlTotalSuffix, urlSuffix);

		printf_rtspdebug("\r\nRTSP: Handling DESCRIBE");


		rtspURL_ptr = GsnRtsp_URL(rtspServerCtx.RTSPClientSession.RtspDataSocketId);

		sdpDescriptionSize = strlen(sdpDescription_v) + 
							strlen(sdpDescription_o) + strlen((const char *)rtspServerCtx.ipAddress) + strlen(sdpDescription_EOL) + 
							strlen(sdpDescription_s) + 
							strlen(sdpDescription_c) + strlen((const char *)rtspServerCtx.ipAddress) + strlen(sdpDescription_EOL) + 
							strlen(sdpDescription_t) + 
							strlen(sdpDescription_a1) + 
							strlen(sdpDescription_m) + strlen((const char *)rtspServerCtx.rtp_port) + strlen(sdpDescription_m_remain) + 
							strlen(sdpDescription_a2) + 
							strlen(sdpDescription_a3);

		printf_rtspdebug("\r\nRTSP: sdpDescriptionSize = %d", sdpDescriptionSize);
		
		snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
			        sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
			"RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
			"%s"
			"Content-Base: %s/\r\n"
			"Content-Type: application/sdp\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			cseq,
			GsnRtsp_DateHeader(),
			rtspURL_ptr,
			sdpDescriptionSize,
			sdpDescription_v,
			sdpDescription_o, rtspServerCtx.ipAddress, sdpDescription_EOL,
			sdpDescription_s,
			sdpDescription_c, rtspServerCtx.ipAddress, sdpDescription_EOL,
			sdpDescription_t,
			sdpDescription_a1,
			sdpDescription_m, rtspServerCtx.rtp_port,sdpDescription_m_remain,
			sdpDescription_a2,
			sdpDescription_a3);

		printf_rtspdebug("\r\nRTSP: Buffer = %s", rtspServerCtx.RTSPClientSession.RtspRespBuff);

		
	} while (0);

}


PRIVATE VOID
GsnRtsp_Setup(INT8 const* cseq,
		   INT8 const* urlPreSuffix, INT8 const* urlSuffix,
		   INT8 const* fullRequestStr) 
{


	INT8 const* streamName = urlPreSuffix; 


	GSN_RTSP_STREAMING_MODE streamingMode;
	INT8* streamingModeString = NULL; 
	INT8* clientsDestinationAddressStr;
	UINT8 clientsDestinationTTL;
	UINT16 clientRTPPortNum, clientRTCPPortNum;

	struct sockaddr_in ourAddress;
	UINT32 ipaddr;
	INT32 namelen = sizeof (ourAddress);

	printf_rtspdebug_imp("\r\nRTSP: SETUP Received");

	if (TRUE == rtspServerCtx.RTSPClientSession.RtspSessionActive)
	{
		snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
	         sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
		     GsnRtsp_DateHeader(), 
		     allowedCommandNames
		    );
		return;
	}

	do
	{
			
		
		if (strcmp(streamName, videoStreamName)) 
		{
			snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
		     sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n%s\r\n",
		     cseq, 
		     GsnRtsp_DateHeader()
		    );
			break;
		}
		else
		{
		    printf_rtspdebug("\r\nRTSP: Requested stream found");
		}

		
		GsnRtsp_ParseTransportHdr(fullRequestStr, 
		                     &streamingMode, streamingModeString,
		                     clientsDestinationAddressStr, &clientsDestinationTTL,
		                     &clientRTPPortNum, &clientRTCPPortNum);


		
		clientRTPPort = clientRTPPortNum;
		clientRTCPPort = clientRTCPPortNum;

		printf_rtspdebug("\r\nRTSP: client RTP-RTCP PortNum = %d-%d",clientRTPPortNum,clientRTCPPortNum);

		
		getsockname(rtspServerCtx.RTSPClientSession.RtspDataSocketId, 
				    (struct sockaddr*)&ourAddress, 
				    &namelen
			       );
		ipaddr = ntohl(ourAddress.sin_addr.s_addr);
		
	  	switch (streamingMode) 
		{
			case GSN_RTSP_MODE_RTP_UDP: 
			{
				snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
				sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %s\r\n"
				"%s"
				"Transport: RTP/AVP;unicast;"
				"source=%d.%d.%d.%d;"
				"client_port=%d-%d;"
				"server_port=%d-%d\r\n"
				"Session: %08X\r\n\r\n",
				cseq,
				GsnRtsp_DateHeader(),
				((ipaddr >> 24) & 0xff),((ipaddr >> 16) & 0xff),((ipaddr >> 8) & 0xff),(ipaddr & 0xff), 
				clientRTPPort, clientRTCPPort, 
				serverRTPPort, serverRTCPPort,
				rtspServerCtx.RTSPClientSession.RtspSessionId);
				rtspServerCtx.RTSPClientSession.RtspSessionActive  = TRUE;
				break;
			}
			case GSN_RTSP_MODE_RTP_TCP: 
			{
//				snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
//			     sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
//			     "RTSP/1.0 461 Unsupported Transport\r\nCSeq: %s\r\n%s\r\n",
//			     cseq, 
//			     GsnRtsp_DateHeader()
//			    );
                          snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
				sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %s\r\n"
				"%s"
				"Transport: RTP/AVP/TCP;interleaved=0-1\r\n"
				"Session: %08X\r\n\r\n",
				cseq,
				GsnRtsp_DateHeader(),
				rtspServerCtx.RTSPClientSession.RtspSessionId);
				rtspServerCtx.RTSPClientSession.RtspSessionActive  = TRUE;
				break;
#if 0
                          snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
				sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %s\r\n"
				"%s"
				"Transport: RTP/AVP;unicast;"
				"source=%d.%d.%d.%d;"
				"client_port=%d-%d;"
				"server_port=%d-%d\r\n"
				"Session: %08X\r\n\r\n",
				cseq,
				GsnRtsp_DateHeader(),
				((ipaddr >> 24) & 0xff),((ipaddr >> 16) & 0xff),((ipaddr >> 8) & 0xff),(ipaddr & 0xff), 
				clientRTPPort, clientRTCPPort, 
				serverRTPPort, serverRTCPPort,
				rtspServerCtx.RTSPClientSession.RtspSessionId);
				rtspServerCtx.RTSPClientSession.RtspSessionActive  = TRUE;
				break;
#endif
			}
			case GSN_RTSP_MODE_RAW_UDP: 
			{
				snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
			     sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
			     "RTSP/1.0 461 Unsupported Transport\r\nCSeq: %s\r\n%s\r\n",
			     cseq, 
			     GsnRtsp_DateHeader()
			    );
				break;
			}
		}
		
	} while (0);

}


PRIVATE VOID
GsnRtsp_ProcessCommand(INT8 const* cmdName,
			  INT8 const* urlPreSuffix, INT8 const* urlSuffix,
			  INT8 const* cseq, INT8 const* fullRequestStr) 
{
//        int ret;
	printf_rtspdebug("\r\nRTSP: GsnRtsp_ProcessCommand");

	if (TRUE == rtspServerCtx.RTSPClientSession.RtspSessionActive)
	{
		if (strcmp(cmdName, "TEARDOWN") == 0) 
		{
                      printf_rtspdebug("\r\nRTSP: TEARDOWN1 Received\r\n");
                      GsnRtsp_Teardown( cseq);
//                      soc_close(rtsp_sd);
//                      rtsp_sd=-1;
                      AppVideo_RTSP_Comm_ThreadStop();                         
		} else if (strcmp(cmdName, "PLAY") == 0) 
		{
                  GsnRtsp_Play( cseq, fullRequestStr);
		}
	}
	else
	{
		printf_rtspdebug_fail("\r\nRTSP: Session is inactive");
		snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
	         sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 400 Bad Request\r\n%sAllow: %s\r\n\r\n",
		     GsnRtsp_DateHeader(), 
		     allowedCommandNames
		    );
	}

}


extern UINT32	T0us,T500us, T1ms, T3ms, T5ms, T10ms, T20ms, T50ms, T100ms, T100Plus, T1sendFailed, T1SendPassed;

PRIVATE VOID
GsnRtsp_Teardown( INT8 const* cseq) 
{

    printf_rtspdebug_imp("\r\nRTSP: TEARDOWN2 Received");

	printf_rtspdebug_imp("\r\n\r\n*************Video Statistics******************\r\n");

	printf_rtspdebug_imp("\r\n\t Data sent in zero       us      = %d", T0us);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 500us    = %d", T500us);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 1ms      = %d", T1ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 3ms      = %d", T3ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 5ms      = %d", T5ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 10ms     = %d", T10ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 20ms     = %d", T20ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 50ms     = %d", T50ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 100ms    = %d", T100ms);
	printf_rtspdebug_imp("\r\n\t Data sent in more than 100 Plus ms = %d", T100Plus);

	printf_rtspdebug_imp("\r\n\t Data Send Failed = %d", T1sendFailed);
	printf_rtspdebug_imp("\r\n\t Data Send Passed = %d", T1SendPassed);

	printf_rtspdebug_imp("\r\n\r\n*************Video Statistics******************\r\n\r\n");
	
	snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
		     sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		     "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n",
		     cseq, 
		     GsnRtsp_DateHeader()
		    );
}

PRIVATE VOID
GsnRtsp_Play(INT8 const* cseq, INT8 const* fullRequestStr) 
{

    INT8* prtspURL;
	UINT32 rtpSeqNum = 0;
//    int ret;
    printf_rtspdebug_imp("\r\nRTSP: PLAY Received");
//    AppDbg_Printf("\r\nactivating timer\r\n");    
//    while((ret=tx_timer_activate(&rtsp_timer))){
//      if(ret==0){
//        AppDbg_Printf("\r\n timer activated \r\n");
//        break;
//      } else if(ret==0x17){
//        AppDbg_Printf("\r\n timer already activated \r\n");
//        break;
//      }
//    }    
//    printf_rtspdebug_imp("\r\n return from timer activate ret:%d\r\n",ret);
	printf_rtspdebug_imp("\r\n\r\n*************Video Statistics******************\r\n");

	printf_rtspdebug_imp("\r\n\t Data sent in zero       us      = %d", T0us);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 500us    = %d", T500us);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 1ms      = %d", T1ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 3ms      = %d", T3ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 5ms      = %d", T5ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 10ms     = %d", T10ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 20ms     = %d", T20ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 50ms     = %d", T50ms);
	printf_rtspdebug_imp("\r\n\t Data sent in Less than 100ms    = %d", T100ms);
	printf_rtspdebug_imp("\r\n\t Data sent in more than 100 Plus ms = %d", T100Plus);

	printf_rtspdebug_imp("\r\n\t Data Send Failed = %d", T1sendFailed);
	printf_rtspdebug_imp("\r\n\t Data Send Passed = %d", T1SendPassed);

	printf_rtspdebug_imp("\r\n\r\n*************Video Statistics******************\r\n\r\n");

    rtpSeqNum = GsnRtp_GetSeqNum();

		
    prtspURL = GsnRtsp_URL(rtspServerCtx.RTSPClientSession.RtspDataSocketId);

	snprintf((INT8*)rtspServerCtx.RTSPClientSession.RtspRespBuff, 
		 sizeof (rtspServerCtx.RTSPClientSession.RtspRespBuff),
		 "RTSP/1.0 200 OK\r\n"
		 "%s"
		 "RTP-Info: "
		 "url=%s"
		 ";seq=%d"
		 ";rtptime=%u\r\n"
		 "Session: %08X\r\n"
		 "CSeq: %s\r\n"
		 "\r\n\0",
		 GsnRtsp_DateHeader(),
		 prtspURL,
		 rtpSeqNum,
		 0,
		 rtspServerCtx.RTSPClientSession.RtspSessionId,
		 cseq
		 );
}


