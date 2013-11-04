//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgPassFailPage.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVRange.h"
#include "SVResult.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogPassFailPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogPassFailPageClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogPassFailPageClass::SVToolAdjustmentDialogPassFailPageClass( SVToolClass* pTool ) 
	: CPropertyPage( SVToolAdjustmentDialogPassFailPageClass::IDD )
{
	pRange = NULL;
	
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogPassFailPageClass)
		warnHigh = 0.0;
		warnLow = 0.0;
		failHigh = 0.0;
		failLow = 0.0;
	//}}AFX_DATA_INIT

	if( pTool )
	{
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVResultObjectType;

		SVResultClass* pResult = ( SVResultClass * )SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info );
		if( pResult )
		{
			info.ObjectType = SVRangeObjectType;
			pRange = (SVRangeClass *)SVSendMessage( pResult, SVM_GETFIRST_OBJECT, NULL, ( DWORD )&info );
		}
	}
}

SVToolAdjustmentDialogPassFailPageClass::~SVToolAdjustmentDialogPassFailPageClass()
{
}

HRESULT SVToolAdjustmentDialogPassFailPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pRange != NULL )
	{
		UpdateData( TRUE ); // get data from dialog

		l_hrOk = AddInputRequest( &( pRange->FailHigh ), failHigh );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pRange->FailLow ), failLow );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pRange->WarnHigh ), warnHigh );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pRange->WarnLow ), warnLow );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pRange->GetTool() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

void SVToolAdjustmentDialogPassFailPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVToolAdjustmentDialogPassFailPageClass)
	DDX_Control(pDX, IDC_WARN_LOW, m_warnLowEditCtrl);
	DDX_Control(pDX, IDC_WARN_HIGH, m_warnHighEditCtrl);
	DDX_Control(pDX, IDC_FAIL_LOW, m_failLowEditCtrl);
	DDX_Control(pDX, IDC_FAIL_HIGH, m_failHighEditCtrl);
	DDX_Text(pDX, IDC_FAIL_HIGH, failHigh);
	DDX_Text(pDX, IDC_FAIL_LOW, failLow);
	DDX_Text(pDX, IDC_WARN_HIGH, warnHigh);
	DDX_Text(pDX, IDC_WARN_LOW, warnLow);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( pRange );

	if( pRange )
	{
		pRange->FailHigh.GetValue( failHigh );
		pRange->FailLow.GetValue( failLow );
		pRange->WarnHigh.GetValue( warnHigh );
		pRange->WarnLow.GetValue( warnLow );
	}

	UpdateData( FALSE ); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnSetActive() 
{
	if( pRange)
	{
		pRange->FailHigh.GetValue( failHigh );
		pRange->FailLow.GetValue( failLow );
		pRange->WarnHigh.GetValue( warnHigh );
		pRange->WarnLow.GetValue( warnLow );
	}
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogPassFailPageClass::OnKillActive() 
{
	UpdateData(TRUE);

	UpdateRangeValues();
	
	return CPropertyPage::OnKillActive();
}

void SVToolAdjustmentDialogPassFailPageClass::OnOK() 
{
	UpdateData(TRUE);

	UpdateRangeValues();
}

void SVToolAdjustmentDialogPassFailPageClass::UpdateRangeValues()
{
	SetInspectionData();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgPassFailPage.cpp_v  $
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
