//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVirtualCamera
//* .File Name       : $Workfile:   SVVirtualCamera.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVInfoStructs.h"
#include "SVContainerLibrary/SVMap.h"
#include "SVContainerLibrary/SVSet.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVTriggerRelayClass.h"
#include "SVFileAcquisitionInitiator.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVAcquisitionClass.h"
#include "BasicValueObjects.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#pragma endregion Includes

#pragma region Declarations
//Camera variables
const TCHAR CameraSerialNumber[]	= _T( "Serial Number" );
const TCHAR CameraShutter[]			= _T( "Shutter" );
const TCHAR CameraGain[]			= _T( "Gain" );
const TCHAR CameraContrast[]		= _T( "Contrast" ); //Legacy name for Gain was Contrast.

typedef SVVector< SVVirtualCamera* > SVVirtualCameraArray;
typedef SVSet< SVVirtualCamera* > SVVirtualCameraSet;
typedef std::set< SVVirtualCamera* > SVVirtualCameraPtrSet;
typedef SVSet< SVAcquisitionClassPtr > SVAcquisitionClassSet;
typedef SVMap< CString, SVVirtualCamera* > SVVirtualCameraMap;

class SVORequestClass;
class SVDeviceParamCollection;
#pragma endregion Declarations

class SVVirtualCamera : public SVObjectClass
{
public:
#pragma region Constructor
	SVVirtualCamera( LPCSTR ObjectName );
	SVVirtualCamera( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCAMERAOBJECT );
	virtual ~SVVirtualCamera();
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL GetImageInfo( SVImageInfoClass *pImageInfo );

	virtual long GetImageDepth() const;

	virtual HRESULT RefreshObject();

	BOOL Create( LPCTSTR p_szDeviceName );
	BOOL Destroy();

	// Runtime Functions
	BOOL CanGoOnline() const;
	BOOL GoOnline();
	BOOL GoOffline();

	BOOL RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
	BOOL UnregisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );

    SVAcquisitionClassPtr GetAcquisitionDevice() const { return mpsvDevice; }
	
    HRESULT SetLightReference( SVLightReference& rArray );
	HRESULT GetLightReference( SVLightReference& rArray ) const;
	HRESULT SetLut( const SVLut& lut );
	HRESULT GetLut( SVLut& lut ) const;

	HRESULT GetSourceImageIndex( SVDataManagerHandle& p_rHandle, const SVProductInfoStruct& p_rProduct ) const;
	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;

	HRESULT ReserveNextImageHandleIndex( SVDataManagerHandle& p_rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType = SV_ACQUISITION ) const;
	virtual BOOL ReserveImageHandleIndex( const SVDataManagerHandle& p_rDMIndexHandle ) const;

	virtual BOOL CopyValue( const SVDataManagerHandle& p_From, const SVDataManagerHandle& p_To );

	void DumpDMInfo( LPCTSTR p_szName ) const;

	SVAcquisitionClassPtr mpsvDevice;
	SVCallbackClassPtrQueue m_CallbackList;

    HRESULT GetBand(int& riBand) const;
    HRESULT GetBandSize(int& riBandSize) const;

	inline long BandLink() {return mlBandLink;}
	inline long SetBandLink(long l) {return mlBandLink = l;}

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool bFileAcquisition);

	LPCTSTR GetImageFilename() const;
	void SetImageFilename(const SVString& filename);

	LPCTSTR GetImageDirectoryName() const;
	void SetImageDirectoryName(const SVString& directoryName);

	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);

	bool IsFileImageSizeEditModeFileBased() const;
	void SetFileImageSizeEditModeFileBased(bool bFileBsed);

	const SIZE& GetFileImageSize() const;
	void SetFileImageSize(const SIZE& size);

	long GetFileImageWidth() const;
	void SetFileImageWidth(long width);

	long GetFileImageHeight() const;
	void SetFileImageHeight(long height);

	HRESULT RegisterTriggerRelay(SVIOTriggerLoadLibraryClass* triggerDLL, unsigned long ulIndex);
	HRESULT UnregisterTriggerRelay();

	HRESULT UpdateCameraParameters();
	HRESULT UpdateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam);
	HRESULT UpdateDeviceParameters(SVDeviceParamCollection& rCameraParameters);
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	static HRESULT CALLBACK SVImageCallback( void *pvOwner, void *pvCaller, void *pvResponse );

	virtual void FinishProcess( SVODataResponseClass *pResponse );
#pragma endregion Protected Methods

private:
#pragma region Private Methods
	BOOL DestroyLocal();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	long mlBandLink;
	CStdioFile m_LogFile;

private:
	BasicValueObjects m_CameraValues;
	bool m_bFileAcquisition;
	SVString m_imageFilename;
	SVString m_imageDirectoryName;
	bool m_bImageSizeEditModeFileBased;
	long m_imageLoadingMode;
	SIZE m_imageSize;
	SVTriggerRelayClass<SVFileAcquisitionInitiator> m_triggerRelay;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVVirtualCamera.h_v  $
 * 
 *    Rev 1.5   17 Jul 2014 20:51:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jun 2014 10:23:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added UpdateCameraLongParameter method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2014 13:26:54   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed char to TCHAR to match use of _T macro.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Mar 2014 15:33:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added RefreshObject, GetChildObject, UpdateDeviceParameters, UpdateCameraParameters methods.
 *   Added m_CameraValues and camera variable constants.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2014 18:24:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Made methods const.
 *   Reorganized methods to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:24:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   08 Aug 2012 15:42:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Jun 2012 14:29:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   28 Sep 2011 13:17:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Sep 2011 16:23:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   30 Mar 2011 16:15:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   22 Mar 2011 08:03:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Nov 2010 14:49:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   03 Mar 2010 12:03:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove duplicate containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Dec 2009 13:31:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 13:14:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   08 Oct 2008 09:38:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revsied to add Trigger Relay and File Acquisition Initiator to support the use of hardware based triggers with File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Sep 2008 14:38:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and member variables in support of FileAcquistion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   21 Jun 2007 15:07:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14.1.0   14 Jul 2009 15:37:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   23 Aug 2005 10:13:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to handle copy forward correctly.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Aug 2005 10:30:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Aug 2005 12:48:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Feb 2005 12:18:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added typedef for std::set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Feb 2005 15:25:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Feb 2004 17:31:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Jul 2003 07:58:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVLightReferenceArray to SVLightReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Apr 2003 17:30:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Jan 2003 09:39:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetDataManagerHandle method to use the new image circle buffer methodology.  Updated FinishProcess method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jan 2003 17:41:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Get CircleBuffer and Index functions to be used to lock images from the data in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 09:54:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SVVirtualCameraSet
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jan 2003 15:32:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * GetLut
 * SetLut
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Dec 2002 15:23:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   long GetSourceImageIndex( SVProductInfoStruct* pProduct );
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2002 22:35:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVVirtualCamera class with accessor function for mlBandLink and made the member variable protected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:41:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/