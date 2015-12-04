//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/assign/list_of.hpp>
#include "SVToolAdjustmentDialogTwoImagePage.h"
#include "SVMatroxLibrary\SVMatroxEnums.h"
#include "ObjectInterfaces\ImageArthimeticOperators.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui
{
	static LPCTSTR FirstImageTag = _T("First Image");
	static LPCTSTR SecondImageTag = _T("Second Image");
	static LPCTSTR ResultImageTag = _T("Result Image");
	static LPCTSTR NoImageTag = _T("(No Image Available)"); // maybe move this to the control class?
	static LPCSTR ArithmeticOperatorTag = "ArithmeticOperator";
	static const int NumberOfImagesRequired = 2;

	enum ImageTabsEnum
	{
		FirstImageTab = 0,
		SecondImageTab = 1,
		ResultImageTab = 2
	};

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTwoImagePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogTwoImagePageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_CBN_SELCHANGE(IDC_COMBO2, OnSelChangeCombo2)
		ON_CBN_SELCHANGE(IDC_OPERATOR_COMBO, OnSelchangeOperatorCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	#pragma region Constructor
	SVToolAdjustmentDialogTwoImagePageClass::SVToolAdjustmentDialogTwoImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
	: CPropertyPage(SVToolAdjustmentDialogTwoImagePageClass::IDD)
	, SvOg::ImageController(rInspectionID, rTaskObjectID, SVToolImageObjectType)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of(ArithmeticOperatorTag, SVArithmeticOperatorObjectGuid)))
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogTwoImagePageClass)
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogTwoImagePageClass::~SVToolAdjustmentDialogTwoImagePageClass()
	{
	}
	#pragma endregion Constructor

	HRESULT SVToolAdjustmentDialogTwoImagePageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		UpdateData(true); // get data from dialog

		long lOperator = static_cast<long>(m_operatorCtrl.GetItemData(m_operatorCtrl.GetCurSel()));
		m_Values.Set<long>(ArithmeticOperatorTag, lOperator);
		m_Values.Commit();
		
		if (SV_IMGOP_DOUBLE_HEIGHT == lOperator || SV_IMGOP_FLIP_VERTICAL == lOperator || SV_IMGOP_FLIP_HORIZONTAL == lOperator)
		{
			m_secondAvailableSourceImageListBoxCtl.EnableWindow(false);
			//hide second image tab, because in this mode is only on image aloud.
			m_firstImageCtrl.ShowTab(SecondImageTab, false);
			m_secondImageCtrl.ShowTab(SecondImageTab, false);
		}// end if
		else
		{
			m_secondAvailableSourceImageListBoxCtl.EnableWindow(true);
			//show second image tab
			m_firstImageCtrl.ShowTab(SecondImageTab, true);
			m_secondImageCtrl.ShowTab(SecondImageTab, true);
		}// end else

		UpdateData(false);
		return hr;
	}

	void SVToolAdjustmentDialogTwoImagePageClass::refresh()
	{
		SetInspectionData();
		setImages();
	}

	#pragma region Protected Methods
	void SVToolAdjustmentDialogTwoImagePageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogTwoImagePageClass)
		DDX_Control(pDX, IDC_OPERATOR_COMBO, m_operatorCtrl);
		DDX_Control(pDX, IDC_SECOND_IMAGE, m_secondImageCtrl);
		DDX_Control(pDX, IDC_FIRST_IMAGE, m_firstImageCtrl);
		DDX_Control(pDX, IDC_COMBO1, m_firstAvailableSourceImageListBoxCtl);
		DDX_Control(pDX, IDC_COMBO2, m_secondAvailableSourceImageListBoxCtl);
		//}}AFX_DATA_MAP
	}

	BOOL SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
	
		Init();
		m_Values.Init();

		const SvUl::NameGuidList& rAvailableImageList = GetAvailableImageList();
		RetreiveCurrentlySelectedImageNames();
		RetreiveResultImageNames();
	
		m_firstAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_firstImageName, NoImageTag);
		m_secondAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_secondImageName, NoImageTag);

		m_firstImageCtrl.AddTab(FirstImageTag); 
		m_firstImageCtrl.AddTab(SecondImageTag); 
		m_firstImageCtrl.AddTab(ResultImageTag); 
		m_secondImageCtrl.AddTab(FirstImageTag); 
		m_secondImageCtrl.AddTab(SecondImageTag); 
		m_secondImageCtrl.AddTab(ResultImageTag); 
		m_secondImageCtrl.SelectTab(1);
		setImages();
			
		// Fill Arithmetic Combo...
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "AND" ) ), static_cast<DWORD_PTR>(SVImageAnd) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "OR" ) ),  static_cast<DWORD_PTR>(SVImageOr) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "XOR" ) ), static_cast<DWORD_PTR>(SVImageXOr) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "NOR" ) ), static_cast<DWORD_PTR>(SVImageNor) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "XNOR" ) ), static_cast<DWORD_PTR>(SVImageXNor) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "NAND" ) ), static_cast<DWORD_PTR>(SVImageNand) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "ADD" ) ), static_cast<DWORD_PTR>(SVImageAddSaturation) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "SUB" ) ), static_cast<DWORD_PTR>(SVImageSubSaturation) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MULT" ) ), static_cast<DWORD_PTR>(SVImageMulSaturation) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "DIV" ) ), static_cast<DWORD_PTR>(SVImageDiv) );
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MIN" ) ), static_cast<DWORD_PTR>(SVImageMin) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "MAX" ) ), static_cast<DWORD_PTR>(SVImageMax) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "PASS" ) ), static_cast<DWORD_PTR>(SVImagePass) );	
			
		// Special Image Operator... ( not defined by MIL ! )
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "AVERAGE" ) ), static_cast<DWORD_PTR>(SV_IMGOP_AVERAGE) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "DOUBLE HEIGHT" ) ), static_cast<DWORD_PTR>(SV_IMGOP_DOUBLE_HEIGHT) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "FLIP VERTICAL" ) ), static_cast<DWORD_PTR>(SV_IMGOP_FLIP_VERTICAL) );	
		m_operatorCtrl.SetItemData( m_operatorCtrl.AddString( _T( "FLIP HORIZONTAL" ) ), static_cast<DWORD_PTR>(SV_IMGOP_FLIP_HORIZONTAL) );

		// Get Current Arithmetic Operator...
	
		long lOperator = m_Values.Get<long>(ArithmeticOperatorTag);

		for (int i = 0;i < m_operatorCtrl.GetCount(); ++ i)
		{
			if (lOperator == static_cast<long>(m_operatorCtrl.GetItemData(i)))
			{
				m_operatorCtrl.SetCurSel(i); // Set Selected Operator...
				OnSelchangeOperatorCombo();
				break;
			}
		}
		UpdateData(false); // set data to dialog
		return true;
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1()
	{
		UpdateData(true); // get data from dialog

		CString name;
		m_firstAvailableSourceImageListBoxCtl.GetLBText(m_firstAvailableSourceImageListBoxCtl.GetCurSel(), name);
		if (!name.IsEmpty() && name != NoImageTag)
		{
			m_firstImageName = name;
			ConnectToImage(m_firstInputName, m_firstImageName);
			refresh();
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2()
	{
		UpdateData(true); // get data from dialog

		CString name;
		m_secondAvailableSourceImageListBoxCtl.GetLBText(m_secondAvailableSourceImageListBoxCtl.GetCurSel(), name);
		if (!name.IsEmpty() && name != NoImageTag)
		{
			m_secondImageName = name;
			ConnectToImage(m_secondInputName, m_secondImageName);
			refresh();
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeOperatorCombo() 
	{
		refresh();
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	void SVToolAdjustmentDialogTwoImagePageClass::setImages()
	{
		RetreiveCurrentlySelectedImageNames();

		IPictureDisp* pFirstImage = GetImage(m_firstImageName);
		IPictureDisp* pSecondImage = GetImage(m_secondImageName);
		m_firstImageCtrl.setImage(pFirstImage, FirstImageTab);
		m_firstImageCtrl.setImage(pSecondImage, SecondImageTab);
		m_secondImageCtrl.setImage(pFirstImage, FirstImageTab);
		m_secondImageCtrl.setImage(pSecondImage, SecondImageTab);
	
		IPictureDisp* pResultImage = GetImage(m_resultImageID.ToGUID());
		if (pResultImage)
		{
			m_firstImageCtrl.setImage(pResultImage, ResultImageTab);
			m_secondImageCtrl.setImage(pResultImage, ResultImageTab);
			//display result image tab
			m_firstImageCtrl.ShowTab(ResultImageTab, true);
			m_secondImageCtrl.ShowTab(ResultImageTab, true);
		}
		else
		{
			//hide result image tab
			m_firstImageCtrl.ShowTab(ResultImageTab, false);
			m_secondImageCtrl.ShowTab(ResultImageTab, false);
		}

		m_firstImageCtrl.Refresh();
		m_secondImageCtrl.Refresh();
	}

	void SVToolAdjustmentDialogTwoImagePageClass::RetreiveCurrentlySelectedImageNames()
	{
		const SvUl::InputNameGuidPairList& rImageList = GetConnectedImageList(GUID_NULL, NumberOfImagesRequired);

		// This requires that the input name sorts in descending natural order
		// and that the images we are concerned with are first in the list
		if (rImageList.size() && rImageList.size() > 1)
		{
			SvUl::InputNameGuidPairList::const_iterator it = rImageList.begin();
			m_firstInputName = it->first;
			m_firstImageName = it->second.first;
			++it;
			m_secondInputName = it->first;
			m_secondImageName = it->second.first;
		}
	}

	void SVToolAdjustmentDialogTwoImagePageClass::RetreiveResultImageNames()
	{
		const SvUl::NameGuidList& rImageList = GetResultImages();

		SvUl::NameGuidList::const_iterator it = rImageList.begin();
		if (it != rImageList.end())
		{
			m_resultImageName = it->first;
			m_resultImageID = it->second;
		}
	}
	#pragma endregion Private Methods
} /* namespace SVOGui */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogTwoImagePage.cpp_v  $
 * 
 *    Rev 1.3   26 Jun 2014 18:29:28   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 14:37:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:22:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:40:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   30 Jul 2012 13:08:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   27 Jul 2012 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   02 Jul 2012 17:50:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   19 Jun 2012 14:23:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   07 Apr 2011 16:41:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Mar 2011 13:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Feb 2011 12:25:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Dec 2009 13:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   25 Jul 2007 07:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Jan 2006 17:34:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   06 Sep 2005 14:15:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used new method in SVTaskObjectClass to handle the image connections properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Aug 2005 08:45:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:33:50   ebeyeler
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
 *    Rev 1.11   18 Feb 2005 14:05:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added check for SV_HIDDEN image property
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Feb 2005 07:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Reset methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Feb 2005 15:21:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   06 Jan 2004 14:01:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed two small bugs in the flip code for the Image Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   29 Oct 2003 14:09:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  391
 * SCR Title:  Add horizontal and vertical flip for Image Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new operations for filp horizontal and vertical.  If the opertion does not need the second image pane, it will be disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 16:46:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Jan 2003 09:02:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 14:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   theSVObjectManager
 * ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jan 2002 11:21:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  280
 * SCR Title:  Update image tool setup dialog to include all image sources in the tool set
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to remove previous trigger image filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2001 15:39:22   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  240
 * SCR Title:  Color SVIM:  Color images available as sources in image tool.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified function, 
 * SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2001 15:09:48   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified functions, 
 * SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1,  
 * SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2000 16:00:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Moved SVToolAdjustDialogTwoImagePageClass from 
 * SVToolAdjustmentDialog.cpp and SVToolAdjustmentDialog.h
 * to it's own header and implementation file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
