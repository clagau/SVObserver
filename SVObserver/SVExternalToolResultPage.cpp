//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolResultPage
//* .File Name       : $Workfile:   SVExternalToolResultPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 18:51:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolResultPage.h"
#include "Operators/SVExternalToolTask.h"
#include "SVInspectionProcess.h"
#include "SVSetupDialogManager.h"
#include "Operators/SVDLLToolDefinitionStructs.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "Operators/TableObject.h"
#include "Operators/SVVariantResultClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVExternalToolResultPage::SVExternalToolResultPage(LPCTSTR Title, uint32_t inspectionId, uint32_t taskObjectId, int id)
	: CPropertyPage(SVExternalToolResultPage::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
{
	m_pTask = dynamic_cast<SvOp::SVExternalToolTask*>(SVObjectManagerClass::Instance().GetObject(m_TaskObjectID));

	m_sTitle = Title;
	m_psp.pszTitle = m_sTitle.c_str();
	m_psp.dwFlags |= PSP_USETITLE;

	//{{AFX_DATA_INIT(SVExternalToolResultPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVExternalToolResultPage::~SVExternalToolResultPage()
{
}

void SVExternalToolResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolResultPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVExternalToolResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolResultPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_RESULT_LIST, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_RESULT_LIST, OnItemButtonClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolResultPage message handlers

BOOL SVExternalToolResultPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (m_pTask->m_Data.m_lNumResultValues > 0)
	{
		GetDlgItem(IDC_NO_RESULT_TXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESULT_LIST)->ShowWindow(SW_SHOW);

		DWORD dwStyle;
		CRect rc;

		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;

		// Init the control's size to cover the entire client area
		GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
		ScreenToClient(rc);
		// Create SVRPropTree control
		m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
		m_Tree.SetColumn(m_Tree.GetColumn() * 2);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		assert(pRoot);
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("External Tool Results"));
		pRoot->SetInfoText(_T(""));

		
		std::map<std::string, SVRPropertyItem*> mapGroupItems;
		std::map<std::string, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = nullptr;

		int NumResults = m_pTask->m_Data.getNumResults();
		for (int i = 0; i < NumResults; i++)
		{
			SvVol::SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aResultObjects[i];
			SvOp::ResultValueDefinition& rDefinition = m_pTask->m_Data.m_ResultDefinitions[i];
			if (rDefinition.UseDisplayNames())
			{
				std::string GroupName = rDefinition.getGroup();

				if ((iterGroup = mapGroupItems.find(GroupName)) == mapGroupItems.end())
				{	
					bool bTreeStyle = true;	// false = list-style
					pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
					pGroupItem->SetCanShrink(bTreeStyle);
					pGroupItem->SetLabelText(GroupName.c_str());
					pGroupItem->SetInfoText(_T(""));
					pGroupItem->Expand();
					pGroupItem->SetBold(true);
					mapGroupItems[GroupName] = pGroupItem;
				}
				else	
				{
					pGroupItem = iterGroup->second;
				}
			}


			SVRPropertyItem* pParent = pGroupItem != 0 ? pGroupItem : pRoot;
			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pParent);
			if (nullptr == pEdit)
			{
				break;
			}

			int iID = ID_BASE + i;
			pEdit->SetCtrlID(iID);

			std::string  ObjectName = rValue.GetName();
			std::string sLabel = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT_01 + static_cast<int>(i));
			///use the same displayname as in 8.20
			sLabel += " (";
			sLabel += rDefinition.getDisplayName();
			sLabel += ")";
			


			pEdit->SetLabelText(sLabel.c_str());

			std::string Type;
			switch (rDefinition.getVT())
			{
				case VT_BOOL: Type = _T("Bool");   break;
				case VT_I4:   Type = _T("Long");   break;
				case VT_R8:   Type = _T("Double"); break;
				case VT_BSTR: Type = _T("String"); break;
				case VT_ARRAY | VT_R8: Type = _T("Double Array"); break;
				case VT_ARRAY | VT_I4: Type = _T("Long Array"); break;

				default:      Type = _T("???");    break;
			}

			std::string sDescription = SvUl::Format(_T(" (Type : %s)  %s"), Type.c_str(), rDefinition.getHelpText().c_str());
			pEdit->SetInfoText(sDescription.c_str());
			pEdit->SetButtonText(_T("Range"));

			std::string sValue;
			rValue.getValue(sValue);
			pEdit->SetItemValue(sValue.c_str());
			if (rDefinition.getVT() == VT_BSTR || (rDefinition.getVT()& VT_ARRAY))
			{
				pEdit->ReadOnly();
			}
			pEdit->OnRefresh();
		}

		int NumTableResults = m_pTask->m_Data.getNumTableResults();
		for (int i = 0; i < NumTableResults; i++)
		{

			SvOp::ResultTableDefinition& rDefinition = m_pTask->m_Data.m_TableResultDefinitions[i];
			SvOp::TableObject* pTable = m_pTask->m_Data.m_ResultTableObjects[i];
			if (!pTable)
				break;

			if (rDefinition.UseDisplayNames())
			{
				std::string GroupName = rDefinition.getGroup();

				if ((iterGroup = mapGroupItems.find(GroupName)) == mapGroupItems.end())
				{	
					bool bTreeStyle = true;	// false = list-style
					pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
					pGroupItem->SetCanShrink(bTreeStyle);
					pGroupItem->SetLabelText(GroupName.c_str());
					pGroupItem->SetInfoText(_T(""));
					pGroupItem->Expand();
					pGroupItem->SetBold(true);
					mapGroupItems[GroupName] = pGroupItem;
				}
				else	
				{
					pGroupItem = iterGroup->second;
				}
			}

			SVRPropertyItem* pParent = pGroupItem != 0 ? pGroupItem : pRoot;
			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pParent);
			if (nullptr == pEdit)
			{
				break;
			}
			int iID = ID_BASE + NumResults + i;
			pEdit->SetCtrlID(iID);
			std::string objectname = pTable->GetName();
			///use the same displayname as in 8.20
			std::string  sLabel = "Table Object";
			if (i > 0)
			{
				sLabel = SvUl::Format(_T("%s%i"), "Table Object", i);
			}

			{
				sLabel += " (";
				sLabel += rDefinition.getDisplayName();
				sLabel += ")";
			}
			pEdit->SetLabelText(sLabel.c_str());
			pEdit->ReadOnly();

			std::string sDescription = SvUl::Format(_T(" (Type: Tableobject)  %s"), rDefinition.getHelpText().c_str());
			pEdit->SetInfoText(sDescription.c_str());

			pEdit->OnRefresh();
		}



		SVRPropertyItem* pChild = pRoot->GetChild();
		while (pChild)
		{
			pChild->Expand(TRUE);
			pChild = pChild->GetSibling();
		}
		pRoot->Expand(true);	// needed for redrawing
	}
	else
	{
		GetDlgItem(IDC_NO_RESULT_TXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESULT_LIST)->ShowWindow(SW_HIDE);

	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SVExternalToolResultPage::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);

		*plResult = FALSE;

		if (m_pTask != nullptr &&  m_pTask->m_Data.m_ResultDefinitions.size() > iIndex)
		{
			if (m_pTask->m_Data.m_ResultDefinitions[iIndex].getVT() != VT_BSTR &&
				(0 == (m_pTask->m_Data.m_ResultDefinitions[iIndex].getVT() & VT_ARRAY)))
			{
				*plResult = TRUE;	// Show button no string no array
			}
			
		}
	}
}

void SVExternalToolResultPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);

		// display value object picker
		if (SelectObject(iIndex) == IDOK)
		{
			pItem->SetItemValue(_T(""));
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}


// display VO picker dialog and return selection
int SVExternalToolResultPage::SelectObject(int iIndex)
{

	SvOp::SVVariantResultClass* pResult = dynamic_cast<SvOp::SVVariantResultClass*> (m_pTask->GetResultRangeObject(iIndex));

	if (pResult)
	{
		SVSetupDialogManager::Instance().SetupDialog(pResult->GetClassID(), pResult->getObjectId(), this);
	}

	return 0;
}

void SVExternalToolResultPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	*plResult = S_OK;


	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);
		assert(iIndex >= 0);

		// do validation
		bool bValidated = true;
		// @TODO - add validation logic
		if (!bValidated)
		{
			*plResult = S_FALSE;
		}

	}// end if ( pNMPropTree->pItem )

}

// Loops through Tree Items to fill existing SVResultObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVExternalToolResultPage::OnOK()
{

	



		CPropertyPage::OnOK();
	
}

int SVExternalToolResultPage::GetItemIndex(SVRPropertyItem* pItem)
{
	assert(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

