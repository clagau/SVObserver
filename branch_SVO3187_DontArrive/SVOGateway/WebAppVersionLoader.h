//******************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file WebAppVersionLoader.h
/// All Rights Reserved
//******************************************************************************
/// Tool for reading the WebApp version.
//******************************************************************************

#pragma once

#include <string>

namespace SvHttp
{
struct HttpServerSettings;
}

namespace SvOgw
{
class WebAppVersionLoader
{
public:
	explicit WebAppVersionLoader(const SvHttp::HttpServerSettings&);

	std::string getVersion() const;

private:
	const SvHttp::HttpServerSettings& m_rHttpServerSettings;
};
}
