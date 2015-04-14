//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigNameEdit
//* .File Name       : $Workfile:   SVOConfigNameEdit.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:44  $
//******************************************************************************
#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVOConfigNameEdit : public CEdit
		{
			// Construction
		public:
			CSVOConfigNameEdit(LPCTSTR const sExcludedChars);

			// Attributes
		public:

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CSVOConfigNameEdit)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~CSVOConfigNameEdit();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CSVOConfigNameEdit)
			afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			//}}AFX_MSG

		private:
			CString m_sExcludedChars;

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOConfigNameEdit.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:49:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:30:00   rschock
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