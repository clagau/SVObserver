//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorSheet
//* .File Name       : $Workfile:   SVFormulaEditorSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 19:11:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFormulaEditorPage.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{ 
	namespace SVOGui
	{
		class SVFormulaEditorSheetClass : public CPropertySheet
		{
		public:
			SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskOjectID, const SVObjectTypeInfoStruct& rInfo, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
			SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskOjectID, const SVObjectTypeInfoStruct& rInfo, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

			virtual ~SVFormulaEditorSheetClass();

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVFormulaEditorSheetClass)
			public:
			virtual BOOL OnInitDialog();
			//}}AFX_VIRTUAL

		protected:
			void init();

			//{{AFX_MSG(SVFormulaEditorSheetClass)
			afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		protected:
			afx_msg void OnOK();

			typedef SVSharedPtr<SVFormulaEditorPageClass> FormulaEditorPagePtr;
			FormulaEditorPagePtr m_formulaPage;
			GUID m_InspectionID;
			GUID m_TaskObjectID;
			SVObjectTypeInfoStruct m_info;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFormulaEditorSheet.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 19:11:04   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2014 12:22:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changes to use FormulaController object.
 * Changed member variable names to use m_ prefix.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:39:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:45:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Close Button on caption.
 * Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:39:20   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 1999 11:00:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use IDS_INVALID_FORMULA (string table).
 * Added OnOK handler to prevent exiting dialog with an
 * invalid formula.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Sep 1999 13:12:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnInitDilaog Method.
 * Disabled and Hid IDCANCEL button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:17:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Standard Sheet for Formula Editor Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/