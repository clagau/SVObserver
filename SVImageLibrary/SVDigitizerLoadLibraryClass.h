// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDigitizerLoadLibraryClass
// * .File Name       : $Workfile:   SVDigitizerLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:37:20  $
// ******************************************************************************

#ifndef SVDIGITIZERLOADLIBRARYCLASS_H
#define SVDIGITIZERLOADLIBRARYCLASS_H

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVOMFCLibrary/SVOMFCLibrary.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVAcquisitionBufferInterface.h"

typedef HRESULT (WINAPI *SVCreatePtr)( void );
typedef HRESULT (WINAPI *SVDestroyPtr)( void );
typedef HRESULT (WINAPI *SVDigitizerGetCountPtr)( unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetHandlePtr)( SVHANDLE *, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerGetNamePtr)( SVHANDLE, BSTR * );
typedef HRESULT (WINAPI *SVDigitizerLoadCameraFilesPtr)( SVHANDLE, SAFEARRAY* );
typedef HRESULT (WINAPI *SVDigitizerGetBufferWidthPtr)( SVHANDLE, unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetBufferHeightPtr)( SVHANDLE, unsigned long * );
typedef HRESULT (WINAPI *SVDigitizerGetBufferFormatPtr)( SVHANDLE, int * );
typedef HRESULT (WINAPI *SVDigitizerCreateBuffersPtr)( SVHANDLE, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerRegisterBufferInterfacePtr)( SVHANDLE, SVAcquisitionBufferInterface * );
typedef HRESULT (WINAPI *SVDigitizerStartPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerStopPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnregisterBufferInterfacePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerEnablePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerRegisterPtr)( SVHANDLE, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerUnregisterPtr)( SVHANDLE, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVDigitizerInternalTriggerUnregisterAllPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnregisterStartAcquirePtr)( SVHANDLE, unsigned long, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVDigitizerUnregisterStopAcquirePtr)( SVHANDLE, unsigned long, SVCallbackPtr, void *, void * );
typedef HRESULT (WINAPI *SVDigitizerUnregisterAllAcquirePtr)( SVHANDLE, unsigned long );
typedef HRESULT (WINAPI *SVDigitizerDestroyBuffersPtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerUnloadCameraFilePtr)( SVHANDLE );
typedef HRESULT (WINAPI *SVDigitizerSetParametersPtr)( SVHANDLE, const SVDeviceParamCollection* );
typedef HRESULT (WINAPI *SVDigitizerSetParameterPtr)( SVHANDLE, const SVDeviceParamWrapper* );
typedef HRESULT (WINAPI *SVDigitizerGetParameterPtr)( SVHANDLE, SVDeviceParamEnum, SVDeviceParamWrapper** );
typedef HRESULT (WINAPI *SVDigitizerDestroyParameterPtr)( SVHANDLE, SVDeviceParamWrapper* );
typedef HRESULT (WINAPI *SVDigitizerParameterGetListPtr)( SVHANDLE, VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerParameterGetNamePtr)( SVHANDLE, unsigned long, BSTR * );
typedef HRESULT (WINAPI *SVDigitizerParameterGetValuePtr)( SVHANDLE, int, int *,VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerParameterSetValuePtr)( SVHANDLE, int, int, VARIANT * );
typedef HRESULT (WINAPI *SVDigitizerScanForCamerasPtr)();

/*
This class is used to interface into the digitizer DLLs.  The methods of this class directly relate to the exported function of the DLL.
*/
class SVDigitizerLoadLibraryClass  
{
public:
	SVDigitizerLoadLibraryClass();
	~SVDigitizerLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close();
	
	bool IsLoaded() const;
	HRESULT GetLibraryName( BSTR *p_pbstrName );

