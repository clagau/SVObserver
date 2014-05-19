//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogStatisticsPageClass
//* .File Name       : $Workfile:   SVTADlgStatisticsPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:13:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgStatisticsPage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVToolsetOutputSelectionDialog.h"
#include "SVDlgResultPicker.h"
#include "SVToolSet.h"
#include "SVStatTool.h"
#include "SVResult.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////
// Error Codes used by this program
///////////////////////////////////////////////////////////
enum
{
	ErrorBase = 15000,
	Err_15005 = (ErrorBase+5),
	Err_15006 = (ErrorBase+6),
	Err_15007 = (ErrorBase+7),
	Err_15008 = (ErrorBase+8),
};

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogStatisticsPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogStatisticsPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnButtonAdd)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnButtonRemove)
	ON_BN_CLICKED(IDC_SET_RANGE, OnSetRange)
	ON_BN_CLICKED(IDC_PUBLISH_BUTTON, OnPublishButton)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PICKER, OnBtnObjectPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void SVToolAdjustmentDialogStatisticsPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVToolAdjustmentDialogStatisticsPageClass)
	DDX_Control(pDX, IDC_CHILDREN_LIST, m_lbSelectedList);
	DDX_Control(pDX, IDC_AVAILABLE_CHILDREN_LIST, m_lbAvailableList);
	DDX_Text(pDX, IDC_OCCURANCE_TO_TRACK_EDT, m_strTestValue);
	DDX_Text(pDX, IDC_VARIABLE_TO_MONITOR, m_strVariableToMonitor);
	//}}AFX_DATA_MAP
}

SVToolAdjustmentDialogStatisticsPageClass::SVToolAdjustmentDialogStatisticsPageClass( SVToolClass* p_pTool ) 
	: CPropertyPage( SVToolAdjustmentDialogStatisticsPageClass::IDD )
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogStatisticsPageClass)
	m_strTestValue = _T( "" );
	m_strVariableToMonitor = _T("");
	//}}AFX_DATA_INIT

	m_pTool = NULL;
	m_pToolSet = NULL;

	if( m_pTool = dynamic_cast <SVStatisticsToolClass*> (p_pTool) )
	{
		m_pToolSet = dynamic_cast <SVToolSetClass*> (m_pTool->GetOwner());
	}
}

