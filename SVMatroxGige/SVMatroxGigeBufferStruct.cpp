//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferStruct
//* .File Name       : $Workfile:   SVMatroxGigeBufferStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:38:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGigeBufferStruct.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"

SVMatroxGigeBufferStruct::SVMatroxGigeBufferStruct()
: m_ulWidth(640)
, m_ulHeight(480)
, m_ulFormat(SVImageFormatMono8)
, m_ulAddress(0)
, m_bIsLocked(false)
{
}

SVMatroxGigeBufferStruct::SVMatroxGigeBufferStruct( const SVMatroxGigeBufferStruct& p_rValue )
{
	if (this != &p_rValue)
	{
		*this = p_rValue;
	}
}

SVMatroxGigeBufferStruct::~SVMatroxGigeBufferStruct()
{
}

const SVMatroxGigeBufferStruct& SVMatroxGigeBufferStruct::operator=( const SVMatroxGigeBufferStruct& p_rValue )
{
	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;
	m_ulFormat = p_rValue.m_ulFormat;
	m_ulAddress = p_rValue.m_ulAddress;
	m_bIsLocked = p_rValue.m_bIsLocked;

	m_Buffer = p_rValue.m_Buffer;

	return *this;
}

bool SVMatroxGigeBufferStruct::IsValid() const
{
	return (m_Buffer.get() != NULL && !m_Buffer.get()->empty());
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeBufferStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:38:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
