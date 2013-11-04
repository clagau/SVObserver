//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageRotateStruct
//* .File Name       : $Workfile:   SVMatroxImageRotateStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:21:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageRotateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxImageRotateStruct::SVMatroxImageRotateStruct(SVMatroxBuffer& p_rSource)
: m_rSource(p_rSource),
m_dAngle(0.0),
m_dSrcCenX( 0.0 ),
m_dSrcCenY( 0.0 ),
m_dDstCenX( 0.0 ),
m_dDstCenY( 0.0 )
{
}

SVMatroxImageRotateStruct::~SVMatroxImageRotateStruct()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxImageRotateStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:21:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/