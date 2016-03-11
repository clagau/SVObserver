//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomParameterBuilder
//* .File Name       : $Workfile:   SVCustomParameterBuilder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 11:03:58  $
//******************************************************************************
#include "stdafx.h"
#include <map>
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <comdef.h>
#include "SVCustomParameterBuilder.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"

// For Custom params
static LPCTSTR scValue = _T("Value");
static LPCTSTR scCustomID = _T("CustomID");
static LPCTSTR scDataType = _T("DataType");
static LPCTSTR scLongDataType = _T("Long");
static LPCTSTR scBoolDataType = _T("Boolean");
static LPCTSTR scStringDataType = _T("String");
static LPCTSTR scInt64DataType = _T("I64");
static LPCTSTR scComplexDataType = _T("Complex");
static LPCTSTR scUnknownDataType = _T("Unknown");

// Commomn Attribtes
static LPCTSTR scDESCRIPTION = _T("Description");
static LPCTSTR scVISUALNAME = _T("VisualName");
static LPCTSTR scORDER = _T("Order");
static LPCTSTR scDETAILLEVEL = _T("DetailLevel");

// option qualifiers
static LPCTSTR scMIN = _T("min");
static LPCTSTR scMAX = _T("max");
static LPCTSTR scOFFSET = _T("offset");
static LPCTSTR scMULTIPLIER = _T("multiplier");
static LPCTSTR scUNIT_DIVISOR = _T("unit_divisor");
static LPCTSTR scUNIT = _T("unit");

static LPCTSTR scOPTION = _T("Option_");

// defaults
static const double DefaultMultiplier = 1.0;
static const double DefaultDivisor = 1.0;
static LPCTSTR scDEFAULTUNIT = "Unit";

typedef std::map<SVString, SVDeviceParamDataTypeEnum> DeviceDataTypeEnumNameMap;
static const DeviceDataTypeEnumNameMap g_DeviceDataTypeEnumNameMap = boost::assign::map_list_of<SVString, SVDeviceParamDataTypeEnum>
(scLongDataType, DeviceDataTypeLong)
(scBoolDataType, DeviceDataTypeBool)
(scStringDataType, DeviceDataTypeString)
(scInt64DataType, DeviceDataTypei64)
(scComplexDataType, DeviceDataTypeComplex)
(scUnknownDataType, DeviceDataTypeUnknown)
;

typedef std::map<SVString, SVDeviceParamEnum> DeviceParamEnumNameMap;
static const DeviceParamEnumNameMap g_DeviceParamEnumNameMap = boost::assign::map_list_of<SVString, SVDeviceParamEnum>
(DeviceParamGigeCustom1_String, DeviceParamGigeCustom1)
(DeviceParamGigeCustom2_String, DeviceParamGigeCustom2)
(DeviceParamGigeCustom3_String, DeviceParamGigeCustom3)
(DeviceParamGigeCustom4_String, DeviceParamGigeCustom4)
(DeviceParamGigeCustom5_String, DeviceParamGigeCustom5)
(DeviceParamGigeCustom6_String, DeviceParamGigeCustom6)
(DeviceParamGigeCustom7_String, DeviceParamGigeCustom7)
(DeviceParamGigeCustom8_String, DeviceParamGigeCustom8)
(DeviceParamGigeCustom9_String, DeviceParamGigeCustom9)
(DeviceParamGigeCustom10_String, DeviceParamGigeCustom10)
(DeviceParamGigeCustom11_String, DeviceParamGigeCustom11)
(DeviceParamGigeCustom12_String, DeviceParamGigeCustom12)
(DeviceParamGigeCustom13_String, DeviceParamGigeCustom13)
(DeviceParamGigeCustom14_String, DeviceParamGigeCustom14)
(DeviceParamGigeCustom15_String, DeviceParamGigeCustom15)
(DeviceParamGigeCustom16_String, DeviceParamGigeCustom16)
(DeviceParamGigeCustom17_String, DeviceParamGigeCustom17)
(DeviceParamGigeCustom18_String, DeviceParamGigeCustom18)
(DeviceParamGigeCustom19_String, DeviceParamGigeCustom19)
(DeviceParamGigeCustom20_String, DeviceParamGigeCustom20)
(DeviceParamGigeCustom21_String, DeviceParamGigeCustom21)
(DeviceParamGigeCustom22_String, DeviceParamGigeCustom22)
(DeviceParamGigeCustom23_String, DeviceParamGigeCustom23)
(DeviceParamGigeCustom24_String, DeviceParamGigeCustom24)
(DeviceParamGigeCustom25_String, DeviceParamGigeCustom25)
(DeviceParamGigeCustom26_String, DeviceParamGigeCustom26)
(DeviceParamGigeCustom27_String, DeviceParamGigeCustom27)
(DeviceParamGigeCustom28_String, DeviceParamGigeCustom28)
(DeviceParamGigeCustom29_String, DeviceParamGigeCustom29)
(DeviceParamGigeCustom30_String, DeviceParamGigeCustom30)
;

