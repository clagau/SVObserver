//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGraphics
//* .File Name       : $Workfile:   SVMatroxGraphics.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:17:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGraphics.h"
#include "SVMatroxGraphicsInterface.h"

SVMatroxGraphics::SVMatroxGraphics()
:m_GraphicsId(0)
{
}

SVMatroxGraphics::~SVMatroxGraphics()
{
}

bool SVMatroxGraphics::empty() const
{
	return m_GraphicsId == 0;
}

void SVMatroxGraphics::LocalClear()
{
	SVMatroxGraphicsInterface l_Interface;

	l_Interface.Destroy( *this );
}


SVMatroxGraphics& SVMatroxGraphics::operator=( const SVMatroxGraphics& p_rhs )
{
	m_GraphicsId = p_rhs.m_GraphicsId;

	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxGraphics.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:17:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:36:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
