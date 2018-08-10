//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file JwtFactory.h
/// All Rights Reserved
//******************************************************************************
/// Factory for JWTs (JSON Web Tokens).
//******************************************************************************

#pragma once

#include <string>

#include <SVAuth/Crypto.h>
#include <SVAuth/Jwt.h>
#include <SVProtoBuf/SVAuth.h>

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

	JwtFactory(SignatureContext);

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
	std::string encode_header(const JwtAlgorithm&);
	std::string encode_payload(const google::protobuf::Message&, uint64_t validity);
	std::string create_signature(const std::string& header_and_payload, const JwtAlgorithm&);

	bool split_token(std::array<std::string, 3>&, const std::string& token);
	bool decode_header(JwtHeader&, const std::string& header64);
	bool decode_payload(JwtStandardFields&, google::protobuf::Message&, const std::string& payload64);
	bool check_timestamps(const JwtStandardFields&);

private:
	SignatureContext m_SignatureContext;
	Crypto m_Crypto;
};

} // namespace SvAuth

#include <SVAuth/JwtFactory.inl>