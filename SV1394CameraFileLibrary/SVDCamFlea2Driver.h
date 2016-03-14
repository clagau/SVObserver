//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamFlea2Driver
//* .File Name       : $Workfile:   SVDCamFlea2Driver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:40  $
//******************************************************************************

#ifndef SVDCAMFLEA2DRIVER_H
#define SVDCAMFLEA2DRIVER_H

#include "SVDCamStandardDriver.h"

class SVDCamFlea2Driver : public SVDCamStandardDriver
{
public:
	SVDCamFlea2Driver();
	virtual ~SVDCamFlea2Driver();

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

	typedef std::vector< unsigned long > SVLUTVector;

	union SVLUTRegisterUnion
	{
		struct SVLUTRegister
		{
			unsigned long l_Entries : 16;
			unsigned long l_BitDepth : 8;
			unsigned long l_Reserved2 : 1;
			unsigned long l_OnOff : 1;
			unsigned long l_Channels : 3;
			unsigned long l_Reserved1 : 2;
			unsigned long l_Presence : 1;
		} m_Bits;
		unsigned long m_Value;
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

	HRESULT NeutralLutToRawLut( const SVLut& rLut, SVLUTVector& raData );

	virtual unsigned long GetDCAMTriggerPolarity() const;

};



#endif

