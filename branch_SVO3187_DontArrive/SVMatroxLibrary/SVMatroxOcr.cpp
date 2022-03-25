// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcr
// * .File Name       : $Workfile:   SVMatroxOcr.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:22:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxOcr.h"
#include "SVMatroxOcrInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcr::SVMatroxOcr()
: m_OcrFontID(0), m_bVerify(false)
{
}

SVMatroxOcr::~SVMatroxOcr()
{
//	if( m_OcrFontID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxOcr::empty() const
{
	return m_OcrFontID == 0;
}

void SVMatroxOcr::LocalClear()
{
	SVMatroxOcrInterface l_Interface;
	l_Interface.Destroy( *this );
}

bool SVMatroxOcr::operator==( const SVMatroxOcr& p_rhs ) const
{
	return m_OcrFontID == p_rhs.m_OcrFontID;
}

