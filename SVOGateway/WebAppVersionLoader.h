//******************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
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
	WebAppVersionLoader(const SvHttp::HttpServerSettings&);

	std::string getVersion() const;

private:
	const SvHttp::HttpServerSettings& m_rHttpServerSettings;
};
}
