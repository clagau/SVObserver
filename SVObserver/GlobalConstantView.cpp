//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class is a list view for the Global constants
/// Used to display, add, edit and delete the constants
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "GlobalConstantView.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVToolSet.h"
#include "SVConfigurationObject.h"
#include "SVSVIMStateClass.h"
#include "RootObject.h"
#include "TextDefinesSvO.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVOGui/GlobalConstantDlg.h"
#include "SVOGui/SVShowDependentsDialog.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int GlobalConstantWidth = 125;
static const int TypeWidth = 50;
static const int ValueWidth = 300;
static const int DescriptionWidth = 500;

#pragma endregion Declarations

IMPLEMENT_DYNCREATE(GlobalConstantView, CListView)

BEGIN_MESSAGE_MAP(GlobalConstantView, CListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_GLOBAL_CONSTANT_ADD, &GlobalConstantView::OnAddItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GLOBAL_CONSTANT_ADD,  &GlobalConstantView::OnUpdateAddItem)
	ON_COMMAND(ID_EDIT_GLOBAL_CONSTANT_EDIT, &GlobalConstantView::OnEditItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GLOBAL_CONSTANT_EDIT,  &GlobalConstantView::OnUpdateEditItem)
	ON_COMMAND(ID_EDIT_GLOBAL_CONSTANT_DELETE, &GlobalConstantView::OnDeleteItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GLOBAL_CONSTANT_DELETE,  &GlobalConstantView::OnUpdateDeleteItem)
	ON_COMMAND(ID_GLOBAL_CONSTANT_DEPENDENCIES, &GlobalConstantView::OnShowDependencies)
	ON_UPDATE_COMMAND_UI(ID_GLOBAL_CONSTANT_DEPENDENCIES,  &GlobalConstantView::OnUpdateShowDependencies)
END_MESSAGE_MAP()


#pragma region Constructor
GlobalConstantView::GlobalConstantView() :
	m_rCtrl( GetListCtrl() )
{
	m_pDocument = dynamic_cast<SVIODoc *> ( CListView::GetDocument() );
	VERIFY(m_ContextMenuItem.LoadMenu( IDR_GLOBAL_CONSTANT_CONTEXT_MENU ));
}

GlobalConstantView::~GlobalConstantView()
{
	m_ContextMenuItem.DestroyMenu();
}
#pragma endregion Constructor


#pragma region Public Methods
void GlobalConstantView::updateView()
{
	OnUpdate(this, 0, nullptr );
}

void GlobalConstantView::OnAddItem()
{
	if( editItem( -1 ) )
	{
		SVIODoc* pIODoc = GetDocument();
		if( nullptr != pIODoc )
		{
			pIODoc->SetModifiedFlag();
		}
	}
}

void GlobalConstantView::OnUpdateAddItem( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TheSVObserverApp.OkToEdit() );
}

#pragma endregion Public Methods

#pragma region Protected Methods
BOOL GlobalConstantView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (LVS_SHOWSELALWAYS | LVS_REPORT );
	cs.style &= ~LVS_SORTASCENDING;
	cs.style &= ~LVS_SORTDESCENDING;
	cs.dwExStyle |= LVS_EX_GRIDLINES;

	return CListView::PreCreateWindow(cs);
}

void GlobalConstantView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( ::IsWindow(m_hWnd) && nullptr != pConfig )
	{
		m_rCtrl.SetRedraw( false );
		m_rCtrl.DeleteAllItems();

		m_DataList.clear();

		BasicValueObjects::ValueVector GlobalObjects;

		RootObject::getRootChildObjectList( GlobalObjects, SvOl::FqnGlobal, 0 );
		BasicValueObjects::ValueVector::const_iterator Iter( GlobalObjects.begin() );
		while( GlobalObjects.end() != Iter && !Iter->empty() )
		{
			SVString Name( (*Iter)->GetCompleteObjectName() );
			SvUl_SF::MakeUpper(Name);
			m_DataList.push_back( std::make_pair( Name,  *Iter ) );
			++Iter;
		}

		std::sort( m_DataList.begin(), m_DataList.end() );

		int Pos( 0 );
		NameBasicValuePtrVector::const_iterator IterSorted( m_DataList.cbegin() );
		while( m_DataList.cend() != IterSorted )
		{
			Pos++;
			insertItem( IterSorted->second, Pos );
			++IterSorted;
		}
		
		m_rCtrl.SetRedraw(true);
	}
}

