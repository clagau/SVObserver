//******************************************************************************
/// \copyright (c) 2017,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file Jwt.h
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/optional.hpp>
#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 5054)
#include <rapidjson/document.h>
#pragma warning(pop)

#include "Crypto.h"
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvAuth
{

template<class Payload>
JwtFactory::ParseResult JwtFactory::parse(Jwt<Payload>& jwt, const std::string& token)
{
	JwtParts parts;
	if (!split_token(parts, token))
	{
		return Parse_InvalidFormat;
	}

	JwtHeader header;
	if (!decode_header(header, parts.header))
	{
		return Parse_InvalidFormat;
	}

	JwtAlgorithm algorithm;
	if (!JwtAlgorithm_Parse(header.alg(), &algorithm))
	{
		return Parse_UnsupportedAlgorithm;
	}

	if (!verify_signature(parts, algorithm))
	{
		return Parse_InvalidSignature;
	}

	JwtStandardFields standardFields;
	if (!decode_payload(standardFields, jwt.getPayload(), parts.payload))
	{
		return Parse_InvalidPayload;
	}

	if (!check_timestamps(standardFields))
	{
		return Parse_Expired;
	}

	// TODO: check nonce to protect against replay attacks
	// NOTE: we already check iat and exp, so we are a bit safe already.
	//       but we could restrict the nonce of certain token for one-time use only.
	//       e.g. access token, but we might run into trouble in real-life (flaky
	//       network connections and stuff...)

	jwt.setAlgorithm(algorithm);
	jwt.setValidityInSeconds(standardFields.exp() - standardFields.iat());

	return Parse_Success;
}

template<class Payload>
bool JwtFactory::generate(std::string& token, const Jwt<Payload>& jwt)
{
	const auto header = encode_header(jwt.getAlgorithm());
	const auto payload = encode_payload(jwt.getPayload(), jwt.getValidityInSeconds());
	token = header + "." + payload;

	auto signature = create_signature(token, jwt.getAlgorithm());
	token += "." + signature;

	return true;
}

} // namespace SvAuth
