//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTwoPointRotateDialog
//* .File Name       : $Workfile:   SVTwoPointRotateDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:52:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVTwoPointRotateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef SVTESTI

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVTwoPointRotateDialogClass 


SVTwoPointRotateDialogClass::SVTwoPointRotateDialogClass( CWnd* pParent /*=NULL*/ )
							:CDialog( SVTwoPointRotateDialogClass::IDD, pParent )
{
	PTool			= NULL;
	PToolSet		= NULL;
	PResultList		= NULL;
	POriginalResult = NULL;
	PAnalyzer       = NULL;

	pOriginalTwoPointAnalyzer	= NULL;
	pTwoPointAnalyzer			= NULL;

	//{{AFX_DATA_INIT(SVTwoPointRotateDialogClass)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void SVTwoPointRotateDialogClass::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(SVTwoPointRotateDialogClass)
	DDX_Control( pDX, IDC_SECOND_Y_COMBO, SecondYCombo );
	DDX_Control( pDX, IDC_SECOND_X_COMBO, SecondXCombo );
	DDX_Control( pDX, IDC_FIRST_Y_COMBO, FirstYCombo );
	DDX_Control( pDX, IDC_FIRST_X_COMBO, FirstXCombo );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( SVTwoPointRotateDialogClass, CDialog )
	//{{AFX_MSG_MAP(SVTwoPointRotateDialogClass)
	ON_BN_CLICKED( IDC_LEARN_BUTTON, OnLearnButton )
	ON_CBN_SELCHANGE(IDC_FIRST_X_COMBO, OnSelChangeFirstXCombo)
	ON_CBN_SELCHANGE(IDC_FIRST_Y_COMBO, OnSelChangeFirstYCombo)
	ON_CBN_SELCHANGE(IDC_SECOND_X_COMBO, OnSelChangeSecondXCombo)
	ON_CBN_SELCHANGE(IDC_SECOND_Y_COMBO, OnSelChangeSecondYCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVTwoPointRotateDialogClass 

void SVTwoPointRotateDialogClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVTwoPointRotateDialogClass::OnOK() 
{
	if( pOriginalTwoPointAnalyzer )
	{
		pOriginalTwoPointAnalyzer->PreDestroy()->Destroy();
		pOriginalTwoPointAnalyzer = NULL;
	}
	
	CDialog::OnOK();
}

BOOL SVTwoPointRotateDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( PTool && PToolSet && PResultList && /*POriginalResult && */
		PAnalyzer /*&& PAnalyzer->IsKindOf( RUNTIME_CLASS( SVTwoPointRotationAnalyzerClass ) )*/
	  )
	{
		// Save the original...
		pOriginalTwoPointAnalyzer = ( SVTwoPointRotationAnalyzerClass* ) PAnalyzer->BuildCopy();

		// Init current...
		pTwoPointAnalyzer = ( SVTwoPointRotationAnalyzerClass* ) PAnalyzer;

		// Collect all proceeding results...
		SVResultClass resultList;
		for( int i = 0; i < PToolSet->GetIndex( PTool ); ++i )
			if( PResultList->GetAt( i ) )
				PResultList->GetAt( i )->GetBottomResults( &resultList );

		int comboIndex = 0;
		// Fill first x combo...
		for( i = 0; i < resultList.GetSize(); ++i )
		{
			if( resultList.GetAt( i ) && resultList.GetAt( i )->GetResourceID() == IDS_CLASSNAME_SVXRESULT )
			{
				FirstXCombo.SetItemData( comboIndex = FirstXCombo.AddString( resultList.GetAt( i )->GetCompleteObjectName() ), ( DWORD ) resultList.GetAt( i ) );
				if( resultList.GetAt( i )->GetUniqueObjectID() == pTwoPointAnalyzer->firstPointXGUID && pTwoPointAnalyzer->firstPointXGUID != SVInvalidGUID )
					FirstXCombo.SetCurSel( comboIndex );
			}
		}

		// Fill first y combo...
		for( i = 0; i < resultList.GetSize(); ++i )
		{
			if( resultList.GetAt( i ) && resultList.GetAt( i )->GetResourceID() == IDS_CLASSNAME_SVYRESULT )
			{
				FirstYCombo.SetItemData( comboIndex = FirstYCombo.AddString( resultList.GetAt( i )->GetCompleteObjectName() ), ( DWORD ) resultList.GetAt( i ) );
				if( resultList.GetAt( i )->GetUniqueObjectID() == pTwoPointAnalyzer->firstPointYGUID && pTwoPointAnalyzer->firstPointYGUID != SVInvalidGUID )
					FirstYCombo.SetCurSel( comboIndex );
			}
		}

		// Fill second x combo...
		for( i = 0; i < resultList.GetSize(); ++i )
		{
			if( resultList.GetAt( i ) && resultList.GetAt( i )->GetResourceID() == IDS_CLASSNAME_SVXRESULT )
			{
				SecondXCombo.SetItemData( comboIndex = SecondXCombo.AddString( resultList.GetAt( i )->GetCompleteObjectName() ), ( DWORD ) resultList.GetAt( i ) );
				if( resultList.GetAt( i )->GetUniqueObjectID() == pTwoPointAnalyzer->secondPointXGUID && pTwoPointAnalyzer->secondPointXGUID != SVInvalidGUID )
					SecondXCombo.SetCurSel( comboIndex );
			}
		}

		// Fill second y combo...
		for( i = 0; i < resultList.GetSize(); ++i )
		{
			if( resultList.GetAt( i ) && resultList.GetAt( i )->GetResourceID() == IDS_CLASSNAME_SVYRESULT )
			{
				SecondYCombo.SetItemData( comboIndex = SecondYCombo.AddString( resultList.GetAt( i )->GetCompleteObjectName() ), ( DWORD ) resultList.GetAt( i ) );
				if( resultList.GetAt( i )->GetUniqueObjectID() == pTwoPointAnalyzer->secondPointYGUID && pTwoPointAnalyzer->secondPointYGUID != SVInvalidGUID )
					SecondYCombo.SetCurSel( comboIndex );
			}
		}

	}
	else
		SendMessage( WM_CLOSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVTwoPointRotateDialogClass::OnLearnButton() 
{
	if( POriginalResult )
		POriginalResult->Reset();
}

void SVTwoPointRotateDialogClass::OnSelChangeFirstXCombo() 
{
	int comboIndex = 0;
	if( pTwoPointAnalyzer && ( comboIndex = FirstXCombo.GetCurSel() ) >= 0 && comboIndex != CB_ERR )
	{
		pTwoPointAnalyzer->pFirstPointX = ( SVNewResultClass* ) FirstXCombo.GetItemData( comboIndex );
		if( pTwoPointAnalyzer->pFirstPointX )
			pTwoPointAnalyzer->firstPointXGUID = pTwoPointAnalyzer->pFirstPointX->GetUniqueObjectID();
		else
			pTwoPointAnalyzer->firstPointXGUID = SVInvalidGUID;
	}
}

void SVTwoPointRotateDialogClass::OnSelChangeFirstYCombo() 
{
	int comboIndex = 0;
	if( pTwoPointAnalyzer && ( comboIndex = FirstYCombo.GetCurSel() ) >= 0 && comboIndex != CB_ERR )
	{
		pTwoPointAnalyzer->pFirstPointY = ( SVNewResultClass* ) FirstYCombo.GetItemData( comboIndex );
		if( pTwoPointAnalyzer->pFirstPointY )
			pTwoPointAnalyzer->firstPointYGUID = pTwoPointAnalyzer->pFirstPointY->GetUniqueObjectID();
		else
			pTwoPointAnalyzer->firstPointYGUID = SVInvalidGUID;
	}
}

void SVTwoPointRotateDialogClass::OnSelChangeSecondXCombo() 
{
	int comboIndex = 0;
	if( pTwoPointAnalyzer && ( comboIndex = SecondXCombo.GetCurSel() ) >= 0 && comboIndex != CB_ERR )
	{
		pTwoPointAnalyzer->pSecondPointX = ( SVNewResultClass* ) SecondXCombo.GetItemData( comboIndex );
		if( pTwoPointAnalyzer->pSecondPointX )
			pTwoPointAnalyzer->secondPointXGUID = pTwoPointAnalyzer->pSecondPointX->GetUniqueObjectID();
		else
			pTwoPointAnalyzer->secondPointXGUID = SVInvalidGUID;
	}
}

void SVTwoPointRotateDialogClass::OnSelChangeSecondYCombo() 
{
	int comboIndex = 0;
	if( pTwoPointAnalyzer && ( comboIndex = SecondYCombo.GetCurSel() ) >= 0 && comboIndex != CB_ERR )
	{
		pTwoPointAnalyzer->pSecondPointY = ( SVNewResultClass* ) SecondYCombo.GetItemData( comboIndex );
		if( pTwoPointAnalyzer->pSecondPointY )
			pTwoPointAnalyzer->secondPointYGUID = pTwoPointAnalyzer->pSecondPointY->GetUniqueObjectID();
		else
			pTwoPointAnalyzer->secondPointYGUID = SVInvalidGUID;
	}
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTwoPointRotateDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:52:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 17:18:42   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:28:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed reference to SVToolSetViewClass and added header.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/