#pragma endregion Protected Methods


#pragma region Private Methods
#ifdef _DEBUG
void GlobalConstantView::AssertValid() const
{
	CListView::AssertValid();
}

void GlobalConstantView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif

bool GlobalConstantView::editItem( int Item )
{
	bool Result( false );

	SvOi::GlobalConstantData GlobalData;
	SvOg::GlobalConstantDlg GlobalDlg( GlobalData, this );

	SVStringArray GlobalConstantList;
	RootObject::getRootChildNameList( GlobalConstantList, SvOl::FqnGlobal );
	GlobalDlg.setExistingNames( GlobalConstantList );

	if( -1 != Item )
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = Item;
		lvItem.iSubItem = 0;

		if( m_rCtrl.GetItem( &lvItem ) )
		{
			BasicValueObject* pObject(nullptr);

			pObject = reinterpret_cast<BasicValueObject*> ( lvItem.lParam );
			if( nullptr != pObject )
			{
				pObject->getValue( GlobalData.m_Value );
				GlobalData.m_Guid = pObject->GetUniqueObjectID();
				GlobalData.m_DottedName = pObject->GetCompleteObjectName();
				GlobalData.m_Description = pObject->getDescription();
			}
		}
	}

	SVSVIMStateClass::AddState( SV_STATE_EDITING );
	if( IDOK == GlobalDlg.DoModal() )
	{
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		Result = true;

		//New or editing Global value ?
		if( SV_GUID_NULL == GlobalData.m_Guid )
		{
			insertGlobalConstant( GlobalData );
			updateView();
		}
		else
		{
			editGlobalConstant( GlobalData );
			updateView();
			updateAllIPDocs( false );
		}
	}
	SVSVIMStateClass::RemoveState( SV_STATE_EDITING );

	return Result;
}

bool GlobalConstantView::deleteItem( int Item )
{
	bool Result( false );

	if( -1 != Item )
	{
		SVSVIMStateClass::AddState( SV_STATE_EDITING );
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = Item;
		lvItem.iSubItem = 0;

		if( m_rCtrl.GetItem( &lvItem ) )
		{
			BasicValueObject* pObject(nullptr);
			pObject = reinterpret_cast<BasicValueObject*> ( lvItem.lParam );

			if( nullptr != pObject )
			{
				if( checkAllDependencies( pObject, true ) )
				{
					if( S_OK ==  RootObject::deleteRootChildValue( pObject->GetCompleteObjectName() ) )
					{
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
						Result = true;
						updateView();
						updateAllIPDocs( true );
					}
				}
			}
		}
		SVSVIMStateClass::RemoveState( SV_STATE_EDITING );
	}

	return Result;
}

int GlobalConstantView::insertItem(const BasicValueObjectPtr& rpObject, int Pos )
{
	LVITEM lvItem;
	_variant_t Value;
	SVString Type;
	int InsertPos;

	SVString ValueText;
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvItem.pszText = _T("");
	lvItem.iIndent = 0;
	lvItem.state = INDEXTOSTATEIMAGEMASK( 1 );
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.iImage = 0;
	lvItem.iItem = Pos;
	lvItem.iSubItem = 0;
	lvItem.lParam =  reinterpret_cast<LPARAM> (rpObject.get());
	InsertPos =  m_rCtrl.InsertItem( &lvItem );
	m_rCtrl.SetItemText( InsertPos, GlobalConstantCol, rpObject->GetName() );

	rpObject->getValue( Value );
	switch( Value.vt )
	{
	case VT_R8:
		{
			Type = SvOg::GlobalConstantTypes[SvOi::GlobalConstantData::DecimalType];
			ValueText = SvUl_SF::Format( _T("%.06f"), Value.dblVal );
		}
		break;
	case VT_BSTR:
		{
			Type = SvOg::GlobalConstantTypes[SvOi::GlobalConstantData::TextType];
			ValueText = SvUl_SF::createSVString(Value);
		}
		break;
	default:
		{
			Type = SvO::Invalid;
			ValueText.clear();
		}
		break;
	}

	m_rCtrl.SetItemText( InsertPos, TypeCol, Type.c_str() );
	m_rCtrl.SetItemText( InsertPos, ValueCol, ValueText.c_str() );

	//Replace any new line with a space to display it in one line
	SVString Description( rpObject->getDescription() );
	SvUl_SF::searchAndReplace( Description, _T("\r\n"), _T(" ") );
	m_rCtrl.SetItemText( InsertPos, DescriptionCol, Description.c_str() );

	return InsertPos;
}

