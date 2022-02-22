//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LoadConfigWarning.cpp
/// This file contains methods to check the loading configuration version and add display warnings if needed.
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "LoadConfigWarning.h"
#include "SVLibrary/SVOINIClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

struct checkStruct
{
	checkStruct() = default;
	checkStruct(const std::string& rText, unsigned long minVersion, unsigned long maxVersion) 
		: m_text(rText)
		,m_minConfigVersion(minVersion)
		,m_maxConfigVersion(maxVersion)
	{}

	std::string m_text;
	unsigned long m_minConfigVersion = 0;
	unsigned long m_maxConfigVersion = 0;
};

SvSyl::SVVersionUnion convertToVersion(std::string versionString)
{
	uint8_t majorV = 0;
	uint8_t minorV = 0;
	if (2 == sscanf_s(versionString.c_str(), "%hhu.%hhu", &majorV, &minorV))
	{
		SvSyl::SVVersionUnion version;
		version.m_VersionParts.m_Major = majorV;
		version.m_VersionParts.m_Minor = minorV;
		return version;
	}
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_ConfigWarnSyntaxError, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

std::vector<checkStruct> setValueMap(unsigned long svoVersion)
{
	SvLib::SVOINIClass svimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	std::string warningFilePath = SvStl::GlobalPath::Inst().GetConfigWarningFilename();
	if (warningFilePath.empty())
	{	//if no path set, do no check
		return{};
	}

	std::vector<checkStruct> checkVec;
	unsigned long svoVersionMod = (0x00FFFF00 & svoVersion);

	try
	{
		boost::property_tree::ptree pt1;
		boost::property_tree::read_xml(warningFilePath, pt1);
		const auto& root = pt1.get_child("Root");
		for (const auto& node : root)
		{
			const boost::property_tree::ptree& rSubtree = node.second;

			if (node.first == "element")
			{
				std::string minSVO = rSubtree.get<std::string>("versions.<xmlattr>.minSVO", "");
				if (!minSVO.empty())
				{
					auto version = convertToVersion(minSVO);
					if (version.m_Version > svoVersionMod)
					{	//discard entry because it is not for this SVO-version.
						continue;
					}
				}
				std::string maxSVO = rSubtree.get<std::string>("versions.<xmlattr>.maxSVO", "");
				if (!maxSVO.empty())
				{
					auto version = convertToVersion(maxSVO);
					if (version.m_Version < svoVersionMod)
					{	//discard entry because it is not for this SVO-version.
						continue;
					}
				}
				std::string minConfig = rSubtree.get<std::string>("versions.<xmlattr>.minConfig", "");
				SvSyl::SVVersionUnion minVersion;
				if (!minConfig.empty())
				{
					minVersion = convertToVersion(minConfig);
				}
				std::string maxConfig = rSubtree.get<std::string>("versions.<xmlattr>.maxConfig", "");
				SvSyl::SVVersionUnion maxVersion;
				if (!maxConfig.empty())
				{
					maxVersion = convertToVersion(maxConfig);
				}
				std::string text = rSubtree.get<std::string>("en", "");
				if (text.empty())
				{
					SvDef::StringVector msgList;
					msgList.emplace_back(warningFilePath);
					SvStl::MessageManager Exception(SvStl::MsgType::Data);
					Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_ConfigWarnFileError, msgList, SvStl::SourceFileParams(StdMessageParams));
					Exception.Throw();
				}
				checkVec.emplace_back(text, minVersion.m_Version, maxVersion.m_Version);
			}
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(rExp.getMessage());
	}
	catch (...)
	{
		SvDef::StringVector msgList;
		msgList.emplace_back(warningFilePath);
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_ConfigWarnFileError, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return std::move(checkVec);
};

void checkVersionAndDisplayWarnings(unsigned long svoVersion, unsigned long configVersion)
{
	static const std::vector<checkStruct> s_ValueMap = setValueMap(svoVersion);
	unsigned long configVersionMod = (0x00FFFF00 & configVersion);
	auto findIter = std::find_if(s_ValueMap.begin(), s_ValueMap.end(), [&configVersionMod](const auto& rEntry) 	{ 
		return (0 < rEntry.m_minConfigVersion && configVersionMod >= rEntry.m_minConfigVersion) &&
		(0 < rEntry.m_maxConfigVersion && configVersionMod <= rEntry.m_maxConfigVersion); });
	
	if (s_ValueMap.end() != findIter)
	{
		SvDef::StringVector msgList;
		msgList.emplace_back(findIter->m_text);
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
}
