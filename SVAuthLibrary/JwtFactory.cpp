//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Jwt.cpp
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include "stdafx.h"

#include <chrono>

#include "SVAuthLibrary/JwtFactory.h"
#include "SVProtoBuf/Protobuf2Rapidjson.h"

using namespace Protobuf2Rapidjson;

namespace SvAuth
{

JwtFactory::JwtFactory(SignatureContext ctx)
	: m_SignatureContext(std::move(ctx))
{
}

std::string JwtFactory::encode_header(JwtAlgorithm alg)
{
	JwtHeader header;
	header.set_typ("JWT");
	header.set_alg(JwtAlgorithm_Name(alg));

	const auto str = encode_to_string(header);
	return m_Crypto.encodeBase64Jwt(str);
}

std::string JwtFactory::encode_payload(
	const google::protobuf::Message& payload,
	uint64_t validity)
{
	JwtStandardFields standards;

	const auto now = std::chrono::seconds(std::time(NULL)).count();
	standards.set_iat(now);
	standards.set_nbf(now);
	standards.set_exp(now + validity);

	// TODO: set all other important standard fields!

	rapidjson::Document json;
	encode_to_json_doc(standards, &json);
	encode_to_json_doc(payload, &json);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json.Accept(writer);

	return m_Crypto.encodeBase64Jwt(buffer.GetString());
}

std::string JwtFactory::create_signature(
	const std::string& header_and_payload,
	JwtAlgorithm algorithm)
{
	std::string signature;
	switch (algorithm)
	{
		case JwtAlgorithm::HS256:
			if (m_SignatureContext.Secret.empty())
			{
				throw std::runtime_error("No secret configured");
			}
			signature = m_Crypto.hmac(header_and_payload, m_SignatureContext.Secret, CALG_SHA_256);
			break;

		case JwtAlgorithm::RS256:
			if (m_SignatureContext.PrivateKey.empty())
			{
				throw std::runtime_error("No private key configured");
			}
			signature = m_Crypto.rsaSign(header_and_payload, m_SignatureContext.PrivateKey);
			break;

		default:
			throw std::runtime_error("Unsupported JWT algorithm");
			break;
	}

	return m_Crypto.encodeBase64Jwt(signature);
}

bool JwtFactory::verify_signature(
	const JwtParts& parts,
	JwtAlgorithm algorithm)
{

	switch (algorithm)
	{
		case JwtAlgorithm::HS256:
		{
			const auto signature = create_signature(parts.header_and_payload, algorithm);
			return signature == parts.signature;
		}

		case JwtAlgorithm::RS256:
		{
			const auto& publicKey = m_SignatureContext.PublicKey;
			if (publicKey.empty())
			{
				throw std::runtime_error("No private key configured");
			}
			const auto signature = m_Crypto.decodeBase64Jwt(parts.signature);
			return m_Crypto.rsaVerify(parts.header_and_payload, signature, publicKey);
		}

		default:
			throw std::runtime_error("Unsupported JWT algorithm");
	}
}

bool JwtFactory::split_token(JwtParts& parts, const std::string& token)
{
	auto header_end = token.find('.');
	if (header_end == std::string::npos)
	{
		return false;
	}
	auto payload_end = token.find('.', header_end + 1);
	if (payload_end == std::string::npos)
	{
		return false;
	}
	parts.header = token.substr(0, header_end);
	parts.payload = token.substr(header_end + 1, payload_end - header_end - 1);
	parts.header_and_payload = token.substr(0, payload_end);
	parts.signature = token.substr(payload_end + 1);

	return true;
}

bool JwtFactory::decode_header(JwtHeader& header, const std::string& header64)
{
	const auto json = m_Crypto.decodeBase64Jwt(header64);

	DecodeContext ctx;
	ctx.ignoreUnknownFields = false;
	return decode_from_cstring(json.data(), json.size(), &header, ctx);
}

bool JwtFactory::decode_payload(
	JwtStandardFields& standardFields,
	google::protobuf::Message& payload,
	const std::string& payload64)
{
	const auto json = m_Crypto.decodeBase64Jwt(payload64);

	DecodeContext ctx1;
	ctx1.ignoreUnknownFields = true;
	if (!decode_from_cstring(json.data(), json.size(), &standardFields, ctx1))
	{
		return false;
	}

	DecodeContext ctx2;
	ctx2.ignoreUnknownFields = true;
	if (!decode_from_cstring(json.data(), json.size(), &payload, ctx2))
	{
		return false;
	}

	return true;
}

bool JwtFactory::check_timestamps(const JwtStandardFields& fields)
{
	const auto leeway = 10; // allow a margin of a few seconds to account for clock skew
	const auto now = static_cast<uint64_t>(std::chrono::seconds(std::time(NULL)).count());

	if (fields.exp() > 0 && fields.exp() < now - leeway)
	{
		return false;
	}

	if (fields.nbf() > 0 && fields.nbf() > now + leeway)
	{
		return false;
	}

	return true;
}

} // namespace SvAuth
