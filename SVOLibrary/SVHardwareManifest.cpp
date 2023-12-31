//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHardwareManifest
//* .File Name       : $Workfile:   SVHardwareManifest.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   23 Oct 2014 13:27:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVHardwareManifest.h"
#include "Definitions/TriggerType.h"
#pragma endregion Includes

constexpr const char* cSvimTriggerSourceIoBoard = _T("IO_Board_1");
constexpr const char* cSvimSoftwareTriggerSource = _T("SoftwareTrigger_1");
constexpr const char* cSvimHardwareTriggerSource = _T("HardwareTrigger");
constexpr const char* cSvimCameraTriggerSource = _T("CameraTrigger");
constexpr const char* cSvimDigName = _T(".Dig_");

typedef std::set<SVIMProductEnum> SVIMProductEnumSet;
typedef std::map<SVIMProductEnum, SVIMProductEnumSet> SVIMProductCompatibilityList;

const SVHardwareManifest::SVIMTypeMap SVHardwareManifest::m_SVIMTypeMap
{
	// SVIMProductEnum,                             SUPPORTED, DIGITIZERS, TRIGGERS
	{ SVIM_PRODUCT_X2_GD1A,			SVIMTypeInfoStruct(true,      1,			1)},
	{ SVIM_PRODUCT_X2_GD1A_COLOR,	SVIMTypeInfoStruct(false,     1,			1)},
	{ SVIM_PRODUCT_X2_GD2A,			SVIMTypeInfoStruct(false,     2,			2)},
	{ SVIM_PRODUCT_X2_GD2A_COLOR,	SVIMTypeInfoStruct(false,     2,			2)},
	{ SVIM_PRODUCT_X2_GD4A,			SVIMTypeInfoStruct(false,     4,			3)},
	{ SVIM_PRODUCT_X2_GD4A_COLOR,	SVIMTypeInfoStruct(false,     4,			3)},
	{ SVIM_PRODUCT_X2_GD8A,			SVIMTypeInfoStruct(true,      8,			3)},
	{ SVIM_PRODUCT_X2_GD8A_COLOR,	SVIMTypeInfoStruct(false,     8,			3)},
	{ SVIM_PRODUCT_NEO,				SVIMTypeInfoStruct(true,      8,			4) },
	{ SVIM_PRODUCT_ETHERCATIO,		SVIMTypeInfoStruct(true,      8,			4) }
};

SVHardwareManifest& SVHardwareManifest::Instance()
{
	static SVHardwareManifest l_Object;
	return l_Object;
}

SVHardwareManifest::SVHardwareManifest()
{}

SVHardwareManifest::~SVHardwareManifest()
{
	Shutdown();
}

void SVHardwareManifest::Startup()
{
	///Maximum number of triggers is 4
	m_TriggerDeviceParams = SVTriggerDeviceParamsVector
	{
		{_T("HardwareTrigger.Dig_0"), 0},
		{_T("HardwareTrigger.Dig_1"), 1},
		{_T("HardwareTrigger.Dig_2"), 2},
		{_T("HardwareTrigger.Dig_3"), 3},
		{_T("IO_Board_1.Dig_0"), 0},
		{_T("IO_Board_1.Dig_1"), 1},
		{_T("IO_Board_1.Dig_2"), 2},
		{_T("IO_Board_1.Dig_3"), 3},
		{_T("SoftwareTrigger_1.Dig_0"), 0},
		{_T("SoftwareTrigger_1.Dig_1"), 1},
		{_T("SoftwareTrigger_1.Dig_2"), 2},
		{_T("SoftwareTrigger_1.Dig_3"), 3},
		{_T("CameraTrigger.Dig_0"), 0},
		{_T("CameraTrigger.Dig_1"), 1},
		{_T("CameraTrigger.Dig_2"), 2},
		{_T("CameraTrigger.Dig_3"), 3},
	};

	m_AcquisitionDeviceParams = SVAcquisitionConstructParamsVector
	{
		{_T("File.Dig_0"),  0, 0,  1, 1},
		{_T("File.Dig_1"),  0, 1,  1, 1},
		{_T("File.Dig_2"),  0, 2,  1, 1},
		{_T("File.Dig_3"),  0, 3,  1, 1},
		{_T("File.Dig_4"),  0, 4,  1, 1},
		{_T("File.Dig_5"),  0, 5,  1, 1},
		{_T("File.Dig_6"),  0, 6,  1, 1},
		{_T("File.Dig_7"),  0, 7,  1, 1},
		{_T("File.Dig_8"),  0, 8,  1, 1},
		{_T("File.Dig_9"),  0, 9,  1, 1},
		{_T("File.Dig_10"), 0, 10, 1, 1},
		{_T("File.Dig_11"), 0, 11, 1, 1},
		{_T("Matrox_GIGE.Dig_0"),  0, 0,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_1"),  0, 1,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_2"),  0, 2,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_3"),  0, 3,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_4"),  0, 4,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_5"),  0, 5,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_6"),  0, 6,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_7"),  0, 7,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_8"),  0, 8,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_9"),  0, 9,  1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_10"), 0, 10, 1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
		{_T("Matrox_GIGE.Dig_11"), 0, 11, 1, 1, SVLutInfo(1, 255, 254, SVDefaultLutTransform(SVLutTransformOperationNormal())), 1},
	};
}

