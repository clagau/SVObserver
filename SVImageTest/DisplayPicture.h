// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : DisplayPicture.h
// * .File Name       : $Workfile:   DisplayPicture.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:07:54  $
// ******************************************************************************

#ifndef DISPLAYPICTURE_H
#define DISPLAYPICTURE_H

#include "SVImageLibrary/SVImageBufferHandleInterface.h"

struct SVImageBufferInterface;

class CDisplayPicture : public CStatic
{
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayPicture)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDisplayPicture)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	CDisplayPicture();

	bool UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat );

	HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );

	long m_BufferInUse;
	SVSmartHandlePointer m_pImage;

	int m_ScrollY;
	int m_ScrollX;
	virtual ~CDisplayPicture();

	// Generated message map functions
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\DisplayPicture.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:07:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:42:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jul 2009 15:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 12:22:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
