// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParams
// * .File Name       : $Workfile:   SVDeviceParams.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   15 May 2013 19:42:28  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDeviceParams.h"
#include "SVNullDeviceParam.h"
#include "SVBoolValueDeviceParam.h"
#include "SVi64ValueDeviceParam.h"
#include "SVLongValueDeviceParam.h"
#include "SVStringValueDeviceParam.h"
#include "SVCustomDeviceParam.h"
#include "SVParamListDeviceParam.h"

namespace	// create unnamed namespace for the variables to live in
{
	REGISTER_DEVICE_PARAM( DeviceParamInvalid,             DeviceParamInvalid_String,                 SVNullDeviceParam );
	const bool bRegisterDefault = TheDeviceParamFactory::Instance().SetDefault( DeviceParamInvalid );

	REGISTER_DEVICE_PARAM( DeviceParamDeviceDigNum,        DeviceParamDeviceDigNum_String,            SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamDeviceName,          DeviceParamDeviceName_String,              SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamDeviceChannelName,   DeviceParamDeviceChannelName_String,       SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVendorId,            DeviceParamVendorId_String,                SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVendorName,          DeviceParamVendorName_String,              SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamModelName,           DeviceParamModelName_String,               SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFirmware,            DeviceParamFirmware_String,                SVStringValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamSerialNumber,        DeviceParamSerialNumber_String,            SVi64ValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSerialNumberString,  DeviceParamSerialNumberString_String,      SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamShutter,             DeviceParamShutter_String,                 SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGamma,               DeviceParamGamma_String,                   SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamBrightness,          DeviceParamBrightness_String,              SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGain,                DeviceParamGain_String,                    SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamWhiteBalanceUB,      DeviceParamWhiteBalanceUB_String,          SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamWhiteBalanceVR,      DeviceParamWhiteBalanceVR_String,          SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSharpness,           DeviceParamSharpness_String,               SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamHue,                 DeviceParamHue_String,                     SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamSaturation,          DeviceParamSaturation_String,              SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamRegisters,           DeviceParamRegisters_String,               SVParamListDeviceParam );
	//REGISTER_DEVICE_PARAM( DeviceParamWidth,               DeviceParamWidth_String,                   SVLongValueDeviceParam );	// obsolete
	//REGISTER_DEVICE_PARAM( DeviceParamHeight,              DeviceParamHeight_String,                  SVLongValueDeviceParam );	// obsolete
	REGISTER_DEVICE_PARAM( DeviceParamSizeBands,           DeviceParamSizeBands_String,               SVLongValueDeviceParam );
	//REGISTER_DEVICE_PARAM( DeviceParamNumFormats,          DeviceParamNumFormats_String,              SVLongValueDeviceParam );	// obsolete
	REGISTER_DEVICE_PARAM( DeviceParamFormatStrings,       DeviceParamFormatStrings_String,           SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFormat7Support,      DeviceParamFormat7Support_String,          SVBoolValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerEdge,         DeviceParamTriggerEdge_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamOutputStrobePulse,   DeviceParamOutputStrobePulse_String,       SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseDelay,    DeviceParamStrobePulseDelay_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseDuration, DeviceParamStrobePulseDuration_String,     SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseMultiplier, DeviceParamStrobePulseMultiplier_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraTriggerPin,    DeviceParamCameraTriggerPin_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraStrobePin,     DeviceParamCameraStrobePin_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamExposure,            DeviceParamExposure_String,                SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerMode,         DeviceParamTriggerMode_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTestPattern,         DeviceParamTestPattern_String,             SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamShutterDelay,        DeviceParamShutterDelay_String,            SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobeStartMode,     DeviceParamStrobeStartMode_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamTriggerQueueMode,    DeviceParamTriggerQueueMode_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamStrobePulseAdvance,  DeviceParamStrobePulseAdvance_String,      SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamNumCameraBuffers,    DeviceParamNumCameraBuffers_String,        SVLongValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister1,     DeviceParamCameraRegister1_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister2,     DeviceParamCameraRegister2_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister3,     DeviceParamCameraRegister3_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister4,     DeviceParamCameraRegister4_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister5,     DeviceParamCameraRegister5_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister6,     DeviceParamCameraRegister6_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister7,     DeviceParamCameraRegister7_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister8,     DeviceParamCameraRegister8_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister9,     DeviceParamCameraRegister9_String,         SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister10,    DeviceParamCameraRegister10_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister11,    DeviceParamCameraRegister11_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister12,    DeviceParamCameraRegister12_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister13,    DeviceParamCameraRegister13_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister14,    DeviceParamCameraRegister14_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister15,    DeviceParamCameraRegister15_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister16,    DeviceParamCameraRegister16_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister17,    DeviceParamCameraRegister17_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister18,    DeviceParamCameraRegister18_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister19,    DeviceParamCameraRegister19_String,        SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraRegister20,    DeviceParamCameraRegister20_String,        SVLongValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom1,	DeviceParamGigeCustom1_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom2,	DeviceParamGigeCustom2_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom3,	DeviceParamGigeCustom3_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom4,	DeviceParamGigeCustom4_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom5,	DeviceParamGigeCustom5_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom6,	DeviceParamGigeCustom6_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom7,	DeviceParamGigeCustom7_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom8,	DeviceParamGigeCustom8_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom9,	DeviceParamGigeCustom9_String,  SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom10,	DeviceParamGigeCustom10_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom11,	DeviceParamGigeCustom11_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom12,	DeviceParamGigeCustom12_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom13,	DeviceParamGigeCustom13_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom14,	DeviceParamGigeCustom14_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom15,	DeviceParamGigeCustom15_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom16,	DeviceParamGigeCustom16_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom17,	DeviceParamGigeCustom17_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom18,	DeviceParamGigeCustom18_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom19,	DeviceParamGigeCustom19_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom20,	DeviceParamGigeCustom20_String,	SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom21,	DeviceParamGigeCustom21_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom22,	DeviceParamGigeCustom22_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom23,	DeviceParamGigeCustom23_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom24,	DeviceParamGigeCustom24_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom25,	DeviceParamGigeCustom25_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom26,	DeviceParamGigeCustom26_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom27,	DeviceParamGigeCustom27_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom28,	DeviceParamGigeCustom28_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom29,	DeviceParamGigeCustom29_String, SVCustomDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeCustom30,	DeviceParamGigeCustom30_String,	SVCustomDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerSelection, DeviceParamAcquisitionTriggerSelection_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerType,      DeviceParamAcquisitionTriggerType_String,      SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerName,      DeviceParamAcquisitionTriggerName_String,      SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionTriggerEdge,      DeviceParamAcquisitionTriggerEdge_String,      SVBoolValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamAcquisitionStrobeEdge,       DeviceParamAcquisitionStrobeEdge_String,       SVBoolValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamIOStrobeInvert,              DeviceParamIOStrobeInvert_String,              SVBoolValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamIOTriggerInvert,             DeviceParamIOTriggerInvert_String,             SVBoolValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamCameraTriggerMode, DeviceParamCameraTriggerMode_String, SVLongValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamCameraInput, DeviceParamCameraInput_String, SVStringValueDeviceParam );

