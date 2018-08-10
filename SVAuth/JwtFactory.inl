//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Jwt.h
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <rapidjson/document.h>

#include <SVAuth/Crypto.h>
#include <SVProtoBuf/SVAuth.h>

namespace SvAuth
{

template<class Payload>
JwtFactory::ParseResult JwtFactory::parse(Jwt<Payload>& jwt, const std::string& token)
{
	std::array<std::string, 3> parts;
	if (!split_token(parts, token))
	{
		return Parse_InvalidFormat;
	}

	JwtHeader header;
	if (!decode_header(header, parts[0]))
	{
		return Parse_InvalidFormat;
	}

	JwtAlgorithm algorithm;
	if (!JwtAlgorithm_Parse(header.alg(), &algorithm))
	{
		return Parse_UnsupportedAlgorithm;
	}

	const auto data = parts[0] + "." + parts[1];
	auto signature = create_signature(data, algorithm);
	if (signature != parts[2])
	{
		return Parse_InvalidSignature;
	}
	
	JwtStandardFields standardFields;
	if (!decode_payload(standardFields, jwt.getPayload(), parts[1]))
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
