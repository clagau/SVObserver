//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxPolarTransformStruct
//* .File Name       : $Workfile:   SVMatroxPolarTransformStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:26:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxPolarTransformStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxPolarTransformStruct::SVMatroxPolarTransformStruct(SVMatroxBuffer& p_rSource)
: m_rSource( p_rSource ),
m_dCenterX( 0.0 ),
m_dCenterY( 0.0 ),
m_dStartRadius( 0.0 ),
m_dEndRadius( 0.0 ),
m_dStartAngle( 0.0 ),
m_dEndAngle( 0.0 ),
m_dDestSizeX( 0.0 ),
m_dDestSizeY( 0.0 )
{
}

SVMatroxPolarTransformStruct::~SVMatroxPolarTransformStruct()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxPolarTransformStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:26:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/