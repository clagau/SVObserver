// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplay
// * .File Name       : $Workfile:   SVMatroxDisplay.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:26  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxDisplay.h"
#include "SVMatroxDisplayInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxDisplay::SVMatroxDisplay()
: m_DisplayIdentifier(0)
{
}

SVMatroxDisplay::~SVMatroxDisplay()
{
//	if( m_DisplayIdentifier != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxDisplay::empty() const
{
	return m_DisplayIdentifier == 0;
}

void SVMatroxDisplay::LocalClear()
{
	SVMatroxDisplayInterface l_Interface;
	l_Interface.Destroy( *this );
}


