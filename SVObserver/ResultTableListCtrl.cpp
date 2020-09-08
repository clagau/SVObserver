//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Table Window.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ResultTableListCtrl.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

IMPLEMENT_DYNCREATE( ResultTableListCtrl, CListCtrl )

BEGIN_MESSAGE_MAP( ResultTableListCtrl, CListCtrl )
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

#pragma region Constructor
ResultTableListCtrl::ResultTableListCtrl() : CListCtrl()
{
	VERIFY(m_ContextMenuItem.LoadMenu(IDR_RESULTS_TABLE_CONTEXT_MENU));
}

ResultTableListCtrl::~ResultTableListCtrl()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL ResultTableListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= (LVS_SHOWSELALWAYS | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER);
	BOOL Result = CListCtrl::Create(dwStyle, rect, pParentWnd, nID);

	SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return Result;
}

void ResultTableListCtrl::updateList()
{
	bool Update = true;
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ObjectIdEnum::EnvironmentResultUpdateId, pObject);
	if (nullptr != pObject)
	{
		double Value;
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		Update = 0.0 < Value ? true : false;
	}
	Update = Update || !SVSVIMStateClass::CheckState(SV_STATE_RUNNING);
	if (!Update)
	{ 
		return;
	}

	if (nullptr != m_pDoc)
	{
		m_ResultData = m_pDoc->getResultTableData();
		// If Result table data size is 0 then no updating required
		if (0 == m_ResultData.size())
		{
			DeleteAllItems();
			//Deletes all columns
			addColumnHeadings(m_ResultData);
			m_UpdateTimeStamp = 0;
			return;
		}

		SetRedraw(false);

		if (m_UpdateTimeStamp < m_pDoc->getResultDefinitionUpdatTimeStamp())
		{
			addColumnHeadings(m_ResultData);
			m_UpdateTimeStamp = m_ResultData[0].m_LastUpdateTimeStamp;
		}

		int rowCountOld = GetItemCount();
		if (0 < m_ResultData.size())
		{
			bool isArray = VT_ARRAY == (m_ResultData[0].m_rowData.vt & VT_ARRAY);
			int rowCountNew = 0;
			if (isArray)
			{
				rowCountNew = SvUl::getArraySizeFromOneDim(m_ResultData[0].m_rowData);
			}
			for (int i = rowCountOld; i < rowCountNew; i++)
			{
				CString tmp;
				tmp.Format("%3d", i + 1);
				InsertItem(i, tmp);
			}
			for (int i = 0; i < m_ResultData.size(); i++)
			{
				if (isArray)
				{
					std::vector<double> values = SvUl::getVectorFromOneDim<double>(m_ResultData[i].m_rowData);
					for (int j = 0; j < values.size(); j++)
					{
						CString tmp;
						tmp.Format("%f", values[j]);
						SetItemText(j, i + 1, tmp);
					}
				}
			}
			if (0 < rowCountNew)
			{
				while (GetItemCount() > rowCountNew)
				{
					DeleteItem(rowCountNew);
				}
			}
			else
			{
				DeleteAllItems();
			}
		}
		else
		{
			DeleteAllItems();
		}

		SetRedraw(true);
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
	}
}
#pragma endregion Public Methods

#pragma region Private Methods
void ResultTableListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	//	Allow the Result Picker in every mode except Stop
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_EDIT | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
	{
		CMenu* pPopupMenu = m_ContextMenuItem.GetSubMenu(0);
		assert(nullptr != pPopupMenu);
		CWnd* pOwner = this;
		while (pOwner->GetStyle() & WS_CHILD)
		{
			pOwner = pOwner->GetParent();
		}
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pOwner);
	}
}

void ResultTableListCtrl::addColumnHeadings(const std::vector <SvIe::IPResultTableData>& rDataVec)
{
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	int nNewCount = static_cast<int>(rDataVec.size()) + 1;

	if (nColumnCount != nNewCount)
	{
		m_ColumnWidthSet = false;
		if (nColumnCount > nNewCount)
		{
			// Delete columns if too many. 
			for (int i = 0; i < nColumnCount-nNewCount; i++)
			{
				DeleteColumn(nNewCount);
			}
		}
		else
		{	// Add columns if too less
			for (int i = nColumnCount; i < nNewCount; i++)
			{
				InsertColumn(i, _T("No."), LVCFMT_LEFT, LVSCW_AUTOSIZE, i);
			}			
		}
	}	

	// load the Column names
	int i = 1;
	for(SvIe::IPResultTableData data : rDataVec )
	{
		LVCOLUMN col;
		col.mask = LVCF_TEXT;
		col.pszText = const_cast< LPSTR >(data.m_columnName.c_str());
		SetColumn(i, &col);
		i++;
	}
	m_ColumnCount = i-1;
	setColumnWidths();
}

void ResultTableListCtrl::setColumnWidths()
{
	if (!m_ColumnWidthSet )
	{
		CRect viewRect;
		GetClientRect( viewRect );
		const int NoRowWidth = 30;

		//calc the width of the first three columns
		int columnWidth = ( viewRect.Width() - NoRowWidth) / std::max(m_ColumnCount,1);

		SetColumnWidth(0, NoRowWidth);
		//set the width of the first three columns
		for( int i = 1; i <= m_ColumnCount; i++ )
		{
			SetColumnWidth( i, columnWidth );
		}

		m_ColumnWidthSet = true;
	}
}
#pragma endregion Private Methods

