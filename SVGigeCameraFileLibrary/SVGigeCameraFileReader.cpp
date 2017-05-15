//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraFileReader
//* .File Name       : $Workfile:   SVGigeCameraFileReader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2013 19:35:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVGigeCameraFileReader.h"
#include "SVMessage/SVMessage.h"

#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "CameraLibrary/SVi64ValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVCustomParameterBuilder.h"
#pragma endregion Includes

// section headings
static const TCHAR* const cINFO = _T("Info");
static const TCHAR* const cSETTINGS = _T("Settings");
static const TCHAR* const cCUSTOMSETTINGS = _T("Custom Settings");

// common tags
static const TCHAR* const cDESCRIPTION = _T("_Description");
static const TCHAR* const cVISUALNAME = _T("_VisualName");
static const TCHAR* const cORDER = _T("_Order");
static const TCHAR* const cDETAILLEVEL = _T("_DetailLevel");
static const TCHAR* const cOPTIONFORMAT = _T("%s_Option_%d");
static const TCHAR* const cOPTIONDESCFORMAT = _T("%s_Option_%d_Description");

// option qualifiers
static const TCHAR* const cMIN = _T("_min");
static const TCHAR* const cMAX = _T("_max");
static const TCHAR* const cOFFSET = _T("_offset");
static const TCHAR* const cMULTIPLIER = _T("_multiplier");
static const TCHAR* const cUNIT_DIVISOR = _T("_unit_divisor");
static const TCHAR* const cUNIT = _T("_unit");

// defaults
static const TCHAR* const cDEFAULTMULTIPLIER = _T("1.0");
static const TCHAR* const cDEFAULTDIVISOR = _T("1.0");
static const TCHAR* const cDEFAULTUNIT = "Unit";

// Info Tags
static const TCHAR* const scVERSION = _T("Version");
static const TCHAR* const scCAMERATYPE = _T("CameraType");

// Settings Tags
static const TCHAR* const scHSTEP = _T("_HStep");
static const TCHAR* const scVSTEP = _T("_VStep");
static const TCHAR* const scHPOSSTEP = _T("_HPosStep");
static const TCHAR* const scVPOSSTEP = _T("_VPosStep");
static const TCHAR* const scMINTRANSFERTIME = _T("_MinTransferTime");
static const TCHAR* const scMAXTRANSFERTIME = _T("_MaxTransferTime");
static const TCHAR* const scOPTIMIZEDTRANSFERX = _T("_OptimizedTransferX");
static const TCHAR* const scOPTIMIZEDTRANSFERY = _T("_OptimizedTransferY");

// LUT Tags
static const TCHAR* const scNUMBANDS = _T("_NumBands");
static const TCHAR* const scBANDSIZE = _T("_BandSize");
static const TCHAR* const scMAXVALUE = _T("_MaxValue");

static const int iKEY_DOES_NOT_EXIST = -987654321;
static const int MAX_STRING_BUFFER = 128;

// For Custom params
static const TCHAR* const cValue = _T("Value");

static SVMaterialsTree::iterator GetBaseNode(SVMaterialsTree& rTree, const SVString& rKey)
{
	SVString SearchKey(rKey.c_str() );
	SVMaterialsTree::iterator it( SVMaterialsTree::find( rTree, SearchKey ));
	if (it == rTree.end())
	{
		SVMaterialsTree::SVTreeElement Element( SearchKey, SvXml::SVMaterialDataPtr( nullptr ) );
		SVMaterialsTree::iterator Iter( rTree.insert(Element) );
		return Iter;
	}
	return it;
}

static SVMaterialsTree::iterator GetOptionNode(SVMaterialsTree& rTree, const SVString& rKey, const SVString& rOptionKey)
{
	SVMaterialsTree::iterator Result( rTree.end() );
	SVMaterialsTree::iterator itBase = GetBaseNode(rTree, rKey);
	
	if (itBase != rTree.end())
	{
		SVMaterialsTree::SVTreeContainer* pSubTree = itBase.node();
		if( nullptr != pSubTree)
		{
			SVString Key( rOptionKey.c_str());
			SVMaterialsTree::iterator FindIt( SVMaterialsTree::find( *pSubTree, Key) );
			if( FindIt != pSubTree->end() )
			{
				Result = FindIt;
			}
			else
			{
				SVMaterialsTree::SVTreeElement Element(Key, SvXml::SVMaterialDataPtr( nullptr ) );
				SVMaterialsTree::iterator NewIter( pSubTree->insert( Element ) );
				if( pSubTree->end() != NewIter )
				{
					Result =  NewIter;
				}
			}
		}
	}
	return Result;
}


