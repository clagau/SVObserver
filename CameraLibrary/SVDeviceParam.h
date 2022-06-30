// ******************************************************************************
// * COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParam
// * .File Name       : $Workfile:   SVDeviceParam.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:02:04  $
// ******************************************************************************

#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include "SVVisitor.h"
#include "SVLibrary/SVTemplate.h"


enum SVDeviceParamDataTypeEnum
{
	DeviceDataTypeLong,
	DeviceDataTypei64,
	DeviceDataTypeBool,
	DeviceDataTypeString,
	DeviceDataTypeComplex,
	DeviceDataTypeCustom, // Holder of Long/I64/Bool/String types
	DeviceDataTypeUnknown
};

enum SVDeviceParamEnum	// do not change exitsing param numbers!
{
	DeviceParamInvalid = 0,

	// SVO camera identification params
	DeviceParamDeviceDigNum = 100,
	DeviceParamDeviceName = 101,
	DeviceParamDeviceChannelName = 102,

	DeviceParamVendorId = 103,
	DeviceParamVendorName = 104,
	DeviceParamModelName = 105,
	DeviceParamFirmware = 106,
	DeviceParamIPAddress = 107,  //new for MIL 9.0 - used for GigE cameras
	DeviceParamCameraDefaultSettings = 108,  //Place this in the front so that it is done first

	// FileAcquistion params
	DeviceParamFileAcqImageFileName = 200,
	DeviceParamFileAcqImageDirName = 201,
	DeviceParamFileAcqLoadingMode = 202,
	DeviceParamFileAcqImageWidth = 203,
	DeviceParamFileAcqImageHeight = 204,
	DeviceParamFileAcqImageFormat = 205,

	// physical camera params
	DeviceParamSerialNumber = 1000,
	DeviceParamSerialNumberString = 1001,
	DeviceParamShutter = 1002,
	DeviceParamGamma = 1003,
	DeviceParamBrightness = 1004,
	DeviceParamGain = 1005,
	DeviceParamRegisters = 1006,
	DeviceParamSizeBands = 1009,
	DeviceParamFormatStrings = 1011,
	DeviceParamFormat7Support = 1012,
	DeviceParamTriggerEdge = 1013,
	DeviceParamOutputStrobePulse = 1014,
	DeviceParamStrobePulseDelay = 1015,
	DeviceParamStrobePulseDuration = 1016,
	DeviceParamCameraTriggerPin = 1017,
	DeviceParamCameraStrobePin = 1018,
	DeviceParamExposure = 1019,
	DeviceParamTriggerMode = 1021,
	DeviceParamNumCameraQueuedTriggers = 1022,
	DeviceParamTestPattern = 1023,
	DeviceParamShutterDelay = 1024,
	DeviceParamStrobeStartMode = 1025,
	DeviceParamTriggerQueueMode = 1026,
	DeviceParamStrobePulseAdvance = 1027,
	DeviceParamWhiteBalanceUB = 1028,
	DeviceParamWhiteBalanceVR = 1029,
	DeviceParamSharpness = 1030,
	DeviceParamHue = 1031,
	DeviceParamSaturation = 1032,
	DeviceParamStrobePulseMultiplier = 1033,
	DeviceParamNumCameraBuffers = 1034,

	DeviceParamHorizontalBinning = 1100, // must be less than DeviceParamCameraFormats (so it gets set before)
	DeviceParamVerticalBinning = 1101, // must be less than DeviceParamCameraFormats (so it gets set before)

	DeviceParamGigeCustom1 = 3050,
	DeviceParamGigeCustom2 = 3051,
	DeviceParamGigeCustom3 = 3052,
	DeviceParamGigeCustom4 = 3053,
	DeviceParamGigeCustom5 = 3054,
	DeviceParamGigeCustom6 = 3055,
	DeviceParamGigeCustom7 = 3056,
	DeviceParamGigeCustom8 = 3057,
	DeviceParamGigeCustom9 = 3058,
	DeviceParamGigeCustom10 = 3059,
	DeviceParamGigeCustom11 = 3060,
	DeviceParamGigeCustom12 = 3061,
	DeviceParamGigeCustom13 = 3062,
	DeviceParamGigeCustom14 = 3063,
	DeviceParamGigeCustom15 = 3064,
	DeviceParamGigeCustom16 = 3065,
	DeviceParamGigeCustom17 = 3066,
	DeviceParamGigeCustom18 = 3067,
	DeviceParamGigeCustom19 = 3068,
	DeviceParamGigeCustom20 = 3069,
	DeviceParamGigeCustom21 = 3070,
	DeviceParamGigeCustom22 = 3071,
	DeviceParamGigeCustom23 = 3072,
	DeviceParamGigeCustom24 = 3073,
	DeviceParamGigeCustom25 = 3074,
	DeviceParamGigeCustom26 = 3075,
	DeviceParamGigeCustom27 = 3076,
	DeviceParamGigeCustom28 = 3077,
	DeviceParamGigeCustom29 = 3078,
	DeviceParamGigeCustom30 = 3079,

