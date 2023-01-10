//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalResultPage
//* .File Name       : $Workfile:   SVTADlgExternalResultPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 18:51:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgExternalResultPage.h"
#include "SVOGuiUtility/ExternalToolTaskController.h"
#include "SVObserver/SVSetupDialogManager.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVTADlgExternalResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgExternalResultPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_RESULT_LIST, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_RESULT_LIST, OnItemButtonClick)
END_MESSAGE_MAP()

SVTADlgExternalResultPage::SVTADlgExternalResultPage(LPCTSTR Title, uint32_t inspectionId, uint32_t taskObjectId, SvOgu::ExternalToolTaskController& rExternalToolTaskController)
	: CPropertyPage(SVTADlgExternalResultPage::IDD)
	, m_rExternalToolTaskController(rExternalToolTaskController)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_sTitle(Title)
	, m_ValueController{ SvOgu::BoundValues{ inspectionId, m_TaskObjectID } }
{
	m_psp.pszTitle = m_sTitle.c_str();
	m_psp.dwFlags |= PSP_USETITLE;

	//{{AFX_DATA_INIT(SVTADlgExternalResultPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVTADlgExternalResultPage::~SVTADlgExternalResultPage()
{
}

void SVTADlgExternalResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgExternalResultPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SVTADlgExternalResultPage message handlers

BOOL SVTADlgExternalResultPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (m_InspectionID > 0)
	{
		SvCmd::RunOnceSynchronous(m_InspectionID);
	}
	
	DWORD dwStyle;
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;

	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
	ScreenToClient(rc);
	// Create SVRPropTree control

	m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
	m_Tree.SetColumn(m_Tree.GetColumn() * 2); //sets the splitter position to twice the default

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SVTADlgExternalResultPage::rebuildPropertyTree()
{
	m_Tree.DeleteAllItems();

	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	Log_Assert(pRoot);
	pRoot->SetCanShrink(false);
	pRoot->SetLabelText(_T("External Tool Results"));
	pRoot->SetInfoText(_T(""));

	auto [mapGroupItems, NumResults] = AssembleTree(pRoot);

	HandleTables(mapGroupItems, NumResults, pRoot);

	SVRPropertyItem* pChild = pRoot->GetChild();
	while (pChild)
	{
		pChild->Expand(TRUE);
		pChild = pChild->GetSibling();
	}
	pRoot->Expand(true);	// needed for redrawing

	UpdateData(FALSE);
}


std::pair<std::map<std::string, SVRPropertyItem*>, int> SVTADlgExternalResultPage::AssembleTree(SVRPropertyItem* pRoot)
{
	std::map<std::string, SVRPropertyItem*> mapGroupItems;

	SVRPropertyItem* pGroupItem = nullptr;

	auto resultDefinitions = m_rExternalToolTaskController.getResultValuesDefinition().resultvaluesdefinition();
	int NumResults = static_cast<int>(resultDefinitions.size());
	for (int i = 0; i < NumResults; i++)
	{
		auto rDefinition = resultDefinitions[i];
		if (rDefinition.usedisplaynames())
		{
			std::string GroupName = rDefinition.groupname();
			auto iterGroup = mapGroupItems.find(GroupName);
			if (iterGroup == mapGroupItems.end())
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

		std::string sLabel = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT_01 + static_cast<int>(i));
		///use the same displayname as in 8.20
		sLabel += " (";
		sLabel += rDefinition.displayname();
		sLabel += ")";

		pEdit->SetLabelText(sLabel.c_str());

		std::string Type;
		switch (rDefinition.vt())
		{
			case VT_BOOL: Type = _T("Bool");   break;
			case VT_I4:   Type = _T("Long");   break;
			case VT_R8:   Type = _T("Double"); break;
			case VT_BSTR: Type = _T("String"); break;
			case VT_ARRAY | VT_R8: Type = _T("Double Array"); break;
			case VT_ARRAY | VT_I4: Type = _T("Long Array"); break;

			default:      Type = _T("???");    break;
		}

		std::string sDescription = SvUl::Format(_T(" (Type : %s)  %s"), Type.c_str(), rDefinition.helptext().c_str());
		pEdit->SetInfoText(sDescription.c_str());
		pEdit->SetButtonText(_T("Range"));
		pEdit->ReadOnly();
		_variant_t temp = m_ValueController.Get<_variant_t>(SvPb::ExternalResultEId + i);
		std::string sValue = SvUl::VariantToString(temp);

		pEdit->SetItemValue(sValue.c_str());
		if (rDefinition.vt() == VT_BSTR || (rDefinition.vt() & VT_ARRAY))
		{
			pEdit->ReadOnly();
		}
		pEdit->OnRefresh();
	}

	return {mapGroupItems, NumResults};
}



void SVTADlgExternalResultPage::HandleTables(std::map<std::string, SVRPropertyItem*>& rMapGroupItems, int NumResults, SVRPropertyItem* pRoot)
{
	auto tableResultsResponse = m_rExternalToolTaskController.getTableResults();

	int NumTableResults = tableResultsResponse.tableresultsdefinition_size();
	for (int i = 0; i < NumTableResults; i++)
	{
		SVRPropertyItem* pGroupItem = nullptr;
		auto rDefinition = tableResultsResponse.tableresultsdefinition()[i];
		auto table = tableResultsResponse.tableobjects()[i];

		if (rDefinition.usedisplaynames())
		{
			std::string GroupName = rDefinition.groupname();
			auto iterGroup = rMapGroupItems.find(GroupName);

			if (iterGroup == rMapGroupItems.end())
			{
				bool bTreeStyle = true;	// false = list-style
				pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
				pGroupItem->SetCanShrink(bTreeStyle);
				pGroupItem->SetLabelText(GroupName.c_str());
				pGroupItem->SetInfoText(_T(""));
				pGroupItem->Expand();
				pGroupItem->SetBold(true);
				rMapGroupItems[GroupName] = pGroupItem;
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
		std::string objectname = table.name();
		///use the same displayname as in 8.20
		std::string  sLabel = "Table Object";
		if (i > 0)
		{
			sLabel = SvUl::Format(_T("%s%i"), "Table Object", i);
		}

		{
			sLabel += " (";
			sLabel += rDefinition.displayname();
			sLabel += ")";
		}
		pEdit->SetLabelText(sLabel.c_str());
		pEdit->ReadOnly();

		std::string sDescription = SvUl::Format(_T(" (Type: Tableobject)  %s"), rDefinition.helptext().c_str());
		pEdit->SetInfoText(sDescription.c_str());

		pEdit->OnRefresh();
	}
}


void SVTADlgExternalResultPage::DisplayResults()
{
	m_ValueController.Init();

	if (m_rExternalToolTaskController.getNumResultValues() > 0)
	{
		GetDlgItem(IDC_NO_RESULT_TXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESULT_LIST)->ShowWindow(SW_SHOW);

		rebuildPropertyTree();
	}
	else
	{
		GetDlgItem(IDC_NO_RESULT_TXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESULT_LIST)->ShowWindow(SW_HIDE);
	}
}



void SVTADlgExternalResultPage::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);

		auto resultDefinitions = m_rExternalToolTaskController.getResultValuesDefinition().resultvaluesdefinition();
		*plResult = FALSE;

		if (resultDefinitions.size() > iIndex)
		{
			if (resultDefinitions[iIndex].vt() != VT_BSTR &&
				(0 == (resultDefinitions[iIndex].vt() & VT_ARRAY)))
			{
				*plResult = TRUE;	// Show button no string no array
			}
			
		}
	}
}

void SVTADlgExternalResultPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
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
int SVTADlgExternalResultPage::SelectObject(int iIndex)
{
	auto response = m_rExternalToolTaskController.getResultRangeObjectAtIndex(iIndex);

	if (response.classid() != SvPb::ClassIdEnum::NoObjectClassId && response.objectid() > 0)
	{
		SVSetupDialogManager::Instance().SetupDialog(response.classid(), m_InspectionID, response.objectid(), this);
	}

	return 0;
}

void SVTADlgExternalResultPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	*plResult = S_OK;


	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);
		Log_Assert(iIndex >= 0);	UNREFERENCED_PARAMETER(iIndex);
		// @TODO - add validation logic, e.g.: if (!bValidated) {*plResult = S_FALSE;}
	}

}

// Loops through Tree Items to fill existing SVResultObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVTADlgExternalResultPage::OnOK()
{
	CPropertyPage::OnOK();
}

BOOL SVTADlgExternalResultPage::OnSetActive()
{
	auto ret = CPropertyPage::OnSetActive();

	DisplayResults();

	return ret;
}

int SVTADlgExternalResultPage::GetItemIndex(SVRPropertyItem* pItem)
{
	Log_Assert(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

