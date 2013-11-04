// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVStateClass
// * .File Name       : $Workfile:   SVStateClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVStateClass.h"



//##ModelId=3AC0E6A6003F
SVStateClass::SVStateClass()
{
	mdwState = 0;
}

//##ModelId=3AC0E69D00FA
SVStateClass::~SVStateClass()
{
}

//##ModelId=3AC0E47C009C
BOOL SVStateClass::GetState(DWORD * pdwState)
{
	*pdwState = mdwState;
	return TRUE;
}

//##ModelId=3AC0E50D00FA
BOOL SVStateClass::SetState(DWORD dwState)
{
	mdwState = dwState;
	return TRUE;
}

//##ModelId=3AC0ED3D02AF
BOOL SVStateClass::AddState(DWORD dwState)
{
	mdwState |= dwState;
	return TRUE;
}

//##ModelId=3AC0ED850157
BOOL SVStateClass::RemoveState(DWORD dwState)
{
	mdwState &= ~dwState;
	return TRUE;
}

//##ModelId=3AB7CF850167
BOOL SVStateClass::CheckState(DWORD dwState)
{
	if (mdwState != dwState)
		return FALSE;
	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVStateClass.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2001 10:23:48   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  92
 * SCR Title:  Do not clear current inspection configuration when returning to Logo Screen
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Filled methods with source code using Functional Requirement FR0010103.0530.01
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jun 2001 10:09:58   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  92
 * SCR Title:  Do not clear current inspection configuration when returning to Logo Screen
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Empty classes created by Rational Rose.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/