void GlobalConstantView::insertGlobalConstant( const SvOi::GlobalConstantData& rGlobalData ) const
{
	BasicValueObjectPtr pGlobalObject;

	pGlobalObject = RootObject::setRootChildValue( rGlobalData.m_DottedName.c_str(),  rGlobalData.m_Value );
	if( !pGlobalObject.empty() )
	{
		pGlobalObject->setDescription( rGlobalData.m_Description.c_str() );
		//All Global constants can be remotely settable
		pGlobalObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
		//If the Global Constant is of type text then not selectable for equations so remove the attribute
		if( VT_BSTR == rGlobalData.m_Value.vt )
		{
			pGlobalObject->ObjectAttributesAllowedRef() &= ~SV_SELECTABLE_FOR_EQUATION;
		}
	}
}

void GlobalConstantView::editGlobalConstant( const SvOi::GlobalConstantData& rGlobalData ) const
{
	BasicValueObject* pGlobalObject(nullptr);

	pGlobalObject = dynamic_cast<BasicValueObject*> ( SVObjectManagerClass::Instance().GetObject( rGlobalData.m_Guid ) );
	if( nullptr != pGlobalObject )
	{
		if( rGlobalData.m_DottedName != SVString( pGlobalObject->GetCompleteObjectName() ) )
		{
			SVObjectNameInfo ParseName;
			SVString OldName( pGlobalObject->GetName() );
			ParseName.ParseObjectName( rGlobalData.m_DottedName );
			pGlobalObject->SetName( ParseName.m_NameArray[ParseName.m_NameArray.size() - 1].c_str() );
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if( nullptr != pConfig )
			{
				SVInspectionProcessPtrList Inspections;
				pConfig->GetInspections( Inspections );
				SVInspectionProcessPtrList::iterator Iter( Inspections.begin() );
				for( ; Inspections.end() != Iter; ++Iter )
				{
					::SVSendMessage( *Iter, SVM_OBJECT_RENAMED,
						reinterpret_cast <DWORD_PTR> ( static_cast <SVObjectClass*> (pGlobalObject) ),
						reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>( OldName.c_str() )) );
				}
			}
		}
		pGlobalObject->setValue( rGlobalData.m_Value );
		pGlobalObject->setDescription( rGlobalData.m_Description.c_str() );
	}
}

SVIODoc* GlobalConstantView::GetDocument()
{
	if( nullptr == m_pDocument )
	{
		m_pDocument = dynamic_cast<SVIODoc *> (CListView::GetDocument());
	}
	return m_pDocument;
}

void GlobalConstantView::updateAllIPDocs( bool RunOnce ) const
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		SVInspectionProcessPtrList Inspections;
		pConfig->GetInspections( Inspections );
		SVInspectionProcessPtrList::const_iterator Iter( Inspections.begin() );
		while( Inspections.end() != Iter )
		{
			SVIPDoc* pDoc =  TheSVObserverApp.GetIPDoc( (*Iter)->GetUniqueObjectID() );
			if( nullptr != pDoc )
			{
				pDoc->RebuildResultsList();
				pDoc->UpdateWithLastProduct();
				if( RunOnce )
				{
					pDoc->RunOnce();
				}
			}
			++Iter;
		}
	}	
}

