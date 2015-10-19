//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionClass.h
//* .File Name       : $Workfile:   SVAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Mar 2014 18:05:22  $
//******************************************************************************

#ifndef SVACQUISITIONCLASS_H
#define SVACQUISITIONCLASS_H

#pragma region Includes
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVOLibrary/SVODataDeviceClass.h"
#include "SVOLibrary/SVODeviceClass.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

#include "SVFileNameArrayClass.h"
#include "SVImageObjectClass.h"
#include "SVSubscriberProviderInterfaces.h"
#pragma endregion Includes

#pragma region Declarations
struct SVAcquisitionConstructParams;
#pragma endregion Declarations

/**
@SVObjectName Acquisition Class

@SVObjectOverview This object is the device wrapper for the attached cameras.  This object manages the connection to the image signals and the image buffers.

@SVObjectOperations This object maintains operations for connecting and configuring the camera device information.  This includes but is not limited to setting up the notification signals, starting the notification process, stopping the notification process, creating buffers based on the image parameters, etc. 

*/
class SVAcquisitionClass :
	public SVODataDeviceClass,
	public SVSubscriberInterface,
	public SVAcquisitionBufferInterface
{
public:
	SVAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVAcquisitionClass();

	virtual bool IsValid() const;  // SVODataDeviceClass Override
	virtual bool IsStarted() const;  // SVODataDeviceClass Override

	virtual void ClearDevice();  // SVODataDeviceClass Override

	virtual HRESULT Create( unsigned long ulSize = 50 );  // SVODataDeviceClass Override
	virtual HRESULT Destroy();  // SVODataDeviceClass Override

	virtual HRESULT Start();  // SVODataDeviceClass Override
	virtual HRESULT Stop();  // SVODataDeviceClass Override
	virtual HRESULT Reset();  // SVODataDeviceClass Override

	virtual HRESULT AllocDigitizer( SVDeviceParamCollection& rDevice );
	virtual HRESULT DeallocDigitizer( SVDeviceParamCollection& rDevice );

	virtual HRESULT CreateBuffers( SVImageInfoClass IInfo, unsigned long ulSize );
	virtual HRESULT DestroyBuffers();

	virtual HRESULT GetFileNameArraySize( long &rlSize ) const;
	virtual HRESULT GetFileName( long lIndex, SVFileNameClass &rFileName ) const;
	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray );
	virtual HRESULT UnloadFiles();
	virtual HRESULT ReadCameraFile( const CString& sFile, SVDeviceParamCollection& rParams );

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT LoadLightReference( SVLightReference& rArray );
	virtual HRESULT GetLightReference( SVLightReference& rLR ) const;
	virtual HRESULT SetLightReference( SVLightReference& rLR, int iBand = -1 );
	virtual HRESULT ResetLightReference();

	virtual HRESULT CreateLightReferenceBand( int iBand, int iAttributes );
	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const;

	virtual HRESULT CreateLut( const SVLutInfo& info );
	virtual HRESULT DestroyLut();
	virtual HRESULT ResetLut();
	virtual HRESULT GetLut( SVLut& lut );
	virtual HRESULT SetLut( const SVLut& lut, int iBand = -1 );
	virtual HRESULT SetLut( const SVLutBand& lutBand );

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const;

	virtual SVImageObjectClassPtr GetCircleBuffer();
	virtual long GetCircleBufferSize() const;

	virtual CString GetRootDeviceName() const;

	virtual HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle ) const;
	virtual HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	inline CString DeviceName() const { return mDeviceName.c_str(); }
	inline CString DigName() const { return mcsDigName; }
	inline int Channel() const { return miChannel; }
	inline int DigNumber() const { return miDigNumber; }
	inline int BandSize() const { return miBandSize; }
	inline int BandMaxSize() const { return miBandMaxSize; }

	inline CString SetDigName(const CString& s) {return mcsDigName = s;}
	inline int     SetChannel(int i) {return miChannel = i;}
	inline int     SetDigNumber(int i) {return miDigNumber = i;}
	inline int     SetBandSize(int i) {return miBandSize = i;}
	inline int     SetBandMaxSize(int i) {return miBandMaxSize = i;}

	virtual bool IsValidBoard() const;

	virtual bool IsColor() const { return (miBandSize > 1) ; };

	virtual HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	HRESULT GetHardwareDeviceCapabilities( SVDeviceParamCollection& rDeviceParams );

	// called on creation
	virtual HRESULT SetHardwareDeviceCapabilities( const SVDeviceParamCollection& rDeviceParams );

	// called from SVImageBoardProcessingClass::DisconnectDevices
	virtual void ClearDeviceIdentifier();

	bool IsDigitizerSubsystemValid() const;


	virtual HRESULT SingleGrab( SVSmartHandlePointer p_SingleGrabHandle );

	// temp hack
	virtual SVOCallbackPtr GetCallbackFunction(const CString& sType) const {return NULL;}

	virtual BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const;
	virtual BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const;
	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	void DumpDMInfo( LPCTSTR p_szName ) const;

	virtual SVClock::SVTimeStamp GetTimeStamp() const;

	virtual unsigned long GetBufferWidth() const;
	virtual unsigned long GetBufferHeight() const;
	virtual int GetBufferFormat() const;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer );

	virtual HRESULT StoreLastImage();
	virtual HRESULT RestoreLastImage();

	SVHANDLE m_hDigitizer;
	BSTR m_LastImage;

