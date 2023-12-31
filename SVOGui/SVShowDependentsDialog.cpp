//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShowDependentsDialog
//* .File Name       : $Workfile:   SVShowDependentsDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:00:16  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <functional>
#include "SVShowDependentsDialog.h"
#include "Definitions/Color.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionCommands\CommandExternalHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
BEGIN_MESSAGE_MAP(SVShowDependentsDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
ON_NOTIFY(NM_CUSTOMDRAW, IDC_DEPENDENCY_LIST, &SVShowDependentsDialog::OnCustomdrawDependencyList)
END_MESSAGE_MAP()

const int DependentColumnNumber = 2;
const TCHAR* const ColumnHeadings[] = {_T("Client"), _T("Supplier")};

SVShowDependentsDialog::SVShowDependentsDialog(const std::set<uint32_t>& rIdsOfObjectsDependedOn, SvPb::SVObjectTypeEnum objectType, LPCTSTR DisplayText, DialogType Type /*= DeleteConfirm*/, CWnd* pParent /*=nullptr*/)
	: CDialog(SVShowDependentsDialog::IDD, pParent)
	, m_DisplayText((nullptr != DisplayText) ? DisplayText : std::string())
	, m_DialogType(Type)
{	
	FillDependencyList(rIdsOfObjectsDependedOn, objectType);
}

SVShowDependentsDialog::SVShowDependentsDialog(SvDef::StringPairVector dependencyList, LPCTSTR DisplayText /*= nullptr*/, DialogType Type /*= Show*/, CWnd* pParent /*= nullptr*/)
	: CDialog(SVShowDependentsDialog::IDD, pParent)
	, m_dependencyList(dependencyList)
	, m_DisplayText((nullptr != DisplayText) ? DisplayText : std::string())
	, m_DialogType(Type)
{
}

void SVShowDependentsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVShowDependentsDialog)
	DDX_Control(pDX, IDC_DEPENDENCY_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
}

BOOL SVShowDependentsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (DeleteConfirm == m_DialogType)
	{
		if (m_dependencyList.size() > 0 || m_dependencyResponse.dependencypair_size() > 0)
		{
			CWnd* pControl = GetDlgItem(IDC_WARNING_TEXT);
			if (nullptr != pControl)
			{
				pControl->SetWindowText(m_DisplayText.c_str());
			}
		}
		else
		{
			EndDialog(IDOK);
			return false;
		}
	}
	else
	{
		CWnd* pControl = GetDlgItem(IDC_WARNING_TEXT);
		if (nullptr != pControl)
		{
			pControl->ShowWindow(SW_HIDE);
		}
		pControl = GetDlgItem(IDCANCEL);
		if (nullptr != pControl)
		{
			pControl->ShowWindow(SW_HIDE);
		}
	}

	CRect wndRect;
	GetWindowRect(&wndRect);
	m_minSize = {wndRect.Width(), wndRect.Height()};

	// Build ListCtrl Headers
	addColumnHeadings();

	// Set the widths
	setColumnWidths();

	addItems();

	return TRUE;  // return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void SVShowDependentsDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	setColumnWidths();
}

void SVShowDependentsDialog::OnGetMinMaxInfo(MINMAXINFO* pMinMaxInfo)
{
	// Set the Minimum Track Size. Used while resizing.
	pMinMaxInfo->ptMinTrackSize.x = m_minSize.cx;
	pMinMaxInfo->ptMinTrackSize.y = m_minSize.cy;
}

void SVShowDependentsDialog::OnCustomdrawDependencyList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);

	// Take the default processing unless we 
	// set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.
		// We'll cycle the colors through red, green, and light blue.

		//! Default text color is black
		COLORREF TextColor = SvDef::Black;
		//! Get the client item text
		CString Text = m_ListCtrl.GetItemText(static_cast<int> (pLVCD->nmcd.dwItemSpec), 0);
		std::string ItemText(Text.GetString());

		for (SvPb::DependencyPair dependencyPair : m_dependencyResponse.dependencypair())
		{
			std::string clientName = dependencyPair.client().name();
			if ((ItemText == clientName || ItemText.starts_with(clientName + ".")) && dependencyPair.sourceisclient())
			{
				TextColor = SvDef::Blue;
			}
		}

		// Store the color back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText = TextColor;

		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
	}
}

void SVShowDependentsDialog::FillDependencyList(const std::set<uint32_t>& rIdsOfObjectsDependedOn, SvPb::SVObjectTypeEnum objectType)
{	
	SvPb::ToolDependencyEnum ToolDependencyProto = (DeleteConfirm == m_DialogType) ? SvPb::ToolDependencyEnum::Client : SvPb::ToolDependencyEnum::ClientAndSupplier;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getdependencyrequest();
	auto* idSet =  pRequest->mutable_idsofobjectsdependedon();
	idSet->Add(rIdsOfObjectsDependedOn.begin(), rIdsOfObjectsDependedOn.end());
	pRequest->set_objecttype(objectType);
	pRequest->set_tooldependecytype(ToolDependencyProto);
	pRequest->set_dependecytype(SvPb::DependecyTypeEnum::Tool);
	
	m_dependencyList.clear();
	HRESULT hr = SvCmd::InspectionCommands(0, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getdependencyresponse())
	{
		m_dependencyResponse = responseCmd.getdependencyresponse();
	}
}

void SVShowDependentsDialog::addColumnHeadings()
{
	// load the Column names
	for (int i = 0; i < DependentColumnNumber; i++)
	{
		m_ListCtrl.InsertColumn(i, ColumnHeadings[i], LVCFMT_LEFT, -1, i);
	}
}

void SVShowDependentsDialog::addItems()
{
	int index = 0;
	CListCtrl& rCtrl = m_ListCtrl;
	
	if (false == m_dependencyList.empty())
	{
		std::for_each(m_dependencyList.begin(), m_dependencyList.end(), [&index, &rCtrl](const SvDef::StringPair& rel)->void
		{
			rCtrl.InsertItem(index, rel.second.c_str());
			rCtrl.SetItemText(index, 1, rel.first.c_str());
			++index;
		}
		);
	}

	if (0 < m_dependencyResponse.dependencypair_size())
	{
		for (SvPb::DependencyPair dependencyPair : m_dependencyResponse.dependencypair())
		{
			rCtrl.InsertItem(index, dependencyPair.client().name().c_str());
			rCtrl.SetItemText(index, 1, dependencyPair.supplier().name().c_str());
			++index;
		}
	}
	
}

void SVShowDependentsDialog::setColumnWidths()
{
	if (::IsWindow(m_ListCtrl.GetSafeHwnd()))
	{
		CRect viewRect;
		m_ListCtrl.GetClientRect(viewRect);

		int columnWidth = viewRect.Width() / DependentColumnNumber;

		for (int i = 0; i < DependentColumnNumber; i++)
		{
			m_ListCtrl.SetColumnWidth(i, columnWidth);
		}
	}
}

INT_PTR showDependentsDialogIfNecessary(const std::set<uint32_t>& rIdsOfObjectsDependedOn, const std::string& rDisplayString)
{
	SVShowDependentsDialog Dlg(rIdsOfObjectsDependedOn, SvPb::SVToolObjectType, rDisplayString.c_str());
	return Dlg.DoModal();
}

} //namespace SvOg
