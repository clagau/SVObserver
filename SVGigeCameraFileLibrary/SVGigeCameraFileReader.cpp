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

#include "stdafx.h"

//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/algorithm/string.hpp>
#include "SVGigeCameraFileReader.h"
#include "SVMessage/SVMessage.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVCustomParameterBuilder.h"

// section headings
static LPCTSTR scINFO = _T("Info");
static LPCTSTR scSETTINGS = _T("Settings");
static LPCTSTR scCUSTOMSETTINGS = _T("Custom Settings");
static LPCTSTR scCHECKSUM = _T("Checksum");
static LPCTSTR scCHECKSUMSECTION = _T("[Checksum]");

// commomn tags
static LPCTSTR scDESCRIPTION = _T("_Description");
static LPCTSTR scVISUALNAME = _T("_VisualName");
static LPCTSTR scORDER = _T("_Order");
static LPCTSTR scDETAILLEVEL = _T("_DetailLevel");
static LPCTSTR scOPTIONFORMAT = _T("%s_Option_%d");
static LPCTSTR scOPTIONDESCFORMAT = _T("%s_Option_%d_Description");

// option qualifiers
static LPCTSTR scMIN = _T("_min");
static LPCTSTR scMAX = _T("_max");
static LPCTSTR scOFFSET = _T("_offset");
static LPCTSTR scMULTIPLIER = _T("_multiplier");
static LPCTSTR scUNIT_DIVISOR = _T("_unit_divisor");
static LPCTSTR scUNIT = _T("_unit");

// defaults
static LPCTSTR scDEFAULTMULTIPLIER = _T("1.0");
static LPCTSTR scDEFAULTDIVISOR = _T("1.0");
static LPCTSTR scDEFAULTUNIT = "Unit";

// Info Tags
static LPCTSTR scVERSION = _T("Version");
static LPCTSTR scCAMERATYPE = _T("CameraType");

// Settings Tags
static LPCTSTR scHSTEP = _T("_HStep");
static LPCTSTR scVSTEP = _T("_VStep");
static LPCTSTR scHPOSSTEP = _T("_HPosStep");
static LPCTSTR scVPOSSTEP = _T("_VPosStep");
static LPCTSTR scMINTRANSFERTIME = _T("_MinTransferTime");
static LPCTSTR scMAXTRANSFERTIME = _T("_MaxTransferTime");
static LPCTSTR scOPTIMIZEDTRANSFERX = _T("_OptimizedTransferX");
static LPCTSTR scOPTIMIZEDTRANSFERY = _T("_OptimizedTransferY");

// LUT Tags
static LPCTSTR scNUMBANDS = _T("_NumBands");
static LPCTSTR scBANDSIZE = _T("_BandSize");
static LPCTSTR scMAXVALUE = _T("_MaxValue");

static const int iKEY_DOES_NOT_EXIST = -987654321;
static const CString sKEY_DOES_NOT_EXIST = _T("");
static const int MAX_STRING_BUFFER = 128;

// For Custom params
static LPCTSTR scValue = _T("Value");

static SVMaterialsTree::iterator GetBaseNode(SVMaterialsTree& rTree, const std::string& key)
{
	SVString SearchKey(key.c_str() );
	SVMaterialsTree::iterator it( SVMaterialsTree::find( rTree, SearchKey ));
	if (it == rTree.end())
	{
		SVMaterialsTree::SVTreeElement Element( SearchKey, SVMaterialDataPtr( nullptr ) );
		SVMaterialsTree::iterator Iter( rTree.insert(Element) );
		return Iter;
	}
	return it;
}

