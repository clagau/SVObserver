//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file JwtFactory.h
/// All Rights Reserved
//******************************************************************************
/// Factory for JWTs (JSON Web Tokens).
//******************************************************************************

#pragma once

#pragma region Includes
#include "Crypto.h"
#include "Jwt.h"
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace google
{
namespace protobuf
{
class Message;
}
}

namespace SvAuth
{
class JwtFactory
{
public:
	struct SignatureContext
	{
		std::string Secret;
		std::string PublicKey;
		std::string PrivateKey;
	};

	explicit JwtFactory(SignatureContext);

	enum ParseResult
	{
		Parse_Success = 0,
		Parse_InvalidFormat = 1,
		Parse_UnsupportedAlgorithm = 2,
		Parse_InvalidSignature = 3,
		Parse_InvalidPayload = 4,
		Parse_Expired = 5,
	};

	template<class Payload>
	ParseResult parse(Jwt<Payload>& jwt, const std::string& token);

	template<class Payload>
	bool generate(std::string& token, const Jwt<Payload>& jwt);

private:
	struct JwtParts
	{
		std::string header_and_payload;
		std::string header;
		std::string payload;
		std::string signature;
	};

	std::string encode_header(JwtAlgorithm);
	std::string encode_payload(const google::protobuf::Message&, uint64_t validity);
	std::string create_signature(const std::string& header_and_payload, JwtAlgorithm);
	bool verify_signature(const JwtParts&, JwtAlgorithm);

	bool split_token(JwtParts&, const std::string& token);
	bool decode_header(JwtHeader&, const std::string& header64);
	bool decode_payload(JwtStandardFields&, google::protobuf::Message&, const std::string& payload64);
	bool check_timestamps(const JwtStandardFields&);

private:
	SignatureContext m_SignatureContext;
	Crypto m_Crypto;
};

} // namespace SvAuth

#include <SVAuthLibrary/JwtFactory.inl>