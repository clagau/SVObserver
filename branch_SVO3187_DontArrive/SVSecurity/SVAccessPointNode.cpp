//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	m_Name = _T("");
	m_NTGroup = _T("Administrators");
}

SVAccessPointNode::~SVAccessPointNode()
{
}

SVAccessPointNode::SVAccessPointNode(const SVAccessPointNode& rRhs)
{
	*this = rRhs;
}

const SVAccessPointNode& SVAccessPointNode::operator=(const SVAccessPointNode& rRhs)
{
	m_lID = rRhs.m_lID;
	m_bForcePrompt = rRhs.m_bForcePrompt;
	m_Name = rRhs.m_Name;
	m_NTGroup = rRhs.m_NTGroup;
	m_bHasData = rRhs.m_bHasData;
	m_bDataCannotChange = rRhs.m_bDataCannotChange;
	return *this;
}

const SVAccessPointNode& SVAccessPointNode::CopyData( const SVAccessPointNode& rRhs )
{
	m_bForcePrompt = rRhs.m_bForcePrompt;
	m_NTGroup = rRhs.m_NTGroup;
	m_bHasData = rRhs.m_bHasData;
	m_bDataCannotChange = rRhs.m_bDataCannotChange;
	return *this;
}

SVAccessPointNode::SVAccessPointNode(long lID, LPCTSTR Name, LPCTSTR NTGroup, bool bForce )
{
	m_lID = lID;
	m_bForcePrompt = bForce;
	m_Name = Name;
	if( nullptr == NTGroup )
	{
		m_NTGroup.clear();
		m_bHasData = false;
	}
	else
	{
		m_NTGroup = NTGroup;
		m_bHasData = true;
	}
	m_bDataCannotChange = false;
}	
