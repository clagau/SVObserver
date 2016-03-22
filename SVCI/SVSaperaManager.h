// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSaperaManager
// * .File Name       : $Workfile:   SVSaperaManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:58:56  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <corapi.h>

#include "SVUtilityLibrary/SVString.h"

// Uncomment the define below to directly use the library interface.
//#define USE_CORAPI_LIB
#ifdef USE_CORAPI_LIB
	#pragma comment( lib, "corapi" )
	#pragma comment( lib, "corppapi" )
#endif

class SVSaperaManager
{
public:
	static SVSaperaManager& Instance();

	virtual ~SVSaperaManager();

	void Startup();
	void Shutdown();

	HRESULT GetErrorNumber( CORSTATUS p_hErrorCode, const SVString& p_rContext );

	HRESULT AcqGetCap( CORACQ hAcq, UINT32 cap, void *value );
	HRESULT AcqGetCount( CORSERVER hServer, PUINT32 count );
	HRESULT AcqGetHandle( CORSERVER hServer, UINT32 index, CORACQ *hAcq );
	HRESULT AcqGetPrm( CORACQ hAcq, UINT32 prm, void *value );
	HRESULT AcqSetLut( CORACQ hAcq, CORLUT hLut, UINT32 lutNumber );
	HRESULT AcqSetPrm( CORACQ hAcq, UINT32 prm, UINT32 value );
	HRESULT AcqSetPrms( CORACQ hAcq, CORVIC hVic, CORCAM hCam, UINT32 toUnlock );
	HRESULT AcqRelease( CORACQ hAcq );
	HRESULT AcqReset( CORACQ hAcq );
	HRESULT AcqRegisterCallback( CORACQ hAcq, UINT32 eventType, PCORCALLBACK callback, void *context );
	HRESULT AcqUnregisterCallback( CORACQ hAcq, PCORCALLBACK callback );
	HRESULT AcqSoftwareTrigger( CORACQ hAcq, UINT32 triggerType );
	HRESULT BufferFree( CORBUFFER buffer );
	HRESULT BufferGetPrm( CORBUFFER buffer, UINT32 prm, void *value );
	HRESULT BufferNew( CORSERVER server, UINT32 memWidth, UINT32 memHeight, UINT32 format, UINT32 type, CORBUFFER *buffer );
	HRESULT BufferSetPrm( CORBUFFER buffer, UINT32 prm, UINT32 value );
	HRESULT CamFree( CORCAM hCam );
	HRESULT CamLoad( CORCAM hCam, PCSTR filename );
	HRESULT CamGetPrm( CORCAM hCam, UINT32 param, void *value ); 
	HRESULT CamNew( CORSERVER hServer, CORCAM *pCam );
	HRESULT VicFree( CORVIC hVic );
	HRESULT VicGetPrm( CORVIC hVic, UINT32 param, void *value ); 
	HRESULT VicLoad( CORVIC hVic, PCSTR filename );
	HRESULT VicNew( CORSERVER hServer, CORVIC *pVic );
	HRESULT VicSetPrm( CORVIC hVic, UINT32 param, UINT32 value ); 
	HRESULT LutClip( CORLUT hLut,  INT32 imin,  INT32 imax, CORDATA omin, CORDATA omax );
	HRESULT LutFree( CORLUT hLut );
	HRESULT LutGetPrm( CORLUT hLut, UINT32 prm, void *value );
	HRESULT LutNew( CORSERVER hServer, UINT32 nEntries, UINT32 format, CORLUT *hLut );
	HRESULT LutNormal( CORLUT hLut );
	HRESULT LutRead( CORLUT hLut, UINT32 offset, void *array, UINT32 size );
	HRESULT LutWrite( CORLUT hLut, UINT32 offset, const void *array, UINT32 size );
	HRESULT ManGetLocalServer( CORSERVER *p_phServer );
	HRESULT ManGetServerCount( PUINT32 nServer );
	HRESULT ManGetServerByIndex( UINT32 index, PSTR name, PCORSERVER server );
	HRESULT ManResetServer( CORSERVER hServer );
	HRESULT ManReleaseServer( CORSERVER hServer );
	HRESULT XferAbort( CORXFER hXfer );
	HRESULT XferAppend( CORXFER hXfer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc );
	HRESULT XferConnect( CORXFER hXfer );
	HRESULT XferDisconnect( CORXFER hXfer );
	HRESULT XferReset( CORXFER hXfer );
	HRESULT XferFree( CORXFER hXfer );
	HRESULT XferNew( CORSERVER hServer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc, CORXFER *hXfer );
	HRESULT XferRegisterCallback( CORXFER hXfer, UINT32 eventType, PCORCALLBACK callback, void *context );
	HRESULT XferGetPrm( CORXFER hXfer, UINT32 prm, UINT32* value );
	HRESULT XferSetPrm( CORXFER hXfer, UINT32 prm, UINT32 value );
	HRESULT XferStart( CORXFER hXfer, UINT32 count );
	HRESULT XferStop( CORXFER hXfer );
	HRESULT XferUnregisterCallback( CORXFER hXfer, PCORCALLBACK callback );
	HRESULT XferWait( CORXFER hXfer, UINT32 timeout );

protected:
	SVSaperaManager();