static SVMaterialsTree::iterator GetOptionNode(SVMaterialsTree& rTree, const std::string& key, const std::string& optionKey)
{
	SVMaterialsTree::iterator Result( rTree.end() );
	SVMaterialsTree::iterator itBase = GetBaseNode(rTree, key);
	
	if (itBase != rTree.end())
	{
		SVMaterialsTree::SVTreeContainer* pSubTree = itBase.node();
		if( nullptr != pSubTree)
		{
			SVString Key( optionKey.c_str());
			SVMaterialsTree::iterator FindIt( SVMaterialsTree::find( *pSubTree, Key) );
			if( FindIt != pSubTree->end() )
			{
				Result = FindIt;
			}
			else
			{
				SVMaterialsTree::SVTreeElement Element(Key, SVMaterialDataPtr( nullptr ) );
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

SVGigeCameraFileReader::SVGigeCameraFileReader(SVGigeCameraFileInfoStruct& rInfo) 
: m_rInfo( rInfo )
{
}

HRESULT SVGigeCameraFileReader::ReadParams( SVDeviceParamCollection& rParams )
{
	HRESULT hr = S_OK;

	// calculate checksum
	std::ifstream file;
	file.open( m_rInfo.sFilename.c_str() );
	if ( file.is_open() )
	{
		unsigned short w = GetChecksum( file );

		file.close();

		SVString sFileChecksum = SvUl_SF::Format(_T("%04X"), w);

		TCHAR sChecksum[20];
		DWORD nChars = GetPrivateProfileString(scCHECKSUM, scCHECKSUM, sKEY_DOES_NOT_EXIST, sChecksum, 10, m_rInfo.sFilename.c_str());

		if (SVString(sChecksum) != sFileChecksum)
		{
			// checksum doesn't match
			hr = SVMSG_INCORRECT_CHECKSUM;
		}

		{	// begin block

			SVString sSection;
			sSection = scINFO;

			TCHAR sVal[129];
			DWORD dwLen = GetPrivateProfileString(sSection.c_str(), scVERSION, sKEY_DOES_NOT_EXIST, sVal, 32, m_rInfo.sFilename.c_str() );
			m_rInfo.sVersion = sVal;

			sVal[0] = _T('\0');
			dwLen = GetPrivateProfileString( sSection.c_str(), scCAMERATYPE, sKEY_DOES_NOT_EXIST, sVal, 128, m_rInfo.sFilename.c_str() );
			m_rInfo.sCameraType = sVal;

			ReadCameraFileStringParam( rParams, DeviceParamVendorName,  sSection );
			ReadCameraFileStringParam( rParams, DeviceParamModelName, sSection );
			ReadCameraFileStringParam( rParams, DeviceParamFirmware,  sSection );

			sSection = scSETTINGS;

			ReadCameraFileCameraFormatsParam( rParams, DeviceParamCameraFormats, sSection );
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

			// Test Pattern
			//ReadCameraFileLongParam( rParams, DeviceParamTestPattern, sSection );

			// Binning (optional)
			ReadCameraFileLongParam( rParams, DeviceParamHorizontalBinning, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamVerticalBinning, sSection );
		}
		//
		file.close();

		ReadCustomParams(m_rInfo.sFilename, rParams);
	}
	else
	{
		// couldn't open file
		hr = S_FALSE;
	}
	return hr;
}

typedef std::deque< std::string > split_list_type;

HRESULT SVGigeCameraFileReader::ReadCustomParams(const SVString& filename, SVDeviceParamCollection& rParams)
{
	HRESULT hr = S_OK;
	std::string section(scCUSTOMSETTINGS);
	DWORD size = 64 * 1024; // 64K is that large enough ?
	char* x = new char[size];
	
	DWORD len = GetPrivateProfileSection(section.c_str(), x, size, filename.c_str());
	while (len == (size - 2)) // means the buffer is not large enough
	{
		delete [] x;
		size = size * 2; // keep doubling it...
		len = GetPrivateProfileSection(section.c_str(), x, size, filename.c_str());
	}

	SVMaterialsTree customParams;

	bool bDone = false;
	size_t offset = 0;
	while (!bDone)
	{
		// get first string
		std::string key = (x + offset);
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
							SVMaterialDataPtr pMaterial = new SVMaterialData( _variant_t( splitContainer[1].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(scValue), pMaterial ));
						}
					}
					break;

					case 3: // Attribute
					{
						SVMaterialsTree::iterator it = GetBaseNode(customParams, splitContainer[0]);
						if( customParams.end() != it )
						{
							SVMaterialDataPtr pMaterial = new SVMaterialData( _variant_t( splitContainer[2].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(splitContainer[1].c_str()), pMaterial ));
						}
					}
					break;

					case 4: // Option Value
					{
						SVMaterialsTree::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						if( customParams.end() != it )
						{
							SVMaterialDataPtr pMaterial = new SVMaterialData( _variant_t( splitContainer[3].c_str() ) );
							it.node()->insert( SVMaterialsTree::SVTreeElement( SVString(scValue), pMaterial ));
						}
					}
					break;

					case 5: // Option Attribute (desc)
					{
						SVMaterialsTree::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						if( customParams.end() != it )
						{
							SVMaterialDataPtr pMaterial = new SVMaterialData( _variant_t( splitContainer[4].c_str() ) );
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
	delete [] x;

	// Process Materials tree for the Custom parameters
	typedef std::insert_iterator<SVDeviceParamMap> Insertor;
	SVCustomParameterBuilder::BuildCustomDeviceParams(customParams, Insertor(rParams.mapParameters, rParams.mapParameters.end()));
	return hr;
}

unsigned short SVGigeCameraFileReader::GetChecksum(std::istream& rFile)
{
	unsigned short wChecksum = 0;
	
	std::string sLine;

	do
	{
		rFile >> sLine;
		if (rFile.gcount() > 0)
		if ( sLine == scCHECKSUMSECTION )
			break;

		for (size_t i = 0; i < sLine.length(); i++)
		{
			unsigned char w = sLine[i];
			ROL(wChecksum);
			wChecksum = wChecksum + w;
		}
	} while (!rFile.eof() && !rFile.fail());

	return wChecksum;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	CString csVal;
	DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	if ( csVal != sKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );

		SVStringValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->strValue = csVal;
		pParam->SetName(SVString(csKey));

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);

		while ( (dwLen = GetPrivateProfileString( sSection.c_str(), csOptionKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str() )) > 0 )
		{
			csVal.ReleaseBuffer(dwLen);
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			dwLen = GetPrivateProfileString(sSection.c_str(), csOptionKey, _T(""), csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVStringValueDeviceParam::OptionType option = SVStringValueDeviceParam::OptionType( SVString(csVal), SVString(csOptionDescription) );
			pParam->info.options.push_back(option);

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection.c_str(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVBoolValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->bValue = iVal != 0;
		pParam->SetName(SVString(csKey));

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		int iVal = iKEY_DOES_NOT_EXIST;
		while ( (iVal = GetPrivateProfileInt( sSection.c_str(), csOptionKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str() )) != iKEY_DOES_NOT_EXIST )
		{
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csOptionKey, sKEY_DOES_NOT_EXIST, csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVBoolValueDeviceParam::OptionType option( iVal != 0, SVString(csOptionDescription) );
			pParam->info.options.push_back(option);

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection.c_str(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
	if (iVal != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLongValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = iVal;
		pParam->SetName(SVString(csKey));

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);

		while ( (iVal = GetPrivateProfileInt( sSection.c_str(), csOptionKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str() )) != iKEY_DOES_NOT_EXIST )
		{
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csOptionKey, _T(""), csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVLongValueDeviceParam::OptionType option( iVal, SVString(csOptionDescription) );
			pParam->info.options.push_back(option);

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}

		ReadCameraFileLongValueInfo(sSection, SVString(csKey), m_rInfo.sFilename, pParam);
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	CString csVal;
	DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	if ( csVal != sKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVCameraFormatsDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->strValue = csVal;
		pParam->SetName(SVString(csKey));

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		CString csOption;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		while ( (dwLen = GetPrivateProfileString( sSection.c_str(), csOptionKey, sKEY_DOES_NOT_EXIST, csOption.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str() )) > 0 )
		{
			SVCameraFormat cf;

			csOption.ReleaseBuffer(dwLen);

			cf.ParseAndAssignCameraFormat( csOption );

			CString csOptionDescriptionKey;
			csOptionDescriptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			dwLen = GetPrivateProfileString(sSection.c_str(), csOptionDescriptionKey, sKEY_DOES_NOT_EXIST, csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.c_str());
			csOptionDescription.ReleaseBuffer(dwLen);
			cf.strDescription = csOptionDescription;

			int iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scHSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lHStep = iVal;
			}
			
			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scVSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lVStep = iVal;
			}
			
			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scHPOSSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lHPosStep = iVal;
			}
			else
			{
				cf.lHPosStep = cf.lHStep;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scVPOSSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lVPosStep = iVal;
			}
			else
			{
				cf.lVPosStep = cf.lVStep;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scMINTRANSFERTIME, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lMinTransferTime = iVal;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scMAXTRANSFERTIME, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lMaxTransferTime = iVal;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scOPTIMIZEDTRANSFERX, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.bOptimizedTransferRate_X = iVal != 0;
			}

			iVal = GetPrivateProfileInt(sSection.c_str(), csOptionKey + scOPTIMIZEDTRANSFERY, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.bOptimizedTransferRate_Y = iVal != 0;
			}

			cf.lOrder = iOption;
			pParam->options[SVString(csOption)] = cf;

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}

		if ( iOption == 0 )
		{
			// default an RS170 image
			SVCameraFormat cf;
			cf.m_strName = _T("M_RS170");
			cf.strDescription = _T("RS170 - 640x480, Mono 8-bit, 12.5 Hz");
			cf.lWidth = cf.lWidthMax = 640;
			cf.lHeight = cf.lHeightMax = 480;
			cf.lFrameRate = 12;
			cf.bVariableROI = false;

			pParam->options[cf.m_strName] = cf;
			pParam->strValue = cf.m_strName;
		}

		// fix for SCR 565; EB 20060613
		if ( !m_rInfo.bColorSystem && pParam->options[ pParam->strValue ].bColor )
		{
			// find first non-color mode and use it
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for ( iter = pParam->options.begin(); iter != pParam->options.end(); ++iter )
			{
				if ( !iter->second.bColor )
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
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	CString csVal;

	int iTransformOperation = GetPrivateProfileInt(sSection.c_str(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.c_str());
	if (iTransformOperation != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLutDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->SetName(SVString(csKey));

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		SVLut& rLut = pParam->lut;

		SVLutInfo lutinfo;

		int iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scNUMBANDS, 1, m_rInfo.sFilename.c_str());	// default to mono
		lutinfo.SetBands( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scBANDSIZE, 1, m_rInfo.sFilename.c_str());
		ASSERT( iVal != 1 );	// file needs to define this!!!
		lutinfo.SetBandSize( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scMAXVALUE, 1024, m_rInfo.sFilename.c_str());
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

HRESULT SVGigeCameraFileReader::ReadCameraFileCommonParams(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParam* pParam)
{
	CString csKey = sKey.c_str();

	CString csVal;
	DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csKey + scDESCRIPTION, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	pParam->SetDescription(SVString(csVal));

	csVal = sKEY_DOES_NOT_EXIST;
	dwLen = GetPrivateProfileString(sSection.c_str(), csKey + scVISUALNAME, csKey, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	pParam->SetVisualName(SVString(csVal));
	
	int iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scORDER, iKEY_DOES_NOT_EXIST, sFilename.c_str());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		pParam->SetOrder( iVal );
	}
	
	iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scDETAILLEVEL, iKEY_DOES_NOT_EXIST, sFilename.c_str());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		pParam->SetDetailLevel( iVal );
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLongValueInfo(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVLongValueDeviceParam* pParam)
{
	CString csKey = sKey.c_str();

	int iVal = iKEY_DOES_NOT_EXIST;
	// now load info (e.g. min/max)
	if ( (iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scMIN, iKEY_DOES_NOT_EXIST, sFilename.c_str())) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.min = iVal;
	}

	if ( (iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scMAX, iKEY_DOES_NOT_EXIST, sFilename.c_str())) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.max = iVal;
	}

	// load offset, multiplier, and Unit string.
	iVal = GetPrivateProfileInt(sSection.c_str(), csKey + scOFFSET, 0, sFilename.c_str());
	pParam->info.offset = iVal;

	CString csVal; 
	DWORD dwLen = GetPrivateProfileString(sSection.c_str(), csKey + scMULTIPLIER, scDEFAULTMULTIPLIER, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.multiplier = atof(csVal);
	if (pParam->info.multiplier == 0)
	{
		pParam->info.multiplier = 1.0;
	}

	dwLen = GetPrivateProfileString(sSection.c_str(), csKey + scUNIT_DIVISOR, scDEFAULTDIVISOR, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.unit_divisor = atof(csVal);
	if (pParam->info.unit_divisor == 0)
	{
		pParam->info.unit_divisor = 1.0;
	}

	dwLen = GetPrivateProfileString(sSection.c_str(), csKey + scUNIT, scDEFAULTUNIT, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.c_str());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.sUnits = csVal;

	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadGigeFeatureOverrides()
{
	HRESULT hr = S_OK;

	std::ifstream is;
	is.open(m_rInfo.sFilename.c_str());
	if (is.is_open())
	{
		char buf[512];
		bool bFound = false;
		// Find the xml tag
		while (!is.eof() && !bFound)
		{
			is.getline(buf, sizeof(buf) - 1);
			if (is.gcount())
			{
				if (strstr(buf, "<?xml"))
				{
					bFound = true;
					m_rInfo.sFeatureOverrides = buf;
				}
			}
		}

		if (bFound)
		{
			// Read the xml data
			while (!is.eof())
			{
				is.read(buf, sizeof(buf) - 1);
				std::streamsize amtRead = is.gcount();
				if (amtRead)
				{
					buf[amtRead] = 0;
					m_rInfo.sFeatureOverrides += buf;
				}
			}
		}
		is.close();
	}
	else
	{
		hr = ERROR_FILE_NOT_FOUND;
	}
	return hr;
}

