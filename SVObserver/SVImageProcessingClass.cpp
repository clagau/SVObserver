//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageProcessingClass
//* .File Name       : $Workfile:   SVImageProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:54:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVImageObjectClass.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVUtilityLibrary/SVImageCopyUtility.h"

SVImageProcessingClass& SVImageProcessingClass::Instance()
{
	static SVImageProcessingClass l_Object;

	return l_Object;
}

SVImageProcessingClass::SVImageProcessingClass()
{
}

SVImageProcessingClass::~SVImageProcessingClass()
{
	DestroyLocal();
}

void SVImageProcessingClass::DestroyLocal()
{
	SVDigitizerProcessingClass::Instance().clear();
}

HRESULT SVImageProcessingClass::CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageBuffer( rInfo, rHandle );
}

HRESULT SVImageProcessingClass::CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageBuffer( rFromHandle, p_lConversionType, rToHandle );
}


HRESULT SVImageProcessingClass::CreateImageChildBuffer( const SVImageInfoClass& rParentInfo,
                                                     SVSmartHandlePointer rParentHandle,
                                                     SVImageInfoClass& rChildInfo,
                                                     SVSmartHandlePointer& rChildHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageChildBuffer( rParentInfo, rParentHandle, rChildInfo, rChildHandle );
}

HDC SVImageProcessingClass::CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateBufferDC( rInfo, rHandle );
}

HRESULT SVImageProcessingClass::DestroyBufferDC( SVSmartHandlePointer rHandle, HDC hDC )
{
	return SVMatroxImageProcessingClass::Instance().DestroyBufferDC( rHandle, hDC );
}

HRESULT SVImageProcessingClass::InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue )
{
	return SVMatroxImageProcessingClass::Instance().InitBuffer( rHandle, dwValue );
}

HRESULT SVImageProcessingClass::LoadImageBuffer( LPCTSTR tstrImagePathName, 
                                              SVImageInfoClass& rInfo,
                                              SVSmartHandlePointer& rHandle, 
                                              BOOL bBrowseIfNotExists )
{
	return SVMatroxImageProcessingClass::Instance().LoadImageBuffer( tstrImagePathName, rInfo, rHandle, bBrowseIfNotExists );
}

HRESULT SVImageProcessingClass::LoadImageBuffer( void* pBuffer, 
                                              SVImageInfoClass& rBufferInfo, 
                                              SVSmartHandlePointer& rBufferHandle,
                                              SVImageInfoClass& rCameraInfo )
{
	return SVMatroxImageProcessingClass::Instance().LoadImageBuffer( pBuffer, rBufferInfo, rBufferHandle, rCameraInfo );
}

HRESULT SVImageProcessingClass::SaveImageBuffer( LPCTSTR tstrImagePathName, const SVSmartHandlePointer& rHandle )
{
	return SVMatroxImageProcessingClass::Instance().SaveImageBuffer( tstrImagePathName, rHandle );
}

HRESULT SVImageProcessingClass::ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType )
{
	return SVMatroxImageProcessingClass::Instance().ConvertImageBuffer( p_rToDIB, rFromHandle, p_lConversionType );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( rToHandle, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( p_rToDIB, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( p_rToDIB, p_rToBitmapInfo, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBufferToFileDIB( p_rToDIB, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVImageCopyUtility& copier, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer(copier, rFromHandle );
}

HRESULT SVImageProcessingClass::CreateDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().CreateDataBuffer( pDataInfo );
}

HRESULT SVImageProcessingClass::DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().DestroyDataBuffer( pDataInfo );
}

HRESULT SVImageProcessingClass::ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength )
{
	return SVMatroxImageProcessingClass::Instance().ResizeDataBuffer( pDataInfo, lNewLength );
}

HRESULT SVImageProcessingClass::CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().CreateBarCodeBuffer( pDataInfo );
}

HRESULT SVImageProcessingClass::GetAcquisitionDevice( LPCTSTR szName, SVAcquisitionClassPtr& pDevice )
{
	HRESULT hr = S_OK;

	pDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( szName );

	return hr;
}

HRESULT SVImageProcessingClass::GetDigitizer(LPCTSTR szName, SVAcquisitionClassPtr& pDevice)
{
	HRESULT hr = S_OK;

	pDevice = SVDigitizerProcessingClass::Instance().GetDigitizer( szName );

	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageProcessingClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:54:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   11 Feb 2013 14:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   18 Jul 2012 13:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   10 Jul 2012 12:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added addition image copy method to interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   02 Jul 2012 16:58:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   28 Jun 2012 15:14:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   19 Dec 2011 15:58:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow data to be processed into shared memory and create shared memory structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Sep 2011 16:00:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   13 Apr 2011 19:15:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to handle new fixed image type and fix color regression issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   30 Mar 2011 16:00:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   22 Mar 2011 07:54:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   10 Feb 2011 14:45:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   04 Nov 2010 13:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Feb 2010 12:29:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   27 Oct 2009 18:52:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   15 Oct 2009 14:49:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised AddSubsystem and SetFileAcquisition to accomadate Matrox GIGE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 11:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Sep 2008 13:48:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetFileAcquisition method.
 * Removed unused code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   24 Jul 2007 13:31:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.0   14 Jul 2009 13:40:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   11 Nov 2005 10:39:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to check if digitizer procecessing class pointer was NULL before using it.
 * 
 * Added functionality to add at least one digital and one analog system to digitizer subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Nov 2005 15:53:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Feb 2005 14:27:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Nov 2003 13:48:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed GetSVIMType to GetBoardType(GetSVIMType())
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   26 Aug 2003 12:56:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Create method to fix issues with model verification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   31 Jul 2003 10:34:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed AddBoard to AddSubsystem, fixed problems in that function
 * Added code in GetAcquisitionDevice and GetDigitizer to make them work without boards present
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Jul 2003 09:26:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   expanded to allow for Digital system
 * changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 10:18:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Feb 2003 08:42:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   commented out SVMatroxBoardImageProcessingClass creation for now; will not be in 4.20 release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Feb 2003 08:01:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added provision for multiple boards
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Feb 2003 10:57:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem where Coreco Image processing Reset was not being called
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Feb 2003 10:11:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   No longer a base class; contains Board and Display Image Processing classes, serves as an application interface to these two subsystems, delegates calls to the appropriate piece.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jan 2003 09:15:50   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all appropriate methods with SVImageBufferHandleStruct or SVImageObjectClass parameter for methods that used SVImageInfoClass as a parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 09:45:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SetLut(band) and ResetLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2003 15:29:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * GetLut
 * SetLut
 * CalculateLut
 * CreateLut
 * DestroyLut
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:54:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