SVDeviceParamEnum GetEnumFromCustomID(const SVString& tag)
{
	SVDeviceParamEnum paramEnum(DeviceParamInvalid);
	DeviceParamEnumNameMap::const_iterator it = g_DeviceParamEnumNameMap.find(tag);
	if (it != g_DeviceParamEnumNameMap.end())
	{
		paramEnum = it->second;
	}
	return paramEnum;
}

SVDeviceParamDataTypeEnum GetDataTypeEnumFromString(const SVString& tag)
{
	SVDeviceParamDataTypeEnum dataTypeEnum(DeviceDataTypeUnknown);
	DeviceDataTypeEnumNameMap::const_iterator it = g_DeviceDataTypeEnumNameMap.find(tag);
	if (it != g_DeviceDataTypeEnumNameMap.end())
	{
		dataTypeEnum = it->second;
	}
	return dataTypeEnum;
}

SVCustomDeviceParam* SVCustomParameterBuilder::BuildCustomDeviceParam( const SVMaterialsTree::SVTreeContainer& rOptions )
{
	SVCustomDeviceParam* pCustomDeviceParam = nullptr;
	
	SVMaterialsTree::ElementData MaterialData;
	_variant_t customID;

	// Get Custom Id
	if( S_OK == SVMaterialsTree::getData( rOptions, scCustomID, MaterialData ) )
	{
		customID = MaterialData;
		SVDeviceParamEnum paramEnum = GetEnumFromCustomID(SvUl_SF::createSVString(customID.bstrVal));
		_variant_t dataType;
		// Get Data Type
		MaterialData.clear();
		if( S_OK == SVMaterialsTree::getData( rOptions, scDataType, MaterialData ) )
		{
			dataType = MaterialData;
			
			SVDeviceParamDataTypeEnum dataTypeEnum = GetDataTypeEnumFromString(SvUl_SF::createSVString(dataType.bstrVal));
			switch (dataTypeEnum)
			{
				case DeviceDataTypeLong:
				{
					SVLongValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl_SF::createSVString(customID));
					SVCustomParameterBuilder::BuildLongParam(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypei64:
				{
					SVi64ValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl_SF::createSVString(customID));
					SVCustomParameterBuilder::BuildInt64Param(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypeBool:
				{
					SVBoolValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl_SF::createSVString(customID));
					SVCustomParameterBuilder::BuildBoolParam(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypeString:
				{
					SVStringValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl_SF::createSVString(customID));
					SVCustomParameterBuilder::BuildStringParam(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypeComplex: // Not Supported
				case DeviceDataTypeUnknown: // Not Supported
					break;
			}
		}
	}
	return pCustomDeviceParam;
}

void SVCustomParameterBuilder::BuildOptions( SVDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions )
{
	SVString Value;
	SVString Description;

	for (SVMaterialsTree::const_iterator it = rOptions.begin();it!= rOptions.end();++it)
	{
		//If this is a branch generate parameters
		if( it->second.empty() )
		{
			const SVMaterialsTree::SVTreeContainer& rTree( *it.node() );
			SVMaterialsTree::ElementData MaterialData;

			// Get Value
			if( S_OK == SVMaterialsTree::getData( rTree, scValue, MaterialData ) )
			{
				Value = SvUl_SF::createSVString(static_cast<const _variant_t> (MaterialData));
			}
			// Get Description
			MaterialData.clear();
			if( S_OK == SVMaterialsTree::getData( rTree, scDESCRIPTION, MaterialData ) )
			{
				Description = SvUl_SF::createSVString(static_cast<const _variant_t> (MaterialData));
			}

			switch (pParam->DataType())
			{
				case DeviceDataTypeLong:
				{
					int iVal = atoi(Value.c_str());
					SVLongValueDeviceParam::OptionType option(iVal, Description.c_str());
					SVLongValueDeviceParam* pLongDeviceParam = dynamic_cast<SVLongValueDeviceParam *>(pParam);
					pLongDeviceParam->info.options.push_back(option);
				}
				break;

				case DeviceDataTypeBool:
				{
					int iVal = atoi(Value.c_str());
					SVBoolValueDeviceParam::OptionType option(iVal != 0, Description.c_str());
					SVBoolValueDeviceParam* pBoolDeviceParam = dynamic_cast<SVBoolValueDeviceParam *>(pParam);
					pBoolDeviceParam->info.options.push_back(option);
				}
				break;
				
				case DeviceDataTypeString:
				{
					SVStringValueDeviceParam::OptionType option(Value.c_str(), Description.c_str());
					SVStringValueDeviceParam* pStringDeviceParam = dynamic_cast<SVStringValueDeviceParam *>(pParam);
					pStringDeviceParam->info.options.push_back(option);
				}
				break;
			}
		}
	}
}

void SVCustomParameterBuilder::BuildCommonAttributes( SVDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions )
{
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	// Get Visual name
	if( S_OK == SVMaterialsTree::getData( rOptions, scVISUALNAME, MaterialData ) )
	{
		Value = MaterialData;
		pParam->SetVisualName(SvUl_SF::createSVString(Value));
	}
	// Get Description
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scDESCRIPTION, MaterialData ) )
	{
		Value = MaterialData;
		pParam->SetDescription( SvUl_SF::createSVString(Value) );
	}
	// Get Order
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scORDER, MaterialData ) )
	{
		Value = MaterialData;
		int order = atoi( SvUl_SF::createSVString(Value).c_str());
		pParam->SetOrder(order);
	}
	// Get Detail Level
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scDETAILLEVEL, MaterialData ) )
	{
		Value = MaterialData;
		long level = atol( SvUl_SF::createSVString(Value).c_str());
		pParam->SetDetailLevel(level);
	}
}

