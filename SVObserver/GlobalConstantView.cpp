//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This class is a list view for the Global constants
/// Used to display, add, edit and delete the constants
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "GlobalConstantView.h"
#include "RootObject.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "TextDefinesSvO.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/GlobalConstantData.h"
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOGui/TextDefinesSvOg.h"
#include "SVOGui/GlobalConstantDlg.h"
#include "SVOGui/SVShowDependentsDialog.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/EditLock.h"
#include "SVStatusLibrary/SvimState.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int GlobalConstantWidth = 125;
constexpr int TypeWidth = 50;
constexpr int ValueWidth = 300;
constexpr int DescriptionWidth = 500;

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
	m_pDocument = dynamic_cast<SVIODoc*> (CListView::GetDocument());
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
		if(nullptr != m_pDocument)
		{
			m_pDocument->SetModifiedFlag();
		}
	}
}

void GlobalConstantView::OnUpdateAddItem( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( TheSVObserverApp().OkToEdit() );
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

void GlobalConstantView::OnUpdate( CView* , LPARAM , CObject*  )
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if ( ::IsWindow(m_hWnd) && nullptr != pConfig )
	{
		m_rCtrl.SetRedraw( false );
		m_rCtrl.DeleteAllItems();

		m_DataList.clear();

		SvVol::BasicValueObjects::ValueVector GlobalObjects;

		RootObject::getRootChildObjectList( GlobalObjects, SvDef::FqnGlobal, 0 );
		SvVol::BasicValueObjects::ValueVector::const_iterator Iter( GlobalObjects.begin() );
		while(GlobalObjects.end() != Iter && nullptr != *Iter)
		{
			std::string Name = SvUl::MakeUpper((*Iter)->GetCompleteName().c_str());
			m_DataList.push_back( std::make_pair( Name,  *Iter ) );
			++Iter;
		}

		std::sort( m_DataList.begin(), m_DataList.end() );

		int Pos( 0 );
		SvVol::NameBasicValuePtrVector::const_iterator IterSorted( m_DataList.cbegin() );
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
bool GlobalConstantView::editItem( int Item )
{
	bool Result( false );

	SvUl::GlobalConstantData GlobalData;
	SvOg::GlobalConstantDlg GlobalDlg( GlobalData, this );

	SvDef::StringVector GlobalConstantList;
	RootObject::getRootChildNameList( GlobalConstantList, SvDef::FqnGlobal );
	GlobalDlg.setExistingNames( GlobalConstantList );

	if( -1 != Item )
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = Item;
		lvItem.iSubItem = 0;

		if( m_rCtrl.GetItem( &lvItem ) )
		{
			SvVol::BasicValueObject* pObject(nullptr);

			pObject = reinterpret_cast<SvVol::BasicValueObject*> ( lvItem.lParam );
			if( nullptr != pObject )
			{
				pObject->getValue( GlobalData.m_Value );
				GlobalData.m_objectId = pObject->getObjectId();
				GlobalData.m_DottedName = pObject->GetCompleteName();
				GlobalData.m_Description = pObject->getDescription();
			}
		}
	}

	SvimState::SetResetState srs(SV_STATE_EDITING, SvStl::EditLock::acquire, SvStl::EditLock::release);
	if (false == srs.conditionOk())
	{
		return false;
	}
	if (IDOK == GlobalDlg.DoModal())
	{
		SvimState::AddState(SV_STATE_MODIFIED);
		Result = true;

		//New or editing Global value ?
		if (SvDef::InvalidObjectId == GlobalData.m_objectId)
		{
			insertGlobalConstant(GlobalData);
			updateView();
		}
		else
		{
			editGlobalConstant(GlobalData);
			updateView();
			updateAllIPDocs(false);
		}
	}
	return Result;
}

bool GlobalConstantView::deleteItem( int Item )
{
	bool Result( false );

	if( -1 != Item )
	{
		SvimState::SetResetState srs(SV_STATE_EDITING, SvStl::EditLock::acquire, SvStl::EditLock::release);
		if (false == srs.conditionOk())
		{
			return false;
		}
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = Item;
		lvItem.iSubItem = 0;

		if (m_rCtrl.GetItem(&lvItem))
		{
			SvVol::BasicValueObject* pObject(nullptr);
			pObject = reinterpret_cast<SvVol::BasicValueObject*> (lvItem.lParam);

			if (nullptr != pObject)
			{
				if (checkAllDependencies(pObject, true))
				{
					if (S_OK == RootObject::deleteRootChildValue(pObject->GetCompleteName().c_str()))
					{
						SvimState::AddState(SV_STATE_MODIFIED);
						Result = true;
						updateView();
						updateAllIPDocs(true);
					}
				}
			}
		}
	}

	return Result;
}

int GlobalConstantView::insertItem(const SvVol::BasicValueObjectPtr& rpObject, int Pos )
{
	LVITEM lvItem;
	_variant_t Value;
	std::string Type;
	int InsertPos;

	std::string ValueText;
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvItem.pszText = nullptr;
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
			Type = SvOg::GlobalConstantTypes[SvUl::GlobalConstantData::DecimalType];
			ValueText = std::format( _T("{:.6f}"), Value.dblVal );
		}
		break;
	case VT_BSTR:
		{
			Type = SvOg::GlobalConstantTypes[SvUl::GlobalConstantData::TextType];
			ValueText = SvUl::createStdString( Value );
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
	std::string Description( rpObject->getDescription() );
	SvUl::searchAndReplace( Description, _T("\r\n"), _T(" ") );
	m_rCtrl.SetItemText( InsertPos, DescriptionCol, Description.c_str() );

	return InsertPos;
}

void GlobalConstantView::insertGlobalConstant( const SvUl::GlobalConstantData& rGlobalData ) const
{
	SvVol::BasicValueObjectPtr pGlobalObject = RootObject::setRootChildValue( rGlobalData.m_DottedName.c_str(),  rGlobalData.m_Value );

	if(nullptr != pGlobalObject)
	{
		pGlobalObject->setDescription( rGlobalData.m_Description.c_str() );
		//All Global constants can be remotely settable
		pGlobalObject->SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	}
}

void GlobalConstantView::editGlobalConstant( const SvUl::GlobalConstantData& rGlobalData ) const
{
	SvVol::BasicValueObject* pGlobalObject = dynamic_cast<SvVol::BasicValueObject*> ( SVObjectManagerClass::Instance().GetObject( rGlobalData.m_objectId ) );

	if( nullptr != pGlobalObject )
	{
		if( rGlobalData.m_DottedName != std::string( pGlobalObject->GetCompleteName() ) )
		{
			SVObjectNameInfo ParseName;
			std::string OldName( pGlobalObject->GetName() );
			ParseName.ParseObjectName( rGlobalData.m_DottedName );
			pGlobalObject->SetName( ParseName.m_NameArray[ParseName.m_NameArray.size() - 1].c_str() );
			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
			if( nullptr != pConfig )
			{
				const SVInspectionProcessVector& rInspections = pConfig->GetInspections();
				for (auto pInspection : rInspections)
				{
					if (nullptr != pInspection && nullptr != pGlobalObject)
					{
						pInspection->OnObjectRenamed(*pGlobalObject, OldName);
					}
				}
			}
		}
		pGlobalObject->setValue( rGlobalData.m_Value );
		pGlobalObject->setDescription( rGlobalData.m_Description.c_str() );
	}
}

void GlobalConstantView::updateAllIPDocs( bool RunOnce ) const
{
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		const SVInspectionProcessVector& rInspections = pConfig->GetInspections();
		for (auto pInspection : rInspections)
		{
			SVIPDoc* pDoc = GetIPDocByInspectionID( pInspection->getObjectId() );
			if( nullptr != pDoc )
			{
				if (RunOnce)
				{
					pInspection->resetAllObjects();
				}
				pDoc->RebuildResultsList();
				pDoc->UpdateWithLastProduct();
				if( RunOnce )
				{
					pDoc->RunOnce();
				}
			}
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

bool GlobalConstantView::checkAllDependencies(SvVol::BasicValueObject* pObject, bool ConfirmDelete) const
{
	bool Result( true );

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		std::string DisplayText = SvUl::LoadStdString(IDS_DELETE_CHECK_DEPENDENCIES);
		std::string Name( pObject->GetName() );
		std::set<uint32_t> ObjectCheckList;

		try
		{
			DisplayText = std::vformat(DisplayText, std::make_format_args(Name, Name, Name, Name));
		}
		catch (...)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CouldNotExecuteFormatString, {DisplayText}, SvStl::SourceFileParams(StdMessageParams));
		}
		ObjectCheckList.insert( pObject->getObjectId() );
		
		SvOg::SVShowDependentsDialog::DialogType Type( SvOg::SVShowDependentsDialog::Show );
		if( ConfirmDelete )
		{
			Type =  SvOg::SVShowDependentsDialog::DeleteConfirm;
		}

		SvOg::SVShowDependentsDialog DependentsDialog( ObjectCheckList, SvPb::SVInspectionObjectType, DisplayText.c_str(), Type );

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
	if( WM_KEYDOWN == pMsg->message && TheSVObserverApp().OkToEdit() )
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
	{
		return -1;
	}

	m_rCtrl.SetExtendedStyle( m_rCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	CWinApp* pApp = AfxGetApp();
	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_ImageList.Add( pApp->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( pApp->LoadIcon( IDI_NOIOITEM_ICON ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 1, 1 );
	m_StateImageList.Add( pApp->LoadIcon( IDI_GLOBAL_CONSTANT )) ;

	m_rCtrl.SetImageList( &m_StateImageList, LVSIL_STATE );
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

void GlobalConstantView::OnLButtonDblClk(UINT, CPoint point)
{
	UINT flags;
	int item = m_rCtrl.HitTest(point, &flags);

	if ( 0 <= item && (LVHT_ONITEMLABEL == (flags & LVHT_ONITEMLABEL) ))
	{
		if (TheSVObserverApp().OkToEdit())
		{
			if( editItem(item) )
			{
				if (nullptr != m_pDocument)
				{
					m_pDocument->SetModifiedFlag();
				}
			}
			m_rCtrl.SetItemState( item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void GlobalConstantView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (TheSVObserverApp().OkToEdit())
	{
		CMenu* PupupMenu = m_ContextMenuItem.GetSubMenu( 0 );
		Log_Assert( nullptr != PupupMenu );
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
			if(nullptr != m_pDocument)
			{
				m_pDocument->SetModifiedFlag();
			}
		}
	}
}

void GlobalConstantView::OnUpdateEditItem( CCmdUI* pCmdUI )
{
	bool Enabled ( TheSVObserverApp().OkToEdit() );

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
			if(nullptr != m_pDocument)
			{
				m_pDocument->SetModifiedFlag();
			}
		}
	}
}

void GlobalConstantView::OnUpdateDeleteItem( CCmdUI* pCmdUI )
{
	bool Enabled ( TheSVObserverApp().OkToEdit() );

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
			SvVol::BasicValueObject* pObject(nullptr);

			pObject = reinterpret_cast<SvVol::BasicValueObject*> ( lvItem.lParam );
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

