//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeSystem
//* .File Name       : $Workfile:   SVMatroxGigeSystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGigeSystem.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeSystem.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:44:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