SVToolAdjustmentDialogStatisticsPageClass::~SVToolAdjustmentDialogStatisticsPageClass()
{
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pTool )
	{
		getParameters();

		SVObjectReference refObject = m_pTool->GetVariableSelected();
		m_strVariableToMonitor = refObject.GetCompleteObjectNameToObjectType( NULL, m_pToolSet->GetObjectType() );
		m_strFullNameOfVariable = refObject.GetCompleteObjectName();
		//initVariablesComboBox();
	}

	UpdateData( FALSE ); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnButtonAdd() 
{
	DWORD_PTR    lAvailableIndex;
	DWORD_PTR    lCurrentIndex;

    lAvailableIndex = m_lbAvailableList.GetItemData( m_lbAvailableList.GetCurSel() );

    lCurrentIndex = m_lbSelectedList.GetItemData( m_lbSelectedList.GetCurSel() );


	if( lAvailableIndex != LB_ERR && lAvailableIndex >= 0)
	{
	    m_pTool->EnableFeature ((SVStatisticsFeatureEnum) lAvailableIndex);

		// List of not enabled.
		initListBox( &m_lbAvailableList, _T('0') );

		// List of enabled.
		initListBox( &m_lbSelectedList, _T('1') );

        UpdateData (FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnButtonRemove() 
{

	int index = ( int ) m_lbSelectedList.GetItemData( m_lbSelectedList.GetCurSel() );

	if( index != LB_ERR && index >= 0 )
	{
        m_pTool->DisableFeature ((SVStatisticsFeatureEnum) index);

		// List of not enabled.
		initListBox( &m_lbAvailableList, _T('0') );
		
		// List of enabled.
		initListBox( &m_lbSelectedList, _T('1') );

        UpdateData (FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::initListBox(CListBox* pListBox, TCHAR atcTestChar)
{

    long lEmpty;
	long lReturned;
    long lError;

    lError = 0;

    while (1)
    {
		CString featureStr;
		featureStr = m_pTool->GetFeatureString();
		
        pListBox->ResetContent();

        lEmpty = TRUE;

		for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i++ )
		{	
            if (featureStr [i] == atcTestChar)
            {
				lReturned = pListBox->AddString( m_pTool->GetFeatureName( i ) );
                
                if (lReturned == LB_ERR || lReturned == LB_ERRSPACE)
                {
                    lError = -Err_15005;
                    break;
                }

    			lReturned = pListBox->SetItemData( lReturned, static_cast<DWORD_PTR>(i) );
                if (lReturned == LB_ERR)
                {
                    lError = -Err_15006;
                    break;
                }
                lEmpty = FALSE;
	        }
		}

		pListBox->SetCurSel( 0 );

        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::getParameters()
{
	// Get Occurence Value
	if( m_pTool )
		m_strTestValue = m_pTool->GetOccurenceTestValue();

	// List of not enabled.
	initListBox( &m_lbAvailableList, _T('0') );

	// List of enabled.
	initListBox( &m_lbSelectedList, _T('1') );
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::UpdateStatisticsParameters()
{
	if( m_pTool )
	{
		// Get the Occurence Value
		m_pTool->SetOccurenceTestValue( m_strTestValue );

		// Get selected variable
		m_pTool->SetVariableSelected( m_strFullNameOfVariable );
	}
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnOK()
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogStatisticsPageClass::OnKillActive() 
{
	UpdateData(TRUE);

	UpdateStatisticsParameters();
	
	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnSetRange() 
{
    SVResultClass* pResult;
	SVErrorClass err;
    err.ClearLastErrorCd ();

    while (1)
    {
		// Get Selected feature
		int item = m_lbSelectedList.GetCurSel();
		if( item != LB_ERR )
		{
			DWORD_PTR index = m_lbSelectedList.GetItemData( item );
			pResult = m_pTool->GetResultObject( (SVStatisticsFeatureEnum)index );

			if (!pResult)
			{
				err.msvlErrorCd = -Err_15007;
				SV_TRAP_ERROR_BRK (err, Err_15007);
			}

			if (SVSetupDialogManager::Instance().SetupDialog( pResult->GetClassID(), pResult->GetUniqueObjectID(), this ) != S_OK)
			{
				err.msvlErrorCd = -Err_15008;
				SV_TRAP_ERROR_BRK (err, Err_15008);
			}
		}
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
void SVToolAdjustmentDialogStatisticsPageClass::OnPublishButton() 
{
	SVDlgResultPicker   dlg;

    CString  publishedResultString;

    dlg.PTaskObjectList = m_pTool;
    dlg.uAttributesDesired = SV_PUBLISHABLE;

    publishedResultString.LoadString ( IDS_PUBLISHABLE_RESULTS );
    dlg.SetCaptionTitle (publishedResultString);

	if( dlg.DoModal() == IDOK )
	{
		// refresh the publish list
		SVInspectionProcess* pInspection = m_pTool->GetInspection();
		ASSERT( pInspection );
		if( pInspection )
		{
			SVPublishListClass& publishList = pInspection->GetPublishList();
			publishList.Refresh( static_cast <SVTaskObjectClass*> (pInspection->GetToolSet()) );

			SVIPDoc* l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}
	}
	
}

void SVToolAdjustmentDialogStatisticsPageClass::OnBtnObjectPicker() 
{
	m_pTool->UpdateTaskObjectOutputListAttributes();

	SVToolsetOutputSelectionDialog dlg;
	
	dlg.PTaskObjectList = m_pToolSet;
	dlg.uAttributesDesired = SV_SELECTABLE_FOR_STATISTICS;
	dlg.m_bUpdateObjectAttributes = true;

	if( dlg.DoModal() == IDOK )
	{
		m_strVariableToMonitor = dlg.m_sSelectedOutputName;
		SVObjectReference refObject = dlg.m_refSelectedObject;
		m_pTool->SetVariableSelected( refObject.GetCompleteObjectName() );
		m_strFullNameOfVariable = refObject.GetCompleteObjectName();
		UpdateData( FALSE );
	}
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgStatisticsPage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 13:13:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:18:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   03 Aug 2012 10:48:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with error number definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   02 Jul 2012 17:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   08 Dec 2010 13:39:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Aug 2005 14:53:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   in OnBtnObjectPicker, call tool UpdateTaskObjectOutputListAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   28 Jul 2005 09:15:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed dead code
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   28 Jul 2005 08:16:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added capability for SVToolsetOutputSelectionDialog to update object attributes when done selecting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   27 Jul 2005 16:05:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated StatisticsTool to use SVObjectReference (array value objects) instead of GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Jun 2005 08:32:08   ebeyeler
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
 *    Rev 3.2   17 Feb 2005 16:04:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:21:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Nov 10 1999 12:38:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Documents marked as changed when Dialog OK buttons pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Oct 1999 16:03:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetComboBoxStringExtent to return 80% of the 
 * string text extent size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Oct 1999 20:02:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnOk method and UpdateStatisticsParameters
 * method.
 * Revised m_strTestValue from type double to type CString.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Oct 1999 16:15:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Code to get the currently selected result for setting the range 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 1999 16:36:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Logic to setup results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Oct 1999 13:07:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. Dialog to setup SVStatisticsToolClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
