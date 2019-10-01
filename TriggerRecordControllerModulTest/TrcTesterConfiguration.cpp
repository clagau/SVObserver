//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file TrcTesterConfiguration.cpp
/// All Rights Reserved 
//*****************************************************************************
/// configures the unit test
/// This class will be added also to TriggerRecordControllerReaderModuleTest-project.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LogClass.h"
#include "TrcTesterConfiguration.h"
#include "SVLibrary\SVOINIClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCSTR strTestConfig = _T("Config");
#pragma endregion Declarations

void freeImages(std::vector<MIL_ID> images)
{
	for (MIL_ID id : images)
	{
		MbufFree(id);
	}
}

std::vector<MIL_ID> loadImages(const std::vector<std::string>& fileNames)
{
	std::vector<MIL_ID> retValue(fileNames.size());
	for (int i = 0; i < fileNames.size(); i++)
	{
		MbufRestore(fileNames[i], M_DEFAULT_HOST, &retValue[i]);
		if (M_NULL == retValue[i])
		{
			retValue.clear();
			CString errorStr;
			errorStr.Format(_T("LoadImages failed by image %d with \"%s\""), i, fileNames[i].c_str());
			throw errorStr;
		}
	}
	return retValue;
}

ParamDef::ParamDef(std::string name, variant_t value, int byteSize)
	: m_name(name), m_value(value), m_byteSize(byteSize)
{
	switch (m_value.vt)
	{
		case VT_I4: case VT_INT: m_byteSize = 4; break;
		case VT_R8: m_byteSize = sizeof(double); break;
		default:
			break;
	}

	assert(m_byteSize > 0);
}


void TrcTesterConfiguration::InspectionsDef::recalcRecordSizes()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(m_minRecordSize, m_maxRecordSize);
	m_recordSize = dist(rd);
	std::uniform_int_distribution<int> distInterest(m_minRecordInterestSize, m_maxRecordInterestSize);
	m_recordInterestSize = std::max<int>(0, distInterest(rd));
}

TrcTesterConfiguration::TrcTesterConfiguration(LogClass& rLogClass, SvLib::SVOINIClass iniFile, bool isLocal)
	: m_isLocal(isLocal)
{
	m_NumberOfRuns = iniFile.GetValueInt(_T("General"), _T("NumberOfRuns"), m_NumberOfRuns);
	m_maxTimeSetBufferPerIter = iniFile.GetValueDouble(_T("MaxTime"), _T("SetBufferPerIter"), m_maxTimeSetBufferPerIter);
	m_maxTimeCheckBufferPerBuffer = iniFile.GetValueDouble(_T("MaxTime"), _T("CheckBufferPerBuffer"), m_maxTimeCheckBufferPerBuffer);
	m_maxTimesetAndReadImage = iniFile.GetValueDouble(_T("MaxTime"), _T("SetAndReadImage"), m_maxTimesetAndReadImage);
	m_maxTimesetAndReadValue = iniFile.GetValueDouble(_T("MaxTime"), _T("SetAndReadValue"), m_maxTimesetAndReadValue);
	m_maxSpecifyBufferFactor = iniFile.GetValueInt(_T("General"), _T("MaxSpecifyFactor"), m_maxSpecifyBufferFactor);
	m_SpecifyBufferDiv = iniFile.GetValueInt(_T("General"), _T("SpecifyBufferDiv"), m_SpecifyBufferDiv);

	init(rLogClass);
}

TrcTesterConfiguration::TrcTesterConfiguration(LogClass& rLogClass)
	: m_isLocal(false)
{
	init(rLogClass);
}

TrcTesterConfiguration::~TrcTesterConfiguration()
{
	for (auto imageList : m_imageLists)
	{
		freeImages(imageList);
	}
	m_imageLists.clear();
}

void TrcTesterConfiguration::recalcRecordSizes() 
{ 
	for (auto& rDataVec : m_testData)
	{
		for (auto& rDef : rDataVec)
		{
			rDef.recalcRecordSizes();
		}
	}
}

void TrcTesterConfiguration::init(LogClass& rLogClass)
{
	//get number from SVIM.ini
	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	m_numberOfRecordsAddOne = l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("AdditionalTRNumber"), 5) + 2;
	m_numberOfKeepFreeRecords = m_isLocal ? 0 : l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("NumberOfTRKeepFreeForWriter"), 2);

	for (auto imageFileList : m_imageFileNameLists)
	{
		std::vector<MIL_ID> imageIds;
		try
		{
			imageIds = loadImages(imageFileList);
		}
		catch (CString& e)
		{
			rLogClass.Log(e, LogLevel::Error, LogType::FAIL, __LINE__, strTestConfig);
		}
		if (imageIds.size() == 0)
		{
			rLogClass.Log(_T("setAndReadImage: loadImage failed. 0 handle available!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestConfig);
		}
		else
		{
			m_imageLists.push_back(imageIds);
		}
	}

	m_testData = {{{10, 30, 1, 20, {m_imageLists[0],m_imageLists[1], m_imageLists[2]}, getValueObjectSet(), m_valueSet}},
		{{10,30, 0, 0, {m_imageLists[1], m_imageLists[2]}, getValueObjectSet(), m_valueSet}, {10, 300, 0, 0, {m_imageLists[0]}, getValueObjectSet(), m_valueSet}}};
}
