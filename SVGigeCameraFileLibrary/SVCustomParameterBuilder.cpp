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
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVCustomParameterBuilder.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "SVUtilityLibrary/StringHelper.h"

// For Custom params
static const TCHAR* const cValue = _T("Value");
static const TCHAR* const cCustomID = _T("CustomID");
static const TCHAR* const cDataType = _T("DataType");
static const TCHAR* const cLongDataType = _T("Long");
static const TCHAR* const cBoolDataType = _T("Boolean");
static const TCHAR* const cStringDataType = _T("String");
static const TCHAR* const cInt64DataType = _T("I64");
static const TCHAR* const cComplexDataType = _T("Complex");
static const TCHAR* const cUnknownDataType = _T("Unknown");

// Commomn Attribtes
static const TCHAR* const cDESCRIPTION = _T("Description");
static const TCHAR* const cVISUALNAME = _T("VisualName");
static const TCHAR* const cORDER = _T("Order");
static const TCHAR* const cDETAILLEVEL = _T("DetailLevel");

// option qualifiers
static const TCHAR* const cMIN = _T("min");
static const TCHAR* const cMAX = _T("max");
static const TCHAR* const cOFFSET = _T("offset");
static const TCHAR* const cMULTIPLIER = _T("multiplier");
static const TCHAR* const cUNIT_DIVISOR = _T("unit_divisor");
static const TCHAR* const cUNIT = _T("unit");

static const TCHAR* const cOPTION = _T("Option_");

// defaults
static const double DefaultMultiplier = 1.0;
static const double DefaultDivisor = 1.0;
static const TCHAR* const cDEFAULTUNIT = _T("Unit");

typedef std::map<std::string, SVDeviceParamDataTypeEnum> DeviceDataTypeEnumNameMap;
static const DeviceDataTypeEnumNameMap g_DeviceDataTypeEnumNameMap = boost::assign::map_list_of<std::string, SVDeviceParamDataTypeEnum>
(cLongDataType, DeviceDataTypeLong)
(cBoolDataType, DeviceDataTypeBool)
(cStringDataType, DeviceDataTypeString)
(cInt64DataType, DeviceDataTypei64)
(cComplexDataType, DeviceDataTypeComplex)
(cUnknownDataType, DeviceDataTypeUnknown)
;

typedef std::map<std::string, SVDeviceParamEnum> DeviceParamEnumNameMap;
static const DeviceParamEnumNameMap g_DeviceParamEnumNameMap = boost::assign::map_list_of<std::string, SVDeviceParamEnum>
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

SVDeviceParamEnum GetEnumFromCustomID(const std::string& tag)
{
	SVDeviceParamEnum paramEnum(DeviceParamInvalid);
	DeviceParamEnumNameMap::const_iterator it = g_DeviceParamEnumNameMap.find(tag);
	if (it != g_DeviceParamEnumNameMap.end())
	{
		paramEnum = it->second;
	}
	return paramEnum;
}

