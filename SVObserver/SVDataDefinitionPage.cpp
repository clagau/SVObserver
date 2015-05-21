//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionPage
//* .File Name       : $Workfile:   SVDataDefinitionPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 13:58:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDataDefinitionPage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVToolSet.h"
#include "SVIPDoc.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVDataDefinitionPage::SVDataDefinitionPage( SVDataDefinitionSheet* pParent, CString p_szCaption)
	: CPropertyPage( SVDataDefinitionPage::IDD)
{
	m_strCaption = p_szCaption;
	m_pSheet = pParent;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;

	if( m_strCaption.Find("Image") != -1 )
	{
		m_bIsImagesPage = true;
	}
	else
	{
		m_bIsImagesPage = false;
	}

	//{{AFX_DATA_INIT(SVDataDefinitionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVDataDefinitionPage::~SVDataDefinitionPage()
{
}
#pragma endregion Constructor

void SVDataDefinitionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDataDefinitionPage)
	DDX_Control(pDX, IDC_LIST_SELECTED, m_lbSelected);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_BTN_CLEAR_ALL, m_btnClearAll);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	//}}AFX_DATA_MAP
}

void SVDataDefinitionPage::InitPage( SVInspectionProcess* pInspection )
{
	ASSERT( nullptr != pInspection );
	if( nullptr == pInspection )
	{
		return;
	}

	m_Tree.InitOutputListTreeCtrl();
	m_Tree.AllowNodeItemChecks( true );
	SVToolSetClass* pToolSet = pInspection->GetToolSet();

	if( m_bIsImagesPage )
	{
		SVImageListClass l_ImageList;
		pToolSet->GetImageList( l_ImageList );
		m_Tree.BuildImageListTree( &l_ImageList, SV_DD_IMAGE, SVNotSetObjectType );
	}
	else
	{
		SVTaskObjectListClass* pTaskObjectList = static_cast <SVTaskObjectListClass*> ( pToolSet );
		m_Tree.BuildOutputList( pTaskObjectList, SV_DD_VALUE, SV_DD_VALUE | SV_VIEWABLE  );
	}
}

BOOL SVDataDefinitionPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitPage( m_pSheet->m_pInspection );

	m_Tree.SetCanSelectObjectCallback( SVObjectTreeCanSelectObjectCallbackFn(this, &SVDataDefinitionPage::CanSelectObjectCallback) );

	// fill the list
	CString strPrefix = m_pSheet->m_pInspection->GetName();
	strPrefix += _T(".Tool Set.");

	std::vector<SVObjectReference> vecObjects;
	std::vector<int> vecIndexes;
	m_Tree.GetSelectedObjectsInTreeOrder( vecObjects, vecIndexes );

	for ( size_t i = 0; i < vecObjects.size(); ++i )
	{
		SVObjectReference refObject = vecObjects.at(i);
		CString strName = refObject.GetCompleteOneBasedObjectName();

		if ( strName.GetLength() > 0 )
		{
			strName = strName.Mid( strPrefix.GetLength() );
		}

		int iIndex = m_lbSelected.AddString( strName );
		m_lbSelected.SetItemData( iIndex, vecIndexes.at(i) );
	}

	return TRUE;
}

bool SVDataDefinitionPage::CanSelectObjectCallback( SVObjectReference refObject, bool bCurrentState, int p_iIndex )
{
	UpdateData();

	CString strPrefix = m_pSheet->m_pInspection->GetName();
	strPrefix += _T(".Tool Set.");

	CString strName = refObject.GetCompleteOneBasedObjectName();

	if ( strName.GetLength() > 0 )
		strName = strName.Mid( strPrefix.GetLength() );

	if ( bCurrentState == false )// want to select
	{
		bool bInserted = false;
		for ( int i = 0; i < m_lbSelected.GetCount(); ++i )
		{
			if ( static_cast< int >( m_lbSelected.GetItemData( i ) ) > p_iIndex )
			{
				m_lbSelected.InsertString( i, strName );
				m_lbSelected.SetItemData( i, p_iIndex );
				bInserted = true;
				break;
			}
		}
		if ( !bInserted )
		{
			int iIndex = m_lbSelected.AddString( strName );
			m_lbSelected.SetItemData( iIndex, p_iIndex );
		}
	}
	else // want to deselect
	{
		int iIndex = m_lbSelected.FindStringExact( 0, strName );
		if ( iIndex != LB_ERR )
			m_lbSelected.DeleteString( iIndex );
	}

	return true;
}

BEGIN_MESSAGE_MAP(SVDataDefinitionPage, CDialog)
	//{{AFX_MSG_MAP(SVDataDefinitionPage)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ALL, OnBtnClearAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SVDataDefinitionPage::OnBtnClear()
{
	std::vector<int> vecIndexes;
	if ( m_lbSelected.GetCount() > 0 )
	{
		vecIndexes.resize(m_lbSelected.GetCount());
		int iItems = m_lbSelected.GetSelItems(m_lbSelected.GetCount(), &vecIndexes[0]);
		for ( int i = iItems-1; i >= 0; --i )
		{
			int iIndex = static_cast< int >( m_lbSelected.GetItemData( vecIndexes.at( i ) ) );
			m_Tree.SelectItemByIndex( iIndex, false );
		}
	}
}

void SVDataDefinitionPage::OnBtnClearAll()
{
	int iItems = m_lbSelected.GetCount();
	for ( int i = iItems-1; i >= 0; --i )
	{
		int iIndex = static_cast< int >( m_lbSelected.GetItemData( i ) );
		m_Tree.SelectItemByIndex( iIndex, false );
	}
}

void SVDataDefinitionPage::OnOK()
{
	std::set<SVObjectReference> vecSelected;
	m_Tree.GetChangedObjects( vecSelected );
	if( vecSelected.size() > 0 )
	{
		SVIPDoc* l_pIPDoc = nullptr;

		if( m_pSheet->m_pInspection != nullptr )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pSheet->m_pInspection->GetUniqueObjectID() );

			if( l_pIPDoc != nullptr )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}
	}

	if( m_bIsImagesPage )
	{
		for ( std::set<SVObjectReference>::iterator it = vecSelected.begin(); it != vecSelected.end(); ++it )
		{
			if( it->Object() )
			{
				SVObjectReference& l_Ref = const_cast<SVObjectReference&>( *it );

				if( l_Ref.ObjectAttributesSet() & SV_DD_IMAGE )
				{
					SetBits( l_Ref.ObjectAttributesSetRef(), SV_DD_IMAGE, false );
				}
				else
				{
					SetBits( l_Ref.ObjectAttributesSetRef(), SV_DD_IMAGE, true );
				}
			}
		}
	}
	else
	{
		for ( std::set<SVObjectReference>::iterator it = vecSelected.begin(); it != vecSelected.end(); ++it )
		{
			if( it->Object() )
			{
				SVObjectReference& l_Ref = const_cast<SVObjectReference&>( *it );

				if( l_Ref.ObjectAttributesSet() & SV_DD_VALUE )
				{
					SetBits( l_Ref.ObjectAttributesSetRef(), SV_DD_VALUE, false );
				}
				else
				{
					SetBits( l_Ref.ObjectAttributesSetRef(), SV_DD_VALUE, true );
				}
			}
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDataDefinitionPage.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 13:58:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:05:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2012 16:45:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2010 07:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Mar 2008 09:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Property Sheet for Data and image Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/