SVGigeCameraFileReader::SVGigeCameraFileReader(const SVString &rFilename ,bool bColorSystem)
	: m_Filename(rFilename), m_isColorSystem(bColorSystem)
{
}


HRESULT SVGigeCameraFileReader::ReadCameraFileImpl( SVDeviceParamCollection &rParams )
{
	HRESULT hr = S_OK;

	CStdioFile file;
	if ( file.Open( m_Filename.c_str(), CFile::modeRead | CFile::shareDenyWrite ) )
	{
		file.Close();

		rParams.SetParameterDefaults();

		// Read the Gige Feature Overrides
		hr = ReadGigeFeatureOverrides();

		if (S_OK == hr)
		{
			hr = ReadParams( rParams );
		}
	}
	else
	{
		// couldn't open file
		hr = S_FALSE;
	}
	return hr;
}


HRESULT SVGigeCameraFileReader::ReadParams( SVDeviceParamCollection& rParams )
{
	HRESULT Result( S_OK );
	SVString sSection( cINFO );

	ReadCameraFileStringParam( rParams, DeviceParamVendorName,  sSection );
	ReadCameraFileStringParam( rParams, DeviceParamModelName, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamFirmware,  sSection );

	sSection = cSETTINGS;

	ReadCameraFileCameraFormatsParam( rParams, DeviceParamCameraFormats, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamCameraDefaultSettings, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamNumCameraBuffers, sSection );
	ReadCameraFileLutParam( rParams, DeviceParamLut, sSection );

	// Exposure
	ReadCameraFileLongParam( rParams, DeviceParamShutter, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamShutterDelay, sSection );

	// Analog Controls
	ReadCameraFileLongParam( rParams, DeviceParamGamma, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamBrightness, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamGain, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamSharpness, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamHue, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamSaturation, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamExposure, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamWhiteBalanceUB, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamWhiteBalanceVR, sSection );

	// Trigger
	ReadCameraFileStringParam( rParams, DeviceParamGigeTriggerSource, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamGigeTriggerEdge, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamGigeTriggerEnable, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamShutterDelay, sSection );
	ReadCameraFileBoolParam( rParams, DeviceParamIOTriggerInvert, sSection );

	// Strobe
	ReadCameraFileStringParam( rParams, DeviceParamGigeStrobeSource, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamGigeStrobeEdge, sSection );
	ReadCameraFileStringParam( rParams, DeviceParamGigeStrobeEnable, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamStrobePulseDelay, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamStrobePulseDuration, sSection );
	ReadCameraFileBoolParam( rParams, DeviceParamIOStrobeInvert, sSection );
			
	// Inputs
	ReadCameraFileStringParam( rParams, DeviceParamCameraInput, sSection );

	//! Test Pattern used to display a test pattern as a camera image
	//ReadCameraFileLongParam( rParams, DeviceParamTestPattern, sSection );

	// Binning (optional)
	ReadCameraFileLongParam( rParams, DeviceParamHorizontalBinning, sSection );
	ReadCameraFileLongParam( rParams, DeviceParamVerticalBinning, sSection );

	ReadCustomParams(m_Filename, rParams);
	return Result;
}

typedef std::deque< SVString > split_list_type;

