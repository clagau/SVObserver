//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPixelAnalyzerSetup
//* .File Name       : $Workfile:   svpixelanalyzersetup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Sep 2013 11:11:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVPixelAnalyzerSetup.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVPixelAnalyzer.h"        // Required for SVPixelAnalyzerClass
#include "SVTool.h"
#include "SVToolSet.h"              // Required for scripting
#include "SVResult.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPixelAnalyzerSetupClass::SVPixelAnalyzerSetupClass(SVPixelAnalyzerClass* apAnalyzer, 
                                                     CWnd* pParent /*=NULL*/)
	: CDialog(SVPixelAnalyzerSetupClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPixelAnalyzerSetupClass)
	msvGrayscaleToCount = 0;
	msvHighGrayscaleValue = _T("");
	msvLowGrayscaleValue = _T("");
	msvGrayscaleSliderValue = 0;
	//}}AFX_DATA_INIT

    init (apAnalyzer, pParent);
}


HRESULT SVPixelAnalyzerSetupClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( msvpAnalyzer != NULL )
	{
		l_hrOk = AddInputRequest( &( msvpAnalyzer->pixelCountColor ), msvGrayscaleToCount );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( msvpAnalyzer->GetTool() );
		}

		UpdateData( FALSE );
	}

	return l_hrOk;
}

unsigned long SVPixelAnalyzerSetupClass::init (SVPixelAnalyzerClass *apAnalyzer, 
                                               CWnd* apParent)
{
    SVObjectTypeInfoStruct  pixelAnalyzerToolInfo;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        msvpParent = apParent;
        msvpAnalyzer = apAnalyzer;
        if (!msvpAnalyzer)
        {
            msvError.msvlErrorCd = -1098;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1098);
        }

        msvpTool = (SVToolClass *) msvpAnalyzer->GetOwner ();
        if (!msvpTool)
        {
            msvError.msvlErrorCd = -1096;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1096);
        }
        break;
    }

    return msvError.GetLastErrorCd ();
}


void SVPixelAnalyzerSetupClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPixelAnalyzerSetupClass)
	DDX_Control(pDX, IDC_GRAYSCALE_SLD, msvGrayscaleSld);
	DDX_Control(pDX, IDC_OTHER_RAD, msvOtherRad);
	DDX_Control(pDX, IDC_WHITE_RAD, msvWhiteRad);
	DDX_Control(pDX, IDC_BLACK_RAD, msvBlackRad);
	DDX_Control(pDX, IDC_GRAYSCALE_EDT, msvGrayScaleEdt);
	DDX_Text(pDX, IDC_GRAYSCALE_EDT, msvGrayscaleToCount);
	DDX_Text(pDX, IDC_HIGHGRAYSCALE, msvHighGrayscaleValue);
	DDX_Text(pDX, IDC_LOWGRAYSCALE, msvLowGrayscaleValue);
	DDX_Slider(pDX, IDC_GRAYSCALE_SLD, msvGrayscaleSliderValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPixelAnalyzerSetupClass, CDialog)
	//{{AFX_MSG_MAP(SVPixelAnalyzerSetupClass)
	ON_BN_CLICKED(IDC_WHITE_RAD, OnWhiteRad)
	ON_BN_CLICKED(IDC_BLACK_RAD, OnBlackRad)
	ON_BN_CLICKED(IDC_OTHER_RAD, OnOtherRad)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_GRAYSCALE_EDT, OnChangeGrayscaleEdt)
	ON_BN_CLICKED(IDC_PIXEL_SET_RANGE, OnPixelSetRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPixelAnalyzerSetupClass message handlers

void SVPixelAnalyzerSetupClass::OnOK() 
{
	// TODO: Add extra validation here
    ASSERT(msvpTool);
    if(msvpTool)
    {
		SVIPDoc* l_pIPDoc = NULL;

		if( msvpTool->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( msvpTool->GetInspection()->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}
    }
    	
	CDialog::OnOK();
}


BOOL SVPixelAnalyzerSetupClass::OnInitDialog() 
{
    BYTE    byGrayscale;

    msvError.ClearLastErrorCd ();

    while (1)
    {
       	CDialog::OnInitDialog();

				SetTaskObject( msvpTool );

        if (!msvpTool)
        {
            msvError.msvlErrorCd = -1103;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1103);
        }


        msvulMinGrayscale = 0;
        msvulMaxGrayscale = (1 << msvpAnalyzer->GetInputPixelDepth ()) - 1;

	    msvHighGrayscaleValue.Format (_T("%d"), msvulMaxGrayscale);
		msvLowGrayscaleValue.Format (_T("%d"), msvulMinGrayscale);

        msvpAnalyzer->pixelCountColor.GetValue(byGrayscale);

        if (byGrayscale == msvulMinGrayscale) //Black
        {
            SetBlack ();
        }
        else
        if (byGrayscale == msvulMaxGrayscale) //White
        {
            SetWhite ();
        }
        else
        {
            SetOther (byGrayscale);

        }
            
        msvGrayscaleSld.SetRangeMin (-((long)msvulMaxGrayscale), FALSE);
        msvGrayscaleSld.SetRangeMax (-((long)msvulMinGrayscale), FALSE);

        UpdateData (FALSE);
        break;
    }
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


unsigned long SVPixelAnalyzerSetupClass::SetBlack ()
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_UNCHECKED);
        msvBlackRad.SetCheck (BST_CHECKED);
		msvOtherRad.SetCheck(BST_UNCHECKED);
        msvGrayscaleSld.EnableWindow (FALSE);
        msvGrayScaleEdt.EnableWindow (FALSE);

        SetGrayscale (msvulMinGrayscale);
        break;
    }


    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetWhite ()
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_CHECKED);
        msvBlackRad.SetCheck (BST_UNCHECKED);
		msvOtherRad.SetCheck(BST_UNCHECKED);
        msvGrayscaleSld.EnableWindow (FALSE);
        msvGrayScaleEdt.EnableWindow (FALSE);

        SetGrayscale (msvulMaxGrayscale);
        break;
    }

    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetOther (long alGrayscale)
{
    msvError.ClearLastErrorCd ();

    while (1)
    {
		msvWhiteRad.SetCheck(BST_UNCHECKED);
        msvBlackRad.SetCheck (BST_UNCHECKED);
		msvOtherRad.SetCheck(BST_CHECKED);
        msvGrayscaleSld.EnableWindow (TRUE);
        msvGrayScaleEdt.EnableWindow (TRUE);
        
        SetGrayscale (alGrayscale);
        break;
    }

    return msvError.GetLastErrorCd ();
}


