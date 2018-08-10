//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Crypto.h
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#pragma once

#include <array>
#include <string>

#include <wincrypt.h>

namespace SvAuth
{
class Crypto
{
public:
	Crypto();

	std::string hmac(const std::string& payload, const std::string& secret, ALG_ID alg);

	std::string rsaSign(const std::string& payload, const std::string& privateKey);

	std::string encodeBase64(const std::string&);
	std::string decodeBase64(const std::string&);

	std::string encodeBase64Url(const std::string&);
	std::string decodeBase64Url(const std::string&);

	// Same as encodeBase64Url but without padding.
	std::string encodeBase64Jwt(const std::string&);
	std::string decodeBase64Jwt(const std::string&);

private:
	std::string encode_base64(const std::string&, const std::array<char, 64>&, const std::string&);
	std::string decode_base64(const std::string&, const std::array<char, 64>&, const std::string&);
};

} // namespace SvAuth