void SVCustomParameterBuilder::BuildLongParam(SVLongValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);
	// now load options
	SVCustomParameterBuilder::BuildOptions(pParam, rOptions);
	
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	// Get Min
	if( S_OK == SVMaterialsTree::getData( rOptions, scMIN, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.min = atol( SvUl_SF::createSVString(Value).c_str() );
	}
	// Get Max
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scMAX, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.max = atol( SvUl_SF::createSVString(Value).c_str() );
	}
	// Get Offset (optional ?)
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scOFFSET, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.offset = atol( SvUl_SF::createSVString(Value).c_str() );
	}
	// Get Multiplier
	pParam->info.multiplier = DefaultMultiplier;
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scMULTIPLIER, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.multiplier = atof( SvUl_SF::createSVString(Value).c_str() );
		if (pParam->info.multiplier == 0) // don't you mean < epsilon ?
		{
			pParam->info.multiplier = DefaultMultiplier;
		}
	}
	// Get Divisor
	pParam->info.unit_divisor = DefaultDivisor;
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scUNIT_DIVISOR, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.unit_divisor = atof( SvUl_SF::createSVString(Value).c_str() );
		if (pParam->info.unit_divisor == 0) // don't you mean < epsilon ?
		{
			pParam->info.unit_divisor = 1.0;
		}
	}
	// Get Value
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, scValue, MaterialData ) )
	{
		Value = MaterialData;
		pParam->lValue = atol( SvUl_SF::createSVString(Value).c_str() );
	}
}

void SVCustomParameterBuilder::BuildInt64Param(SVi64ValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;

	if( S_OK == SVMaterialsTree::getData( rOptions, scValue, MaterialData ) )
	{
		Value = MaterialData;
		try
		{
			Value.ChangeType( VT_I8 );
			pParam->SetValue( Value );
		}
		catch(_com_error& e)
		{
			TRACE1("%s\n", e.ErrorMessage());
		}
	}
}

void SVCustomParameterBuilder::BuildStringParam(SVStringValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);
	SVCustomParameterBuilder::BuildOptions(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	if( S_OK == SVMaterialsTree::getData( rOptions, scValue, MaterialData ) )
	{
		Value = MaterialData;
		pParam->strValue = SvUl_SF::createSVString(Value).c_str();
	}
}

void SVCustomParameterBuilder::BuildBoolParam(SVBoolValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);
	SVCustomParameterBuilder::BuildOptions(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	if( S_OK == SVMaterialsTree::getData( rOptions, scValue, MaterialData ) )
	{
		Value = MaterialData;
		long lVal = atol(SvUl_SF::createSVString(Value).c_str());
		pParam->bValue = lVal != 0;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVCustomParameterBuilder.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 11:03:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 15:54:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct memory leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:41:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Apr 2013 13:58:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
