//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeExports
//* .File Name       : $Workfile:   SVMatroxGigeExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:18  $
//******************************************************************************

#ifndef SVMATROXGIGEEXPORTS_H
#define SVMATROXGIGEEXPORTS_H

#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"

class SVAcquisitionBufferInterface;

typedef HRESULT ( CALLBACK *SVMatroxGigeCallbackPtr )( void *,   /* Owner */
                                               void * ); /* Data */ 

// General Exports

HRESULT WINAPI SVCreate();
HRESULT WINAPI SVDestroy();

// Trigger Exports

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVCallbackPtr p_pCallback, void *p_pOwner, void *p_pData );
HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

// Digitizer Exports

HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount );
HRESULT WINAPI SVDigitizerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex );
HRESULT WINAPI SVDigitizerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName );
HRESULT WINAPI SVDigitizerLoadCameraFiles( unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames );
HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long p_ulHandle, unsigned long *p_pulWidth );
HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long p_ulHandle, unsigned long *p_pulHeight );
HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long p_ulHandle, int *p_piFormat );
HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long p_ulHandle, unsigned long p_ulCount );
HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long p_ulHandle, SVAcquisitionBufferInterface* p_pInterface );
HRESULT WINAPI SVDigitizerStart( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerStop( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerUnloadCameraFile( unsigned long p_ulHandle );

HRESULT WINAPI SVDigitizerParameterGetList( unsigned long p_ulHandle, VARIANT *p_pvarValue );
HRESULT WINAPI SVDigitizerParameterGetName( unsigned long p_ulHandle, int p_iParameterID, BSTR *p_pbstrName );
HRESULT WINAPI SVDigitizerParameterGetValue( unsigned long p_ulHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue );
HRESULT WINAPI SVDigitizerParameterSetValue( unsigned long p_ulHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue );

HRESULT WINAPI SVDigitizerSetParameters( unsigned long p_ulHandle, const SVDeviceParamCollection* p_pParameters );
HRESULT WINAPI SVDigitizerSetParameter( unsigned long p_ulHandle, const SVDeviceParamWrapper* p_pParameter );
HRESULT WINAPI SVDigitizerGetParameter( unsigned long p_ulHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter );
HRESULT WINAPI SVDigitizerDestroyParameter( unsigned long p_ulHandle, SVDeviceParamWrapper* p_pParameter );

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long p_ulHandle );
HRESULT WINAPI SVDigitizerInternalTriggerRegister( unsigned long p_ulHandle, SVMatroxGigeCallbackPtr& callback, void* pOwner, void* pData );
HRESULT WINAPI SVDigitizerInternalTriggerUnregister( unsigned long p_ulHandle, SVMatroxGigeCallbackPtr& callback, void* pOwner, void* pData );
HRESULT WINAPI SVDigitizerInternalTriggerUnregisterAll( unsigned long p_ulHandle );

HRESULT WINAPI SVDigitizerScanForCameras();

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeExports.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:44:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jan 2013 09:59:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 12:29:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
