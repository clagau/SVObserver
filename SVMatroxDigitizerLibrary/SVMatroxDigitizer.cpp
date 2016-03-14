//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizer
//* .File Name       : $Workfile:   SVMatroxDigitizer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxDigitizer.h"
#include "SVMatroxDigitizerInterface.h"

SVMatroxDigitizer::SVMatroxDigitizer()
: m_DigitizerIdentifier(0)
{
}

SVMatroxDigitizer::~SVMatroxDigitizer()
{
	LocalClear();
}

bool SVMatroxDigitizer::operator<( const SVMatroxDigitizer& p_rDigitizer ) const 
{
	return m_DigitizerIdentifier < p_rDigitizer.m_DigitizerIdentifier; 
}

bool SVMatroxDigitizer::empty() const
{
	return (m_DigitizerIdentifier == 0);
}

void SVMatroxDigitizer::LocalClear()
{
	SVMatroxDigitizerInterface::Destroy(*this);
}

