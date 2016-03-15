// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSaperaManager
// * .File Name       : $Workfile:   SVSaperaManager.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:08  $
// ******************************************************************************

#include "stdafx.h"
#include "SVSaperaManager.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"

SVSaperaManager& SVSaperaManager::Instance()
{
	static SVSaperaManager l_Object;

	return l_Object;
}

SVSaperaManager::SVSaperaManager()
#ifndef USE_CORAPI_LIB
	: m_LibHandle( NULL )
	, m_pAcqGetCap( NULL )
	, m_pAcqGetCount( NULL )
	, m_pAcqGetHandle( NULL )
	, m_pAcqGetPrm( NULL )
	, m_pAcqSetLut( NULL )
	, m_pAcqSetPrm( NULL )
	, m_pAcqSetPrms( NULL )
	, m_pAcqRelease( NULL )
	, m_pAcqReset( NULL )
	, m_pAcqRegisterCallback( NULL )
	, m_pAcqUnregisterCallback( NULL )
	, m_pAcqSoftwareTrigger( NULL )
	, m_pBufferFree( NULL )
	, m_pBufferGetPrm( NULL )
	, m_pBufferNew( NULL )
	, m_pBufferSetPrm( NULL )
	, m_pCamFree( NULL )
	, m_pCamLoad( NULL )
	, m_pCamGetPrm( NULL )
	, m_pCamNew( NULL )
	, m_pVicFree( NULL )
	, m_pVicGetPrm( NULL )
	, m_pVicLoad( NULL )
	, m_pVicNew( NULL )
	, m_pVicSetPrm( NULL )
	, m_pLutClip( NULL )
	, m_pLutFree( NULL )
	, m_pLutGetPrm( NULL )
	, m_pLutNew( NULL )
	, m_pLutNormal( NULL )
	, m_pLutRead( NULL )
	, m_pLutWrite( NULL )
	, m_pManGetLocalServer( NULL )
	, m_pManGetServerCount( NULL )
	, m_pManGetServerByIndex( NULL )
	, m_pManResetServer( NULL )
	, m_pManReleaseServer( NULL )
	, m_pXferAbort( NULL )
	, m_pXferAppend( NULL )
	, m_pXferConnect( NULL )
	, m_pXferDisconnect( NULL )
	, m_pXferReset( NULL )
	, m_pXferFree( NULL )
	, m_pXferNew( NULL )
	, m_pXferRegisterCallback( NULL )
	, m_pXferGetPrm( NULL )
	, m_pXferSetPrm( NULL )
	, m_pXferStart( NULL )
	, m_pXferStop( NULL )
	, m_pXferUnregisterCallback( NULL )
	, m_pXferWait( NULL )
#endif
{
	#ifndef USE_CORAPI_LIB
		LoadDLL();
	#endif
}

SVSaperaManager::~SVSaperaManager()
{
	#ifndef USE_CORAPI_LIB
		FreeDLL();
	#endif
}

void SVSaperaManager::Startup()
{
	#ifndef USE_CORAPI_LIB
		LoadDLL();
	#endif
}

void SVSaperaManager::Shutdown()
{
	#ifndef USE_CORAPI_LIB
		FreeDLL();
	#endif
}

