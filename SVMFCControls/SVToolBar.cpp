//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBarClass
//* .File Name       : $Workfile:   SVToolBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:41:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		SVToolBarClass::SVToolBarClass()
		{
			m_pTBButtons = NULL;
		}

		SVToolBarClass::~SVToolBarClass()
		{
			if (m_pTBButtons)
				delete []m_pTBButtons;
		}


		BOOL SVToolBarClass::Create(	DWORD dwStyle, const RECT& rect, 
			CWnd* pParentWnd, UINT nID, UINT nBaseID,
			int nCount, const SIZE& size )
		{
			int nIndex( 0 );

			BOOL bRet = CToolBarCtrl::Create(	dwStyle, rect, 
				pParentWnd, nID);

			m_nButtonCount = nCount;

			CSize bSize = size;
			CToolBarCtrl::SetBitmapSize(size);
			bSize.cx += SV_TOOLBAR_BORDER_W;
			bSize.cy += SV_TOOLBAR_BORDER_H;
			CToolBarCtrl::SetButtonSize(bSize);

			VERIFY(AddBitmap(m_nButtonCount,nID) != -1);

			m_pTBButtons = new TBBUTTON[m_nButtonCount];

			for (nIndex = 0; nIndex < m_nButtonCount; nIndex++)
			{
				m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED;
				m_pTBButtons[nIndex].fsStyle = TBSTYLE_BUTTON;
				m_pTBButtons[nIndex].dwData = 0;
				m_pTBButtons[nIndex].iBitmap = nIndex;
				m_pTBButtons[nIndex].idCommand = nIndex + nBaseID;
				m_pTBButtons[nIndex].iString = NULL;
			}

			for (nIndex = 0; nIndex < m_nButtonCount; nIndex++)
			{
				VERIFY(AddButtons(1,&m_pTBButtons[nIndex]));
			}

			return bRet;
		}

		BEGIN_MESSAGE_MAP(SVToolBarClass, CToolBarCtrl)
			//{{AFX_MSG_MAP(SVToolBarClass)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVToolBarClass message handlers

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolBar.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:41:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   30 Aug 2010 13:53:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  700
   SCR Title:  Remove String Buffer Problems
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   21 Jun 2007 15:03:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 16:47:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
 */
