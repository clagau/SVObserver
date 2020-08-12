//******************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file WebAppVersionLoader.cpp
/// All Rights Reserved
//******************************************************************************

#include "stdafx.h"

#include <string>
#include <fstream>
#include <streambuf>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <rapidjson/document.h>
#pragma warning(pop)
#include <rapidjson/istreamwrapper.h>

#include "WebAppVersionLoader.h"

#include "SVHttpLibrary/HttpServerSettings.h"


namespace SvOgw
{
static const char* UNKNOWN_VERSION = "<UNKNOWN>";

WebAppVersionLoader::WebAppVersionLoader(const SvHttp::HttpServerSettings& rHttpServerSettings)
	: m_rHttpServerSettings(rHttpServerSettings)
{
}

std::string  WebAppVersionLoader::getVersion() const
{
	const auto& dataDir = m_rHttpServerSettings.DataDir;
	if (dataDir.empty())
	{
		return UNKNOWN_VERSION;
	}

	const auto versionJsonPath = std::filesystem::path(m_rHttpServerSettings.DataDir).append("/version.json");

	std::ifstream versionJsonStream(versionJsonPath.c_str());
	if (!versionJsonStream.is_open())
	{
		return UNKNOWN_VERSION;
	}

	rapidjson::Document doc;
	rapidjson::IStreamWrapper isw(versionJsonStream);
	doc.ParseStream(isw);

	if (!doc.IsObject())
	{
		return UNKNOWN_VERSION;
	}

	auto it = doc.FindMember("version");
	if (it == doc.MemberEnd())
	{
		return UNKNOWN_VERSION;
	}

	return it->value.GetString();
}
}