	// File Acquisition parameters
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageFileName, DeviceParamFileAcqImageFileName_String, SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageDirName, DeviceParamFileAcqImageDirName_String, SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqLoadingMode, DeviceParamFileAcqLoadingMode_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageWidth, DeviceParamFileAcqImageWidth_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageHeight, DeviceParamFileAcqImageHeight_String, SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamFileAcqImageFormat, DeviceParamFileAcqImageFormat_String, SVLongValueDeviceParam );

	// GIGE Acquisition parameters
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerSource,	DeviceParamAcquisitionTriggerType_String,	SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerEdge,		DeviceParamTriggerEdge_String,				SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerLine,		DeviceParamGigeTriggerLine_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeTriggerEnable,	DeviceParamGigeTriggerEnable_String,		SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeSource,		DeviceParamGigeStrobeSource_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeEdge,		DeviceParamGigeStrobeEdge_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeLine,		DeviceParamGigeStrobeLine_String,			SVStringValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamGigeStrobeEnable,		DeviceParamGigeStrobeEnable_String,			SVStringValueDeviceParam );
	
	REGISTER_DEVICE_PARAM( DeviceParamGigePacketSize,		DeviceParamGigePacketSize_String,			SVLongValueDeviceParam );

	REGISTER_DEVICE_PARAM( DeviceParamHorizontalBinning,	DeviceParamHorizontalBinning_String, 		SVLongValueDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamVerticalBinning,		DeviceParamVerticalBinning_String, 			SVLongValueDeviceParam );
}// end unnamed namespace

