//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TaTableSourcePage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to select a table as source.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <afxctl.h>
#include "TATableSourcePage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "GuiCommands\GetAvailableObjects.h"
#include "GuiCommands\ConnectToObject.h"
#include "GuiCommands\GetInputs.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	static LPCTSTR NoTableTag = _T("(No Table Available)");

	BEGIN_MESSAGE_MAP(TATableSourcePage, CPropertyPage)
		//{{AFX_MSG_MAP(TATableSourcePage)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TATableSourcePage::TATableSourcePage(const SVGUID & rInspectionID, const SVGUID& rTaskObjectID, int id ) 
	: CPropertyPage( id )
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	{
	}

	TATableSourcePage::~TATableSourcePage()
	{
	}
#pragma endregion Constructor

#pragma region Protected Methods
	void TATableSourcePage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TATableSourcePage)
		DDX_Control(pDX, IDC_COMBO1, m_availableSourceTableCB);
		//}}AFX_DATA_MAP
	}

	BOOL TATableSourcePage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		RetrieveAvailableList();
	
		SVString selectedTableName;
		typedef SvCmd::GetInputs Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(TableObjectType, SVNotSetSubObjectType), SVToolObjectType);
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
			else
			{	//this block should not reached, but if no input found, use default inputName and try with them to solve the problem. 
				m_inputName = SvDef::cInputTag_SourceTable;
			}
		}

		m_availableSourceTableCB.Init(m_availableList, selectedTableName, NoTableTag);

		UpdateData(false); // set data to dialog

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void TATableSourcePage::OnSelchangeCombo1() 
	{
		UpdateData(TRUE); // get data from dialog

		int index = m_availableSourceTableCB.GetCurSel();
		if (LB_ERR != index)
		{
			CString tableName;
			m_availableSourceTableCB.GetLBText(index, tableName);
			if (!tableName.IsEmpty() && tableName != NoTableTag)
			{
				HRESULT hr = ConnectToObject(m_inputName, SVString(tableName));
				if ( S_OK != hr )
				{
					SVStringVector msgList;
					msgList.push_back(m_inputName);
					msgList.push_back(SVString(tableName));
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ConnectTableSourceFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}
		}
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	HRESULT TATableSourcePage::RetrieveAvailableList()
	{ 
		typedef SvCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_InspectionID, SVObjectTypeInfoStruct(TableObjectType, SVNotSetSubObjectType), SvCmd::IsObjectFromPriorTool(m_TaskObjectID), SVToolObjectType);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_availableList = commandPtr->AvailableObjects();
		}
		return hr;
	}

	class ByName
	{
		SVString m_name;
	public:
		ByName(const SVString& rName) : m_name(rName) {}
		bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
	};

	HRESULT TATableSourcePage::ConnectToObject(const SVString& inputName, const SVString& name)
	{ 
		HRESULT hr = E_INVALIDARG;
		typedef SvCmd::ConnectToObject Command;
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			CommandPtr commandPtr = new Command(m_TaskObjectID, inputName, it->second, TableObjectType);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}
#pragma endregion Private Methods
} //namespace SvOg

