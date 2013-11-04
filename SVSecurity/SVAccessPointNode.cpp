//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAccessPointNode
//* .File Name       : $Workfile:   SVAccessPointNode.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:00:24  $
//******************************************************************************

// SVAccessPointNode.cpp: implementation of the SVAccessPointNode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SVAccessPointNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVAccessPointNode::SVAccessPointNode()
{
	m_bDataCannotChange = false;
	m_bHasData = false;
	m_lID = 0;
	m_bForcePrompt = true;
	m_strName = _T("");
	m_strNTGroup = _T("Administrators");
}

SVAccessPointNode::~SVAccessPointNode()
{
}

SVAccessPointNode::SVAccessPointNode(const SVAccessPointNode& rhs)
{
	*this = rhs;
}

const SVAccessPointNode& SVAccessPointNode::operator =(const SVAccessPointNode& rhs)
{
	m_lID = rhs.m_lID;
	m_bForcePrompt = rhs.m_bForcePrompt;
	m_strName = rhs.m_strName;
	m_strNTGroup = rhs.m_strNTGroup;
	m_bHasData = rhs.m_bHasData;
	m_bDataCannotChange = rhs.m_bDataCannotChange;
	return *this;
}

const SVAccessPointNode& SVAccessPointNode::CopyData( const SVAccessPointNode& rhs )
{
	m_bForcePrompt = rhs.m_bForcePrompt;
	m_strNTGroup = rhs.m_strNTGroup;
	m_bHasData = rhs.m_bHasData;
	m_bDataCannotChange = rhs.m_bDataCannotChange;
	return *this;
}

SVAccessPointNode::SVAccessPointNode(long lID, LPCTSTR strName, LPCTSTR strNTGroup, bool bForce )
{
	m_lID = lID;
	m_bForcePrompt = bForce;
	m_strName = strName;
	m_strNTGroup = strNTGroup;
	m_bHasData = true;
	m_bDataCannotChange = false;
}	

SVAccessPointNode::SVAccessPointNode( long lID, LPCTSTR strName )
{
	m_strName = strName;
	m_lID = lID;
	m_strNTGroup.Empty();
	m_bHasData = false;
	m_bDataCannotChange = false;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVAccessPointNode.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:00:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Aug 2005 11:58:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Load Bug by copying only data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:06:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/