unsigned long SVPixelAnalyzerSetupClass::SetGrayscale (long alGrayscale)
{
	msvError.ClearLastErrorCd ();
	
	msvGrayscaleToCount = alGrayscale;
	msvGrayscaleSliderValue = -alGrayscale;

	SetInspectionData();
	
	return msvError.GetLastErrorCd ();
}



void SVPixelAnalyzerSetupClass::OnWhiteRad() 
{
    SetWhite ();	
}


void SVPixelAnalyzerSetupClass::OnBlackRad() 
{
    SetBlack ();	
}


void SVPixelAnalyzerSetupClass::OnOtherRad() 
{
    BYTE    byGrayscale;

    msvpAnalyzer->pixelCountColor.GetValue(byGrayscale);
    SetOther (byGrayscale);	
}


void SVPixelAnalyzerSetupClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	UpdateData (TRUE);
	SetGrayscale (-msvGrayscaleSliderValue);
}

void SVPixelAnalyzerSetupClass::OnChangeGrayscaleEdt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData (TRUE);
	SetGrayscale (msvGrayscaleToCount);
	
}

void SVPixelAnalyzerSetupClass::OnCancel() 
{
	CDialog::OnCancel();
}


void SVPixelAnalyzerSetupClass::OnPixelSetRange() 
{
	// TODO: Add your control notification handler code here
    SVResultClass* pAnalyzerResult;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        pAnalyzerResult = msvpAnalyzer->GetResultObject();

        if (!pAnalyzerResult)
        {
            msvError.msvlErrorCd = -1121;
            SV_TRAP_ERROR_BRK (msvError, 1121);
        }

		if (SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), msvpParent) != S_OK)
        {
            msvError.msvlErrorCd = -1122;
            SV_TRAP_ERROR_BRK (msvError, 1122);
        }

        break;
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\svpixelanalyzersetup.cpp_v  $
 * 
 *    Rev 1.1   25 Sep 2013 11:11:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  862
 * SCR Title:  Pixel Analyzer Dialog Displays Multiple Selections for Count if Using Keyboard
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed SetBlack, SetWhite, and SetOther methods to uncheck the non-selected options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:22:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   04 Sep 2012 15:56:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   02 Jul 2012 17:30:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Dec 2010 13:25:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Dec 2009 12:25:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   09 Aug 2005 07:59:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 15:17:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 15:01:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 13:52:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 10:44:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 *  Changed TheObjectManager -> TheSVObjectManager
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   19 Apr 2001 20:24:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 20:05:16   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:26:08   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Nov 10 1999 12:44:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Documents marked as modified if Dialog OK button pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 19:01:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed call to RemoveExternalDependencies
 * (Fix for restoring inputs via script)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Sep 1999 18:45:10   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  30
 * SCR Title:  Change White Pixel Analyzer to work on all Gray Scale values.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Added support for Set Range button.  
 * 
 * Added 
 * void SVPixelAnalyzerSetupClass::OnPixelSetRange() 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Sep 1999 13:19:26   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  30
 * SCR Title:  Change White Pixel Analyzer to work on all Gray Scale values.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Initial integration of setup dialog for SVPixelAnalyzerSetupClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
