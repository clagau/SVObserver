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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVAccessPointNode.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:00:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Aug 2005 11:58:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Load Bug by copying only data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 09:21:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSecurityStorageClass to use stl instead of the Microsoft CArray.  Fixed Load problem with SecurityStorage.
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