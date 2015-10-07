//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <string>
#include <boost/assign/list_of.hpp>
#include "SVTADlgPassFailPage.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogPassFailPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogPassFailPageClass)
	ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect)
	ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVToolAdjustmentDialogPassFailPageClass::SVToolAdjustmentDialogPassFailPageClass(const GUID& rInspectionID, const GUID& rTaskObjectID)
: CPropertyPage( SVToolAdjustmentDialogPassFailPageClass::IDD )
, RangeController(rInspectionID, rTaskObjectID)
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogPassFailPageClass)
	//}}AFX_DATA_INIT
	Init();
}
#pragma endregion Constructor

SVToolAdjustmentDialogPassFailPageClass::~SVToolAdjustmentDialogPassFailPageClass()
{
}

#pragma region Public Methods
bool SVToolAdjustmentDialogPassFailPageClass::QueryAllowExit()
{
	UpdateData(true);

	return UpdateRangeValues();
}
#pragma endregion Public Methods

HRESULT SVToolAdjustmentDialogPassFailPageClass::SetInspectionData(SVString& rMsg)
{
	UpdateData(true); // get data from dialog

	HINSTANCE resHandle = AfxGetResourceHandle();
	// Validate Entered data for existance and if within bounds
	HRESULT hr = IsFieldValid(rMsg, RangeEnum::ERange2String(resHandle, RangeEnum::ER_FailHigh), static_cast<LPCSTR>(m_FailHigh));
	if (S_OK == hr)
	{
		hr = IsFieldValid(rMsg, RangeEnum::ERange2String(resHandle, RangeEnum::ER_FailLow), static_cast<LPCSTR>(m_FailLow));
	}
	if (S_OK == hr)
	{
		hr = IsFieldValid(rMsg, RangeEnum::ERange2String(resHandle, RangeEnum::ER_WarnHigh), static_cast<LPCSTR>(m_WarnHigh));
	}
	if (S_OK == hr)
	{
		hr = IsFieldValid(rMsg, RangeEnum::ERange2String(resHandle, RangeEnum::ER_WarnLow), static_cast<LPCSTR>(m_WarnLow));
	}
	if (S_OK == hr)
	{
		Set(FailHigh, static_cast<LPCSTR>(m_FailHigh));
		Set(FailLow, static_cast<LPCSTR>(m_FailLow));
		Set(WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
		Set(WarnLow, static_cast<LPCSTR>(m_WarnLow));
	}
	return hr;
}

#pragma region Protected Methods
void SVToolAdjustmentDialogPassFailPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVToolAdjustmentDialogPassFailPageClass)
	DDX_Text(pDX, IDC_EDIT_FAILHIGH, m_FailHigh);
	DDX_Text(pDX, IDC_EDIT_WARNHIGH, m_WarnHigh);
	DDX_Text(pDX, IDC_EDIT_WARNLOW, m_WarnLow);
	DDX_Text(pDX, IDC_EDIT_FAILLOW, m_FailLow);
	DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
	DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
	//}}AFX_DATA_MAP
}

void SVToolAdjustmentDialogPassFailPageClass::InitData()
{
	try
	{
		m_FailHigh = Get(FailHigh).c_str();
		m_FailLow = Get(FailLow).c_str();
		m_WarnHigh = Get(WarnHigh).c_str();
		m_WarnLow  = Get(WarnLow).c_str();
	}
	catch (...) // find out what to catch here...
	{
	}
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);

	//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
	// And it is NOT needed as the implicit cast will call the casting operator.
	m_ButtonFailHigh.SetBitmap(m_downArrowBitmap);
	m_ButtonWarnHigh.SetBitmap(m_downArrowBitmap);
	m_ButtonWarnLow.SetBitmap(m_downArrowBitmap);
	m_ButtonFailLow.SetBitmap(m_downArrowBitmap);

	InitData();
	UpdateData(false); // set data to dialog

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnSetActive() 
{
	InitData();
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnKillActive() 
{
	UpdateData(true);
	bool bOk = UpdateRangeValues();

	if (bOk)
	{
		return CPropertyPage::OnKillActive();
	}
	return bOk;
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect()
{
	setValuePerObjectSelector(m_FailHigh, RangeEnum::ER_FailHigh);
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect()
{
	setValuePerObjectSelector(m_WarnHigh, RangeEnum::ER_WarnHigh);
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect()
{
	setValuePerObjectSelector(m_WarnLow, RangeEnum::ER_WarnLow);
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect()
{
	setValuePerObjectSelector(m_FailLow, RangeEnum::ER_FailLow);
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool SVToolAdjustmentDialogPassFailPageClass::UpdateRangeValues()
{
	bool bRetVal = true;
	
	SVString errorMsg;
	HRESULT retVal = SetInspectionData(errorMsg);
	if (S_OK != retVal)
	{
		AfxMessageBox(errorMsg.c_str(), MB_OK | MB_ICONERROR);
		bRetVal = false;
	}
	else
	{
		retVal = Validate(errorMsg, AfxGetResourceHandle());
		if (S_OK != retVal)
		{
			// Show error msg
			AfxMessageBox(errorMsg.c_str(), MB_OK | MB_ICONERROR);
			bRetVal = false;
		}
		else
		{
			Commit();
		}
	}
	return bRetVal;
}

void SVToolAdjustmentDialogPassFailPageClass::setValuePerObjectSelector(CString& value, RangeEnum::ERange fieldEnum)
{
	if (ShowObjectSelector(value, fieldEnum))
	{
		 UpdateData(false);
	}
}

bool SVToolAdjustmentDialogPassFailPageClass::ShowObjectSelector(CString& name, RangeEnum::ERange fieldEnum)
{
	bool retValue = false;

	FillObjectSelector();

	if (name.GetLength() > 0)
	{
		SVStringSet nameSet;
		nameSet.insert(name);
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	SVString Title = GetOwnerName();
	Title += _T(": ");
	Title += RangeEnum::ERange2String(AfxGetResourceHandle(), fieldEnum);

	CString mainTabTitle;
	mainTabTitle.LoadString(IDS_SELECT_TOOLSET_OUTPUT);
	CString FilterTab;
	FilterTab.LoadString(IDS_FILTER);

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title, mainTabTitle, FilterTab, this);

	if (IDOK == Result)
	{
		name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); // @TODO:  Should we check the return values of getSingleObjectResult and getLocation?
		retValue = true;
	}
	return retValue;
}

#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgPassFailPage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:50:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:14:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:16:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   16 Dec 2009 13:12:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Aug 2005 08:18:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Feb 2005 15:19:04   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 16:12:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 13:46:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     SetValue Data Indexes
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
