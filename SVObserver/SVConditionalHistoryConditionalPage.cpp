//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistoryConditionalPage
//* .File Name       : $Workfile:   SVConditionalHistoryConditionalPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:00:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVConditionalHistoryConditionalPage.h"
#include "SVConditionalHistorySheet.h"
#include "SVInspectionProcess.h"
#include "SVTaskObjectList.h"
#include "SVToolSet.h"
#include "SVIPDoc.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistoryConditionalPage property page

BEGIN_MESSAGE_MAP(SVConditionalHistoryConditionalPage, SVConditionalHistoryBasePage)
	//{{AFX_MSG_MAP(SVConditionalHistoryConditionalPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVConditionalHistoryConditionalPage::SVConditionalHistoryConditionalPage( SVConditionalHistorySheet* pParent, const CString& szCaption, int id )
: SVConditionalHistoryBasePage(pParent, szCaption, id)
{
	//{{AFX_DATA_INIT(SVConditionalHistoryConditionalPage)
	//}}AFX_DATA_INIT
}

SVConditionalHistoryConditionalPage::~SVConditionalHistoryConditionalPage()
{
}
#pragma endregion Constructor

void SVConditionalHistoryConditionalPage::DoDataExchange(CDataExchange* pDX)
{
	SVConditionalHistoryBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVConditionalHistoryConditionalPage)
	//}}AFX_DATA_MAP
}

bool SVConditionalHistoryConditionalPage::QueryAllowExit()
{
	return true;
}

void SVConditionalHistoryConditionalPage::OnSelChangeListSelected()
{
}

BOOL SVConditionalHistoryConditionalPage::OnInitDialog()
{
	SVConditionalHistoryBasePage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVConditionalHistoryConditionalPage::InitPage( SVInspectionProcess* pInspection )
{
	ASSERT( nullptr != pInspection );
	if( nullptr == pInspection )
	{
		return;
	}
	// load values
	std::vector<SVScalarValue> vecValues, vecImages, vecConditionals;
	HRESULT hr = pInspection->GetConditionalHistoryList( vecValues, vecImages, vecConditionals );

	m_Tree.InitOutputListTreeCtrl();

	SVToolSetClass* pToolSet = pInspection->GetToolSet();
	SVTaskObjectListClass* pTaskObjectList = static_cast <SVTaskObjectListClass*> ( pToolSet );
	m_Tree.BuildOutputList( pTaskObjectList, SV_CH_CONDITIONAL, SV_CH_CONDITIONAL | SV_VIEWABLE );
}

void SVConditionalHistoryConditionalPage::OnOK()
{
	std::set<SVObjectReference> setChanged;
	HRESULT hr = m_Tree.GetChangedObjects( setChanged );
	m_bObjectsChanged = !setChanged.empty();

	std::vector<SVObjectReference> vecSelected;
	m_Tree.GetSelectedObjects( vecSelected );

	std::vector<SVScalarValue> vecValues;
	vecValues.reserve( vecSelected.size() );
	for ( size_t i=0; i < vecSelected.size(); ++i )
		vecValues.push_back( SVScalarValue( vecSelected[i] ) );

	m_pSheet->m_pInspection->SetConditionalHistoryList( NULL, NULL, &vecValues, false );
	// sheet does the reset for efficiency
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistoryConditionalPage.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:00:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Dec 2010 07:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Oct 2010 13:39:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2007 12:01:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2006 09:45:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   set Tree attributes so the tree elements are correct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   26 Jan 2006 16:27:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed tree selection bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2006 12:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added object list functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/