//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TaTableAnalyzerPage.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to control the analyzer for the table.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TaTableAnalyzerPage.h"
#include "ObjectInterfaces/ObjectDefines.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "TextDefinesSvOg.h"
#include "GuiCommands/GetCreatableObjects.h"
#include "GuiCommands/GetAvailableObjects.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "GuiCommands/GetObjectTypeInfo.h"
#include "GuiCommands/ConstructAndInsertTaskObject.h"
#include "GuiCommands/DestroyChildObject.h"
#include "GuiCommands/ConnectToObject.h"
#include "GuiCommands/GetInputs.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVClsids.h"
#include "ToolSetItemSelector.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui {

	static LPCTSTR NoColumnTag = _T("(No Column Available)");
	static const std::string IsAscTag(_T("IsAsc"));
	static const std::string ExcludeHighTag(_T("ExcludeHigh"));
	static const std::string ExcludeHighLinkTag(_T("ExcludeHigh_Link"));
	static const std::string ExcludeLowTag(_T("ExcludeLow"));
	static const std::string ExcludeLowLinkTag(_T("ExcludeLow_Link"));
	static const std::string LimitValueTag(_T("LimitValue"));
	static const std::string LimitValueLinkTag(_T("LimitValue_Link"));

	BEGIN_MESSAGE_MAP(TaTableAnalyzerPage, CPropertyPage)
		//{{AFX_MSG_MAP(TaTableAnalyzerPage)
		ON_BN_CLICKED(IDC_BUTTON3, OnButtonClearAll)
		ON_BN_CLICKED(IDC_BUTTON2, OnButtonDeleteCurrentAnalyzer)
		ON_BN_CLICKED(IDC_BUTTON1, OnButtonInsertNewAnalyzer)
		ON_LBN_SELCHANGE(IDC_ANALYZER_LIST, OnSelchangeAnalyzerList)
		ON_CBN_SELCHANGE(IDC_COLUMN_SELECT_CBOX, OnChangeColumnSelection)
		ON_BN_CLICKED(IDC_BUTTON_EXCLUDE_HIGH, OnButtonClickExcludeHigh)
		ON_BN_CLICKED(IDC_BUTTON_EXCLUDE_LOW, OnButtonClickExcludeLow)
		ON_BN_CLICKED(IDC_BUTTON_LIMIT_VALUE, OnButtonClickLimitValue)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TaTableAnalyzerPage::TaTableAnalyzerPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
		: CPropertyPage(TaTableAnalyzerPage::IDD)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
		, m_selectedAnalyzerID(SV_GUID_NULL)
		, m_SortDirection(-1)
		, m_selectedSubType(SVNotSetSubObjectType)
		, m_inputName(_T(""))
		, m_objectSelector(rInspectionID)
	{
	}

	TaTableAnalyzerPage::~TaTableAnalyzerPage()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TaTableAnalyzerPage::QueryAllowExit()
	{
		HRESULT hResult = SetInspectionData();
		if (S_OK == hResult)
		{
			hResult = checkAllAnaylzer();
		}
		return (S_OK == hResult);
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TaTableAnalyzerPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TaTableAnalyzerPage)
		DDX_Control(pDX, IDC_ANALYZER_LIST, m_analyzerListBox);
		DDX_Control(pDX, IDC_AVAILABLE_ANALYZER_COMBO, m_availableAnaylzerCB);
		DDX_Control(pDX, IDC_COLUMN_SELECT_CBOX, m_columnSelectionCB);
		DDX_Control(pDX, IDC_EDIT_EXCLUDE_HIGH, m_EditExcludeHigh);
		DDX_Control(pDX, IDC_EDIT_EXCLUDE_LOW, m_EditExcludeLow);
		DDX_Control(pDX, IDC_EDIT_LIMIT_VALUE, m_EditLimitValue);
		DDX_Control(pDX, IDC_BUTTON_EXCLUDE_HIGH, m_ButtonExcludeHigh);
		DDX_Control(pDX, IDC_BUTTON_EXCLUDE_LOW, m_ButtonExcludeLow);
		DDX_Control(pDX, IDC_BUTTON_LIMIT_VALUE, m_ButtonLimitValue);
		DDX_Radio(pDX, IDC_ASC_RADIO, m_SortDirection);
		//}}AFX_DATA_MAP
	}

	BOOL TaTableAnalyzerPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		typedef GuiCmd::GetCreatableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(TableAnalyzerType, SVNotSetSubObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
		}

		m_availableAnaylzerCB.Init(availableList, _T(""), Analyzer_NoAnalyzerAvailable );

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

		m_ButtonExcludeHigh.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );
		m_ButtonExcludeLow.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );
		m_ButtonLimitValue.SetBitmap( static_cast<HBITMAP> (m_downArrowBitmap) );

		RetrieveAvailableColumnList();
		refresh();
		SetPropertyControls();

		UpdateData( FALSE );

		return TRUE;
	}

	void TaTableAnalyzerPage::OnButtonClearAll() 
	{
		// For all Items in the Selected (Instantiated) Analyzer list
		typedef GuiCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList availableList;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(TableAnalyzerType, SVNotSetSubObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
		}

		// remove all Analyzer items (instantiated)
		size_t listSize = availableList.size();
		for (int i=0; i<listSize; ++i)
		{
			SVGUID analyzerGUID = availableList[i].second;
			if( SV_GUID_NULL != analyzerGUID )
			{
				GuiCmd::DestroyChildObject::FlagEnum flag = GuiCmd::DestroyChildObject::Flag_None;
				// Close, Disconnect and Delete it
				typedef GuiCmd::DestroyChildObject DestroyCommand;
				typedef SVSharedPtr<DestroyCommand> DestroyCommandPtr;
				DestroyCommandPtr destroyCommandPtr = new DestroyCommand(m_TaskObjectID, analyzerGUID, flag);
				SVObjectSynchronousCommandTemplate<DestroyCommandPtr> destroyCmd(m_InspectionID, destroyCommandPtr);
				destroyCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			}
		}

		m_analyzerListBox.SetCurSel( -1 );

		// Refresh Dialog...
		refresh();
	}

	void TaTableAnalyzerPage::OnButtonDeleteCurrentAnalyzer() 
	{
		if( SV_GUID_NULL != m_selectedAnalyzerID ) 
		{
			// Close, Disconnect and Delete it
			typedef GuiCmd::DestroyChildObject DestroyCommand;
			typedef SVSharedPtr<DestroyCommand> DestroyCommandPtr;
			DestroyCommandPtr destroyCommandPtr = new DestroyCommand(m_TaskObjectID, m_selectedAnalyzerID);
			SVObjectSynchronousCommandTemplate<DestroyCommandPtr> destroyCmd(m_InspectionID, destroyCommandPtr);
			destroyCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			m_selectedAnalyzerID = SV_GUID_NULL;
			m_selectedSubType = SVNotSetSubObjectType;

			// Refresh Dialog...
			refresh();
		}
	}

	void TaTableAnalyzerPage::OnButtonInsertNewAnalyzer() 
	{ 
		if (S_OK != prepareSwitchOfAnalyzerSelection())
		{
			return;
		}

		const SVGUID classID = m_availableAnaylzerCB.getSelectedGUID();

		if( SV_GUID_NULL != classID )
		{
			int destinyIndex = m_analyzerListBox.GetCurSel();

			if( LB_ERR == destinyIndex  ) // First Entryitem
			{
				destinyIndex = 0;
			}

			// Construct and Create the Analyzer Class Object
			typedef GuiCmd::ConstructAndInsertTaskObject Command;
			typedef SVSharedPtr<Command> CommandPtr;
			CommandPtr commandPtr = new Command(m_TaskObjectID, classID, destinyIndex);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK != hr)
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_CreationAnalyzerFailed, SvStl::SourceFileParams(StdMessageParams) );
			}

			// Refresh Dialog...
			refresh();
		}
	}

	void TaTableAnalyzerPage::OnSelchangeAnalyzerList() 
	{
		if (S_OK != prepareSwitchOfAnalyzerSelection())
		{
			return;
		}

		int index = m_analyzerListBox.GetCurSel();
		m_selectedAnalyzerID = m_analyzerListBox.getGUID(index);

		SetPropertyControls();
	}// end OnSelchangeAnalyzerList


	void TaTableAnalyzerPage::OnChangeColumnSelection()
	{
		UpdateData(TRUE); // get data from dialog

		SVGUID columnGuid = m_columnSelectionCB.getSelectedGUID();
		if (SV_GUID_NULL != columnGuid && SV_GUID_NULL != m_selectedAnalyzerID)
		{
			HRESULT hr = E_INVALIDARG;
			typedef GuiCmd::ConnectToObject Command;
			typedef SVSharedPtr<Command> CommandPtr;
			CommandPtr commandPtr = new Command(m_selectedAnalyzerID, m_inputName, columnGuid, DoubleSortValueObjectType);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if ( S_OK != hr )
			{
				SVStringVector msgList;
				msgList.push_back(m_inputName);
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ConnectFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
	}

	void TaTableAnalyzerPage::OnButtonClickExcludeHigh()
	{
		CString Temp;
		m_EditExcludeHigh.GetWindowText( Temp );
		SVString Value = Temp;
		SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE );
		if (m_objectSelector.Show<ToolSetItemSelector<>>( Value, Title, this))
		{
			m_EditExcludeHigh.SetWindowText( Value.c_str() );
		}
		UpdateData( false );
	}

	void TaTableAnalyzerPage::OnButtonClickExcludeLow()
	{
		CString Temp;
		m_EditExcludeLow.GetWindowText( Temp );
		SVString Value = Temp;
		SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE );
		if (m_objectSelector.Show<ToolSetItemSelector<>>( Value, Title, this ))
		{
			m_EditExcludeLow.SetWindowText( Value.c_str() );
		}
		UpdateData( false );
	}

	void TaTableAnalyzerPage::OnButtonClickLimitValue()
	{
		CString Temp;
		m_EditLimitValue.GetWindowText( Temp );
		SVString Value = Temp;
		SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE );
		if (m_objectSelector.Show<ToolSetItemSelector<>>( Value, Title, this ))
		{
			m_EditLimitValue.SetWindowText( Value.c_str() );
		}
		UpdateData( false );
	}

	BOOL TaTableAnalyzerPage::OnKillActive()
	{
		HRESULT hResult = SetInspectionData();
		if (S_OK == hResult)
		{
			hResult = checkAllAnaylzer();
		}

		if (S_OK == hResult)
		{
			return CPropertyPage::OnKillActive();
		}
		else
		{
			return FALSE;
		}
	}

	HRESULT TaTableAnalyzerPage::SetInspectionData()
	{
		UpdateData( TRUE ); // get data from dialog
		SvStl::MessageContainerVector errorMessageList;
		HRESULT hrOk = S_OK;

		if (!m_Values.empty())
		{
			switch (m_selectedSubType)
			{
			case TableAnalyzerSortType:
				m_Values->Set<bool>(IsAscTag, 0 == m_SortDirection);
				hrOk = m_Values->Commit();
				errorMessageList = m_Values->getCommitErrorList();
				break;
			case TableAnalyzerExcludeType:
				{
					CString Value;
					m_EditExcludeHigh.GetWindowText( Value );
					m_Values->Set<CString>(ExcludeHighTag, Value);
					m_EditExcludeLow.GetWindowText( Value );
					m_Values->Set<CString>(ExcludeLowTag, Value);
					hrOk = m_Values->Commit(true);
					errorMessageList = m_Values->getCommitErrorList();
				}
				break;
			case TableAnalyzerLimitType:
				{
					CString Value;
					m_EditLimitValue.GetWindowText( Value );
					m_Values->Set<CString>(LimitValueTag, Value);
					hrOk = m_Values->Commit(true);
					errorMessageList = m_Values->getCommitErrorList();
				}
				break;
			default:
				break;
			}
		}

		UpdateData( FALSE );

		if (S_OK != hrOk)
		{
			if (0 < errorMessageList.size())
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( errorMessageList[0].getMessage() );
			}
			else
			{
				//display an error if set failed.
				SVStringVector msgList;
				msgList.push_back(SvUl_SF::Format(_T("%d"), hrOk));
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_SetTableAnalyzerData, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}

		return hrOk;
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	void TaTableAnalyzerPage::refresh()
	{
		typedef GuiCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		SvUl::NameGuidList availableList;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(TableAnalyzerType, SVNotSetSubObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			availableList = commandPtr->AvailableObjects();
		}

		// Populate Analyzer list box and run Analyzer...
		m_analyzerListBox.init( availableList, Analyzer_NoAnalyzer );
		int index = m_analyzerListBox.GetCurSel();
		m_selectedAnalyzerID = m_analyzerListBox.getGUID(index);

		SetPropertyControls();
	}

	void TaTableAnalyzerPage::SetPropertyControls( )
	{
		m_selectedSubType = SVNotSetSubObjectType;
		if( SV_GUID_NULL != m_selectedAnalyzerID ) 
		{
			typedef GuiCmd::GetObjectTypeInfo Command;
			typedef SVSharedPtr<Command> CommandPtr;
			CommandPtr commandPtr = new Command(m_selectedAnalyzerID);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				SVObjectTypeInfoStruct info = commandPtr->GetTypeInfo();
				m_selectedSubType = info.SubType;
			}
		}// end if

		ShowControls(m_selectedSubType);
		switch (m_selectedSubType)
		{
		case TableAnalyzerSortType:
			setSortProperties();
			break;
		case TableAnalyzerExcludeType:
			setExcludeProperties();
			break;
		case TableAnalyzerLimitType:
			setLimitProperties();
			break;
		default:
			break;
		}
	}

	HRESULT TaTableAnalyzerPage::RetrieveAvailableColumnList()
	{ 
		typedef GuiCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(DoubleSortValueObjectType, SVNotSetSubObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_availableColumn = commandPtr->AvailableObjects();
		}
		return hr;
	}

	void TaTableAnalyzerPage::ShowControls( long SubType )
	{
		GetDlgItem( IDC_COLUMN_SELECT_CBOX )->ShowWindow( (TableAnalyzerSortType==SubType || TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );

		GetDlgItem( IDC_SORT_LABEL )->ShowWindow( (TableAnalyzerSortType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_ASC_RADIO )->ShowWindow ( (TableAnalyzerSortType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_DESC_RADIO )->ShowWindow( (TableAnalyzerSortType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_STATIC_ASC )->ShowWindow( (TableAnalyzerSortType==SubType) ? SW_SHOW : SW_HIDE);

		GetDlgItem( IDC_EXCLUDE_LABEL )->ShowWindow(( TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_EXCLUDE_LABEL2 )->ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_EXCLUDE_LABEL3 )->ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		m_EditExcludeHigh.ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		m_EditExcludeLow.ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_BUTTON_EXCLUDE_HIGH )->ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_BUTTON_EXCLUDE_LOW )->ShowWindow( (TableAnalyzerExcludeType==SubType) ? SW_SHOW : SW_HIDE );

		GetDlgItem( IDC_LIMIT_LABEL )->ShowWindow( (TableAnalyzerLimitType==SubType) ? SW_SHOW : SW_HIDE );
		m_EditLimitValue.ShowWindow( (TableAnalyzerLimitType==SubType) ? SW_SHOW : SW_HIDE );
		GetDlgItem( IDC_BUTTON_LIMIT_VALUE )->ShowWindow( (TableAnalyzerLimitType==SubType) ? SW_SHOW : SW_HIDE );

		RedrawWindow();
	}

	void TaTableAnalyzerPage::setSortProperties()
	{
		setColumnSelectionCB();

		if (m_Values.empty() || m_selectedAnalyzerID != m_Values->GetOwnerID())
		{
			m_Values.clear();
			m_Values = new Controller(SvOg::BoundValues(m_InspectionID, m_selectedAnalyzerID, boost::assign::map_list_of(IsAscTag, TableAnaylzerSortIsASCGuid)));
			m_Values->Init();
		}
		
		m_SortDirection = m_Values->Get<bool>(IsAscTag)?0:1;
		UpdateData( FALSE );
	}

	void TaTableAnalyzerPage::setExcludeProperties()
	{
		setColumnSelectionCB();

		if (m_Values.empty() || m_selectedAnalyzerID != m_Values->GetOwnerID())
		{
			m_Values.clear();
			m_Values = new Controller(SvOg::BoundValues(m_InspectionID, m_selectedAnalyzerID, boost::assign::map_list_of
				(ExcludeHighTag, TableAnaylzerExcludeHighGuid)
				(ExcludeHighLinkTag, TableAnaylzerExcludeHigh_LinkGuid)
				(ExcludeLowTag, TableAnaylzerExcludeLowGuid)
				(ExcludeLowLinkTag, TableAnaylzerExcludeLow_LinkGuid)
				));
			m_Values->Init();
		}

		SVString highString = m_Values->Get<CString>(ExcludeHighLinkTag);
		if( highString.empty() )
		{
			highString = m_Values->Get<CString>(ExcludeHighTag);
		}
		m_EditExcludeHigh.SetWindowText(highString.c_str());
		SVString lowString = m_Values->Get<CString>(ExcludeLowLinkTag);
		if( lowString.empty() )
		{
			lowString = m_Values->Get<CString>(ExcludeLowTag);
		}
		m_EditExcludeLow.SetWindowText(lowString.c_str());
	}

	void TaTableAnalyzerPage::setLimitProperties()
	{
		if (m_Values.empty() || m_selectedAnalyzerID != m_Values->GetOwnerID())
		{
			m_Values.clear();
			m_Values = new Controller(SvOg::BoundValues(m_InspectionID, m_selectedAnalyzerID, boost::assign::map_list_of
				(LimitValueTag, TableAnaylzerLimitValueGuid)
				(LimitValueLinkTag, TableAnaylzerLimitValue_LinkGuid)
				));
			m_Values->Init();
		}

		SVString valueString = m_Values->Get<CString>(LimitValueLinkTag);
		if( valueString.empty() )
		{
			valueString = m_Values->Get<CString>(LimitValueTag);
		}
		m_EditLimitValue.SetWindowText(valueString.c_str());
	}

	void TaTableAnalyzerPage::setColumnSelectionCB()
	{
		SVString selectedTableName;
		typedef GuiCmd::GetInputs Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_selectedAnalyzerID, SVObjectTypeInfoStruct(DoubleSortValueObjectType, SVNotSetSubObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			const SvUl::InputNameGuidPairList& rConnectedList = commandPtr->ConnectedObjects();
			if (rConnectedList.size())
			{
				m_inputName = rConnectedList.begin()->first;
				selectedTableName = rConnectedList.begin()->second.first;
			}
		}

		m_columnSelectionCB.Init(m_availableColumn, selectedTableName, NoColumnTag);
	}

	HRESULT TaTableAnalyzerPage::prepareSwitchOfAnalyzerSelection()
	{
		//Set Values before change selection
		HRESULT Result = SetInspectionData(); 
		if (S_OK != Result)
		{ 
			int index = LB_ERR;
			for (int i=0; i< m_analyzerListBox.GetCount(); i++)
			{
				if (m_analyzerListBox.getGUID(i) == m_selectedAnalyzerID)
				{
					index = i;
					break;
				}
			}
			m_analyzerListBox.SetCurSel(index);
		}
		return Result;
	}

	HRESULT TaTableAnalyzerPage::checkAllAnaylzer()
	{
		HRESULT hrOk = S_OK;
		//reset all analyzer to check if they correct
		for (int i=0; S_OK == hrOk && i< m_analyzerListBox.GetCount(); i++)
		{
			SVGUID analyzerGUID = m_analyzerListBox.getGUID(i);
			if ( SV_GUID_NULL != analyzerGUID && analyzerGUID != m_selectedAnalyzerID)
			{
				// Do a reset of the analyzer
				typedef SVSharedPtr<GuiCmd::ResetObject> ResetObjectCommandPtr;
				ResetObjectCommandPtr commandPtr(new GuiCmd::ResetObject(analyzerGUID, true));
				SVObjectSynchronousCommandTemplate<ResetObjectCommandPtr> cmd(m_InspectionID, commandPtr);

				hrOk = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK != hrOk)
				{
					SvStl::MessageContainerVector errorMessageList = commandPtr->getErrorMessages();
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					if (0 < errorMessageList.size())
					{
						Msg.setMessage( errorMessageList[0].getMessage() );
					}
					else
					{
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams) );
					}

					m_analyzerListBox.SetCurSel(i);
					m_selectedAnalyzerID = analyzerGUID;
					SetPropertyControls();
				}
			}
		}
		return hrOk;
	}
#pragma endregion Private Methods
}}  //end namespaces

