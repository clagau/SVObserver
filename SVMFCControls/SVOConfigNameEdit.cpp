//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigNameEdit
//* .File Name       : $Workfile:   SVOConfigNameEdit.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVOConfigNameEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		CSVOConfigNameEdit::CSVOConfigNameEdit(LPCTSTR const sExcludedChars)
		{
			m_sExcludedChars = sExcludedChars;
		}

		CSVOConfigNameEdit::~CSVOConfigNameEdit()
		{
		}


		BEGIN_MESSAGE_MAP(CSVOConfigNameEdit, CEdit)
			//{{AFX_MSG_MAP(CSVOConfigNameEdit)
			ON_WM_CHAR()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// CSVOConfigNameEdit message handlers

		void CSVOConfigNameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
		{
			switch (nChar)
			{
			case VK_BACK:
				{
					break;
				}
			default:
				{
					if (::_tcschr(m_sExcludedChars,nChar) )
					{
						return;
					}
				}
			}

			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOConfigNameEdit.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:49:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jun 2005 15:05:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed text literal to SVEXCLUDECHARS_***
 * cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Sep 2003 09:27:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added more chars to exclude
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:29:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/