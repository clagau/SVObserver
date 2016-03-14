//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAccessPointNode
//* .File Name       : $Workfile:   SVAccessPointNode.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:00:38  $
//******************************************************************************

#if !defined(AFX_SVACCESSPOINTNODE_H__BA98421F_1D99_4058_840B_773FC810D11A__INCLUDED_)
#define AFX_SVACCESSPOINTNODE_H__BA98421F_1D99_4058_840B_773FC810D11A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVAccessPointNode  
{
public:
	SVAccessPointNode();
	SVAccessPointNode(const SVAccessPointNode& rhs);
	SVAccessPointNode(long lID, LPCTSTR strName, LPCTSTR m_strNTGroup, bool bForce );
	SVAccessPointNode(long lID, LPCTSTR strName );
	~SVAccessPointNode();
	const SVAccessPointNode& operator=( const SVAccessPointNode& rhs );
	const SVAccessPointNode& CopyData( const SVAccessPointNode& rhs );
	bool m_bHasData;
	long m_lID;
	CString m_strName;
	CString m_strNTGroup;
	bool m_bForcePrompt;
	bool m_bDataCannotChange;
};

#endif // !defined(AFX_SVACCESSPOINTNODE_H__BA98421F_1D99_4058_840B_773FC810D11A__INCLUDED_)

