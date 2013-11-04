//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCorecoAcquisitionClass
//* .File Name       : $Workfile:   SVCorecoAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:01:58  $
//******************************************************************************

#ifndef SVCORECOACQUISITIONCLASS_H
#define SVCORECOACQUISITIONCLASS_H

#include "SVAcquisitionClass.h"

class SVCorecoAcquisitionClass : public SVAcquisitionClass  
{
public:
	SVCorecoAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVCorecoAcquisitionClass();

	virtual bool IsValid();
	virtual bool IsValidBoard();

	virtual HRESULT Destroy();

	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray);
	virtual HRESULT UnloadFiles();

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT LoadLightReference( SVLightReference& rArray );
	virtual HRESULT ResetLightReference();

	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue );
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue );
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue );

	virtual HRESULT CreateLut( const SVLutInfo& info );

	virtual HRESULT GetImageInfo(SVImageInfoClass *pImageInfo);

protected:
	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );

	virtual HRESULT GetCameraImageInfo(SVImageInfoClass *pImageInfo);

	virtual HRESULT SetLutImpl( const SVLut& lut );
	virtual HRESULT GetLutImpl( SVLut& lut );

	HRESULT DestroyLocal();

	enum
	{
		LR_BRIGHTNESS = 0,
		LR_CONTRAST   = 1
	};
	

private:

	bool mbIsCorFilesLoaded;

	enum
	{
		DEFAULT_CONTRAST          = 10000,    // this is the correct default value, not 100
		DEFAULT_CONTRAST_RGB_MONO = 8100,
		DEFAULT_BRIGHTNESS        = 0,
		CORECO_SCALE_VALUE        = 10    // what this is for, I don't know!
	};
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCorecoAcquisitionClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:01:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Sep 2011 15:50:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated camera management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   27 Jan 2011 10:58:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   15 Feb 2010 12:25:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   03 Sep 2009 09:59:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 11:18:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   04 Nov 2008 12:21:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  639
 * SCR Title:  Update SVObserver to better handle going offline while triggering
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added m_numberOfBuffers
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16.1.0   14 Jul 2009 12:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 Oct 2003 10:48:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Sep 2003 14:01:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  353
 * SCR Title:  New ViperRGB Firmware
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed code dealing with Trash Buffer.  The RGB will be using the cycle mode Next Empty
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   29 Aug 2003 13:17:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fix RGB Color default contrast
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   07 Aug 2003 12:51:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * changed DestroyImageBuffer to DestroyImageAcquisitionBuffer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Jul 2003 11:55:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Coreco to DLL
 * changed architecture for IO
 * changed architecture for Digital
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Apr 2003 17:25:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Mar 2003 08:57:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetRootDevice
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Feb 2003 10:53:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetNextRequest, NotifyEndFrame, Reset, CreateLightReference, and Create methods to fix issues with the ViperRGB.
 * Updated UnlockBuffer, LockBuffer, NotifyStartFrame, UnregisterAllCallbacks, UnregisterCallback, RegisterCallback, StartProcess, CanProcess, DestroyLocal, UnloadFiles, Stop, Start, DestroyBuffers, CreateBuffers, LoadFiles, IsValidBoard, IsValid and constructor methods to fix issues with ViperRGB.
 * Updated global callback function SVOStartFrameEndFrameCallback to fix issues with ViperRGB.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Feb 2003 10:11:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed mulType reference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   06 Feb 2003 10:39:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut functions (was in ImageProcessing)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jan 2003 09:42:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all appropriate methods with SVImageBufferHandleStruct or SVImageObjectClass parameter for methods that used SVImageInfoClass as a parameter.  Updated start frame and end frame tracking techniques for handling the new acquisition tranfer mode next empty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Jan 2003 09:20:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new method IsValidBoard() which will check to see if the mcorAcq and mcorBoard  is !CORHANDLE_IS_NULL.  Also changed the method IsValid to use the macro CORHANDLE_IS_NULL instead of SV_IS_VALID_CORHANDLE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 09:43:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ResetLut
 * Modified SetLut to use bands
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jan 2003 15:26:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * CreateLut
 * DestroyLut
 * GetLut
 * SetLut
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 19:06:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  310
 * SCR Title:  Update SVObserver to Version 4.01 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods associated with ModelessDebug dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2002 21:50:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetRootDeviceName method to SVCorecoAcquisitionClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:51:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
