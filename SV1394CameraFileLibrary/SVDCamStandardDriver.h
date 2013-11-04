//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamStandardDriver
//* .File Name       : $Workfile:   SVDCamStandardDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:36  $
//******************************************************************************

#ifndef SVDCAMSTANDARDDRIVER_H
#define SVDCAMSTANDARDDRIVER_H

#include "SVDCamDriver.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

#pragma warning( disable : 4065 )	// switch with no case

enum DCAMRegistersEnum
{
	DCAM_BRIGHTNESS           = 0x0800,
	DCAM_AUTO_EXPOSURE        = 0x0804,
	DCAM_SHARPNESS            = 0x0808,
	DCAM_WHITE_BALANCE        = 0x080C,
	DCAM_HUE                  = 0x0810,
	DCAM_SATURATION           = 0x0814,
	DCAM_GAMMA                = 0x0818,
	DCAM_SHUTTER              = 0x081C,
	DCAM_GAIN                 = 0x0820,

	DCAM_TRIGGER_MODE         = 0x0830,

	// DCAM 1.31
	DCAM_TRIGGER_DELAY        = 0x0834,
	DCAM_SOFTWARE_TRIGGER	  = 0x062C,
};

// DCAM terminology is whacked when it comes to bits
// their bit 0 is really bit 31 (high bit), their bit 31 is really bit 0 (low bit).
// here are some helpers to overcome this confusion.
enum DCAM_WHACKED_BITS
{
	DCAMBIT0  = BIT31,
	DCAMBIT1  = BIT30,
	DCAMBIT2  = BIT29,
	DCAMBIT3  = BIT28,
	DCAMBIT4  = BIT27,
	DCAMBIT5  = BIT26,
	DCAMBIT6  = BIT25,
	DCAMBIT7  = BIT24,
	DCAMBIT8  = BIT23,
	DCAMBIT9  = BIT22,
	DCAMBIT10 = BIT21,
	DCAMBIT11 = BIT20,
	DCAMBIT12 = BIT19,
	DCAMBIT13 = BIT18,
	DCAMBIT14 = BIT17,
	DCAMBIT15 = BIT16,
	DCAMBIT16 = BIT15,
	DCAMBIT17 = BIT14,
	DCAMBIT18 = BIT13,
	DCAMBIT19 = BIT12,
	DCAMBIT20 = BIT11,
	DCAMBIT21 = BIT10,
	DCAMBIT22 = BIT9,
	DCAMBIT23 = BIT8,
	DCAMBIT24 = BIT7,
	DCAMBIT25 = BIT6,
	DCAMBIT26 = BIT5,
	DCAMBIT27 = BIT4,
	DCAMBIT28 = BIT3,
	DCAMBIT29 = BIT2,
	DCAMBIT30 = BIT1,
	DCAMBIT31 = BIT0,

	DCAM_BYTE0 = DCAMBIT0 | DCAMBIT1 | DCAMBIT2 | DCAMBIT3 | DCAMBIT4 | DCAMBIT5 | DCAMBIT6 | DCAMBIT7,
	DCAM_BYTE1 = DCAMBIT8 | DCAMBIT9 | DCAMBIT10 | DCAMBIT11 | DCAMBIT12 | DCAMBIT13 | DCAMBIT14 | DCAMBIT15,
	DCAM_BYTE2 = DCAMBIT16 | DCAMBIT17 | DCAMBIT18 | DCAMBIT19 | DCAMBIT20 | DCAMBIT21 | DCAMBIT22 | DCAMBIT23,
	DCAM_BYTE3 = DCAMBIT24 | DCAMBIT25 | DCAMBIT26 | DCAMBIT27 | DCAMBIT28 | DCAMBIT29 | DCAMBIT30 | DCAMBIT31,

	DCAM_LOWORD = DCAM_BYTE0 | DCAM_BYTE1,
	DCAM_HIWORD = DCAM_BYTE2 | DCAM_BYTE3,

	DCAMBITS_12_15 = DCAMBIT12 | DCAMBIT13 | DCAMBIT14 | DCAMBIT15,
};

class SVDCamStandardDriver : public SVDCamDriver
{
public:
	SVDCamStandardDriver();
	virtual ~SVDCamStandardDriver();

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );
	virtual HRESULT SoftwareTrigger();
	virtual bool CanSoftwareTrigger();
	virtual HRESULT GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue);
	virtual HRESULT GoOnline();
	virtual HRESULT GoOffline();
	virtual CString GetFirmwareVersion();

	static SVDCamDriver* CreateNew();

	CString m_strCameraFileFirmware;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline DWORD GetValueAtBits(DWORD dwInput, DWORD dwBitRange/*Must be contiguous, as in BIT5 | BIT6 | BIT7*/)
	// returns decimal value of the bits masked by dwBitRange
	// e.g. GetValueAtBits(11 /*00001011*/, BIT2 | BIT3) returns the value 2 /*00000010*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
		DWORD dwValue=0;
		if (dwBitRange == 0)
			return 0;
		
		// first, mask out all unwanted values
		dwInput &= dwBitRange;

		// now find low end of range
		BYTE byShift=0;
		while (!(dwBitRange & BIT0))	// while low bit is not set
			{
			byShift++;
			dwBitRange >>= 1;
			}
		
		// and shift that many positions
		dwValue = dwInput >> byShift;

		return dwValue;
		}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline void SetValueAtBits(DWORD* pdwSource, DWORD dwBitRange/*Must be contiguous, as in BIT5 | BIT6 | BIT7*/,
							   DWORD dwValue)
	// sets value at the bits masked by dwBitRange
	// e.g. DWORD dwVar = 11; /* 00001011 in binary */
	//      SetValueAtBits(&dwVar, BIT4 | BIT5 | BIT6, 5 /* 101 in binary */);
	//      dwVar == 91 /* 01011011 in binary */;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
		if (dwBitRange == 0)
			return;
		// first, mask out all nonzero values in Source that fall in bitrange
		(*pdwSource) &= ~dwBitRange;

		// now find low end of range
		BYTE byShift=0;
		while (!(dwBitRange & BIT0))
			{
			byShift++;
			dwBitRange >>= 1;
			}
		
		// mask out all unwanted bits in value
		dwValue &= dwBitRange;
		// and shift value that many positions
		dwValue <<= byShift;

		// now assign the bits to Source
		(*pdwSource) |= dwValue;

		}

protected:
	virtual unsigned long GetDCAMTriggerPolarity() const;
	virtual unsigned long GetDCAMTriggerOnOff() const;
	virtual unsigned long GetDCAMTriggerMode() const;

	virtual unsigned long GetTriggerEdge() const;

	HRESULT SetDeviceParamAcquisitionTriggerEdge();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamStandardDriver.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:13:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2009 19:19:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Oct 2009 10:42:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSoftwareTriggerRegisterandValue method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:57:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Mar 2006 14:04:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented camera file firmware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Aug 2005 11:53:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DCAM registers
 * moved DCAM definitions out of class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   26 Oct 2004 09:10:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   allow public access to enums
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Feb 2004 16:12:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   disabled compiler warning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Feb 2004 09:32:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added DCAM_TRIGGER_DELAY
 * added GetFirmwareVersion
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Oct 2003 08:21:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Sep 2003 14:56:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added gamma device param support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Aug 2003 10:22:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GoOnline and GoOffline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Jul 2003 09:48:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include "SVImagingDeviceParams.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 10:45:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed DCAM_HIWORD and DCAM_BYTE3 ( was DCAM_BYTE4 )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:44:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
