// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCISAPERA
// * .File Name       : $Workfile:   SVCISapera.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:06  $
// ******************************************************************************

#ifndef SVCISAPERA_H
#define SVCISAPERA_H

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVCIAcqStruct.h"
#include "SVCIBufferStruct.h"
#include "SVImageLibrary/SVLut.h"
#include "SVImageLibrary/SVLightReference.h"

HRESULT g_CICreate();
HRESULT g_CIDestroy( bool p_bClose = false );

HRESULT g_CIBufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, unsigned long p_ulType, unsigned long *p_pulIndex );
HRESULT g_CIBufferCreate( BSTR p_bstrFileName, unsigned long *p_pulIndex );
HRESULT g_CIBufferDestroy( unsigned long p_ulIndex );

HRESULT g_CIAcqCreate( unsigned long p_ulServerIndex, unsigned long p_ulAcqIndex, unsigned long *p_pulIndex );
HRESULT g_CIAcqLoadCameraFiles( unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames );
HRESULT g_CIAcqBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
HRESULT g_CIAcqTriggerRegister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback );
HRESULT g_CIAcqRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
HRESULT g_CIAcqStart( unsigned long p_ulIndex );
HRESULT g_CIAcqStop( unsigned long p_ulIndex );
HRESULT g_CIAcqUnregisterBufferInterface( unsigned long p_ulIndex );
HRESULT g_CIAcqTriggerUnregister( unsigned long p_ulIndex, SVCallbackStruct &p_rCallback );
HRESULT g_CIAcqTriggerUnregisterAll( unsigned long p_ulIndex );

HRESULT g_CIAcqTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
HRESULT g_CIAcqTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT g_CIAcqTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
HRESULT g_CIAcqTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

HRESULT g_CIAcqBufferDestroyAll( unsigned long p_ulIndex );
HRESULT g_CIAcqUnloadCameraFiles( unsigned long p_ulIndex );
HRESULT g_CIAcqDestroy( unsigned long p_ulIndex );

HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long &p_rlValue );
HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, unsigned long p_ulType, long p_lValue );
HRESULT g_CIAcqGetLightReference( unsigned long p_ulIndex, SVLightReference& lr );
HRESULT g_CIAcqSetLightReference( unsigned long p_ulIndex, const SVLightReference& lr );

HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue );
HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, VARIANT &p_rvarValue );
HRESULT g_CIAcqGetLut( unsigned long p_ulIndex, SVLut& lut );
HRESULT g_CIAcqSetLut( unsigned long p_ulIndex, const SVLut& lut );
HRESULT g_CIAcqNeutralLutToRawLut(const SVLut& lut, CByteArray& aRawBytes);
HRESULT g_CIAcqRawLutToNeutralLut(const CByteArray& aRawBytes, SVLut& lut);

HRESULT g_CIAcqEnableSoftwareTrigger( unsigned long p_ulIndex );
HRESULT g_CIAcqSoftwareTrigger( unsigned long p_ulIndex );

extern SVCIAcqStructVector g_caAcqs;
extern SVCIBufferStructCArray g_caBuffers;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCISapera.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jan 2013 09:42:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added additional methods to trigger interface
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   20 May 2011 09:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Dec 2009 11:17:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2009 14:48:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 11:06:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for Internal Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4.1.0   13 Jul 2009 13:04:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Oct 2003 14:46:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added two new export functions to allow the setting of parameters in a raw mode instead of using classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Oct 2003 12:33:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2003 10:26:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Uncommented get light reference code in the coreco dll project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2003 15:33:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed issues with callback and multi-threading.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 14:16:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New project added to interface I/O and Image processing for the Coreco Sapera Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
