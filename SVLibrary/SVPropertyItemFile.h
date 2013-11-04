// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.h
// * .File Name       : $Workfile:   SVPropertyItemFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:47:00  $
// ******************************************************************************

#ifndef PROPERTYITEMFILE_H
#define PROPERTYITEMFILE_H

#include "SVPropertyItem.h"
#include "SVFileEditCtrl.h"

class SVPropertyItemFile : public SVPropertyItem
{
	SVPropertyItemFile(const SVPropertyItemFile& d);
	SVPropertyItemFile& operator=(const SVPropertyItemFile& d);

protected:
	DECLARE_SERIAL(SVPropertyItemFile)

public:
	SVPropertyItemFile();
	SVPropertyItemFile(LPCTSTR lpszName, LPCTSTR lpszPath, DWORD dwStyle, LPCTSTR strFilter=NULL);
	virtual void Serialize(CArchive& ar);

	// Attributes
	void SetData(LPCTSTR lpszPath, DWORD dwStyle, LPCTSTR lpszFilter=NULL);
	void SetFilter(LPCTSTR pszFilter);

	// Operations
	LPCTSTR GetFolder(void);
	LPCTSTR GetFilename(void);
	LPCTSTR GetExtension(void);
	LPCTSTR GetPathName(void);

protected:
	// Operations
	virtual void Draw(CDC* pDC, CRect& rect);
	virtual BOOL Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows=TRUE);
	virtual void EndEdit(void);
	virtual void OnEndEdit(LPLVITEM plvItem);
	virtual BOOL HitTest(CRect rect, CPoint point);

	// Data
private:
	CString				m_strPath;
	DWORD					m_dwStyle;
	CString				m_strFilter;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemFile.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:47:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   22 Oct 2010 08:10:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:56:54   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus. Also used by the SVReport project for SmithKline Beecham.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:14   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Dec 2000 11:57:34   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split out several classes into their own files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:38   cSchmittinger
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
