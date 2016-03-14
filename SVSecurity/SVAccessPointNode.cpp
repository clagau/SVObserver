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