// these conversion routines can be moved to another file
HRESULT ToVariant( long lValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_I4;
	rv.lVal = lValue;

	return hr;
}

HRESULT ToVariant( __int64 iValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	// no real support for VT_I8 yet; stuff into a string; see also FromVariant
	//rv.vt = VT_I8;
	//rv.lVal = iValue;
	rv.vt = VT_BSTR;
	CString s;
	s.Format("%I64d", iValue);
	rv.bstrVal = s.AllocSysString();

	return hr;
}

HRESULT ToVariant( bool bValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_BOOL;
	rv.boolVal = bValue ? VARIANT_TRUE : VARIANT_FALSE;

	return hr;
}

HRESULT ToVariant( const SVString& sValue, VARIANT& rv )
{
	HRESULT hr = S_OK;

	::VariantClear( &rv );

	rv.vt = VT_BSTR;
	rv.bstrVal = _bstr_t(sValue.c_str()).Detach();

	return hr;
}

HRESULT FromVariant( long& rlValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_I4:
			rlValue = rv.lVal;
			break;
		case VT_UI4:
			rlValue = (long) rv.ulVal;
			break;
		case VT_I2:
			rlValue = (long) rv.iVal;
			break;
		case VT_UI2:
			rlValue = (long) rv.uiVal;
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( __int64& riValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		// no real support for VT_I8 yet; we'll use strings for now; see also ToVariant
		case VT_I8:
			hr = S_FALSE;
			break;
		case VT_UI8:
			hr = S_FALSE;
			break;
		case VT_BSTR:
			{
				CString s = rv.bstrVal;
				if ( s.GetLength() > 0 )
				{
					sscanf((LPCTSTR) s, "%I64d", &riValue);
				}
				else
					hr = S_FALSE;
			}
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( bool& rbValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_BOOL:
			rbValue = rv.boolVal != 0;
		case VT_I4:
			rbValue = rv.lVal != 0;
			break;
		case VT_UI4:
			rbValue = rv.ulVal != 0;
			break;
		case VT_I2:
			rbValue = rv.iVal != 0;
			break;
		case VT_UI2:
			rbValue = rv.uiVal != 0;
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( SVString& rsValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_BSTR:
			{
			CString s( rv.bstrVal );
			rsValue = s;
			}
			break;
		default:
			hr = S_FALSE;
	}

	return hr;
}

HRESULT FromVariant( CString& rsValue, const VARIANT& rv )
{
	HRESULT hr = S_OK;

	switch ( rv.vt )
	{
		case VT_BSTR:
			rsValue = rv.bstrVal;
			break;
		default:
			hr = S_FALSE;
	}
	return hr;
}

SVDeviceParamStructTestCases::SVDeviceParamStructTestCases(SVDeviceParamCollection& rDevice)
{
	// test destruction
	{
		SVDeviceParamCollection collection;
		collection.SetParameter( DeviceParamBrightness, SVLongValueDeviceParam( 5 ) );
		collection.SetParameter( DeviceParamGain, DeviceParam( 3 ) );
		collection.SetParameter( DeviceParamShutter, DeviceParam( 2 ) );

		SVDeviceParamCollection collection2;
		collection2 = collection;

		SVDeviceParamCollection collection3(collection);

		collection3 = collection2;
	}
	int abc=0;
	abc = 5;

	/*
	SVLutDeviceParam* pParam1 = DerivedValue< SVLutDeviceParam >( rDevice.GetParameter( DeviceParamLut ) );
	if (pParam1)
	{
		SVLut lut1 = pParam1->lut;
	}

	SVLutDeviceParam* pParam2 = rDevice.GetParameter( DeviceParamLut ).DerivedValue( pParam2 );
	if (pParam2)
	{
		SVLut lut2 = pParam2->lut;
	}

	SVLutDeviceParam* pParam3 = NULL;
	HRESULT hr = rDevice.GetParameter( DeviceParamLut ).GetDerivedValue( pParam3 );
	SVLut lut3;
	if ( pParam3 != NULL && hr == S_OK )
	{
		lut3 = pParam3->lut;
	}
	else
	{
		// type mismatch
	}
	

	HRESULT hrTemp = S_OK;
	//SVDeviceParamMap::iterator iter = rDevice.mapParameters.find( DeviceParamLut );
	//if (iter != rDevice.mapParameters.end())
	if ( rDevice.mapParameters.find( DeviceParamLut ) != rDevice.mapParameters.end() )
	{
		SVLutDeviceParam* pParam4 = NULL;
		hrTemp = rDevice.GetParameter( DeviceParamLut ).GetDerivedValue( pParam4 );
		SVLut lut4;
		if ( pParam4 != NULL && hrTemp == S_OK )
		{
			lut4 = pParam4->lut;
		}
		else
		{
			// type mismatch
		}
	}
	else
	{
		hrTemp = ERROR_NOT_FOUND;	// parameter not found
	}

	SVLutDeviceParam* pParam5 = NULL;
	hrTemp = rDevice.GetParameter( DeviceParamLut, pParam5 );
	if ( hrTemp == S_OK )
	{
	}



	SVLutDeviceParam* pParam1b = NULL;
	pParam1b = DerivedValue< SVLutDeviceParam >( rDevice.mapParameters[ DeviceParamLut ] );
	if (pParam1b)
	{
		SVLut lut1b = pParam1b->lut;
	}
	SVLutDeviceParam* pParam2b = NULL;
	pParam2b = rDevice.mapParameters[ DeviceParamLut ].DerivedValue( pParam2b );
	if ( pParam2b )
	{
		SVLut lut2b = pParam2b->lut;
	}
	*/

	// Set

	{// begin scope
		SVLongValueDeviceParam param;
		param.lValue = 10;
		SVLongValueDeviceParam::OptionType option;
		option.value = 5;
		option.strDescription = "hello";
		option.strDescription = SVString("hello");
		param.info.options.push_back(option);
		rDevice.SetParameter( DeviceParamBrightness, param );

		SVLongValueDeviceParam* pParam = NULL;
		HRESULT hr = rDevice.GetParameter( DeviceParamBrightness, pParam );
		if ( pParam )
		{
			bool bOk;
			bOk = pParam->lValue == 10;
			ASSERT( bOk );
			SVLongValueDeviceParam::OptionType option(123, "OneTwoThree");
			pParam->info.options.push_back(option);
			
			rDevice.GetParameter( DeviceParamBrightness ).DerivedValue(pParam)->lValue = 4;


			// test the world of const
			const SVDeviceParamWrapper& w = rDevice.Parameter( DeviceParamBrightness );
			const SVLongValueDeviceParam* pConstParam = w.DerivedValue( pConstParam );
			//hr = w.GetDerivedValue( pConstParam );
			//hr = w.GetDerivedValue( pParam );	// should not compile XXXXXXXXX why does this compile???
			//pParam = w.DerivedValue( pParam );	// should not compile
			//pConstParam->lValue = 5;	// should not compile
			SVDeviceParam* pIWannaBeAClone = pConstParam->Clone();
			SVDeviceParam* pIWannaBeACloneToo = w->Clone();
			SVDeviceParamWrapper w2 = w->Clone();
			SVDeviceParamWrapper w3 = w;
			//w = w3; should not compile
			SVDeviceParamWrapper RapperClone( pIWannaBeAClone );
			pParam = RapperClone.DerivedValue( pParam );
			pParam->lValue = 6;

		}

	}// end scope

	{// begin scope
		// test indexed access
		SVDeviceParamIndexer index(rDevice.mapParameters);
		SVDeviceParamIndexer::iterator iter;
		for (iter = index.begin(); iter != index.end(); ++iter)
		{
			SVDeviceParamWrapper w = rDevice.Parameter(*iter);
			SVDeviceParamWrapper w2 ( rDevice.Parameter(*iter) );
		}
	}// end scope
}

///////////////////////////////////////////////////////////////////////////////////

// long Specialization TDeviceParamInfo Implementation ....
//template<>
TDeviceParamInfo<long>::TDeviceParamInfo() 
{
	min=0; 
	max = 0;
	vendorId = 0;
	mask = 0;
	offset = 0; 
	multiplier=1; 
	unit_divisor = 1;
	sUnits = _T("");
}

//template<>
TDeviceParamInfo<long>::TDeviceParamInfo(const TDeviceParamInfo<long>& rhs)
{
	*this=rhs;
}

//template<>
const TDeviceParamInfo<long>& TDeviceParamInfo<long>::operator= (const TDeviceParamInfo<long>& rhs)
{
	if ( this != &rhs )
	{
		vendorId = rhs.vendorId;
		mask = rhs.mask;
		options = rhs.options;
		min = rhs.min;
		max = rhs.max;
		offset = rhs.offset;
		multiplier = rhs.multiplier;
		unit_divisor = rhs.unit_divisor;
		sUnits = rhs.sUnits;
	}
	return *this;
}


// __in64 Specialization TDeviceParamInfo Implementation ....
//template<>
TDeviceParamInfo<__int64>::TDeviceParamInfo() 
{
	min=0; 
	max = 0; 
	offset = 0; 
	multiplier=1; 
	sUnits = _T("");
}

//template<>
TDeviceParamInfo<__int64>::TDeviceParamInfo(const TDeviceParamInfo<__int64>& rhs)
{
	*this=rhs;
}

//template<>
const TDeviceParamInfo<__int64>& TDeviceParamInfo<__int64>::operator= (const TDeviceParamInfo<__int64>& rhs)
{
	if ( this != &rhs )
	{
		options = rhs.options;
		min = rhs.min;
		max = rhs.max;
		offset = rhs.offset;
		multiplier = rhs.multiplier;
		sUnits = rhs.sUnits;
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////

// **********************************************
HRESULT SVDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam )
{
	HRESULT hr = E_NOTIMPL;

	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVDeviceParams.cpp_v  $
 * 
 *    Rev 1.1   15 May 2013 19:42:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   10 May 2013 10:11:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DeviceParamHorizontalBinning and DevcieParamVerticalBinning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:05:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   01 Apr 2013 13:46:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added enums and strings for the custom Gige device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   18 Jan 2013 16:02:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DeviceParamCameraInput
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   11 Oct 2012 10:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update from post incrementer to pre incrementer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   18 Feb 2011 09:49:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Jun 2009 17:32:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Gige device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   03 Jun 2009 10:04:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved the Derived SVDeviceParam classes to their own modules
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   05 Dec 2008 15:22:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading Modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   16 Sep 2008 13:11:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Device Parameters for FileAcquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   20 Jun 2007 14:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   23 Jun 2006 09:24:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  568
 * SCR Title:  Add Register/Value Parameter capability to 1394 camera files and SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraRegister parameters
 * added mask to TDeviceParamInfo<long>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   23 Jun 2006 09:24:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  568
 * SCR Title:  Add Register/Value Parameter capability to 1394 camera files and SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraRegister parameters
 * added mask to TDeviceParamInfo<long>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   28 Mar 2006 13:47:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamFirmware
 * added TDeviceParamInfo<long>::unit_divisor
 * removed DeviceParamBulbModeTriggerEdge
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   21 Mar 2006 14:04:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamBulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   21 Mar 2006 14:04:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamBulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   10 Aug 2005 13:17:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved base class code to header file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   10 Aug 2005 13:17:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved base class code to header file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   23 Jul 2004 08:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new camera parameter CameraTriggerMode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   15 Jul 2004 09:17:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  436
 * SCR Title:  Update ISG camera handler
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamNumCameraBuffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 May 2004 08:11:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new device parameters to handle the trigger edge and strobe edge data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   03 Feb 2004 17:17:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   23 Jan 2004 09:56:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added ModelName and VendorName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   21 Jan 2004 15:24:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   renamed an unused paramter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Jan 2004 07:23:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added StrobePulseMultiplier and IOTriggerInvert
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   12 Jan 2004 11:10:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SetMetaData( ..pBaseParam), GetScaledParam(, SetScaledParam( long ), GetScaledMin(, GetScaledMax(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Nov 2003 09:50:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CameraStrobePolarity to IOStrobeInvert
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   12 Nov 2003 09:24:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamVendorId
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Nov 2003 16:20:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DeviceParamDeviceChannelName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Oct 2003 08:07:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for WhiteBalance, Sharpness, Hue, and Saturation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Oct 2003 09:26:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added StrobePulseAdvance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Sep 2003 08:56:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added device parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Aug 2003 13:11:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for NullDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 2003 10:09:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed OutputStrobePulse from Bool to Long
 * added device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Aug 2003 12:32:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renumbered params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jul 2003 08:28:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Lut, Light Reference, and CameraFormats to SVImageLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2003 10:14:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed FromVariant functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 08:41:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraFormats param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
