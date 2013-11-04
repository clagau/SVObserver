//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentPointStruct
//* .File Name       : $Workfile:   SVExtentPointStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentPointStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVExtentPointStruct::SVExtentPointStruct()
{
	Initialize();
}

SVExtentPointStruct::SVExtentPointStruct( const SVExtentPointStruct &p_rsvValue )
{
	*this = p_rsvValue;
}

SVExtentPointStruct::SVExtentPointStruct( const POINT &p_roValue )
{
	*this = p_roValue;
}

SVExtentPointStruct::SVExtentPointStruct( double l_dValueX, double l_dValueY )
{
	Initialize();

	m_dPositionX = l_dValueX;
	m_dPositionY = l_dValueY;
}

SVExtentPointStruct::~SVExtentPointStruct()
{
	Initialize();
}

const SVExtentPointStruct &SVExtentPointStruct::operator=( const SVExtentPointStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_dPositionX = p_rsvValue.m_dPositionX;
		m_dPositionY = p_rsvValue.m_dPositionY;
	}	
	return *this;
}

const SVExtentPointStruct &SVExtentPointStruct::operator=( const POINT &p_roValue )
{
	Initialize();

	m_dPositionX = p_roValue.x;
	m_dPositionY = p_roValue.y;
	
	return *this;
}

HRESULT SVExtentPointStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_dPositionX = 0.0;
	m_dPositionY = 0.0;

	return l_hrOk;
}

bool SVExtentPointStruct::operator==( const SVExtentPointStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = m_dPositionX == p_rsvValue.m_dPositionX;
	l_bOk = l_bOk && m_dPositionY == p_rsvValue.m_dPositionY;

	return l_bOk;
}

bool SVExtentPointStruct::operator!=( const SVExtentPointStruct &p_rsvValue ) const
{
	bool l_bOk = true;

	l_bOk = ! ( *this == p_rsvValue );

	return l_bOk;
}

SVExtentPointStruct::operator POINT() const
{
	POINT l_oPoint;

	l_oPoint.x = (long)m_dPositionX;
	l_oPoint.y = (long)m_dPositionY;

	return l_oPoint;
}

const SVExtentPointStruct& SVExtentPointStruct::operator -= (const SVExtentPointStruct& p_rPoint)
{
	m_dPositionX -= p_rPoint.m_dPositionX;
	m_dPositionY -= p_rPoint.m_dPositionY;
	return *this;
}

const SVExtentPointStruct& SVExtentPointStruct::operator += (const SVExtentPointStruct& p_rPoint)
{
	m_dPositionX += p_rPoint.m_dPositionX;
	m_dPositionY += p_rPoint.m_dPositionY;
	return *this;
}

SVExtentPointStruct operator - (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX - p_rPoint2.m_dPositionX;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY - p_rPoint2.m_dPositionY;
	return l_pt;
}

SVExtentPointStruct operator + (const SVExtentPointStruct& p_rPoint1, const SVExtentPointStruct& p_rPoint2)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX + p_rPoint2.m_dPositionX;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY + p_rPoint2.m_dPositionY;
	return l_pt;
}

SVExtentPointStruct operator / (const SVExtentPointStruct& p_rPoint1, double p_Constant)
{
	SVExtentPointStruct l_pt;
	l_pt.m_dPositionX = p_rPoint1.m_dPositionX / p_Constant;
	l_pt.m_dPositionY = p_rPoint1.m_dPositionY / p_Constant;
	return l_pt;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentPointStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:40:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2012 12:19:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent structures to provide a hit test methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 10:02:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2005 14:37:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operators
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:03:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
