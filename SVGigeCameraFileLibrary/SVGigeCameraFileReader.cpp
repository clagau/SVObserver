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
#define _SCL_SECURE_NO_WARNINGS
#include <boost/config.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <deque>
#include "SVGigeCameraFileReader.h"
#include "SVMessage/SVMessage.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"
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

static SVMaterialsTreeAdapter::iterator GetBaseNode(SVMaterialsTree& rTree, const std::string& key)
{
	SVMaterialsTreeAdapter::iterator it(rTree.find(key.c_str()));
	if (it == rTree.end())
	{
		SVMaterialsTreeAdapter::SVMaterialsElement l_Element(SVString(key.c_str()), SVMaterials());
		SVMaterialsTreeAdapter::iterator l_Iter(rTree.insert(l_Element, rTree.end()));
		return l_Iter;
	}
	return it;
}

static SVMaterialsTreeAdapter::iterator GetOptionNode(SVMaterialsTree& rTree, const std::string& key, const std::string& optionKey)
{
	SVMaterialsTreeAdapter::iterator itBase = GetBaseNode(rTree, key);
	
	if (itBase != rTree.end())
	{
		SVMaterialsTreeAdapter::SVTreeContainer* pTree = itBase.GetChildTree();
		if (pTree)
		{
			SVMaterialsTreeAdapter rChildTree(*pTree);
			
			// Find or insert Option Node
			SVMaterialsTreeAdapter::iterator optIt(rChildTree.find(optionKey.c_str()));
			if (optIt == rChildTree.end())
			{
				SVMaterialsTreeAdapter::SVMaterialsElement l_Element(SVString(optionKey.c_str()), SVMaterials());
				SVMaterialsTreeAdapter::iterator l_Iter(rChildTree.insert(l_Element, rChildTree.end()));
				return l_Iter;
			}
			return optIt;
		}
	}
	return rTree.end();
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
	file.open( m_rInfo.sFilename.ToString() );
	if ( file.is_open() )
	{
		unsigned short w = GetChecksum( file );

		file.close();

		SVString sFileChecksum;
		sFileChecksum.Format(_T("%04X"), w);

		TCHAR sChecksum[20];
		DWORD nChars = GetPrivateProfileString(scCHECKSUM, scCHECKSUM, sKEY_DOES_NOT_EXIST, sChecksum, 10, m_rInfo.sFilename.ToString());

		if (sChecksum != sFileChecksum)
		{
			// checksum doesn't match
			hr = SVMSG_INCORRECT_CHECKSUM;
		}

		{	// begin block

			SVString sSection;
			sSection = scINFO;

			TCHAR sVal[129];
			DWORD dwLen = GetPrivateProfileString(sSection.ToString(), scVERSION, sKEY_DOES_NOT_EXIST, sVal, 32, m_rInfo.sFilename.ToString() );
			m_rInfo.sVersion = sVal;

			sVal[0] = _T('\0');
			dwLen = GetPrivateProfileString( sSection.ToString(), scCAMERATYPE, sKEY_DOES_NOT_EXIST, sVal, 128, m_rInfo.sFilename.ToString() );
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
						SVMaterialsTreeAdapter::iterator it = GetBaseNode(customParams, splitContainer[0]);
						SVMaterials& rMaterials = it->second;
						rMaterials.AddMaterial(scValue, splitContainer[1].c_str());
					}
					break;

					case 3: // Attribute
					{
						SVMaterialsTreeAdapter::iterator it = GetBaseNode(customParams, splitContainer[0]);
						SVMaterials& rMaterials = it->second;
						rMaterials.AddMaterial(splitContainer[1].c_str(), splitContainer[2].c_str());
					}
					break;

					case 4: // Option Value
					{
						SVMaterialsTreeAdapter::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						SVMaterials& rMaterials = it->second;
						rMaterials.AddMaterial(scValue, splitContainer[3].c_str());
					}
					break;

					case 5: // Option Attribute (desc)
					{
						SVMaterialsTreeAdapter::iterator it = GetOptionNode(customParams, splitContainer[0], splitContainer[1] + "_" + splitContainer[2]);
						SVMaterials& rMaterials = it->second;
						rMaterials.AddMaterial(splitContainer[3].c_str(), splitContainer[4].c_str());
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
	DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	if ( csVal != sKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );

		SVStringValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->strValue = csVal;
		pParam->SetName(csKey);

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);

		while ( (dwLen = GetPrivateProfileString( sSection.ToString(), csOptionKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString() )) > 0 )
		{
			csVal.ReleaseBuffer(dwLen);
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			dwLen = GetPrivateProfileString(sSection.ToString(), csOptionKey, _T(""), csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVStringValueDeviceParam::OptionType option( csVal, csOptionDescription );
			pParam->info.options.push_back(option);

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection.ToString(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVBoolValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->bValue = iVal != 0;
		pParam->SetName(csKey);

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		int iVal = iKEY_DOES_NOT_EXIST;
		while ( (iVal = GetPrivateProfileInt( sSection.ToString(), csOptionKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString() )) != iKEY_DOES_NOT_EXIST )
		{
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csOptionKey, sKEY_DOES_NOT_EXIST, csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVBoolValueDeviceParam::OptionType option( iVal != 0, csOptionDescription );
			pParam->info.options.push_back(option);

			csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection )
{
	const CString& csKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection.ToString(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
	if (iVal != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLongValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = iVal;
		pParam->SetName(csKey);

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);

		while ( (iVal = GetPrivateProfileInt( sSection.ToString(), csOptionKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString() )) != iKEY_DOES_NOT_EXIST )
		{
			csOptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csOptionKey, _T(""), csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
			csOptionDescription.ReleaseBuffer(dwLen);
			
			SVLongValueDeviceParam::OptionType option( iVal, csOptionDescription );
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
	DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csKey, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	if ( csVal != sKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVCameraFormatsDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->strValue = csVal;
		pParam->SetName(csKey);

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		// now load options
		int iOption = 0;
		CString csOptionKey;
		CString csOptionDescription;
		CString csOption;
		csOptionKey.Format(scOPTIONFORMAT, csKey, ++iOption);
		while ( (dwLen = GetPrivateProfileString( sSection.ToString(), csOptionKey, sKEY_DOES_NOT_EXIST, csOption.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString() )) > 0 )
		{
			SVCameraFormat cf;

			csOption.ReleaseBuffer(dwLen);

			cf.ParseAndAssignCameraFormat( csOption );

			CString csOptionDescriptionKey;
			csOptionDescriptionKey.Format(scOPTIONDESCFORMAT, csKey, iOption);
			dwLen = GetPrivateProfileString(sSection.ToString(), csOptionDescriptionKey, sKEY_DOES_NOT_EXIST, csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, m_rInfo.sFilename.ToString());
			csOptionDescription.ReleaseBuffer(dwLen);
			cf.strDescription = csOptionDescription;

			int iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scHSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lHStep = iVal;
			}
			
			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scVSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lVStep = iVal;
			}
			
			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scHPOSSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lHPosStep = iVal;
			}
			else
			{
				cf.lHPosStep = cf.lHStep;
			}

			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scVPOSSTEP, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lVPosStep = iVal;
			}
			else
			{
				cf.lVPosStep = cf.lVStep;
			}

			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scMINTRANSFERTIME, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lMinTransferTime = iVal;
			}

			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scMAXTRANSFERTIME, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.lMaxTransferTime = iVal;
			}

			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scOPTIMIZEDTRANSFERX, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.bOptimizedTransferRate_X = iVal != 0;
			}

			iVal = GetPrivateProfileInt(sSection.ToString(), csOptionKey + scOPTIMIZEDTRANSFERY, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
			if ( iVal != iKEY_DOES_NOT_EXIST )
			{
				cf.bOptimizedTransferRate_Y = iVal != 0;
			}

			cf.lOrder = iOption;
			pParam->options[csOption] = cf;

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

	int iTransformOperation = GetPrivateProfileInt(sSection.ToString(), csKey, iKEY_DOES_NOT_EXIST, m_rInfo.sFilename.ToString());
	if (iTransformOperation != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLutDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->SetName(csKey);

		ReadCameraFileCommonParams(sSection, SVString(csKey), m_rInfo.sFilename, pParam);

		SVLut& rLut = pParam->lut;

		SVLutInfo lutinfo;

		int iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scNUMBANDS, 1, m_rInfo.sFilename.ToString());	// default to mono
		lutinfo.SetBands( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scBANDSIZE, 1, m_rInfo.sFilename.ToString());
		ASSERT( iVal != 1 );	// file needs to define this!!!
		lutinfo.SetBandSize( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scMAXVALUE, 1024, m_rInfo.sFilename.ToString());
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
	CString csKey = sKey.ToString();

	CString csVal;
	DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csKey + scDESCRIPTION, sKEY_DOES_NOT_EXIST, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	pParam->SetDescription(csVal);

	csVal = sKEY_DOES_NOT_EXIST;
	dwLen = GetPrivateProfileString(sSection.ToString(), csKey + scVISUALNAME, csKey, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	pParam->SetVisualName(csVal);
	
	int iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scORDER, iKEY_DOES_NOT_EXIST, sFilename.ToString());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		pParam->SetOrder( iVal );
	}
	
	iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scDETAILLEVEL, iKEY_DOES_NOT_EXIST, sFilename.ToString());
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		pParam->SetDetailLevel( iVal );
	}
	return S_OK;
}
/*
HRESULT SVGigeCameraFileReader::ReadCameraFileOptions(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParamOptionInsertor& rInsertor)
{
	// load options
	int iOption = 0;
	CString csOptionKey;
	CString csOptionDescription;
	csOptionKey.Format(scOPTIONFORMAT, sKey.ToString(), ++iOption);
	int iVal = iKEY_DOES_NOT_EXIST;
	while ( (iVal = GetPrivateProfileInt( sSection.ToString(), csOptionKey, iKEY_DOES_NOT_EXIST, sFilename.ToString() )) 
	         != iKEY_DOES_NOT_EXIST )
	{
		csOptionKey.Format(scOPTIONDESCFORMAT, sKey.ToString(), iOption);
		DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csOptionKey, sKEY_DOES_NOT_EXIST, csOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
		csOptionDescription.ReleaseBuffer(dwLen);
		
		//rInsertor(iVal, csOptionDescription);
		
		SVBoolValueDeviceParam::OptionType option( iVal != 0, csOptionDescription );
		pParam->info.options.push_back(option);
		
		csOptionKey.Format(scOPTIONFORMAT, sKey.ToString(), ++iOption);
	}
	return S_OK;
}
*/