void SVHardwareManifest::Shutdown()
{
	m_TriggerDeviceParams.clear();
	m_AcquisitionDeviceParams.clear();
}

const SVTriggerDeviceParamsVector& SVHardwareManifest::GetTriggerDeviceParams() const
{
	return m_TriggerDeviceParams;
}

const SVAcquisitionConstructParamsVector& SVHardwareManifest::GetAcquisitionDeviceParams() const
{
	return m_AcquisitionDeviceParams;
}

SVIMTypeInfoStruct SVHardwareManifest::GetSVIMTypeInfo(SVIMProductEnum p_ID)
{
	SVIMTypeMap::const_iterator it = m_SVIMTypeMap.find(p_ID);
	if (it != m_SVIMTypeMap.end())
	{
		return it->second;
	}
	return SVIMTypeInfoStruct(false, 0, 0);
}

std::string SVHardwareManifest::BuildTriggerDeviceName(SVIMProductEnum productType, int digitizerIndex, SvDef::TriggerType triggerType)
{
	std::string Result;

	switch (triggerType)
	{
		case SvDef::TriggerType::HardwareTrigger:
		{
			switch (productType)
			{
				case SVIM_PRODUCT_X2_GD1A:
				case SVIM_PRODUCT_X2_GD1A_COLOR:
				case SVIM_PRODUCT_X2_GD2A:
				case SVIM_PRODUCT_X2_GD2A_COLOR:
				case SVIM_PRODUCT_X2_GD4A:
				case SVIM_PRODUCT_X2_GD4A_COLOR:
				case SVIM_PRODUCT_X2_GD8A:
				case SVIM_PRODUCT_X2_GD8A_COLOR:
				{
					Result = SVHardwareManifest::BuildIOBoardTriggerDeviceName(digitizerIndex);
					break;
				}
				case SVIM_PRODUCT_NEO:
				case SVIM_PRODUCT_ETHERCATIO:
				{
					Result = SVHardwareManifest::BuildHardwareTriggerDeviceName(digitizerIndex);
					break;
				}

				default:
				{
					break;
				}
			}
			break;
		}
		case SvDef::TriggerType::SoftwareTrigger:
		{
			Result = SVHardwareManifest::BuildSoftwareTriggerDeviceName(digitizerIndex);
			break;
		}
		case SvDef::TriggerType::CameraTrigger:
		{
			Result = SVHardwareManifest::BuildAcquisitionTriggerDeviceName(digitizerIndex);
			break;
		}
	}
	return Result;
}

std::string SVHardwareManifest::BuildSoftwareTriggerDeviceName(int iDig)
{
	return std::format("{}{}{}", cSvimSoftwareTriggerSource, cSvimDigName, iDig);
}

std::string SVHardwareManifest::BuildAcquisitionTriggerDeviceName(int iDig)
{
	return std::format("{}{}{}", cSvimCameraTriggerSource, cSvimDigName, iDig);
}

std::string SVHardwareManifest::BuildIOBoardTriggerDeviceName(int iDig)
{
	return std::format("{}{}{}", cSvimTriggerSourceIoBoard, cSvimDigName, iDig);
}

std::string SVHardwareManifest::BuildHardwareTriggerDeviceName(int iDig)
{
	return std::format("{}{}{}", cSvimHardwareTriggerSource, cSvimDigName, iDig);
}

bool SVHardwareManifest::IsValidProductType(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 10> list
	{
		SVIM_PRODUCT_X2_GD1A_COLOR,
		SVIM_PRODUCT_X2_GD2A_COLOR,
		SVIM_PRODUCT_X2_GD4A_COLOR,
		SVIM_PRODUCT_X2_GD8A_COLOR,
		SVIM_PRODUCT_X2_GD1A,
		SVIM_PRODUCT_X2_GD2A,
		SVIM_PRODUCT_X2_GD4A,
		SVIM_PRODUCT_X2_GD8A,
		SVIM_PRODUCT_NEO,
		SVIM_PRODUCT_ETHERCATIO,
	};

	auto it = std::find(list.begin(), list.end(), productType);
	return (it != list.end());
}