protected:
	static DWORD WINAPI SingleGrabHelperFn(LPVOID lpParameter);

	friend class SVMatroxDCamAcquisitionProxy;	// to access these protected functions
	friend class SVIntekDCamAcquisitionProxy;	// to access these protected functions
	friend class SVMatroxGigeCameraProxy;		// to access these protected functions

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline() const;
	virtual bool IsBufferCreated() const { return mbIsBufferCreated; };

	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
	virtual SVDeviceParamCollection& HardwareCapabilities();
	virtual HRESULT GetLutImpl( SVLut& lut );
	virtual HRESULT SetLutImpl( const SVLut& lut );

	virtual HRESULT StartDigitizer();

	SVSmartIndexArrayHandlePtr m_pDataManagerHandle;
	
	SVImageInfoClass msvImageInfo;
	SVImageObjectClassPtr m_AcquisitionBuffersPtr;

	SVDeviceParamCollection m_DeviceParams;
	SVDeviceParamCollection m_CameraFileDeviceParams;
	SVLut& Lut();
	SVLightReference mLightReference;
	SVFileNameArrayClass mFiles;

	CString mcsDigName;
	int miChannel; //&&& Var needs to be set
	int miDigNumber; // &&& Var needs to be set
	int miBandSize;
	int miBandMaxSize;

	long mlStartFrameIndex;
	long mlDMStartFrameIndex;

	bool mbIsBufferCreated;
	bool mbTempOnline;
	SVSmartHandlePointer m_SingleGrabHandle;

private:
	SVAcquisitionClass();

	unsigned long mulSize;

	bool m_ImageAquired;

};

typedef SVSharedPtr< SVAcquisitionClass > SVAcquisitionClassPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAcquisitionClass.h_v  $
 * 
 *    Rev 1.3   07 Mar 2014 18:05:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 * Made  methods const.
 * Moved mbIsBufferCreated here from SVMatroxGigeAcquisitionClass.
 * Added virtual method IsBufferCreated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 11:54:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   29 Apr 2013 14:04:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:23:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   16 Sep 2011 15:37:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated acquisition device classes to handle new data management and callback fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   30 Mar 2011 15:52:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   22 Mar 2011 07:42:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   10 Feb 2011 14:24:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   04 Nov 2010 13:18:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   07 Jul 2010 16:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   24 Feb 2010 10:30:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to handle storing and restoring the image buffers when the system is going on and off line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   15 Feb 2010 12:22:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   15 Dec 2009 14:55:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   22 Oct 2009 19:27:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   14 Oct 2009 17:00:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Acquisition class to handle go-online functionality correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   03 Sep 2009 09:59:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   30 Jul 2009 11:12:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Jun 2009 17:57:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVMatroxGigeProxy as friend
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   21 Jun 2007 11:51:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22.1.0   14 Jul 2009 11:15:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated buffer transfer methodology, data management, and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   29 Nov 2006 15:39:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CreateSingleGrabImageBuffer method
 * Added DestroySingleGrabImageBuffer method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   29 Nov 2006 14:22:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquistion Proxies.
 * Added ClearDeviceIdentifier
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Nov 2005 15:36:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Jan 2004 07:38:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added SVSubscriberInterface support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   31 Oct 2003 08:19:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Stop virtual member fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Oct 2003 10:48:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   22 Sep 2003 14:54:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Lut handling from member var to DeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Sep 2003 09:00:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added AllocDigitizer and DeallocDigitizer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   07 Aug 2003 12:38:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SingleGrab
 * changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   31 Jul 2003 10:28:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added IsDigitizerSubsystemValid and IsImageSubsystemValid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Jul 2003 08:36:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to use DigitizerSubsystem and to use SVDeviceParamCollection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Apr 2003 16:14:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Mar 2003 08:58:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetRootDevice
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Feb 2003 10:31:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated EndProcess, GetNextIndex, NotifyEndFrame, and CreateBuffers methods to resolve issues with ViperRGB Board.
 * Added IsBufferEmpty method to allow verification of buffer state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Feb 2003 10:06:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed mulType and GetType and type enumeration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   06 Feb 2003 10:26:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Feb 2003 16:20:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved LockIndex and UnlockIndex from protected to public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jan 2003 16:36:52   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added NotifyStartFrame, NotifyEndFrame, LockBuffer, UnlockBuffer, GetDataManagerHandle, GetNextIndex, LockRequestResource, UnlockRequestResource, EndProcess, LockIndex, UnlockIndex, AttachResponseToRequest and CheckQueues methods to the class.  Modified constructor, Start, GetImageInfo, GetCircleBuffer, DestroyBuffers and CreateBuffers methods to handle new Data Manager and image buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Jan 2003 15:34:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added virtual method IsValidBoard()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jan 2003 09:35:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ResetLut
 * Modified SetLut to include band info
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jan 2003 15:23:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * SetImageProcessingClass
 * CreateLut
 * DestroyLut
 * GetLut
 * SetLut
 * member variable mLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2002 16:53:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added method GetRootDeviceName to SVAcquisitionClass and added public inline accessor functions for member variables and made member variables protected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:43:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