	HRESULT GetCount( unsigned long *p_pulCount );
	HRESULT GetHandle( SVHANDLE *p_phHandle, unsigned long p_ulIndex );
	HRESULT GetName( SVHANDLE p_hHandle, BSTR *p_pbstrName );
	HRESULT LoadCameraFiles( SVHANDLE p_hHandle, SAFEARRAY* p_psaFileNames );
	HRESULT GetBufferWidth( SVHANDLE p_hHandle, unsigned long *p_pulWidth );
	HRESULT GetBufferHeight( SVHANDLE p_hHandle, unsigned long *p_pulHeight );
	HRESULT GetBufferFormat( SVHANDLE p_hHandle, int *p_piFormat );
	HRESULT CreateBuffers( SVHANDLE p_hHandle, unsigned long p_ulCount );
	HRESULT RegisterBufferInterface( SVHANDLE p_hHandle, SVAcquisitionBufferInterface* p_pInterface );
	HRESULT Start( SVHANDLE p_hHandle );
	HRESULT Stop( SVHANDLE p_hHandle );
	HRESULT UnregisterBufferInterface( SVHANDLE p_hHandle );
	
	HRESULT InternalTriggerEnable( SVHANDLE p_hHandle );
	HRESULT InternalTrigger( SVHANDLE p_hHandle );
	HRESULT InternalTriggerRegister( SVHANDLE p_hHandle, SVCallbackStruct &p_rCallback );
	HRESULT InternalTriggerUnregister( SVHANDLE p_hHandle, SVCallbackStruct &p_rCallback );
	HRESULT InternalTriggerUnregisterAll( SVHANDLE p_hHandle );
	
	HRESULT DestroyBuffers( SVHANDLE p_hHandle );
	HRESULT UnloadCameraFile( SVHANDLE p_hHandle );
	HRESULT SetParameters( SVHANDLE p_hHandle, const SVDeviceParamCollection* p_pParameters );
	HRESULT SetParameter( SVHANDLE p_hHandle, const SVDeviceParamWrapper* p_pParameter );
	HRESULT GetParameter( SVHANDLE p_hHandle, SVDeviceParamEnum p_eParameter, SVDeviceParamWrapper** p_ppParameter );
	HRESULT DestroyParameter( SVHANDLE p_hHandle, SVDeviceParamWrapper* p_pParameter );
	HRESULT ParameterGetList( SVHANDLE p_hHandle, VARIANT *p_pvarName );
	HRESULT ParameterGetName( SVHANDLE p_hHandle, int p_iParameterID, BSTR *p_pbstrName );
	HRESULT ParameterGetValue( SVHANDLE p_hHandle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarName );
	HRESULT ParameterSetValue( SVHANDLE p_hHandle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarName );

	HRESULT ScanForCameras();

private:
	//This attribute holds the handle to the open DLL.
	HMODULE m_hmHandle;

