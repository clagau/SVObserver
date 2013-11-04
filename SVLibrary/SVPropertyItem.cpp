// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItem.cpp
// * .File Name       : $Workfile:   SVPropertyItem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:37:52  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyListBox.h"
#include "SVPropertyItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPropertyItem

IMPLEMENT_SERIAL(SVPropertyItem, CObject, 1)

SVPropertyItem::SVPropertyItem(LPCTSTR lpszName/*=NULL*/)
{
	m_strName  = lpszName;
	m_bEditing = FALSE;
	m_pEditWnd = NULL;
}

SVPropertyItem::~SVPropertyItem()
{
}

void SVPropertyItem::Draw(CDC *pDC, CRect &rect)
{
}

BOOL SVPropertyItem::Edit(CWnd* pParent,
													int nItem,
													CRect rect,
													CPoint point,
													UINT nID,
													UINT nChar,
													BOOL bExitOnArrows/*=TRUE*/)
{
	return FALSE;
}

void SVPropertyItem::EndEdit()
{
}

void SVPropertyItem::OnEndEdit(LPLVITEM plvItem)
{
	m_bEditing = FALSE;
	m_pEditWnd = NULL;
}


void SVPropertyItem::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_strName;
	}
	else
	{	// loading code
		ar >> m_strName;
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItem.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:37:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:48:58   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:08   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Dec 2000 11:57:30   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split out several classes into their own files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:40   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   SVPropertyListBox and associated item classes.
 * SVPropertyItem
 * SVPropertyItemCombo
 * SVPropertyItemEdit
 * SVPropertyItemFile
 * SVPropertyItemFont
 * 
 * Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/