//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeSystem
//* .File Name       : $Workfile:   SVMatroxGigeSystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:34  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeSystem.h"
#pragma endregion Includes

SVMatroxGigeSystem::SVMatroxGigeSystem()
: m_Handle(0xFF)
{
}

SVMatroxGigeSystem::SVMatroxGigeSystem( const SVMatroxGigeSystem& p_rValue )
{
	*this = p_rValue;
}

SVMatroxGigeSystem::~SVMatroxGigeSystem()
{
	m_digitizers.clear();
}

const SVMatroxGigeSystem& SVMatroxGigeSystem::operator=( const SVMatroxGigeSystem& p_rValue )
{
	if (this != &p_rValue)
	{
		m_matroxSystemName = p_rValue.m_matroxSystemName;
		m_Name = p_rValue.m_Name;
		m_System = p_rValue.m_System;
		m_Handle = p_rValue.m_Handle;
		m_digitizers = p_rValue.m_digitizers;
	}
	return *this;
}

HRESULT SVMatroxGigeSystem::AddDigitizer(SVMatroxGigeDigitizer& digitizer)
{
	return m_digitizers.Add(digitizer);
}

HRESULT SVMatroxGigeSystem::RemoveDigitizer(unsigned char digitizerHandle)
{
	return m_digitizers.Remove(digitizerHandle);
}

SVMatroxGigeDigitizer& SVMatroxGigeSystem::GetDigitizer(unsigned char digitizerHandle, HRESULT& status )
{
	return m_digitizers.Get(digitizerHandle, status);
}

const SVMatroxGigeDigitizer& SVMatroxGigeSystem::GetDigitizer(unsigned char digitizerHandle, HRESULT& status ) const
{
	return m_digitizers.Get(digitizerHandle, status);
}

bool SVMatroxGigeSystem::IsValidHandle(unsigned char digitizerHandle) const
{
	return m_digitizers.IsValidHandle(digitizerHandle);
}

SVMatroxGigeDigitizerList& SVMatroxGigeSystem::GetDigitizerList()
{
	return m_digitizers;
}

const SVMatroxGigeDigitizerList& SVMatroxGigeSystem::GetDigitizerList() const
{
	return m_digitizers;
}