	// Imaging parameters
	DeviceParamCameraFormats = 4000,
	DeviceParamLightReference = 4001,
	DeviceParamLut = 4002,

	// Acquisition I/O parameters
	DeviceParamAcquisitionTriggerSelection = 5000,	// ?? not used
	DeviceParamAcquisitionTriggerType = 5001,	// external, software, software grab
	DeviceParamAcquisitionTriggerName = 5002,	// NOT USED YET; will when connecting triggers with acquisition devices

	DeviceParamAcquisitionTriggerEdge = 5003,  // Internal Parameters Only - Rising or Falling
	DeviceParamAcquisitionStrobeEdge = 5013,  // Internal Parameters Only - Rising or Falling

	DeviceParamCameraInput = 5050,

	DeviceParamIOStrobeInvert = 5100,
	DeviceParamIOTriggerInvert = 5101,

	DeviceParamCameraTriggerMode = 5200,

	// GIGE Acquisition parameters
	DeviceParamGigeTriggerSource = 6000,
	DeviceParamGigeTriggerEdge = 6001,
	DeviceParamGigeTriggerLine = 6002,
	DeviceParamGigeTriggerEnable = 6003,

	DeviceParamGigeStrobeSource = 6050,
	DeviceParamGigeStrobeEdge = 6051,
	DeviceParamGigeStrobeLine = 6052,
	DeviceParamGigeStrobeEnable = 6053,

	DeviceParamGigePacketSize = 6100,

	// be sure to register the create function in SVDeviceParams.cpp if you add a parameter
};

class SVDeviceParam : public SVClonable, public SvCam::BaseVisitable<HRESULT>
{
public:
	SVDeviceParam();
	explicit SVDeviceParam( SVDeviceParamEnum e );
	virtual ~SVDeviceParam();
	static SVDeviceParam* Create(SVDeviceParamEnum eType);
	static std::string GetParameterName( SVDeviceParamEnum e );
	SVDeviceParam* Clone() const {return static_cast< SVDeviceParam* >( CloneImpl() );}
	virtual SVClonable* CloneImpl() const override = 0;

	virtual HRESULT GetValue( VARIANT& rv ) const;
	virtual HRESULT SetValue( const VARIANT& rv );
	virtual HRESULT SetMetadata(const SVDeviceParam* pBaseParam );

	LPCTSTR Name() const { return m_strName.c_str(); };
	HRESULT SetName( const std::string& strName);

	LPCTSTR VisualName() const { return m_strVisualName.c_str(); };
	HRESULT SetVisualName( const std::string& strName);

	LPCTSTR Description() const { return m_strDescription.c_str(); };
	HRESULT SetDescription( const std::string& strDescription );

	long DetailLevel() const { return m_lDetailLevel; };
	HRESULT SetDetailLevel( long lDetailLevel );

	SVDeviceParamEnum Type() const { return m_eParam; };
	HRESULT SetType( SVDeviceParamEnum e );
	SVDeviceParamDataTypeEnum DataType() const { return m_eDataType; };

	int GetOrder() const { return m_iOrder; };
	void SetOrder(int iOrder) { m_iOrder = iOrder; };

	bool DeviceSupports() const { return m_bSupports; };
	bool Supports() const { return m_bSupports; };
	void Supports(bool bSupports) { m_bSupports = bSupports; };

	DEFINE_VISITABLE()

protected:
	SVDeviceParamEnum m_eParam;
	SVDeviceParamDataTypeEnum m_eDataType;
	std::string m_strName;
	std::string m_strVisualName;
	std::string m_strDescription;
	int m_iOrder;
	long m_lDetailLevel;
	bool m_bSupports;
	
// disable assignment and copy constructor
protected:
	SVDeviceParam( const SVDeviceParam& rRhs );

private:
	const SVDeviceParam& operator = (const SVDeviceParam& rRhs );
};

typedef TValueSemantics<SVDeviceParam> SVDeviceParamWrapper;
typedef std::map <SVDeviceParamEnum, SVDeviceParamWrapper> SVDeviceParamMap;


