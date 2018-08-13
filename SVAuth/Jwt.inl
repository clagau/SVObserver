//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file Jwt.inl
/// All Rights Reserved
//******************************************************************************
/// Not much to see here.
//******************************************************************************

#include <string>
#include "SVProtoBuf/SVAuth.h"

namespace SvAuth
{

template<class Payload>
void Jwt<Payload>::setAlgorithm(JwtAlgorithm v)
{
	m_Algorithm = v;
}

template<class Payload>
JwtAlgorithm Jwt<Payload>::getAlgorithm() const
{
	return m_Algorithm;
}

template<class Payload>
void Jwt<Payload>::setValidityInSeconds(uint64_t v)
{
	m_ValidityInSeconds = v;
}

template<class Payload>
uint64_t Jwt<Payload>::getValidityInSeconds() const
{
	return m_ValidityInSeconds;
}

template<class Payload>
Payload& Jwt<Payload>::getPayload()
{
	return m_Payload;
}

template<class Payload>
const Payload& Jwt<Payload>::getPayload() const
{
	return m_Payload;
}

} // namespace SvAuth
