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
#include "SVTADlgPassFailPage.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVRange.h"
#include "SVResult.h"
#include "SVTool.h"
#include "SVStatusLibrary\MessageHandler.h"
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
SVToolAdjustmentDialogPassFailPageClass::SVToolAdjustmentDialogPassFailPageClass( SVToolClass* pTool ) 
	: CPropertyPage( SVToolAdjustmentDialogPassFailPageClass::IDD )
{
	if( pTool )
	{
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVResultObjectType;

		SVResultClass* pResult = reinterpret_cast<SVResultClass *>(SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
		if( pResult )
		{
			info.ObjectType = SVRangeObjectType;
			SVRangeClass* pRange = reinterpret_cast<SVRangeClass *>(SVSendMessage( pResult, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
			m_RangeHelper.setRangeObject(pRange);
		}
	}
}

SVToolAdjustmentDialogPassFailPageClass::~SVToolAdjustmentDialogPassFailPageClass()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVToolAdjustmentDialogPassFailPageClass::QueryAllowExit()
{
	UpdateData(TRUE);

	return UpdateRangeValues();
}
#pragma endregion Public Methods

#pragma region Protected Methods
void SVToolAdjustmentDialogPassFailPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVToolAdjustmentDialogPassFailPageClass)
	DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_EditFailHigh);
	DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_EditWarnHigh);
	DDX_Control(pDX, IDC_EDIT_WARNLOW, m_EditWarnLow);
	DDX_Control(pDX, IDC_EDIT_FAILLOW, m_EditFailLow);
	DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
	DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

	//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
	m_ButtonFailHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonFailLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );

	m_RangeHelper.SetRangeTaskObject();
	m_RangeHelper.GetAllInspectionData();

	SetDlgData();

	UpdateData( FALSE ); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnSetActive() 
{
	SetDlgData();

	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnKillActive() 
{
	UpdateData(TRUE);

	bool bOk = UpdateRangeValues();
	
	if (bOk)
	{
		return CPropertyPage::OnKillActive();
	}
	else
	{
		return false;
	}
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect()
{
	setValuePerObjectSelector( m_EditFailHigh, ER_FailHigh );
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect()
{
	setValuePerObjectSelector( m_EditWarnHigh, ER_WarnHigh );
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect()
{
	setValuePerObjectSelector( m_EditWarnLow, ER_WarnLow );
}

void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect()
{
	setValuePerObjectSelector( m_EditFailLow, ER_FailLow );
}
#pragma endregion Protected Methods

#pragma region Privated Methods
bool SVToolAdjustmentDialogPassFailPageClass::UpdateRangeValues()
{
	CString errorMsg;
	bool bOK = GetDlgData();

	if(bOK)
	{
		HRESULT hres = m_RangeHelper.CheckInternalData(errorMsg);
		if( S_OK != hres)
		{
			bOK = false;
			AfxMessageBox(errorMsg, MB_OK | MB_ICONERROR);
		}
	}

	if (bOK)
	{
		m_RangeHelper.SetInspectionData();
	}
	return bOK;
}

void SVToolAdjustmentDialogPassFailPageClass::SetDlgData()
{
	m_EditFailHigh.SetWindowText(m_RangeHelper.GetFailHighString().c_str());
	m_EditWarnHigh.SetWindowText(m_RangeHelper.GetWarnHighString().c_str());
	m_EditFailLow.SetWindowText(m_RangeHelper.GetFailLowString().c_str());
	m_EditWarnLow.SetWindowText(m_RangeHelper.GetWarnLowString().c_str());
}

bool SVToolAdjustmentDialogPassFailPageClass::GetDlgData()
{
	bool res = false;
	CString csText;

	try
	{
		m_EditFailHigh.GetWindowText(csText);
		m_RangeHelper.SetInternalData(ER_FailHigh, csText);
		m_EditWarnHigh.GetWindowText(csText);
		m_RangeHelper.SetInternalData(ER_WarnHigh, csText);
		m_EditFailLow.GetWindowText(csText);
		m_RangeHelper.SetInternalData(ER_FailLow, csText);
		m_EditWarnLow.GetWindowText(csText);
		m_RangeHelper.SetInternalData(ER_WarnLow, csText);
		res = true;
	}
	catch ( const SvStl::MessageHandler& rSvE )
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStdDisplay Exception( SvStl::LogAndDisplay );
		Exception.setMessage( rSvE.getMessage() );
	}

	return res;
}

void SVToolAdjustmentDialogPassFailPageClass::setValuePerObjectSelector( CEdit& control, ERange fieldEnum)
{
	CString csText;
	control.GetWindowText(csText); 
	if (ShowObjectSelector(csText, fieldEnum))
	{
		control.SetWindowText(csText);
	}
}

bool SVToolAdjustmentDialogPassFailPageClass::ShowObjectSelector(CString& name, ERange fieldEnum )
{
	bool retValue = m_RangeHelper.FillObjectSelector();

	if ( !retValue )
	{
		return retValue;
	}

	if(name.GetLength() > 0)
	{
		SVStringSet nameSet;
		nameSet.insert(name);
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	CString Title = m_RangeHelper.GetOwnerName();
	Title += _T(": ");
	Title += RangeClassHelper::ERange2String(fieldEnum);

	CString mainTabTitle;
	mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
	CString FilterTab;
	FilterTab.LoadString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, this );

	if( IDOK == Result )
	{
		name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); // @TODO:  Should we check the return values of getSingleObjectResult and getLocation?
		return true;
	}

	return false;
}
#pragma endregion Privated Methods

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