HRESULT SVGigeCameraFileReader::ReadCameraFileLongValueInfo(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVLongValueDeviceParam* pParam)
{
	CString csKey = sKey.ToString();

	int iVal = iKEY_DOES_NOT_EXIST;
	// now load info (e.g. min/max)
	if ( (iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scMIN, iKEY_DOES_NOT_EXIST, sFilename.ToString())) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.min = iVal;
	}

	if ( (iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scMAX, iKEY_DOES_NOT_EXIST, sFilename.ToString())) != iKEY_DOES_NOT_EXIST )
	{
		pParam->info.max = iVal;
	}

	// load offset, multiplier, and Unit string.
	iVal = GetPrivateProfileInt(sSection.ToString(), csKey + scOFFSET, 0, sFilename.ToString());
	pParam->info.offset = iVal;

	CString csVal; 
	DWORD dwLen = GetPrivateProfileString(sSection.ToString(), csKey + scMULTIPLIER, scDEFAULTMULTIPLIER, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.multiplier = atof(csVal);
	if (pParam->info.multiplier == 0)
	{
		pParam->info.multiplier = 1.0;
	}

	dwLen = GetPrivateProfileString(sSection.ToString(), csKey + scUNIT_DIVISOR, scDEFAULTDIVISOR, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.unit_divisor = atof(csVal);
	if (pParam->info.unit_divisor == 0)
	{
		pParam->info.unit_divisor = 1.0;
	}

	dwLen = GetPrivateProfileString(sSection.ToString(), csKey + scUNIT, scDEFAULTUNIT, csVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename.ToString());
	csVal.ReleaseBuffer(dwLen);
	pParam->info.sUnits = csVal;

	return S_OK;
}

HRESULT SVGigeCameraFileReader::ReadGigeFeatureOverrides()
{
	HRESULT hr = S_OK;

	std::ifstream is;
	is.open(m_rInfo.sFilename.ToString());
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVGigeCameraFileReader.cpp_v  $
 * 
 *    Rev 1.2   15 May 2013 19:35:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 May 2013 10:03:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ReadParams to read the Horizontal and Vertical Binning parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 20:07:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 Apr 2013 16:13:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct merge issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 Apr 2013 16:07:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the ReadCustomParams method to correct a crash in the release build when using boost split with a vector by replacing the vector with a deque
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:42:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Apr 2013 15:53:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ReadCustomParams method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Jan 2013 15:32:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added logic to read camera input parameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2011 09:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2010 08:24:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:09:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 16:51:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/