HRESULT SVGigeCameraFileReader::ReadCustomParams(const SVString& filename, SVDeviceParamCollection& rParams)
{
	HRESULT hr = S_OK;
	SVString Section(cCUSTOMSETTINGS);
	DWORD size = 64 * 1024; // 64K is that large enough ?
	SVString CustomSettings;
	CustomSettings.resize( size, '\0' );
	
	DWORD len = GetPrivateProfileSection( Section.c_str(), &CustomSettings.at(0), size, filename.c_str() );
	while (len == (size - 1)) // means the buffer is not large enough
	{
		size = size * 2; // keep doubling it...
		CustomSettings.resize( size, '\0' );
		len = GetPrivateProfileSection( Section.c_str(), &CustomSettings.at(0), size, filename.c_str() );
	}

	SVMaterialsTree customParams;

	bool bDone = false;
	size_t offset = 0;
	while (!bDone)
	{
		// get first string
		SVString key = (&CustomSettings.at(offset));
		if (key.empty())
		{
			bDone = true;
		}
		else
		{
			split_list_type splitContainer;
			boost::algorithm::split(splitContainer, key, boost::algorithm::is_any_of("=_"), boost::algorithm::token_compress_on);

			if (splitContainer.size() > 1)
			{
				switch (splitContainer.size())
				{
					case 2: // Value
					{
						SVMaterialsTree::iterator it = GetBaseNode(customParams, splitContainer[0]);
						if( customParams.end() != it )
						{
							SvXml::SVMaterialDataPtr pMaterial = new SvXml::SVMaterialData( _variant_t( splitContainer[1].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(cValue), pMaterial ));
						}
					}
					break;

					case 3: // Attribute
					{
						SVMaterialsTree::iterator it = GetBaseNode(customParams, splitContainer[0]);
						if( customParams.end() != it )
						{
							SvXml::SVMaterialDataPtr pMaterial = new SvXml::SVMaterialData( _variant_t( splitContainer[2].c_str() ) );
							it.node()->insert(SVMaterialsTree::SVTreeElement( SVString(splitContainer[1].c_str()), pMaterial ));
						}
					}
					break;

					case 4: // Option Value
					{
						SVMaterialsTree::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						if( customParams.end() != it )
						{
							SvXml::SVMaterialDataPtr pMaterial = new SvXml::SVMaterialData( _variant_t( splitContainer[3].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(cValue), pMaterial ));
						}
					}
					break;

					case 5: // Option Attribute (desc)
					{
						SVMaterialsTree::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						if( customParams.end() != it )
						{
							SvXml::SVMaterialDataPtr pMaterial = new SvXml::SVMaterialData( _variant_t( splitContainer[4].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(splitContainer[3].c_str()), pMaterial ));
						}
					}
					break;
				}
			}
			offset += key.size();
			offset++;
		}
	}

	// Process Materials tree for the Custom parameters
	typedef std::insert_iterator<SVDeviceParamMap> Insertor;
	SVCustomParameterBuilder::BuildCustomDeviceParams(customParams, Insertor(rParams.mapParameters, rParams.mapParameters.end()));
	return hr;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& rSection )
{
	const SVString& rKey = SVDeviceParam::GetParameterName( e );

	SVString Value;
	Value.resize( MAX_STRING_BUFFER, '\0' );
	DWORD ValueSize = GetPrivateProfileString( rSection.c_str(), rKey.c_str(), _T(""), &Value.at(0), MAX_STRING_BUFFER, m_Filename.c_str() );
	if ( !Value.empty() )
	{
		Value.resize( ValueSize );
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );

		SVStringValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->strValue = Value;
		pParam->SetName( rKey );

		ReadCameraFileCommonParams( rSection, rKey, m_Filename, pParam );

		// now load options
		int iOption = 0;
		SVString OptionKey;
		OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
		Value.clear();
		Value.resize( MAX_STRING_BUFFER, '\0' );
		while ( (ValueSize = GetPrivateProfileString( rSection.c_str(), OptionKey.c_str(), _T(""), &Value.at(0), MAX_STRING_BUFFER, m_Filename.c_str()) ) > 0 )
		{
			Value.resize( ValueSize );
			OptionKey = SvUl_SF::Format( cOPTIONDESCFORMAT, rKey.c_str(), iOption );
			SVString OptionDescription;
			OptionDescription.resize( MAX_STRING_BUFFER, '\0' );
			DWORD DescriptionSize = GetPrivateProfileString( rSection.c_str(), OptionKey.c_str(), _T(""), &OptionDescription.at(0), MAX_STRING_BUFFER, m_Filename.c_str() );
			OptionDescription.resize( DescriptionSize );
			
			SVStringValueDeviceParam::OptionType option = SVStringValueDeviceParam::OptionType( Value, OptionDescription );
			pParam->info.options.push_back(option);

			OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
			Value.clear();
			Value.resize( MAX_STRING_BUFFER, '\0' );
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const SVString& rKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt( sSection.c_str(), rKey.c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() );
	if ( iKEY_DOES_NOT_EXIST != iVal )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVBoolValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->bValue = iVal != 0;
		pParam->SetName( rKey );

		ReadCameraFileCommonParams( sSection, rKey, m_Filename, pParam );

		// load options
		int iOption = 0;
		SVString OptionKey;
		OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
		int iVal = iKEY_DOES_NOT_EXIST;
		while ( iKEY_DOES_NOT_EXIST != (iVal = GetPrivateProfileInt( sSection.c_str(), OptionKey.c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() )) )
		{
			OptionKey = SvUl_SF::Format( cOPTIONDESCFORMAT, rKey.c_str(), iOption );
			SVString OptionDescription;
			OptionDescription.resize( MAX_STRING_BUFFER, '\0' );
			DWORD ValueSize = GetPrivateProfileString( sSection.c_str(), OptionKey.c_str(), _T(""), &OptionDescription.at(0), MAX_STRING_BUFFER, m_Filename.c_str() );
			OptionDescription.resize( ValueSize );
			
			SVBoolValueDeviceParam::OptionType option( iVal != 0, OptionDescription );
			pParam->info.options.push_back(option);

			OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const SVString& rKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt( sSection.c_str(), rKey.c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() );
	if (iKEY_DOES_NOT_EXIST != iVal)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLongValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = iVal;
		pParam->SetName( rKey );

		ReadCameraFileCommonParams( sSection, rKey, m_Filename, pParam );

		// now load options
		int iOption = 0;
		SVString OptionKey;
		OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );

		while ( iKEY_DOES_NOT_EXIST != (iVal = GetPrivateProfileInt( sSection.c_str(), OptionKey.c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() )) )
		{
			OptionKey = SvUl_SF::Format( cOPTIONDESCFORMAT, rKey.c_str(), iOption );
			SVString OptionDescription;
			OptionDescription.resize( MAX_STRING_BUFFER, '\0' );
			DWORD ValueSize = GetPrivateProfileString( sSection.c_str(), OptionKey.c_str(), _T(""), &OptionDescription.at(0), MAX_STRING_BUFFER, m_Filename.c_str() );
			OptionDescription.resize( ValueSize );
			
			SVLongValueDeviceParam::OptionType option( iVal, OptionDescription );
			pParam->info.options.push_back(option);

			OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
		}

		ReadCameraFileLongValueInfo(sSection, rKey, m_Filename, pParam);
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const SVString& rKey = SVDeviceParam::GetParameterName( e );

	SVString Value;
	Value.resize( MAX_STRING_BUFFER, '\0' );
	DWORD ValueSize = GetPrivateProfileString(sSection.c_str(), rKey.c_str(), _T(""), &Value.at(0), MAX_STRING_BUFFER, m_Filename.c_str());
	if ( !Value.empty() )
	{
		Value.resize( ValueSize );
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVCameraFormatsDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->strValue = Value;
		pParam->SetName( rKey );

		ReadCameraFileCommonParams(sSection, rKey, m_Filename, pParam);

		// now load options
		int iOption = 0;
		SVString OptionKey;
		SVString Option;
		OptionKey = SvUl_SF::Format( cOPTIONFORMAT, rKey.c_str(), ++iOption );
		Option.resize( MAX_STRING_BUFFER, '\0' );
		while ( (ValueSize = GetPrivateProfileString( sSection.c_str(), OptionKey.c_str(), _T(""), &Option.at(0), MAX_STRING_BUFFER, m_Filename.c_str() )) > 0 )
		{
			Option.resize( ValueSize );
			SVCameraFormat cf;

			cf.ParseAndAssignCameraFormat( Option );

			SVString OptionDescriptionKey = SvUl_SF::Format( cOPTIONDESCFORMAT, rKey.c_str(), iOption );
			SVString OptionDescription;
			OptionDescription.resize( MAX_STRING_BUFFER, '\0' );
			ValueSize = GetPrivateProfileString( sSection.c_str(), OptionDescriptionKey.c_str(), _T(""), &OptionDescription.at(0), MAX_STRING_BUFFER, m_Filename.c_str() );
			OptionDescription.resize( ValueSize );
			cf.m_strDescription = OptionDescription;

			int iVal = GetPrivateProfileInt( sSection.c_str(), SVString(OptionKey + scHSTEP).c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str());
			if ( iKEY_DOES_NOT_EXIST != iVal )
			{
				cf.m_lHStep = iVal;
			}
			
			iVal = GetPrivateProfileInt( sSection.c_str(), SVString(OptionKey + scVSTEP).c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str());
			if ( iKEY_DOES_NOT_EXIST != iVal )
			{
				cf.m_lVStep = iVal;
			}
			
			iVal = GetPrivateProfileInt( sSection.c_str(), SVString(OptionKey + scHPOSSTEP).c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() );
			if ( iKEY_DOES_NOT_EXIST != iVal )
			{
				cf.m_lHPosStep = iVal;
			}
			else
			{
				cf.m_lHPosStep = cf.m_lHStep;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), SVString(OptionKey + scVPOSSTEP).c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str());
			if ( iKEY_DOES_NOT_EXIST != iVal )
			{
				cf.m_lVPosStep = iVal;
			}
			else
			{
				cf.m_lVPosStep = cf.m_lVStep;
			}

			cf.m_lOrder = iOption;
			pParam->options[SVString(Option)] = cf;

			OptionKey = SvUl_SF::Format(cOPTIONFORMAT, rKey.c_str(), ++iOption);
			Option.clear();
			Option.resize( MAX_STRING_BUFFER, '\0' );
		}

		if ( 0 == iOption )
		{
			// default an RS170 image
			SVCameraFormat cf;
			cf.m_strName = _T("M_RS170");
			cf.m_strDescription = _T("RS170 - 640x480, Mono 8-bit, 12.5 Hz");
			cf.m_lWidth = cf.m_lWidthMax = 640;
			cf.m_lHeight = cf.m_lHeightMax = 480;
			cf.m_bVariableROI = false;

			pParam->options[cf.m_strName] = cf;
			pParam->strValue = cf.m_strName;
		}

		if ( !m_isColorSystem && pParam->options[ pParam->strValue ].m_bColor )
		{
			// find first non-color mode and use it
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for ( iter = pParam->options.begin(); iter != pParam->options.end(); ++iter )
			{
				if ( !iter->second.m_bColor )
				{
					pParam->strValue = iter->second.m_strName;
					break;
				}
			}
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const SVString& rKey = SVDeviceParam::GetParameterName( e );

	int iTransformOperation = GetPrivateProfileInt( sSection.c_str(), rKey.c_str(), iKEY_DOES_NOT_EXIST, m_Filename.c_str() );
	if (iKEY_DOES_NOT_EXIST != iTransformOperation )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLutDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->SetName( rKey );

		ReadCameraFileCommonParams( sSection, rKey, m_Filename, pParam );

		SVLut& rLut = pParam->lut;

		SVLutInfo lutinfo;

		int iVal = GetPrivateProfileInt(sSection.c_str(), SVString(rKey + scNUMBANDS).c_str(), 1, m_Filename.c_str());	// default to mono
		lutinfo.SetBands( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.c_str(), SVString(rKey + scBANDSIZE).c_str(), 1, m_Filename.c_str());
		ASSERT( iVal != 1 );	// file needs to define this!!!
		lutinfo.SetBandSize( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.c_str(), SVString(rKey + scMAXVALUE).c_str(), 1024, m_Filename.c_str());
		lutinfo.SetMaxValue( iVal );

		lutinfo.SetTransform(SVDefaultLutTransform());
		rLut.Create(lutinfo);

		SVLutTransformParameters param;
		const SVLutTransformOperation* pOperation = SVLutTransform::GetEquivalentType( (SVLutTransformOperationEnum) iTransformOperation );
		ASSERT(pOperation);
		if (pOperation)
		{
			rLut.SetTransformOperation(*pOperation);
			rLut.Transform(param);
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileCommonParams(const SVString& rSection, const SVString& rKey, const SVString& rFilename, SVDeviceParam* pParam)
{

	SVString Value;
	Value.resize( MAX_STRING_BUFFER, '\0' );
	DWORD ValueSize = GetPrivateProfileString( rSection.c_str(), SVString(rKey + cDESCRIPTION).c_str(), _T(""), &Value.at(0), MAX_STRING_BUFFER, rFilename.c_str() );
	Value.resize( ValueSize );

	pParam->SetDescription( Value );

	Value.clear();
	Value.resize( MAX_STRING_BUFFER, '\0' );
	ValueSize = GetPrivateProfileString( rSection.c_str(), SVString(rKey + cVISUALNAME).c_str(), rKey.c_str(), &Value.at(0), MAX_STRING_BUFFER, rFilename.c_str() );
	Value.resize( ValueSize );
	pParam->SetVisualName( Value );
	
	int iVal = GetPrivateProfileInt( rSection.c_str(), SVString(rKey + cORDER).c_str(), iKEY_DOES_NOT_EXIST, rFilename.c_str() );
	if ( iKEY_DOES_NOT_EXIST != iVal )
	{
		pParam->SetOrder( iVal );
	}
	
	iVal = GetPrivateProfileInt( rSection.c_str(), SVString(rKey + cDETAILLEVEL).c_str(), iKEY_DOES_NOT_EXIST, rFilename.c_str() );
	if ( iKEY_DOES_NOT_EXIST != iVal )
	{
		pParam->SetDetailLevel( iVal );
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLongValueInfo(const SVString& rSection, const SVString& rKey, const SVString& rFilename, SVLongValueDeviceParam* pParam)
{
	int iVal = iKEY_DOES_NOT_EXIST;
	// now load info (e.g. min/max)
	if ( (iVal = GetPrivateProfileInt( rSection.c_str(), SVString(rKey + cMIN).c_str(), iKEY_DOES_NOT_EXIST, rFilename.c_str()) ) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.min = iVal;
	}

	if ( (iVal = GetPrivateProfileInt( rSection.c_str(), SVString(rKey + cMAX).c_str(), iKEY_DOES_NOT_EXIST, rFilename.c_str()) ) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.max = iVal;
	}

	// load offset, multiplier, and Unit string.
	iVal = GetPrivateProfileInt( rSection.c_str(), SVString(rKey + cOFFSET).c_str(), 0, rFilename.c_str() );
	pParam->info.offset = iVal;

	SVString Value;
	Value.resize( MAX_STRING_BUFFER, '\0' );
	DWORD ValueSize = GetPrivateProfileString( rSection.c_str(), SVString(rKey + cMULTIPLIER).c_str(), cDEFAULTMULTIPLIER, &Value.at(0), MAX_STRING_BUFFER, rFilename.c_str() );
	Value.resize( ValueSize );
	pParam->info.multiplier = atof( Value.c_str() );
	if (0 == pParam->info.multiplier)
	{
		pParam->info.multiplier = 1.0;
	}

	Value.clear();
	Value.resize( MAX_STRING_BUFFER, '\0' );
	ValueSize = GetPrivateProfileString( rSection.c_str(), SVString(rKey + cUNIT_DIVISOR).c_str(), cDEFAULTDIVISOR, &Value.at(0), MAX_STRING_BUFFER, rFilename.c_str() );
	Value.resize( ValueSize );
	pParam->info.unit_divisor = atof( Value.c_str() );
	if (0 == pParam->info.unit_divisor)
	{
		pParam->info.unit_divisor = 1.0;
	}

	Value.clear();
	Value.resize( MAX_STRING_BUFFER, '\0' );
	ValueSize = GetPrivateProfileString( rSection.c_str(), SVString(rKey + cUNIT).c_str(), cDEFAULTUNIT, &Value.at(0), MAX_STRING_BUFFER, rFilename.c_str() );
	Value.resize( ValueSize );
	pParam->info.sUnits = Value;

	return S_OK;
}


HRESULT SVGigeCameraFileReader::ReadGigeFeatureOverrides()
{
	HRESULT hr = S_OK;

	std::ifstream is;
	is.open(m_Filename.c_str());
	if (is.is_open())
	{
		bool xmlPartOfCameraFileReached = false;

		SVString line;
		while(getline(is, line))
		{
			if (!xmlPartOfCameraFileReached)
				// Find the xml tag
				if (SVString::npos != line.find("<?xml"))
				{
					xmlPartOfCameraFileReached = true;
				}
				else
				{
					continue; //not yet there...
				}
			// Read the xml data
			m_FeatureOverrides += line + _T("\n");
		}
		is.close();
	}
	else
	{
		hr = ERROR_FILE_NOT_FOUND;
	}
	return hr;
}

