// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIServerStruct
// * .File Name       : $Workfile:   SVCIServerStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCIServerStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all attribute data to desired defaults.
*/
SVCIServerStruct::SVCIServerStruct()
{
	m_hCorServer = CORHANDLE_NULL;
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVCIServerStruct::SVCIServerStruct( const SVCIServerStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCIServerStruct::~SVCIServerStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVCIServerStruct &SVCIServerStruct::operator=( const SVCIServerStruct &p_rValue )
{
	m_csName = (LPCTSTR)p_rValue.m_csName;

	m_hCorServer = p_rValue.m_hCorServer;

	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIServerStruct.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 08:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
