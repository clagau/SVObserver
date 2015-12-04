//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTransformImagePage
//* .File Name       : $Workfile:   SVTADlgTransformImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/assign/list_of.hpp>
#include "SVTADlgTransformImagePage.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui
{
	static LPCSTR UseExtentsOnlyTag = "UseExtentsOnly";

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTransformImagePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogTransformImagePageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_BN_CLICKED(IDC_USE_EXTENTS_ONLY, OnUseExtentsOnly)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogTransformImagePageClass::SVToolAdjustmentDialogTransformImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, int id) 
	: SVToolAdjustmentDialogImagePageClass(rInspectionID, rTaskObjectID, id)
	, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(UseExtentsOnlyTag, SVUseExtentsOnlyObjectGuid)))
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogTransformImagePageClass)
		m_useExtentsOnly = FALSE;
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogTransformImagePageClass::~SVToolAdjustmentDialogTransformImagePageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogTransformImagePageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		SetData();
		hr = m_Values.Commit();
		GetData();

		UpdateData(false);
	
		return hr;
	}

	void SVToolAdjustmentDialogTransformImagePageClass::GetData()
	{
		m_useExtentsOnly = m_Values.Get<bool>(UseExtentsOnlyTag);
	}

	void SVToolAdjustmentDialogTransformImagePageClass::SetData()
	{
		m_Values.Set<bool>(UseExtentsOnlyTag, m_useExtentsOnly ? true : false);
	}
		
	void SVToolAdjustmentDialogTransformImagePageClass::CheckSourceImage()
	{
		const SvUl::InputNameGuidPairList& images = GetConnectedImageList();
		if (images.size())
		{
			// Check if Main Image Type
			const SVObjectTypeInfoStruct& typeInfo = GetImageTypeInfo(images.begin()->second.second.ToGUID());
			if (typeInfo.SubType == SVMainImageObjectType)
			{
				m_useExtentsOnly = false;
				// use Extents Only does not Apply
				m_useExtentsOnlyCheckBox.EnableWindow(false);
			}
			else
			{
				m_useExtentsOnlyCheckBox.EnableWindow(true);
			}
		}
	}

	void SVToolAdjustmentDialogTransformImagePageClass::DoDataExchange(CDataExchange* pDX)
	{
		SVToolAdjustmentDialogImagePageClass::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogTransformImagePageClass)
		DDX_Control(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnlyCheckBox);
		DDX_Check(pDX, IDC_USE_EXTENTS_ONLY, m_useExtentsOnly);
		//}}AFX_DATA_MAP
	}
	BOOL SVToolAdjustmentDialogTransformImagePageClass::OnInitDialog() 
	{
	
		BOOL rc = SVToolAdjustmentDialogImagePageClass::OnInitDialog();
		if (rc)
		{
			m_Values.Init();
			GetData();
			CheckSourceImage();
			UpdateData(false);
		}
		else
		{
			// Not valid call...
			if (GetParent())
			{
				GetParent()->SendMessage(WM_CLOSE);
			}
			else
			{
				SendMessage(WM_CLOSE);
			}
		}
		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void SVToolAdjustmentDialogTransformImagePageClass::OnSelchangeCombo1() 
	{
		SVToolAdjustmentDialogImagePageClass::OnSelchangeCombo1();
		CheckSourceImage();
	}

	void SVToolAdjustmentDialogTransformImagePageClass::OnUseExtentsOnly() 
	{
		SetInspectionData();
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgTransformImagePage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 13:07:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:14:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:23:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Dec 2012 12:02:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made for adding the the new Shift Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Jul 2012 17:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Aug 2005 09:45:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2005 08:25:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:21:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:49:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Index for Value Object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:56   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:45:26   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:32:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class for selecting the source image for the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