HRESULT SVSaperaManager::GetErrorNumber( CORSTATUS p_hErrorCode, const SVString& p_rContext )
{
	HRESULT l_hrOk = S_OK;

	if ( p_hErrorCode != CORSTATUS_OK )
	{
		unsigned int l_uiModule = CORSTATUS_MODULE( p_hErrorCode );
		unsigned int l_uiLevel = CORSTATUS_LEVEL( p_hErrorCode );
		unsigned int l_uiInfo = CORSTATUS_INFO( p_hErrorCode );
		unsigned int l_uiStatus = CORSTATUS_ID( p_hErrorCode );

		CString l_Message;
		l_Message.Format( _T( "%s-CorStatus=0x%x-Module=0x%x-Level=0x%x-Info=0x%x-Status=0x%x" ),
			p_rContext.c_str(), p_hErrorCode, l_uiModule, l_uiLevel, l_uiInfo, l_uiStatus );

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVCI_UNKNOWN_INFORMATIONAL, l_Message, StdMessageParams );

		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVSaperaManager::AcqGetCap( CORACQ hAcq, UINT32 cap, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqGetCap( hAcq, cap, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetCap" ) );
	#else
		if( m_pAcqGetCap != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqGetCap( hAcq, cap, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetCap" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqGetCount( CORSERVER hServer, PUINT32 count )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqGetCount( hServer, count );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetCount" ) );
	#else
		if( m_pAcqGetCount != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqGetCount( hServer, count );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetCount" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqGetHandle( CORSERVER hServer, UINT32 index, CORACQ *hAcq )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqGetHandle( hServer, index, hAcq );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetHandle" ) );
	#else
		if( m_pAcqGetHandle != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqGetHandle( hServer, index, hAcq );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetHandle" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqGetPrm( CORACQ hAcq, UINT32 prm, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqGetPrm( hAcq, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetPrm" ) );
	#else
		if( m_pAcqGetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqGetPrm( hAcq, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqSetLut( CORACQ hAcq, CORLUT hLut, UINT32 lutNumber )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqSetLut( hAcq, hLut, lutNumber );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetLut" ) );
	#else
		if( m_pAcqSetLut != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqSetLut( hAcq, hLut, lutNumber );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetLut" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqSetPrm( CORACQ hAcq, UINT32 prm, UINT32 value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqSetPrm( hAcq, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetPrm" ) );
	#else
		if( m_pAcqSetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqSetPrm( hAcq, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqSetPrms( CORACQ hAcq, CORVIC hVic, CORCAM hCam, UINT32 toUnlock )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqSetPrms( hAcq, hVic, hCam, toUnlock );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetPrms" ) );
	#else
		if( m_pAcqSetPrms != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqSetPrms( hAcq, hVic, hCam, toUnlock );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSetPrms" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqRelease( CORACQ hAcq )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqRelease( hAcq );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqRelease" ) );
	#else
		if( m_pAcqRelease != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqRelease( hAcq );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqRelease" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqReset( CORACQ hAcq )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqReset( hAcq );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqReset" ) );
	#else
		if( m_pAcqReset != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqReset( hAcq );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqReset" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqRegisterCallback( CORACQ hAcq, UINT32 eventType, PCORCALLBACK callback, void *context )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqRegisterCallback( hAcq, eventType, callback, context );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqRegisterCallback" ) );
	#else
		if( m_pAcqRegisterCallback != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqRegisterCallback( hAcq, eventType, callback, context );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqRegisterCallback" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqUnregisterCallback( CORACQ hAcq, PCORCALLBACK callback )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqUnregisterCallback( hAcq, callback );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqUnregisterCallback" ) );
	#else
		if( m_pAcqUnregisterCallback != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqUnregisterCallback( hAcq, callback );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqUnregisterCallback" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::AcqSoftwareTrigger( CORACQ hAcq, UINT32 triggerType )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorAcqSoftwareTrigger( hAcq, triggerType );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSoftwareTrigger" ) );
	#else
		if( m_pAcqSoftwareTrigger != NULL )
		{
			CORSTATUS l_CorStatus = m_pAcqSoftwareTrigger( hAcq, triggerType );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorAcqSoftwareTrigger" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::BufferFree( CORBUFFER buffer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorBufferFree( buffer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferFree" ) );
	#else
		if( m_pBufferFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pBufferFree( buffer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferFree" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::BufferGetPrm( CORBUFFER buffer, UINT32 prm, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorBufferGetPrm( buffer, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferGetPrm" ) );
	#else
		if( m_pBufferFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pBufferGetPrm( buffer, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::BufferNew( CORSERVER server, UINT32 memWidth, UINT32 memHeight, UINT32 format, UINT32 type, CORBUFFER *buffer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorBufferNew( server, memWidth, memHeight, format, type, buffer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferNew" ) );
	#else
		if( m_pBufferNew != NULL )
		{
			CORSTATUS l_CorStatus = m_pBufferNew( server, memWidth, memHeight, format, type, buffer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferNew" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::BufferSetPrm( CORBUFFER buffer, UINT32 prm, UINT32 value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorBufferSetPrm( buffer, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferSetPrm" ) );
	#else
		if( m_pBufferSetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pBufferSetPrm( buffer, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorBufferSetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::CamFree( CORCAM hCam )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorCamFree( hCam );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamFree" ) );
	#else
		if( m_pCamFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pCamFree( hCam );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamFree" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::CamLoad( CORCAM hCam, PCSTR filename )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorCamLoad( hCam, filename );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamLoad" ) );
	#else
		if( m_pCamLoad != NULL )
		{
			CORSTATUS l_CorStatus = m_pCamLoad( hCam, filename );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamLoad" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::CamGetPrm( CORCAM hCam, UINT32 param, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorCamGetPrm( hCam, param, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamGetPrm" ) );
	#else
		if( m_pCamGetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pCamGetPrm( hCam, param, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::CamNew( CORSERVER hServer, CORCAM *pCam )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorCamNew( hServer, pCam );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamNew" ) );
	#else
		if( m_pCamNew != NULL )
		{
			CORSTATUS l_CorStatus = m_pCamNew( hServer, pCam );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorCamNew" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::VicFree( CORVIC hVic )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorVicFree( hVic );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicFree" ) );
	#else
		if( m_pVicFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pVicFree( hVic );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicFree" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::VicGetPrm( CORVIC hVic, UINT32 param, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorVicGetPrm( hVic, param, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicGetPrm" ) );
	#else
		if( m_pVicGetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pVicGetPrm( hVic, param, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::VicLoad( CORVIC hVic, PCSTR filename )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorVicLoad( hVic, filename );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicLoad" ) );
	#else
		if( m_pVicLoad != NULL )
		{
			CORSTATUS l_CorStatus = m_pVicLoad( hVic, filename );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicLoad" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::VicNew( CORSERVER hServer, CORVIC *pVic )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorVicNew( hServer, pVic );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicNew" ) );
	#else
		if( m_pVicNew != NULL )
		{
			CORSTATUS l_CorStatus = m_pVicNew( hServer, pVic );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicNew" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::VicSetPrm( CORVIC hVic, UINT32 param, UINT32 value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorVicSetPrm( hVic, param, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicSetPrm" ) );
	#else
		if( m_pVicSetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pVicSetPrm( hVic, param, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorVicSetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutClip( CORLUT hLut, INT32 imin, INT32 imax, CORDATA omin, CORDATA omax )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutClip( hLut, imin, imax, omin, omax );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutClip" ) );
	#else
		if( m_pLutClip != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutClip( hLut, imin, imax, omin, omax );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutClip" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutFree( CORLUT hLut )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutFree( hLut );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutFree" ) );
	#else
		if( m_pLutFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutFree( hLut );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutFree" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutGetPrm( CORLUT hLut, UINT32 prm, void *value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutGetPrm( hLut, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutGetPrm" ) );
	#else
		if( m_pLutGetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutGetPrm( hLut, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutNew( CORSERVER hServer, UINT32 nEntries, UINT32 format, CORLUT *hLut )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutNew( hServer, nEntries, format, hLut );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutNew" ) );
	#else
		if( m_pLutNew != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutNew( hServer, nEntries, format, hLut );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutNew" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutNormal( CORLUT hLut )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutNormal( hLut );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutNormal" ) );
	#else
		if( m_pLutNormal != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutNormal( hLut );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutNormal" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutRead( CORLUT hLut, UINT32 offset, void *array, UINT32 size )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutRead( hLut, offset, array, size );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutRead" ) );
	#else
		if( m_pLutRead != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutRead( hLut, offset, array, size );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutRead" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::LutWrite( CORLUT hLut, UINT32 offset, const void *array, UINT32 size )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorLutWrite( hLut, offset, array, size );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutWrite" ) );
	#else
		if( m_pLutWrite != NULL )
		{
			CORSTATUS l_CorStatus = m_pLutWrite( hLut, offset, array, size );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorLutWrite" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::ManGetLocalServer( CORSERVER *p_phServer )
{
	HRESULT l_Status = S_OK;

	if( p_phServer != NULL )
	{
		#ifdef USE_CORAPI_LIB
			*p_phServer = CorManGetLocalServer();

			if( CORHANDLE_IS_NULL( *p_phServer ) )
			{
				l_Status = E_FAIL;
			}
		#else
			if( m_pManGetLocalServer != NULL )
			{
				*p_phServer = m_pManGetLocalServer();

				if( CORHANDLE_IS_NULL( *p_phServer ) )
				{
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		#endif
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSaperaManager::ManGetServerCount( PUINT32 nServer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorManGetServerCount( nServer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorManGetServerCount" ) );
	#else
		if( m_pManGetServerCount != NULL )
		{
			CORSTATUS l_CorStatus = m_pManGetServerCount( nServer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorManGetServerCount" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::ManGetServerByIndex( UINT32 index, PSTR name, PCORSERVER server )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorManGetServerByIndex( index, name, server );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorManGetServerByIndex" ) );
	#else
		if( m_pManGetServerByIndex != NULL )
		{
			CORSTATUS l_CorStatus = m_pManGetServerByIndex( index, name, server );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorManGetServerByIndex" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::ManResetServer( CORSERVER hServer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorManResetServer( hServer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorManResetServer" ) );
	#else
		if( m_pManResetServer != NULL )
		{
			CORSTATUS l_CorStatus = m_pManResetServer( hServer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorManResetServer" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::ManReleaseServer( CORSERVER hServer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorManReleaseServer( hServer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorManReleaseServer" ) );
	#else
		if( m_pManReleaseServer != NULL )
		{
			CORSTATUS l_CorStatus = m_pManReleaseServer( hServer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorManReleaseServer" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferAbort( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferAbort( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferAbort" ) );
	#else
		if( m_pXferAbort != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferAbort( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferAbort" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferAppend( CORXFER hXfer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferAppend( hXfer, hSrc, hDst, pDesc );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferAppend" ) );
	#else
		if( m_pXferAppend != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferAppend( hXfer, hSrc, hDst, pDesc );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferAppend" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferConnect( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferConnect( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferConnect" ) );
	#else
		if( m_pXferConnect != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferConnect( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferConnect" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferDisconnect( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferDisconnect( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferDisconnect" ) );
	#else
		if( m_pXferDisconnect != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferDisconnect( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferDisconnect" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferReset( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferReset( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferReset" ) );
	#else
		if( m_pXferReset != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferReset( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferReset" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferFree( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferFree( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferFree" ) );
	#else
		if( m_pXferFree != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferFree( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferFree" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferNew( CORSERVER hServer, CORHANDLE hSrc, CORHANDLE hDst, PCORXFER_DESC pDesc, CORXFER *hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferNew( hServer, hSrc, hDst, pDesc, hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferNew" ) );
	#else
		if( m_pXferNew != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferNew( hServer, hSrc, hDst, pDesc, hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferNew" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferRegisterCallback( CORXFER hXfer, UINT32 eventType, PCORCALLBACK callback, void *context )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferRegisterCallback( hXfer, eventType, callback, context );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferRegisterCallback" ) );
	#else
		if( m_pXferRegisterCallback != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferRegisterCallback( hXfer, eventType, callback, context );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferRegisterCallback" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferGetPrm( CORXFER hXfer, UINT32 prm, UINT32* value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferGetPrm( hXfer, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferGetPrm" ) );
	#else
		if( m_pXferGetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferGetPrm( hXfer, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferGetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferSetPrm( CORXFER hXfer, UINT32 prm, UINT32 value )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferSetPrm( hXfer, prm, value );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferSetPrm" ) );
	#else
		if( m_pXferSetPrm != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferSetPrm( hXfer, prm, value );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferSetPrm" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferStart( CORXFER hXfer, UINT32 count )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferStart( hXfer, count );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferStart" ) );
	#else
		if( m_pXferStart != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferStart( hXfer, count );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferStart" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferStop( CORXFER hXfer )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferStop( hXfer );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferStop" ) );
	#else
		if( m_pXferStop != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferStop( hXfer );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferStop" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferUnregisterCallback( CORXFER hXfer, PCORCALLBACK callback )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferUnregisterCallback( hXfer, callback );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferUnregisterCallback" ) );
	#else
		if( m_pXferUnregisterCallback != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferUnregisterCallback( hXfer, callback );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferUnregisterCallback" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

HRESULT SVSaperaManager::XferWait( CORXFER hXfer, UINT32 timeout )
{
	HRESULT l_Status = S_OK;

	#ifdef USE_CORAPI_LIB
		CORSTATUS l_CorStatus = CorXferWait( hXfer, timeout );

		l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferWait" ) );
	#else
		if( m_pXferWait != NULL )
		{
			CORSTATUS l_CorStatus = m_pXferWait( hXfer, timeout );

			l_Status = GetErrorNumber( l_CorStatus, _T( "CorXferWait" ) );
		}
		else
		{
			l_Status = E_FAIL;
		}
	#endif

	return l_Status;
}

#ifndef USE_CORAPI_LIB
	void SVSaperaManager::LoadDLL()
	{
		if( m_LibHandle == NULL )
		{
			m_LibHandle = ::LoadLibrary( "corapi.dll" );

			::Sleep(0);

			if ( m_LibHandle != NULL )
			{
				m_pAcqGetCap = (SVCorAcqGetCapPtr)::GetProcAddress( m_LibHandle, "CorAcqGetCap" );
				m_pAcqGetCount = (SVCorAcqGetCountPtr)::GetProcAddress( m_LibHandle, "CorAcqGetCount" );
				m_pAcqGetHandle = (SVCorAcqGetHandlePtr)::GetProcAddress( m_LibHandle, "CorAcqGetHandle" );
				m_pAcqGetPrm = (SVCorAcqGetPrmPtr)::GetProcAddress( m_LibHandle, "CorAcqGetPrm" );
				m_pAcqSetLut = (SVCorAcqSetLutPtr)::GetProcAddress( m_LibHandle, "CorAcqSetLut" );
				m_pAcqSetPrm = (SVCorAcqSetPrmPtr)::GetProcAddress( m_LibHandle, "CorAcqSetPrm" );
				m_pAcqSetPrms = (SVCorAcqSetPrmsPtr)::GetProcAddress( m_LibHandle, "CorAcqSetPrms" );
				m_pAcqRelease = (SVCorAcqReleasePtr)::GetProcAddress( m_LibHandle, "CorAcqRelease" );
				m_pAcqReset = (SVCorAcqResetPtr)::GetProcAddress( m_LibHandle, "CorAcqReset" );
				m_pAcqRegisterCallback = (SVCorAcqRegisterCallbackPtr)::GetProcAddress( m_LibHandle, "CorAcqRegisterCallback" );
				m_pAcqUnregisterCallback = (SVCorAcqUnregisterCallbackPtr)::GetProcAddress( m_LibHandle, "CorAcqUnregisterCallback" );
				m_pAcqSoftwareTrigger = (SVCorAcqSoftwareTriggerPtr)::GetProcAddress( m_LibHandle, "CorAcqSoftwareTrigger" );
				m_pBufferFree = (SVCorBufferFreePtr)::GetProcAddress( m_LibHandle, "CorBufferFree" );
				m_pBufferGetPrm = (SVCorBufferGetPrmPtr)::GetProcAddress( m_LibHandle, "CorBufferGetPrm" );
				m_pBufferNew = (SVCorBufferNewPtr)::GetProcAddress( m_LibHandle, "CorBufferNew" );
				m_pBufferSetPrm = (SVCorBufferSetPrmPtr)::GetProcAddress( m_LibHandle, "CorBufferSetPrm" );
				m_pCamFree = (SVCorCamFreePtr)::GetProcAddress( m_LibHandle, "CorCamFree" );
				m_pCamLoad = (SVCorCamLoadPtr)::GetProcAddress( m_LibHandle, "CorCamLoad" );
				m_pCamGetPrm = (SVCorCamGetPrmPtr)::GetProcAddress( m_LibHandle, "CorCamGetPrm" );
				m_pCamNew = (SVCorCamNewPtr)::GetProcAddress( m_LibHandle, "CorCamNew" );
				m_pVicFree = (SVCorVicFreePtr)::GetProcAddress( m_LibHandle, "CorVicFree" );
				m_pVicGetPrm = (SVCorVicGetPrmPtr)::GetProcAddress( m_LibHandle, "CorVicGetPrm" );
				m_pVicLoad = (SVCorVicLoadPtr)::GetProcAddress( m_LibHandle, "CorVicLoad" );
				m_pVicNew = (SVCorVicNewPtr)::GetProcAddress( m_LibHandle, "CorVicNew" );
				m_pVicSetPrm = (SVCorVicSetPrmPtr)::GetProcAddress( m_LibHandle, "CorVicSetPrm" );
				m_pLutClip = (SVCorLutClipPtr)::GetProcAddress( m_LibHandle, "CorLutClip" );
				m_pLutFree = (SVCorLutFreePtr)::GetProcAddress( m_LibHandle, "CorLutFree" );
				m_pLutGetPrm = (SVCorLutGetPrmPtr)::GetProcAddress( m_LibHandle, "CorLutGetPrm" );
				m_pLutNew = (SVCorLutNewPtr)::GetProcAddress( m_LibHandle, "CorLutNew" );
				m_pLutNormal = (SVCorLutNormalPtr)::GetProcAddress( m_LibHandle, "CorLutNormal" );
				m_pLutRead = (SVCorLutReadPtr)::GetProcAddress( m_LibHandle, "CorLutRead" );
				m_pLutWrite = (SVCorLutWritePtr)::GetProcAddress( m_LibHandle, "CorLutWrite" );
				m_pManGetLocalServer = (SVCorManGetLocalServerPtr)::GetProcAddress( m_LibHandle, "CorManGetLocalServer" );
				m_pManGetServerCount = (SVCorManGetServerCountPtr)::GetProcAddress( m_LibHandle, "CorManGetServerCount" );
				m_pManGetServerByIndex = (SVCorManGetServerByIndexPtr)::GetProcAddress( m_LibHandle, "CorManGetServerByIndex" );
				m_pManResetServer = (SVCorManResetServerPtr)::GetProcAddress( m_LibHandle, "CorManResetServer" );
				m_pManReleaseServer = (SVCorManReleaseServerPtr)::GetProcAddress( m_LibHandle, "CorManReleaseServer" );
				m_pXferAbort = (SVCorXferAbortPtr)::GetProcAddress( m_LibHandle, "CorXferAbort" );
				m_pXferAppend = (SVCorXferAppendPtr)::GetProcAddress( m_LibHandle, "CorXferAppend" );
				m_pXferConnect = (SVCorXferConnectPtr)::GetProcAddress( m_LibHandle, "CorXferConnect" );
				m_pXferDisconnect = (SVCorXferDisconnectPtr)::GetProcAddress( m_LibHandle, "CorXferDisconnect" );
				m_pXferFree = (SVCorXferFreePtr)::GetProcAddress( m_LibHandle, "CorXferFree" );
				m_pXferNew = (SVCorXferNewPtr)::GetProcAddress( m_LibHandle, "CorXferNew" );
				m_pXferRegisterCallback = (SVCorXferRegisterCallbackPtr)::GetProcAddress( m_LibHandle, "CorXferRegisterCallback" );
				m_pXferReset = (SVCorXferResetPtr)::GetProcAddress( m_LibHandle, "CorXferReset" );
				m_pXferGetPrm = (SVCorXferGetPrmPtr)::GetProcAddress( m_LibHandle, "CorXferGetPrm" );
				m_pXferSetPrm = (SVCorXferSetPrmPtr)::GetProcAddress( m_LibHandle, "CorXferSetPrm" );
				m_pXferStart = (SVCorXferStartPtr)::GetProcAddress( m_LibHandle, "CorXferStart" );
				m_pXferStop = (SVCorXferStopPtr)::GetProcAddress( m_LibHandle, "CorXferStop" );
				m_pXferUnregisterCallback = (SVCorXferUnregisterCallbackPtr)::GetProcAddress( m_LibHandle, "CorXferUnregisterCallback" );
				m_pXferWait = (SVCorXferWaitPtr)::GetProcAddress( m_LibHandle, "CorXferWait" );
			}
		}
	}

	void SVSaperaManager::FreeDLL()
	{
		m_pAcqGetCap = NULL;
		m_pAcqGetCount = NULL;
		m_pAcqGetHandle = NULL;
		m_pAcqGetPrm = NULL;
		m_pAcqSetLut = NULL;
		m_pAcqSetPrm = NULL;
		m_pAcqSetPrms = NULL;
		m_pAcqRelease = NULL;
		m_pAcqReset = NULL;
		m_pAcqRegisterCallback = NULL;
		m_pAcqUnregisterCallback = NULL;
		m_pAcqSoftwareTrigger = NULL;
		m_pBufferFree = NULL;
		m_pBufferGetPrm = NULL;
		m_pBufferNew = NULL;
		m_pBufferSetPrm = NULL;
		m_pCamFree = NULL;
		m_pCamLoad = NULL;
		m_pCamGetPrm = NULL;
		m_pCamNew = NULL;
		m_pVicFree = NULL;
		m_pVicGetPrm = NULL;
		m_pVicLoad = NULL;
		m_pVicNew = NULL;
		m_pVicSetPrm = NULL;
		m_pLutClip = NULL;
		m_pLutFree = NULL;
		m_pLutGetPrm = NULL;
		m_pLutNew = NULL;
		m_pLutNormal = NULL;
		m_pLutRead = NULL;
		m_pLutWrite = NULL;
		m_pManGetLocalServer = NULL;
		m_pManGetServerCount = NULL;
		m_pManGetServerByIndex = NULL;
		m_pManResetServer = NULL;
		m_pManReleaseServer = NULL;
		m_pXferAbort = NULL;
		m_pXferAppend = NULL;
		m_pXferConnect = NULL;
		m_pXferDisconnect = NULL;
		m_pXferReset = NULL;
		m_pXferFree = NULL;
		m_pXferNew = NULL;
		m_pXferRegisterCallback = NULL;
		m_pXferGetPrm = NULL;
		m_pXferSetPrm = NULL;
		m_pXferStart = NULL;
		m_pXferStop = NULL;
		m_pXferUnregisterCallback = NULL;
		m_pXferWait = NULL;

		if( m_LibHandle != NULL )
		{
			::FreeLibrary( m_LibHandle );

			::Sleep( 0 );

			m_LibHandle = NULL;
		}
	}
#endif