int GlobalConstantView::getSelectedItem() const
{
	int Result( -1 );

	POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
	if( nullptr != Pos )
	{
		Result = m_rCtrl.GetNextSelectedItem( Pos );
	}

	return Result;
}

bool GlobalConstantView::checkAllDependencies( BasicValueObject* pObject, bool ConfirmDelete ) const
{
	bool Result( true );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		CString DisplayText;
		CString Name( pObject->GetName() );
		SVObjectVector ObjectCheckList;
		SVObjectPairVector DependencyList;

		DisplayText.Format( IDS_DELETE_CHECK_DEPENDENCIES, Name, Name, Name, Name );

		ObjectCheckList.push_back( pObject );
		SVInspectionProcessPtrList Inspections;
		pConfig->GetInspections( Inspections );
		SVInspectionProcessPtrList::const_iterator Iter( Inspections.begin() );
		while( Inspections.end() != Iter && nullptr != (*Iter)->GetToolSet() )
		{
			SVObjectPairVector IP_DependencyList;
			(*Iter)->GetToolSet()->GetDependentsList( ObjectCheckList, IP_DependencyList );

			SVObjectPairVector::const_iterator PairIter( IP_DependencyList.cbegin() );
			while( IP_DependencyList.cend() != PairIter )
			{
				SVObjectPairVector::const_iterator PairIter2( DependencyList.cbegin() );
				bool NewItem( true );

				//Check if the dependency is already in the main list
				while( DependencyList.cend() != PairIter2 )
				{
					//@WARNING [gra][7.20][22.06.2015] This would be better done using std::find however we do not have the appropriate operator==
					if( PairIter->first == PairIter2->first && PairIter->second == PairIter2->second )
					{
						NewItem = false;
						break;
					}
					++PairIter2;
				}
				if( NewItem )
				{
					DependencyList.push_back( *PairIter );
				}
				++PairIter;
			}
			++Iter;
		}
		
		SvOg::SVShowDependentsDialog::DialogType Type( SvOg::SVShowDependentsDialog::ShowWithIP_Name );
		if( ConfirmDelete )
		{
			Type =  SvOg::SVShowDependentsDialog::DeleteConfirmWithIP_Name;
		}

		SvOg::SVShowDependentsDialog DependentsDialog( DependencyList, DisplayText, Type );

		if( IDCANCEL == DependentsDialog.DoModal() )
		{
			Result = false;
		}
	}	

	return Result;
}

BOOL GlobalConstantView::PreTranslateMessage(MSG* pMsg)
{
	BOOL bRet = false;
	if( WM_KEYDOWN == pMsg->message && TheSVObserverApp.OkToEdit() )
	{
		POSITION Pos = m_rCtrl.GetFirstSelectedItemPosition();
	
		if( nullptr != Pos )
		{
			int item = m_rCtrl.GetNextSelectedItem( Pos );
			if( item != -1 )
			{
				if( VK_DELETE == pMsg->wParam )
				{
					OnDeleteItem();
					bRet = true;
				}
				else if( VK_INSERT == pMsg->wParam )
				{
					OnAddItem();
					bRet = true;
				}
				else if( VK_RETURN == pMsg->wParam )
				{
					OnEditItem();
					bRet = true;
				}
			}
		}
		else
		{
			if( VK_INSERT == pMsg->wParam )
			{
				OnAddItem();
				bRet = true;
			}
		}
	}
	if (!bRet)
	{
		bRet = CListView::PreTranslateMessage(pMsg);
	}
	return bRet;
}

