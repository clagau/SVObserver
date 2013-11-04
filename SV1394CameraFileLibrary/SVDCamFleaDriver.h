//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamFleaDriver
//* .File Name       : $Workfile:   SVDCamFleaDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:42  $
//******************************************************************************

#ifndef SVDCAMFLEADRIVER_H
#define SVDCAMFLEADRIVER_H

#include "SVDCamStandardDriver.h"

class SVDCamFleaDriver : public SVDCamStandardDriver
{
public:
	SVDCamFleaDriver();
	virtual ~SVDCamFleaDriver();

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue);

	static SVDCamDriver* CreateNew();

protected:
	enum SVFleaCameraRegistersEnum
	{
		Flea_GPIO_CONTROL    = 0x1100,
		Flea_GPIO_XTRA       = 0x1104,
		Flea_SHUTTER_DELAY   = 0x1108,
		Flea_GPIO_CTRL_PIN_0 = 0x1110,
		Flea_GPIO_XTRA_PIN_0 = 0x1114,
		Flea_GPIO_CTRL_PIN_1 = 0x1120,
		Flea_GPIO_XTRA_PIN_1 = 0x1124,
		Flea_GPIO_CTRL_PIN_2 = 0x1130,
		Flea_GPIO_XTRA_PIN_2 = 0x1134,
		Flea_LUT_INFO         = 0x1A40,
		Flea_LUT_OFFSET       = 0x1A44,
		Flea_ABS_CSR_HI_INQ_7 = 0x071C,
	};

	enum SVFleaCameraRegisterValuesEnum
	{
		Flea_GPIO_XTRA_StrobeStart_Bit  = DCAMBIT0,
		Flea_GPIO_XTRA_TriggerQueue_Bit = DCAMBIT1,
		Flea_GPIO_XTRA_StrobeMultiplier_Bits = DCAM_BYTE3,
		Flea_SHUTTER_DELAY_ShutterDelay_Bits = DCAM_HIWORD,
		Flea_GPIO_Pin_Mode_Bits = DCAMBITS_12_15,
		Flea_GPIO_Pin_Mode_Input = 0,
		Flea_GPIO_Pin_Mode_Output = 1,
		Flea_GPIO_Pin_Mode_AsyncTrigger = 2,
		Flea_GPIO_Pin_Mode_Strobe = 3,
		Flea_TRIGGER_DELAY_ShutterDelay_Bits = DCAMBIT20 | DCAMBIT21 | DCAMBIT22 | DCAMBIT23 | DCAM_BYTE3,
	};

	enum SVCSRRegisterOffsets
	{
		CSR_VALUE_OFFSET = 8,
	};

	class Firmware
	{
	public:
		Firmware( const CString& strFirmwareString );

		bool operator < ( const Firmware& rhs ) const;
		bool operator == ( const Firmware& rhs ) const;
		bool operator > ( const Firmware& rhs ) const { return ! operator < (rhs) && ! operator == (rhs); }
		bool operator != ( const Firmware& rhs ) const { return ! operator == ( rhs ); }
		bool operator <= ( const Firmware& rhs ) const { return operator < ( rhs ) || operator == ( rhs ); }
		bool operator >= ( const Firmware& rhs ) const { return ! operator < ( rhs ); }

	private:
		struct FirmwareStruct	// based on http://ptgrey.com/support/kb/index.asp?a=4&q=96
		{
			enum TypeOfReleaseEnum; // forward declare

			int               MajorRevision;
			int               MinorRevision;
			TypeOfReleaseEnum TypeOfRelease;
			int               BuildNumber;

			enum TypeOfReleaseEnum
			{
				Alpha            = 0,
				Beta             = 1,
				ReleaseCandidate = 2,
				Release          = 3,
			};

			FirmwareStruct() : MajorRevision(0), MinorRevision(0), TypeOfRelease(Alpha), BuildNumber(0) {}
			FirmwareStruct( int p_MajorRevision, int p_MinorRevision, TypeOfReleaseEnum p_TypeOfRelease, int p_BuildNumber )
				: MajorRevision( p_MajorRevision ), MinorRevision( p_MinorRevision ), TypeOfRelease( p_TypeOfRelease ), BuildNumber( p_BuildNumber ) {}

		};	// end struct FirmwareStruct
		FirmwareStruct m_Firmware;
	};	// end class Firmware

	union FloatLongConvert
	{
		ULONG ulValue;
		float fValue;
	};

	ULONG m_ulGPIO_XTRA;
	ULONG m_ulABS_CSR_SHUTTER_REGISTER_BASE;

	HRESULT NeutralLutToRawLut( const SVLut& rLut, std::vector<ULONG>& raData );

	virtual unsigned long GetDCAMTriggerPolarity() const;

};



#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamFleaDriver.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Aug 2010 10:34:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Nov 2009 18:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated drivers to fix the software trigger functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Oct 2009 19:19:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Oct 2009 10:42:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSoftwareTriggerRegisterandValue method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2007 10:12:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:56:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Mar 2006 14:07:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented firmware check, Absolute Value Shutter, bulb mode fix
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Mar 2006 14:09:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added BulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Feb 2004 09:30:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   implemented chnages for Flea
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Feb 2004 15:57:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   implemented LUT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2004 07:53:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for StrobePulseDelay / StrobePulseDuration multiplier
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Oct 2003 08:21:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Sep 2003 09:01:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Aug 2003 10:21:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed StrobePulse from Bool to Long
 * added enum for bits
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
