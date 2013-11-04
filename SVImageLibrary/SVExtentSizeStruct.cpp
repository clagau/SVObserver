//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentSizeStruct
//* .File Name       : $Workfile:   SVExtentSizeStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentSizeStruct.h"

SVExtentSizeStruct::SVExtentSizeStruct()
{
	Initialize();
}

SVExtentSizeStruct::SVExtentSizeStruct( const SVExtentSizeStruct& p_rsvValue )
{
	*this = p_rsvValue;
}

SVExtentSizeStruct::SVExtentSizeStruct( const SIZE& p_roValue )
{
	*this = p_roValue;
}

SVExtentSizeStruct::SVExtentSizeStruct( double l_dCX, double l_dCY )
{
	m_dCX = l_dCX;
	m_dCY = l_dCY;
}

SVExtentSizeStruct::~SVExtentSizeStruct()
{
	Initialize();
}

const SVExtentSizeStruct& SVExtentSizeStruct::operator = ( const SVExtentSizeStruct& p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_dCX = p_rsvValue.m_dCX;
		m_dCY = p_rsvValue.m_dCY;
	}	
	return *this;
}

const SVExtentSizeStruct& SVExtentSizeStruct::operator = ( const SIZE& p_roValue )
{
	Initialize();

	m_dCX = p_roValue.cx;
	m_dCY = p_roValue.cy;

	return *this;
}

HRESULT SVExtentSizeStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_dCX = 0.0;
	m_dCY = 0.0;

	return l_hrOk;
}

bool SVExtentSizeStruct::operator == ( const SVExtentSizeStruct& p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = m_dCX == p_rsvValue.m_dCX;
	l_bOk = l_bOk && m_dCY == p_rsvValue.m_dCY;

	return l_bOk;
}

bool SVExtentSizeStruct::operator != ( const SVExtentSizeStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = ! ( *this == p_rsvValue );

	return l_bOk;
}

SVExtentSizeStruct::operator SIZE() const
{
	SIZE l_Size;

	l_Size.cx = (long) m_dCX;
	l_Size.cy = (long) m_dCY;

	return l_Size;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentSizeStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:44:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:02:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2005 14:38:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/