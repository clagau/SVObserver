//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Jwt.h
/// All Rights Reserved
//******************************************************************************
/// JWT (JSON Web Token) helper for generating and verfiying tokens.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVAuth.h"
#pragma endregion Includes

namespace SvAuth
{
template<class Payload>
class Jwt
{
public:
	void setAlgorithm(JwtAlgorithm);
	JwtAlgorithm getAlgorithm() const;

	void setValidityInSeconds(uint64_t);
	uint64_t getValidityInSeconds() const;
	
	Payload& getPayload();
	const Payload& getPayload() const;

private:
	JwtAlgorithm m_Algorithm;
	uint64_t m_ValidityInSeconds;
	Payload m_Payload;
};

} // namespace SvAuth

#include "Jwt.inl"