	//This attribute holds the address to the SVCreate function in the DLL.
	SVCreatePtr m_psvCreate;
	//This attribute holds the address to the SVDestroy function in the DLL.
	SVDestroyPtr m_psvDestroy;
	//This attribute holds the address to the SVDigitizerGetCount function in the DLL.
	SVDigitizerGetCountPtr m_psvGetCount;
	//This attribute holds the address to the SVDigitizerGetHandle function in the DLL.
	SVDigitizerGetHandlePtr m_psvGetHandle;
	//This attribute holds the address to the SVDigitizerGetName function in the DLL.
	SVDigitizerGetNamePtr m_psvGetName;
	//This attribute holds the address to the SVDigitizerLoadCameraFiles function in the DLL.
	SVDigitizerLoadCameraFilesPtr m_psvLoadCameraFiles;
	//This attribute holds the address to the SVDigitizerGetBufferWidth function in the DLL.
	SVDigitizerGetBufferWidthPtr m_psvGetBufferWidth;
	//This attribute holds the address to the SVDigitizerGetBufferHeight function in the DLL.
	SVDigitizerGetBufferHeightPtr m_psvGetBufferHeight;
	//This attribute holds the address to the SVDigitizerGetBufferFormat function in the DLL.
	SVDigitizerGetBufferFormatPtr m_psvGetBufferFormat;
	//This attribute holds the address to the SVDigitizerCreateBuffers function in the DLL.
	SVDigitizerCreateBuffersPtr m_psvCreateBuffers;
	//This attribute holds the address to the SVDigitizerRegisterBufferInterface function in the DLL.
	SVDigitizerRegisterBufferInterfacePtr m_psvRegisterBufferInterface;
	//This attribute holds the address to the SVDigitizerStart function in the DLL.
	SVDigitizerStartPtr m_psvStart;
	//This attribute holds the address to the SVDigitizerStop function in the DLL.
	SVDigitizerStopPtr m_psvStop;
	//This attribute holds the address to the SVDigitizerUnregisterBufferInterface function in the DLL.
	SVDigitizerUnregisterBufferInterfacePtr m_psvUnregisterBufferInterface;
	//This attribute holds the address to the SVDigitizerInternalTriggerEnable function in the DLL.
	SVDigitizerInternalTriggerEnablePtr m_psvInternalTriggerEnable;
	//This attribute holds the address to the SVDigitizerInternalTrigger function in the DLL.
	SVDigitizerInternalTriggerPtr m_psvInternalTrigger;
	//This attribute holds the address to the SVDigitizerInternalTriggerRegister function in the DLL.
	SVDigitizerInternalTriggerRegisterPtr m_psvInternalTriggerRegister;
	//This attribute holds the address to the SVDigitizerInternalTriggerUnregister function in the DLL.
	SVDigitizerInternalTriggerUnregisterPtr m_psvInternalTriggerUnregister;
	//This attribute holds the address to the SVDigitizerInternalTriggerUnregisterAll function in the DLL.
	SVDigitizerInternalTriggerUnregisterAllPtr m_psvInternalTriggerUnregisterAll;
	//This attribute holds the address to the SVDigitizerDestroyBuffers function in the DLL.
	SVDigitizerDestroyBuffersPtr m_psvDestroyBuffers;
	//This attribute holds the address to the SVDigitizerUnloadCameraFile function in the DLL.
	SVDigitizerUnloadCameraFilePtr m_psvUnloadCameraFile;
	//This attribute holds the address to the SVDigitizerSetParameters function in the DLL.
	SVDigitizerSetParametersPtr m_psvSetParameters;
	//This attribute holds the address to the SVDigitizerSetParameter function in the DLL.
	SVDigitizerSetParameterPtr m_psvSetParameter;
	//This attribute holds the address to the SVDigitizerGetParameter function in the DLL.
	SVDigitizerGetParameterPtr m_psvGetParameter;
	//This attribute holds the address to the SVDigitizerDestroyParameter function in the DLL.
	SVDigitizerDestroyParameterPtr m_psvDestroyParameter;
	//This attribute holds the address to the SVDigitizerParameterGetList function in the DLL.
	SVDigitizerParameterGetListPtr m_psvParameterGetList;
	//This attribute holds the address to the SVDigitizerParameterGetName function in the DLL.
	SVDigitizerParameterGetNamePtr m_psvParameterGetName;
	//This attribute holds the address to the SVDigitizerParameterGetValue function in the DLL.
	SVDigitizerParameterGetValuePtr m_psvParameterGetValue;
	//This attribute holds the address to the SVDigitizerParameterSetValue function in the DLL.
	SVDigitizerParameterSetValuePtr m_psvParameterSetValue;
	//This attribute holds the address to the SVDigitizerScanForCameras function in the DLL.
	SVDigitizerScanForCamerasPtr m_psvScanForCameras;
};

#include "SVDigitizerLoadLibraryClass.inl"

#endif // _SVDIGITIZERLOADLIBRARYCLASS_H
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVDigitizerLoadLibraryClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:37:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Feb 2010 09:37:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Dec 2009 11:33:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2009 15:38:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Jun 2009 17:31:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ScanForCameras interface
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Feb 2009 13:34:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added method to get module name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Sep 2008 11:09:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for Internal Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   13 Jul 2009 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2005 15:18:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new parameter functions for the DLL and new image formats for the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Oct 2003 12:58:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:57:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added image objects to new image interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