	#ifndef USE_CORAPI_LIB
		void LoadDLL();
		void FreeDLL();

		//CORSTATUS CORAPIFUNC CorAcqGetCap( CORACQ hAcq, UINT32 cap, void *value );
		typedef CORSTATUS (__stdcall* SVCorAcqGetCapPtr)( CORACQ, UINT32, void* );
		//CORSTATUS CORAPIFUNC CorAcqGetCount(CORSERVER hServer, PUINT32 count);
		typedef CORSTATUS (__stdcall* SVCorAcqGetCountPtr)( CORSERVER, PUINT32 );
		//CORSTATUS CORAPIFUNC CorAcqGetHandle(CORSERVER hServer, UINT32 index, CORACQ *hAcq);
		typedef CORSTATUS (__stdcall* SVCorAcqGetHandlePtr)( CORSERVER, UINT32, CORACQ* );
		//CORSTATUS CORAPIFUNC CorAcqGetPrm( CORACQ hAcq, UINT32 prm, void *value );
		typedef CORSTATUS (__stdcall* SVCorAcqGetPrmPtr)( CORACQ, UINT32, void* );
		//CORSTATUS CORAPIFUNC CorAcqSetLut( CORACQ hAcq, CORLUT hLut, UINT32 lutNumber );
		typedef CORSTATUS (__stdcall* SVCorAcqSetLutPtr)( CORACQ, CORLUT, UINT32 );
		//CORSTATUS CORAPIFUNC CorAcqSetPrm( CORACQ hAcq, UINT32 prm, UINT32 value );
		typedef CORSTATUS (__stdcall* SVCorAcqSetPrmPtr)( CORACQ, UINT32, UINT32 );
		//CORSTATUS CORAPIFUNC CorAcqSetPrms( CORACQ hAcq, CORVIC hVic, CORCAM hCam, UINT32 toUnlock );
		typedef CORSTATUS (__stdcall* SVCorAcqSetPrmsPtr)( CORACQ, CORVIC, CORCAM, UINT32 );
		//CORSTATUS CORAPIFUNC CorAcqRelease(CORACQ hAcq);
		typedef CORSTATUS (__stdcall* SVCorAcqReleasePtr)( CORACQ );
		//CORSTATUS CORAPIFUNC CorAcqReset(CORACQ hAcq);
		typedef CORSTATUS (__stdcall* SVCorAcqResetPtr)( CORACQ );
		//CORSTATUS CORAPIFUNC CorAcqRegisterCallback( CORACQ hAcq, UINT32 eventType, PCORCALLBACK callback, void *context);
		typedef CORSTATUS (__stdcall* SVCorAcqRegisterCallbackPtr)( CORACQ, UINT32, PCORCALLBACK, void* );
		//CORSTATUS CORAPIFUNC CorAcqUnregisterCallback( CORACQ hAcq, PCORCALLBACK callback);
		typedef CORSTATUS (__stdcall* SVCorAcqUnregisterCallbackPtr)( CORACQ, PCORCALLBACK );
		//CORSTATUS CORAPIFUNC CorAcqSoftwareTrigger( CORACQ hAcq, UINT32 triggerType);
		typedef CORSTATUS (__stdcall* SVCorAcqSoftwareTriggerPtr)( CORACQ, UINT32 );
		//CORSTATUS CORAPIFUNC CorBufferFree( CORBUFFER buffer);
		typedef CORSTATUS (__stdcall* SVCorBufferFreePtr)( CORBUFFER );
		//CORSTATUS CORAPIFUNC CorBufferGetPrm( CORBUFFER buffer, UINT32 prm, void *value);
		typedef CORSTATUS (__stdcall* SVCorBufferGetPrmPtr)( CORBUFFER, UINT32, void* );
		//CORSTATUS CORAPIFUNC CorBufferNew( CORSERVER server, UINT32 memWidth, UINT32 memHeight, UINT32 format, UINT32 type, CORBUFFER *buffer);
		typedef CORSTATUS (__stdcall* SVCorBufferNewPtr)( CORSERVER, UINT32, UINT32, UINT32, UINT32, CORBUFFER* );
		//CORSTATUS CORAPIFUNC CorBufferSetPrm( CORBUFFER buffer, UINT32 prm, UINT32 value);
		typedef CORSTATUS (__stdcall* SVCorBufferSetPrmPtr)( CORBUFFER, UINT32, UINT32 );
		//CORSTATUS CORAPIFUNC CorCamFree( CORCAM hCam);
		typedef CORSTATUS (__stdcall* SVCorCamFreePtr)( CORCAM );
		//CORSTATUS CORAPIFUNC CorCamLoad( CORCAM hCam, PCSTR filename);
		typedef CORSTATUS (__stdcall* SVCorCamLoadPtr)( CORCAM, PCSTR );
		//CORSTATUS CORAPIFUNC CorCamGetPrm( CORCAM hCam, UINT32 param, void *value); 
		typedef CORSTATUS (__stdcall* SVCorCamGetPrmPtr)( CORCAM, UINT32, void* ); 
		//CORSTATUS CORAPIFUNC CorCamNew( CORSERVER hServer, CORCAM *pCam);
		typedef CORSTATUS (__stdcall* SVCorCamNewPtr)( CORSERVER, CORCAM* );
		//CORSTATUS CORAPIFUNC CorVicFree( CORVIC hVic);
		typedef CORSTATUS (__stdcall* SVCorVicFreePtr)( CORVIC );
		//CORSTATUS CORAPIFUNC CorVicGetPrm( CORVIC hVic, UINT32 param, void *value); 
		typedef CORSTATUS (__stdcall* SVCorVicGetPrmPtr)( CORVIC, UINT32, void* ); 
		//CORSTATUS CORAPIFUNC CorVicLoad( CORVIC hVic, PCSTR filename);
		typedef CORSTATUS (__stdcall* SVCorVicLoadPtr)( CORVIC, PCSTR );
		//CORSTATUS CORAPIFUNC CorVicNew( CORSERVER hServer, CORVIC *pVic);
		typedef CORSTATUS (__stdcall* SVCorVicNewPtr)( CORSERVER, CORVIC* );
		//CORSTATUS CORAPIFUNC CorVicSetPrm( CORVIC hVic, UINT32 param, UINT32 value); 
		typedef CORSTATUS (__stdcall* SVCorVicSetPrmPtr)( CORVIC, UINT32, UINT32 ); 
		//CORSTATUS CORAPIFUNC CorLutClip( CORLUT hLut,  INT32 imin,  INT32 imax, CORDATA omin, CORDATA omax);
		typedef CORSTATUS (__stdcall* SVCorLutClipPtr)( CORLUT, INT32, INT32, CORDATA, CORDATA );
		//CORSTATUS CORAPIFUNC CorLutFree( CORLUT hLut);
		typedef CORSTATUS (__stdcall* SVCorLutFreePtr)( CORLUT );
		//CORSTATUS CORAPIFUNC CorLutGetPrm( CORLUT hLut, UINT32 prm, void *value);
		typedef CORSTATUS (__stdcall* SVCorLutGetPrmPtr)( CORLUT, UINT32, void* );
		//CORSTATUS CORAPIFUNC CorLutNew( CORSERVER hServer, UINT32 nEntries, UINT32 format, CORLUT *hLut);
		typedef CORSTATUS (__stdcall* SVCorLutNewPtr)( CORSERVER, UINT32, UINT32, CORLUT* );
		//CORSTATUS CORAPIFUNC CorLutNormal( CORLUT hLut);
		typedef CORSTATUS (__stdcall* SVCorLutNormalPtr)( CORLUT );
		//CORSTATUS CORAPIFUNC CorLutRead( CORLUT hLut, UINT32 offset, void *array, UINT32 size);
		typedef CORSTATUS (__stdcall* SVCorLutReadPtr)( CORLUT, UINT32, void*, UINT32 );
		//CORSTATUS CORAPIFUNC CorLutWrite( CORLUT hLut, UINT32 offset, const void *array, UINT32 size);
		typedef CORSTATUS (__stdcall* SVCorLutWritePtr)( CORLUT, UINT32, const void*, UINT32 );
		//CORSERVER CORAPIFUNC CorManGetLocalServer(void);
		typedef CORSERVER (__stdcall* SVCorManGetLocalServerPtr)( void );
		//CORSTATUS CORAPIFUNC CorManGetServerCount( PUINT32 nServer);
		typedef CORSTATUS (__stdcall* SVCorManGetServerCountPtr)( PUINT32 );
		//CORSTATUS CORAPIFUNC CorManGetServerByIndex( UINT32 index, PSTR name, PCORSERVER server);
		typedef CORSTATUS (__stdcall* SVCorManGetServerByIndexPtr)( UINT32, PSTR, PCORSERVER );
		//CORSTATUS CORAPIFUNC CorManResetServer( CORSERVER hServer);
		typedef CORSTATUS (__stdcall* SVCorManResetServerPtr)( CORSERVER );
		//CORSTATUS CORAPIFUNC CorManReleaseServer( CORSERVER hServer);
		typedef CORSTATUS (__stdcall* SVCorManReleaseServerPtr)( CORSERVER );
		//CORSTATUS CORAPIFUNC CorXferAbort(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferAbortPtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferAppend(CORXFER hXfer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc);
		typedef CORSTATUS (__stdcall* SVCorXferAppendPtr)( CORXFER, CORHANDLE, CORHANDLE, PCORXFER_DESC );
		//CORSTATUS CORAPIFUNC CorXferConnect(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferConnectPtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferDisconnect(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferDisconnectPtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferReset(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferResetPtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferFree(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferFreePtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferNew(CORSERVER hServer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc, CORXFER *hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferNewPtr)( CORSERVER, CORHANDLE, CORHANDLE, PCORXFER_DESC, CORXFER* );
		//CORSTATUS CORAPIFUNC CorXferRegisterCallback( CORXFER hXfer, UINT32 eventType, PCORCALLBACK callback, void *context);
		typedef CORSTATUS (__stdcall* SVCorXferRegisterCallbackPtr)( CORXFER, UINT32, PCORCALLBACK, void* );
		//CORSTATUS CORAPIFUNC CorXferGetPrm(CORXFER hXfer, UINT32 prm, void *value);
		typedef CORSTATUS (__stdcall* SVCorXferGetPrmPtr)( CORXFER, UINT32, void* );
		//CORSTATUS CORAPIFUNC CorXferSetPrm(CORXFER hXfer, UINT32 prm, UINT32 value);
		typedef CORSTATUS (__stdcall* SVCorXferSetPrmPtr)( CORXFER, UINT32, UINT32 );
		//CORSTATUS CORAPIFUNC CorXferStart(CORXFER hXfer, UINT32 count);
		typedef CORSTATUS (__stdcall* SVCorXferStartPtr)( CORXFER, UINT32 );
		//CORSTATUS CORAPIFUNC CorXferStop(CORXFER hXfer);
		typedef CORSTATUS (__stdcall* SVCorXferStopPtr)( CORXFER );
		//CORSTATUS CORAPIFUNC CorXferUnregisterCallback( CORXFER hXfer, PCORCALLBACK callback);
		typedef CORSTATUS (__stdcall* SVCorXferUnregisterCallbackPtr)( CORXFER, PCORCALLBACK );
		//CORSTATUS CORAPIFUNC CorXferWait(CORXFER hXfer, UINT32 timeout);
		typedef CORSTATUS (__stdcall* SVCorXferWaitPtr)( CORXFER, UINT32 );

