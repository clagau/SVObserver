//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file Crypto.h
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#pragma once

#pragma region Includes
#include <array>
#include <wincrypt.h>
#pragma endregion Includes

namespace SvAuth
{
class Crypto
{
public:
	std::string md5sum(const std::string& buf);

	std::string hmac(const std::string& payload, const std::string& secret, ALG_ID alg);

	std::string rsaSign(const std::string& payload, const std::string& privateKey);
	bool rsaVerify(const std::string& payload, const std::string& signature, const std::string& publicKey);

	std::string encodeBase64(const std::string&);

	// Same as encodeBase64Url but without padding.
	std::string encodeBase64Jwt(const std::string&);
	std::string decodeBase64Jwt(const std::string&);

private:
	std::string encode_base64(const std::string&, const std::array<char, 64>&, const std::string&);
	std::string decode_base64(const std::string&, const std::array<char, 64>&, const std::string&);
};

} // namespace SvAuth
