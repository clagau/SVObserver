//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerFeatureListBox
//* .File Name       : $Workfile:   SVBlobAnalyzerFeatureListBox.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 10:22:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVBlobAnalyzerFeatureListBox.h"
#include "SVBlobAnalyzer.h"

// e.g. IMPLEMENT_DYNCREATE(<ClassName>, <MFCBaseClass>)

BEGIN_MESSAGE_MAP(SVBlobAnalyzeFeatureListBoxClass, CListBox)
	//{{AFX_MSG_MAP(SVBlobAnalyzeFeatureListBoxClass)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVBlobAnalyzeFeatureListBoxClass::SVBlobAnalyzeFeatureListBoxClass()
{
}

SVBlobAnalyzeFeatureListBoxClass::~SVBlobAnalyzeFeatureListBoxClass()
{
}


void SVBlobAnalyzeFeatureListBoxClass::init(SVBlobAnalyzerClass* apCurrentAnalyzer, 
                                            TCHAR atcTestChar)
{

    SVBlobFeatureEnum   i;

    long                lEmpty;
	long			    lReturned;
    long                lError;

    lError = 0;

    while (1)
    {
        ResetContent();

        lEmpty = TRUE;

		for (i = SV_AREA; 
             i < SV_TOPOF_LIST; 
             i = (SVBlobFeatureEnum) (i + 1))
		{	
        
            if (apCurrentAnalyzer->msvszFeaturesEnabled [i] == atcTestChar)
            {
                //
                // Skip the M_SUM_PIXEL blob feature if doing 'available' list.
                //
                if(i == SV_SUM_PIXEL && atcTestChar == (TCHAR)'0')
                {
                    continue;
                }

				lReturned = AddString( apCurrentAnalyzer->msvValue [i].GetName ());
                
                if (lReturned == LB_ERR || lReturned == LB_ERRSPACE)
                {
                    lError = -1147;
                    break;
                }

    			lReturned = SetItemData( lReturned, static_cast<DWORD_PTR>(i) );
                if (lReturned == LB_ERR)
                {
                    lError = -1148;
                    break;
                }

                lEmpty = FALSE;
	        }

		}

        if (lEmpty)
        {
			SetItemData( AddString( _T( "(No Feature)" ) ), LB_ERR );
        }

		SetCurSel( 0 );

        break;
    }
}

BOOL SVBlobAnalyzeFeatureListBoxClass::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ! ( LBS_MULTIPLESEL | LBS_EXTENDEDSEL );
	return CListBox::PreCreateWindow(cs);
}

BOOL SVBlobAnalyzeFeatureListBoxClass::Create(
	LPCTSTR lpszClassName, 
	LPCTSTR lpszWindowName, 
	DWORD dwStyle, 
	const RECT& rect, 
	CWnd* pParentWnd, 
	UINT nID, 
	CCreateContext* pContext
) 
{
	return CWnd::Create(
		lpszClassName, 
		lpszWindowName, 
		dwStyle, 
		rect, 
		pParentWnd, 
		nID, 
		pContext
	);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBlobAnalyzerFeatureListBox.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 10:22:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:42:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   01 Jun 2010 08:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 17:07:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   19 Apr 2000 18:37:12   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBlobAnalyzeFeatureListBox::init () was modified to remove references to SVBlobFeatureClass, and impliment SVBlobFeatureEnum.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 16 2000 16:17:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to add 26 more Blob features that SVObserver can calculate using the MIL 6.0 library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:11:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Nov 16 1999 11:07:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed M_SUM_PIXEL blob feature which caused a 'crash' on binarized images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Sep 1999 17:25:42   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/