		HMODULE m_LibHandle;

		SVCorAcqGetCapPtr m_pAcqGetCap;
		SVCorAcqGetCountPtr m_pAcqGetCount;
		SVCorAcqGetHandlePtr m_pAcqGetHandle;
		SVCorAcqGetPrmPtr m_pAcqGetPrm;
		SVCorAcqSetLutPtr m_pAcqSetLut;
		SVCorAcqSetPrmPtr m_pAcqSetPrm;
		SVCorAcqSetPrmsPtr m_pAcqSetPrms;
		SVCorAcqReleasePtr m_pAcqRelease;
		SVCorAcqResetPtr m_pAcqReset;
		SVCorAcqRegisterCallbackPtr m_pAcqRegisterCallback;
		SVCorAcqUnregisterCallbackPtr m_pAcqUnregisterCallback;
		SVCorAcqSoftwareTriggerPtr m_pAcqSoftwareTrigger;
		SVCorBufferFreePtr m_pBufferFree;
		SVCorBufferGetPrmPtr m_pBufferGetPrm;
		SVCorBufferNewPtr m_pBufferNew;
		SVCorBufferSetPrmPtr m_pBufferSetPrm;
		SVCorCamFreePtr m_pCamFree;
		SVCorCamLoadPtr m_pCamLoad;
		SVCorCamGetPrmPtr m_pCamGetPrm;
		SVCorCamNewPtr m_pCamNew;
		SVCorVicFreePtr m_pVicFree;
		SVCorVicGetPrmPtr m_pVicGetPrm;
		SVCorVicLoadPtr m_pVicLoad;
		SVCorVicNewPtr m_pVicNew;
		SVCorVicSetPrmPtr m_pVicSetPrm;
		SVCorLutClipPtr m_pLutClip;
		SVCorLutFreePtr m_pLutFree;
		SVCorLutGetPrmPtr m_pLutGetPrm;
		SVCorLutNewPtr m_pLutNew;
		SVCorLutNormalPtr m_pLutNormal;
		SVCorLutReadPtr m_pLutRead;
		SVCorLutWritePtr m_pLutWrite;
		SVCorManGetLocalServerPtr m_pManGetLocalServer;
		SVCorManGetServerCountPtr m_pManGetServerCount;
		SVCorManGetServerByIndexPtr m_pManGetServerByIndex;
		SVCorManResetServerPtr m_pManResetServer;
		SVCorManReleaseServerPtr m_pManReleaseServer;
		SVCorXferAbortPtr m_pXferAbort;
		SVCorXferAppendPtr m_pXferAppend;
		SVCorXferConnectPtr m_pXferConnect;
		SVCorXferDisconnectPtr m_pXferDisconnect;
		SVCorXferResetPtr m_pXferReset;
		SVCorXferFreePtr m_pXferFree;
		SVCorXferNewPtr m_pXferNew;
		SVCorXferRegisterCallbackPtr m_pXferRegisterCallback;
		SVCorXferGetPrmPtr m_pXferGetPrm;
		SVCorXferSetPrmPtr m_pXferSetPrm;
		SVCorXferStartPtr m_pXferStart;
		SVCorXferStopPtr m_pXferStop;
		SVCorXferUnregisterCallbackPtr m_pXferUnregisterCallback;
		SVCorXferWaitPtr m_pXferWait;
	#endif

private:
	SVSaperaManager( const SVSaperaManager& p_rObject );
	const SVSaperaManager& operator=( const SVSaperaManager& p_rObject );

};