SVDeviceParamDataTypeEnum GetDataTypeEnumFromString(const std::string& tag)
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
	if( S_OK == SVMaterialsTree::getData( rOptions, cCustomID, MaterialData ) )
	{
		customID = MaterialData;
		SVDeviceParamEnum paramEnum = GetEnumFromCustomID(SvUl::createStdString(customID.bstrVal));
		_variant_t dataType;
		// Get Data Type
		MaterialData.clear();
		if( S_OK == SVMaterialsTree::getData( rOptions, cDataType, MaterialData ) )
		{
			dataType = MaterialData;
			
			SVDeviceParamDataTypeEnum dataTypeEnum = GetDataTypeEnumFromString(SvUl::createStdString(dataType.bstrVal));
			switch (dataTypeEnum)
			{
				case DeviceDataTypeLong:
				{
					SVLongValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl::createStdString(customID));
					SVCustomParameterBuilder::BuildLongParam(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypei64:
				{
					SVi64ValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl::createStdString(customID));
					SVCustomParameterBuilder::BuildInt64Param(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypeBool:
				{
					SVBoolValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl::createStdString(customID));
					SVCustomParameterBuilder::BuildBoolParam(&l_Param, rOptions);
					pCustomDeviceParam = new SVCustomDeviceParam(&l_Param);
				}
				break;

				case DeviceDataTypeString:
				{
					SVStringValueDeviceParam l_Param(paramEnum);
					l_Param.SetName(SvUl::createStdString(customID));
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
	std::string Value;
	std::string Description;

	for (SVMaterialsTree::const_iterator it = rOptions.begin();it!= rOptions.end();++it)
	{
		//If this is a branch generate parameters
		if( it->second.empty() )
		{
			const SVMaterialsTree::SVTreeContainer& rTree( *it.node() );
			SVMaterialsTree::ElementData MaterialData;

			// Get Value
			if( S_OK == SVMaterialsTree::getData( rTree, cValue, MaterialData ) )
			{
				Value = SvUl::createStdString(static_cast<const _variant_t> (MaterialData));
			}
			// Get Description
			MaterialData.clear();
			if( S_OK == SVMaterialsTree::getData( rTree, cDESCRIPTION, MaterialData ) )
			{
				Description = SvUl::createStdString(static_cast<const _variant_t> (MaterialData));
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
	if( S_OK == SVMaterialsTree::getData( rOptions, cVISUALNAME, MaterialData ) )
	{
		Value = MaterialData;
		pParam->SetVisualName(SvUl::createStdString(Value));
	}
	// Get Description
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cDESCRIPTION, MaterialData ) )
	{
		Value = MaterialData;
		pParam->SetDescription( SvUl::createStdString(Value) );
	}
	// Get Order
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cORDER, MaterialData ) )
	{
		Value = MaterialData;
		int order = atoi( SvUl::createStdString(Value).c_str());
		pParam->SetOrder(order);
	}
	// Get Detail Level
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cDETAILLEVEL, MaterialData ) )
	{
		Value = MaterialData;
		long level = atol( SvUl::createStdString(Value).c_str());
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
	if( S_OK == SVMaterialsTree::getData( rOptions, cMIN, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.min = atol( SvUl::createStdString(Value).c_str() );
	}
	// Get Max
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cMAX, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.max = atol( SvUl::createStdString(Value).c_str() );
	}
	// Get Offset (optional ?)
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cOFFSET, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.offset = atol( SvUl::createStdString(Value).c_str() );
	}
	// Get Multiplier
	pParam->info.multiplier = DefaultMultiplier;
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cMULTIPLIER, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.multiplier = atof( SvUl::createStdString(Value).c_str() );
		if (pParam->info.multiplier == 0) // don't you mean < epsilon ?
		{
			pParam->info.multiplier = DefaultMultiplier;
		}
	}
	// Get Divisor
	pParam->info.unit_divisor = DefaultDivisor;
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cUNIT_DIVISOR, MaterialData ) )
	{
		Value = MaterialData;
		pParam->info.unit_divisor = atof( SvUl::createStdString(Value).c_str() );
		if (pParam->info.unit_divisor == 0) // don't you mean < epsilon ?
		{
			pParam->info.unit_divisor = 1.0;
		}
	}
	// Get Value
	MaterialData.clear();
	if( S_OK == SVMaterialsTree::getData( rOptions, cValue, MaterialData ) )
	{
		Value = MaterialData;
		pParam->lValue = atol( SvUl::createStdString(Value).c_str() );
	}
}

void SVCustomParameterBuilder::BuildInt64Param(SVi64ValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;

	if( S_OK == SVMaterialsTree::getData( rOptions, cValue, MaterialData ) )
	{
		Value = MaterialData;
		try
		{
			Value.ChangeType( VT_I8 );
			pParam->SetValue( Value );
		}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		catch(_com_error& e)
		{
			TRACE1("%s\n", e.ErrorMessage());
		}
#else
		catch(_com_error& )
		{
		}
#endif
	}
}

void SVCustomParameterBuilder::BuildStringParam(SVStringValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);
	SVCustomParameterBuilder::BuildOptions(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	if( S_OK == SVMaterialsTree::getData( rOptions, cValue, MaterialData ) )
	{
		Value = MaterialData;
		pParam->strValue = SvUl::createStdString(Value).c_str();
	}
}

void SVCustomParameterBuilder::BuildBoolParam(SVBoolValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions)
{
	SVCustomParameterBuilder::BuildCommonAttributes(pParam, rOptions);
	SVCustomParameterBuilder::BuildOptions(pParam, rOptions);

	// Get Value
	SVMaterialsTree::ElementData MaterialData;
	_variant_t Value;
	if( S_OK == SVMaterialsTree::getData( rOptions, cValue, MaterialData ) )
	{
		Value = MaterialData;
		long lVal = atol(SvUl::createStdString(Value).c_str());
		pParam->bValue = lVal != 0;
	}
}

