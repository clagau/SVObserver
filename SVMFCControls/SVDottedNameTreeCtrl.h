//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedNameTreeCtrl
//* .File Name       : $Workfile:   SVDottedNameTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   21 May 2014 12:21:46  $
//******************************************************************************

#pragma once

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVDottedNameTreeCtrlClass : public CTreeCtrl
		{
			DECLARE_DYNCREATE(SVDottedNameTreeCtrlClass)

			// Construction
		public:
			SVDottedNameTreeCtrlClass();

			// Attributes
		public:
			BOOL		AddItem( CString StrDottedItemName, DWORD_PTR DwItemValue = NULL, BOOL BExpand = TRUE );
			HTREEITEM	GetItem( CString StrDottedItemName );
			DWORD_PTR	GetSelectedItemValue();

			// Operations
		public:

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVDottedNameTreeCtrlClass)
			//}}AFX_VIRTUAL

			// Implementation
		public:
			virtual ~SVDottedNameTreeCtrlClass();

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVDottedNameTreeCtrlClass)
			// NOTE - the ClassWizard will add and remove member functions here.
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDottedNameTreeCtrl.h_v  $
 * 
 *    Rev 1.1   21 May 2014 12:21:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised AddItem and GetItemValue methods to use DWORD_PTR instead of DWORD to correct an issue in 64Bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:20:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 09:09:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Nov 2002 11:58:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Feb 2000 23:21:52   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/