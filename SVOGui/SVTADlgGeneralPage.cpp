//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgGeneralPage
//* .File Name       : $Workfile:   SVTADlgGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/assign/list_of.hpp>
#include "SVTADlgGeneralPage.h"
#include "SVObjectLibrary\SVClsids.h"
#include "ObjectInterfaces\NameValueList.h"
#include "GuiCommands\GetDependencies.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVShowDependentsDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui
{
	static LPCSTR DrawFlagTag = "DrawFlag";

	SVToolAdjustmentDialogGeneralPageClass::SVToolAdjustmentDialogGeneralPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
	: CPropertyPage(SVToolAdjustmentDialogGeneralPageClass::IDD)
	, m_bIsImageTool(false)
	, m_bAuxExtentsAvailable(false)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
			(DrawFlagTag, SVConditionalToolDrawFlagObjectGuid)))
	, m_AuxExtentsController(rInspectionID, rTaskObjectID)
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogGeneralPageClass)
		m_bUpdateAuxiliaryExtents = false;
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogGeneralPageClass::~SVToolAdjustmentDialogGeneralPageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogGeneralPageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		int sel = m_drawToolCombo.GetCurSel();
		if (CB_ERR != sel)
		{
			CString item;
			m_drawToolCombo.GetLBText(sel, item);
			m_Values.Set<CString>(DrawFlagTag, item);
		}			
		m_AuxExtentsController.EnableAuxExtents(m_bUpdateAuxiliaryExtents ? true : false);
			
		hr = m_Values.Commit();
		if (S_OK == hr)
		{
			hr = m_AuxExtentsController.Commit();
		}
		UpdateData(false);

		return hr;
	}

	void SVToolAdjustmentDialogGeneralPageClass::refresh()
	{
		// Update dialog with freeze tool attributes...
		BOOL bOk = false;
		CString strEnum = m_Values.Get<CString>(DrawFlagTag);
		int index = m_drawToolCombo.SelectString(-1, strEnum);
		bOk = true;
	
		// Check, if drawToolCombo must be disabled/enabled...
		// based on What?
		CWnd* pWnd = GetDlgItem(IDC_DRAW_TOOL_COMBO);
		if (pWnd)
		{
			pWnd->EnableWindow(bOk);
		}
		if (m_bIsImageTool)
		{
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		}
		UpdateData(false); // Send Data to Dialog...
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetupAuxExtents()
	{
		m_bUpdateAuxiliaryExtents = m_AuxExtentsController.IsUpdateAuxExtentsEnabled();
		UpdateData(false);
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetupDrawFlagComboBox()
	{
		m_drawToolCombo.ResetContent();
		CString drawFlag = m_Values.Get<CString>(DrawFlagTag);
		const SvOi::NameValueList& flags = m_Values.GetEnumTypes(DrawFlagTag);
		for (SvOi::NameValueList::const_iterator it = flags.begin(); it != flags.end();++it)
		{
			m_drawToolCombo.SetItemData(m_drawToolCombo.AddString(it->first.ToString()), it->second);
		}
		m_drawToolCombo.SelectString(-1, drawFlag);
	}

	void SVToolAdjustmentDialogGeneralPageClass::SetImages()
	{
		m_AvailableSourceImageCombo.ResetContent();
		if (m_bAuxExtentsAvailable)
		{
			const SvUl::NameGuidList& availImages = m_AuxExtentsController.GetAvailableImageList();
			for (SvUl::NameGuidList::const_iterator it = availImages.begin();it != availImages.end();++it)
			{
				m_AvailableSourceImageCombo.AddString(it->first.ToString());
			}
			CString name = m_AuxExtentsController.GetAuxSourceImageName().c_str();
			if (!name.IsEmpty())
			{
				m_AvailableSourceImageCombo.SelectString(-1, name);
			}
		}
	}

	bool SVToolAdjustmentDialogGeneralPageClass::CheckAuxiliaryExtentsAvailable() const
	{
		bool bRetVal = m_AuxExtentsController.AreAuxiliaryExtentsAvailable();
		return bRetVal;
	}

	void SVToolAdjustmentDialogGeneralPageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogGeneralPageClass)
		DDX_Control(pDX, IDC_SOURCE_IMAGE_COMBO, m_AvailableSourceImageCombo);
		DDX_Control(pDX, IDC_DRAW_TOOL_COMBO, m_drawToolCombo);
		DDX_Check(pDX, IDC_ENABLE_AUXILIARY_EXTENTS, m_bUpdateAuxiliaryExtents);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogGeneralPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogGeneralPageClass)
		ON_CBN_SELCHANGE(IDC_DRAW_TOOL_COMBO, OnSelchangeDrawToolCombo)
		ON_BN_CLICKED(IDC_ENABLE_AUXILIARY_EXTENTS, OnUpdateAuxiliaryExtents)
		ON_CBN_SELCHANGE(IDC_SOURCE_IMAGE_COMBO, OnSelchangeSourceImageCombo)
		ON_BN_CLICKED(ID_SHOW_RELATIONS, OnShowRelations)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// SVToolAdjustmentDialogGeneralPageClass message handlers

	BOOL SVToolAdjustmentDialogGeneralPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_AuxExtentsController.Init();
		m_Values.Init();

		SetupDrawFlagComboBox();
		m_bAuxExtentsAvailable = CheckAuxiliaryExtentsAvailable();

		// Get tool's Update Auxilliary Extents...
		if (!m_bAuxExtentsAvailable)
		{
			// Disable Auxilliary Extents
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_HIDE);
			m_bIsImageTool = false;
		}
		else
		{
			// Enable Auxilliary Extents
			GetDlgItem(IDC_ENABLE_AUXILIARY_EXTENTS)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_AUXILIARY_GROUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->ShowWindow(SW_SHOW);
			m_bIsImageTool = true;
		
			SetupAuxExtents();

			GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);

			SetImages();
		}
		UpdateData(false);
		refresh();
		// Success...
		return true;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeDrawToolCombo() 
	{
		SetInspectionData();
		refresh();
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnUpdateAuxiliaryExtents() 
	{
		SetInspectionData();

		const SvUl::NameGuidPair& source = m_AuxExtentsController.GetAuxSourceImage();
		if (!source.first.empty())
		{
			m_AuxExtentsController.SetAuxSourceImage(source.first);
		}

		GetDlgItem(IDC_SOURCE_IMAGE_COMBO)->EnableWindow(m_bUpdateAuxiliaryExtents);
		refresh();
	}

	void SVToolAdjustmentDialogGeneralPageClass::OnSelchangeSourceImageCombo() 
	{
		UpdateData( TRUE ); // get data from dialog

		int index = static_cast<int>(m_AvailableSourceImageCombo.GetCurSel());
		if (CB_ERR != index)
		{
			CString name;
			m_AvailableSourceImageCombo.GetLBText(index, name);
			if (!name.IsEmpty())
			{
				m_AuxExtentsController.SetAuxSourceImage(name.GetString());
			}
		}
		refresh();
	}
	
	void SVToolAdjustmentDialogGeneralPageClass::OnShowRelations() 
	{
		SVShowDependentsDialog Dlg(m_InspectionID, m_TaskObjectID, false, SVToolObjectType, nullptr, SVShowDependentsDialog::Show);
		Dlg.DoModal();
	}

	BOOL SVToolAdjustmentDialogGeneralPageClass::OnSetActive() 
	{
		SetImages();
		refresh();

		return CPropertyPage::OnSetActive();
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgGeneralPage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:50:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:08:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:14:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Jul 2012 17:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Dec 2010 13:39:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2006 17:13:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Oct 2005 09:46:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Oct 2005 13:08:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Spelling error for Auxiliary Extent IDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Sep 2005 15:11:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Auxilliary Extent option from gage tool class and dlg.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Sep 2005 14:50:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Auxilliary Group Box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Sep 2005 11:50:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   OnInitDialog - Changes for GUI
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Aug 2005 13:28:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:18:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2005 08:32:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:12:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:41:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   - Removed Call to EnableAutomation()
 * - Add ResultDataIndex to SetValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:25:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:39:44   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
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
