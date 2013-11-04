//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTAReferenceAdjustmentDialogClass
//* .File Name       : $Workfile:   SVAdjustmentDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:58  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

#include "SVAnalyzer.h"

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVAdjustmentDialog.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************














//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Dialog Class SVTAReferenceAdjustmentDialogClass 
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVTAReferenceAdjustmentDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVTAReferenceAdjustmentDialogClass)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeContrastEdit)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Constructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Standard constructor of class ...
// -----------------------------------------------------------------------------
// .Description : This constructor inits ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: // e.g. int&				MyInt				Index Handle 
//  :
// .Output(s)
//	: // e.g. int&				MyInt				Index Handle 
//  :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
SVTAReferenceAdjustmentDialogClass::SVTAReferenceAdjustmentDialogClass( CWnd* pParent, SVAnalyzerClass* A )
	: CDialog(SVTAReferenceAdjustmentDialogClass::IDD, pParent)
{
	currentAnalyzer		= A;

	//{{AFX_DATA_INIT(SVTAReferenceAdjustmentDialogClass)
	contrastEdit = 0;
	//}}AFX_DATA_INIT
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

//******************************************************************************
// Operator(s):
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
// ... Operator
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Writing Access And Data Exchange:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : // e.g. Put string member function of class ...
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVTAReferenceAdjustmentDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTAReferenceAdjustmentDialogClass)
	DDX_Control(pDX, IDC_SLIDER2, contrastSlider);
	DDX_Text(pDX, IDC_EDIT2, contrastEdit);
	DDV_MinMaxByte(pDX, contrastEdit, 0, 255);
	//}}AFX_DATA_MAP
}


//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
// Drawing Operations(s)
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************

//******************************************************************************
// Message Operation(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnInitDialog
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVTAReferenceAdjustmentDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !currentAnalyzer )
	{
		CDialog::OnCancel();
		return FALSE;
	};

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnVScroll
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: UINT				nSBCode
//	:	UINT			nPos
//	:	CScrollBar*			pScrollBar
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVTAReferenceAdjustmentDialogClass::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	UpdateData( TRUE ); // get data of dialog	

	if( &contrastSlider == ( CSliderCtrl* ) pScrollBar )
	{
		contrastEdit = ( BYTE ) contrastSlider.GetPos();		
	};

	UpdateData( FALSE ); // set data to dialog

	currentAnalyzer->MakeDirty();

	CDialog::OnVScroll( nSBCode, nPos, pScrollBar );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnCancel
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVTAReferenceAdjustmentDialogClass::OnCancel() 
{
//	rEx->contrast = originalContrast;
	currentAnalyzer->MakeDirty();	
	CDialog::OnCancel();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnOK
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVTAReferenceAdjustmentDialogClass::OnOK() 
{
	currentAnalyzer->MakeDirty();
	CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnChangeContrastEdit
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :05.06.1989				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
void SVTAReferenceAdjustmentDialogClass::OnChangeContrastEdit() 
{
	UpdateData( TRUE ); // get data of dialog	
	
	contrastSlider.SetPos( ( int ) contrastEdit );		

	UpdateData( FALSE ); // set data to dialog
	
//	rEx->contrast = ( double ) contrastEdit;
	currentAnalyzer->MakeDirty();	
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAdjustmentDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:26:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   17 Apr 2003 16:33:38   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/