//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:15:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVGUID.h"
#include "SVOGui\BoundValue.h"
#include "SVOGui\ValuesAccessor.h"
#include "SVOGui\GuiController.h"
#include "SVOGui\AuxiliaryExtentsController.h"
#include "SVOGui\AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVToolAdjustmentDialogGeneralPageClass : public CPropertyPage
		{
			typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
			typedef SvOg::GuiController<Values, Values::value_type> ValuesController;
		public:
			SVToolAdjustmentDialogGeneralPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
			virtual ~SVToolAdjustmentDialogGeneralPageClass();

		protected:
			//{{AFX_MSG(SVToolAdjustmentDialogGeneralPageClass)
			virtual BOOL OnInitDialog();
			afx_msg void OnSelchangeDrawToolCombo();
			afx_msg void OnUpdateAuxiliaryExtents();
			afx_msg void OnSelchangeSourceImageCombo();
			afx_msg void OnShowRelations();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			//{{AFX_VIRTUAL(SVToolAdjustmentDialogGeneralPageClass)
			public:
			virtual BOOL OnSetActive();
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

			void refresh();
			bool CheckAuxiliaryExtentsAvailable() const;
			HRESULT SetInspectionData();
			void SetupAuxExtents();
			void SetupDrawFlagComboBox();
			void SetImages();

		public:
			//{{AFX_DATA(SVToolAdjustmentDialogGeneralPageClass)
			enum { IDD = IDD_TA_GENERAL_DIALOG };
			SvOg::AvailableObjectListComboBox m_AvailableSourceImageCombo;
			CComboBox m_drawToolCombo;
			BOOL m_bUpdateAuxiliaryExtents;
			//}}AFX_DATA

		protected:
			bool m_bAuxExtentsAvailable;
			bool m_bIsImageTool;
			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;

			SvOg::AuxiliaryExtentsController m_AuxExtentsController;
			ValuesController m_Values;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgGeneralPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:15:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Jan 2006 17:13:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Sep 2005 11:44:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Changes to allow Auxilliary Image Extent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 08:18:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:12:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:25:44   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Feb 24 2000 12:04:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides a page for general tool settings.
 * ( First General Setting: Implements access to tool draw flag).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