int GlobalConstantView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL;
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_rCtrl.SetExtendedStyle( m_rCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	CWinApp* pApp = AfxGetApp();
	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, true, 2, 2 );
	m_ImageList.Add( pApp->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( pApp->LoadIcon( IDI_NOIOITEM_ICON ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, true, 1, 1 );
	m_StateImageList.Add( pApp->LoadIcon( IDI_GLOBAL_CONSTANT )) ;

	m_rCtrl.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_rCtrl.SetImageList( &m_ImageList, LVSIL_NORMAL );
	m_rCtrl.SetImageList( &m_ImageList, LVSIL_SMALL );

	// insert columns
	m_rCtrl.InsertColumn( GlobalConstantCol, SvO::GlobalViewHeader[GlobalConstantCol], LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( TypeCol, SvO::GlobalViewHeader[TypeCol], LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( ValueCol, SvO::GlobalViewHeader[ValueCol], LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( DescriptionCol, SvO::GlobalViewHeader[DescriptionCol], LVCFMT_LEFT, -1, -1 );

	m_rCtrl.SetColumnWidth( GlobalConstantCol, GlobalConstantWidth );
	m_rCtrl.SetColumnWidth( TypeCol, TypeWidth );
	m_rCtrl.SetColumnWidth( ValueCol, ValueWidth );
	m_rCtrl.SetColumnWidth( DescriptionCol, DescriptionWidth );

	return 0;
}

void GlobalConstantView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UINT flags;
	int item = m_rCtrl.HitTest(point, &flags);

	if ( 0 <= item && (LVHT_ONITEMLABEL == (flags & LVHT_ONITEMLABEL) ))
	{
		if (TheSVObserverApp.OkToEdit())
		{
			if( editItem(item) )
			{
				SVIODoc* pIODoc = GetDocument();
				if (pIODoc)
				{
					pIODoc->SetModifiedFlag();
				}
			}
			m_rCtrl.SetItemState( item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void GlobalConstantView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (TheSVObserverApp.OkToEdit())
	{
		CMenu* PupupMenu = m_ContextMenuItem.GetSubMenu( 0 );
		ASSERT( nullptr != PupupMenu );
		CWnd* PopupOwner = this;
		while ( PopupOwner->GetStyle() & WS_CHILD )
		{
			PopupOwner = PopupOwner->GetParent();
		}
		PupupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, PopupOwner );
	}
}

void GlobalConstantView::OnEditItem()
{
	int Item = getSelectedItem();
	if( -1 != Item )
	{
		if( editItem( Item ) )
		{
			SVIODoc* pIODoc = GetDocument();
			if( nullptr != pIODoc )
			{
				pIODoc->SetModifiedFlag();
			}
		}
	}
}

void GlobalConstantView::OnUpdateEditItem( CCmdUI* pCmdUI )
{
	bool Enabled ( TheSVObserverApp.OkToEdit() );

	if( Enabled && -1 == getSelectedItem() )
	{
		Enabled = false;
	}
	pCmdUI->Enable( Enabled );
}

void GlobalConstantView::OnDeleteItem()
{
	int Item = getSelectedItem();
	if( -1 != Item )
	{
		if ( deleteItem( Item ) )
		{
			SVIODoc* pIODoc = GetDocument();
			if( nullptr != pIODoc )
			{
				pIODoc->SetModifiedFlag();
			}
		}
	}
}

void GlobalConstantView::OnUpdateDeleteItem( CCmdUI* pCmdUI )
{
	bool Enabled ( TheSVObserverApp.OkToEdit() );

	if( Enabled && -1 == getSelectedItem() )
	{
		Enabled = false;
	}
	pCmdUI->Enable( Enabled );
}

void GlobalConstantView::OnShowDependencies()
{
	int Item = getSelectedItem();
	if( -1 != Item )
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = Item;
		lvItem.iSubItem = 0;

		if( m_rCtrl.GetItem( &lvItem ) )
		{
			BasicValueObject* pObject(nullptr);

			pObject = reinterpret_cast<BasicValueObject*> ( lvItem.lParam );
			if( nullptr != pObject )
			{
				checkAllDependencies( pObject, false );
			}
		}
	}
}

void GlobalConstantView::OnUpdateShowDependencies( CCmdUI* pCmdUI )
{
	bool Enabled ( true );

	if( -1 == getSelectedItem() )
	{
		Enabled = false;
	}
	pCmdUI->Enable( Enabled );
}
#pragma endregion Private Methods