bool SVHardwareManifest::IsCompatible(SVIMProductEnum ConfigType, SVIMProductEnum ProductType)
{
	static const SVIMProductCompatibilityList CompatibleList
	{
		{SVIM_PRODUCT_X2_GD1A_COLOR, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A, SVIM_PRODUCT_X2_GD1A}},
		{SVIM_PRODUCT_X2_GD2A_COLOR, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A}},
		{SVIM_PRODUCT_X2_GD4A_COLOR, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A}},
		{SVIM_PRODUCT_X2_GD8A_COLOR, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A}},

		{SVIM_PRODUCT_X2_GD1A, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A, SVIM_PRODUCT_ETHERCATIO}},
		{SVIM_PRODUCT_X2_GD2A, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A}},
		{SVIM_PRODUCT_X2_GD4A, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A}},
		{SVIM_PRODUCT_X2_GD8A, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_ETHERCATIO}},
		{SVIM_PRODUCT_ETHERCATIO, SVIMProductEnumSet{SVIM_PRODUCT_X2_GD8A, SVIM_PRODUCT_X2_GD1A}},
	};


	bool l_bRet = false;

	if (ProductType == ConfigType)
	{
		l_bRet = true;
	}
	else
	{
		SVIMProductCompatibilityList::const_iterator dstIt = CompatibleList.find(ConfigType);
		if (dstIt != CompatibleList.end())
		{
			const SVIMProductEnumSet& l_productSet = dstIt->second;
			SVIMProductEnumSet::const_iterator it = l_productSet.find(ProductType);
			l_bRet = (it != l_productSet.end());
		}
	}
	return l_bRet;
}

bool SVHardwareManifest::IsColorSystem(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 4> colorList
	{
		SVIM_PRODUCT_X2_GD1A_COLOR,
		SVIM_PRODUCT_X2_GD2A_COLOR,
		SVIM_PRODUCT_X2_GD4A_COLOR,
		SVIM_PRODUCT_X2_GD8A_COLOR,
	};

	auto it = std::find(colorList.begin(), colorList.end(), productType);
	return (it != colorList.end());
}

bool SVHardwareManifest::isDiscreteIOSystem(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 5> discreteIOList
	{
		SVIM_PRODUCT_X2_GD1A,
		SVIM_PRODUCT_X2_GD2A,
		SVIM_PRODUCT_X2_GD4A,
		SVIM_PRODUCT_X2_GD8A,
		SVIM_PRODUCT_ETHERCATIO,
	};

	auto it = std::find(discreteIOList.begin(), discreteIOList.end(), productType);
	return (it != discreteIOList.end());
}

bool SVHardwareManifest::isPlcSystem(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 5> plcList
	{
		SVIM_PRODUCT_NEO,
	};

	auto it = std::find(plcList.begin(), plcList.end(), productType);
	return (it != plcList.end());
}

bool SVHardwareManifest::IsMatroxGige(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 10> matroxGigeList
	{
		SVIM_PRODUCT_X2_GD1A,
		SVIM_PRODUCT_X2_GD1A_COLOR,
		SVIM_PRODUCT_X2_GD2A,
		SVIM_PRODUCT_X2_GD2A_COLOR,
		SVIM_PRODUCT_X2_GD4A,
		SVIM_PRODUCT_X2_GD4A_COLOR,
		SVIM_PRODUCT_X2_GD8A,
		SVIM_PRODUCT_X2_GD8A_COLOR,
		SVIM_PRODUCT_NEO,
		SVIM_PRODUCT_ETHERCATIO
	};

	auto it = std::find(matroxGigeList.begin(), matroxGigeList.end(), productType);
	return (it != matroxGigeList.end());
}

bool SVHardwareManifest::IsDigitalSVIM(SVIMProductEnum productType)
{
	constexpr std::array<SVIMProductEnum, 10> digitalList
	{
		SVIM_PRODUCT_X2_GD1A,
		SVIM_PRODUCT_X2_GD1A_COLOR,
		SVIM_PRODUCT_X2_GD2A,
		SVIM_PRODUCT_X2_GD2A_COLOR,
		SVIM_PRODUCT_X2_GD4A,
		SVIM_PRODUCT_X2_GD4A_COLOR,
		SVIM_PRODUCT_X2_GD8A,
		SVIM_PRODUCT_X2_GD8A_COLOR,
		SVIM_PRODUCT_NEO,
		SVIM_PRODUCT_ETHERCATIO,
	};

	auto it = std::find(digitalList.begin(), digitalList.end(), productType);
	return (it != digitalList.end());
}
