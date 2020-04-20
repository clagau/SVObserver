//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file Ranking2FilterDlg.cpp
/// All Rights Reserved
//*****************************************************************************
//* This class is the dialog for the ranking2 filter
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits.h>
#include "Ranking2FilterDlg.h"
#include "BoundValue.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "GuiValueHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "Definitions\GlobalConst.h"
#include "GridCtrlLibrary\GridCellCheck.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
constexpr int cHeaderSize = 1;
constexpr int CellSize = 40;
constexpr int HeaderColumnSize = 60;
constexpr COLORREF KERNEL_CENTER_COLOR = RGB(240, 240, 240);

constexpr char* HeaderCell = _T("Row / Column");
constexpr char* StatusGridSingleCell = _T("Row: %d  Col: %d  Index: %d");
constexpr char* StatusGridMultiCell = _T("Count: %d");
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(Ranking2FilterDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_KERNELWIDTH, OnCbnSelchangeKernelWidth)
	ON_CBN_SELCHANGE(IDC_KERNELHEIGHT, OnCbnSelchangeKernelHeight)
	ON_LBN_SELCHANGE(IDC_RANKINGRANK, OnSelchangeRankingRank)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnGridSelChanged)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_BN_CLICKED(IDOK, OnBnMfcStandardOk)
	ON_BN_CLICKED(ID_OK, OnBnClickedOk)
END_MESSAGE_MAP()

#pragma region Constructor
Ranking2FilterDlg::Ranking2FilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent)
	: CDialog(Ranking2FilterDlg::IDD, pParent)
	, m_KernelWidth(0)
	, m_KernelHeight(0)
	, m_GridStatus(_T(""))
	, m_filterID(filterId)
	, m_InspectionID(inspectionId)
	, m_Values(SvOg::BoundValues(inspectionId, filterId))
{
}

Ranking2FilterDlg::~Ranking2FilterDlg()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT Ranking2FilterDlg::SetInspectionData()
{
	m_Values.Set<long>(SvPb::FilterKernelWidthEId, m_KernelWidth);
	m_Values.Set<long>(SvPb::FilterKernelHeightEId, m_KernelHeight);
	_variant_t value = ConvertVectorToVariantSafeArray<std::vector<long>>(m_KernelArray);
	m_Values.Set<_variant_t>(SvPb::FilterKernelEId, value);
	m_Values.Set<long>(SvPb::RankingFilterRankingRankEId, m_RankingRank);
	return m_Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void Ranking2FilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_KERNELHEIGHT, m_HeightCtrl);
	DDX_Control(pDX, IDC_KERNELWIDTH, m_WidthCtrl);
	DDX_Control(pDX, IDC_RANKINGRANK, m_ctlRankingRank);
	DDX_Text(pDX, IDC_GRID_STATUS, m_GridStatus);
}

BOOL Ranking2FilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initializeFilter();

	std::string Entry;
	for (int i = 1; i <= SvDef::cMaxKernelSize; i++)
	{
		//Check that its an odd value
		if (1 == (i % 2))
		{
			Entry = SvUl::Format(_T("%d"), i);
			m_WidthCtrl.AddString(Entry.c_str());
			m_HeightCtrl.AddString(Entry.c_str());
		}
	}

	m_Grid.SetFixedRowCount(cHeaderSize);
	m_Grid.SetFixedColumnCount(cHeaderSize);
	m_Grid.SetRowResize(FALSE);
	m_Grid.SetColumnResize(FALSE);
	m_Grid.AllowReorderColumn(false);
	m_Grid.EnableDragAndDrop(FALSE);
	//m_Grid.SetEditable(FALSE);

	m_RankingRank = m_Values.Get<long>(SvPb::RankingFilterRankingRankEId);
	m_ctlRankingRank.SetCurSel(m_RankingRank);

	enableCells();
	recalculateKernel(m_KernelWidth, m_KernelHeight);

	UpdateData(FALSE);
	return TRUE;
}

BOOL Ranking2FilterDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL Result(false);

	if (doesControlHaveFocus(IDC_GRID))
	{
		if (WM_KEYDOWN == pMsg->message && VK_SPACE == pMsg->wParam)
		{
			updateGridSelection();
			Result = true;
		}
		else if (WM_KEYDOWN == pMsg->message)
		{
			Result = inputGridCtrlKey(pMsg->wParam);
		}
	}

	if (!Result)
	{
		Result = CDialog::PreTranslateMessage(pMsg);
	}

	return Result;
}

void Ranking2FilterDlg::OnCbnSelchangeKernelWidth()
{
	recalculateKernel(m_WidthCtrl.GetCurSel() * 2 + 1, -1);
	enableCells();
	UpdateData(FALSE);
}

void Ranking2FilterDlg::OnCbnSelchangeKernelHeight()
{
	recalculateKernel(-1, m_HeightCtrl.GetCurSel() * 2 + 1);
	enableCells();
	UpdateData(FALSE);
}

void Ranking2FilterDlg::OnSelchangeRankingRank()
{
	long lIndex = m_ctlRankingRank.GetCurSel();

	if (LB_ERR != lIndex)
	{
		m_RankingRank = static_cast<long>(m_ctlRankingRank.GetItemData(lIndex));
	}// end if

}// end OnSelchangeRankingRank

void Ranking2FilterDlg::OnGridSelChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UpdateData(FALSE);
	updateEditCellandStatus();
	setKernelFromControl();
}

void Ranking2FilterDlg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	setKernelFromControl();
}

void Ranking2FilterDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	try
	{
		isDataValid();
		SetInspectionData();
		CDialog::OnOK();
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(rSvE.getMessage());
	}
}

void Ranking2FilterDlg::initializeFilter()
{
	m_Values.Init();
	m_KernelWidth = m_Values.Get<long>(SvPb::FilterKernelWidthEId);
	m_KernelHeight = m_Values.Get<long>(SvPb::FilterKernelHeightEId);

	m_KernelArray.clear();
	m_KernelArray = ConvertVariantSafeArrayToVector<long>(m_Values.Get<_variant_t>(SvPb::FilterKernelEId));
}

bool Ranking2FilterDlg::inputGridCtrlKey(WPARAM Key)
{
	bool Result(false);

	if (VK_TAB == Key)
	{
		bool ShiftKeyPressed(false);

		ShiftKeyPressed = 0 != HIBYTE(GetKeyState(VK_SHIFT));
		SvGcl::CCellID Cell = m_Grid.GetFocusCell();
		if (m_KernelHeight == Cell.row && m_KernelWidth == Cell.col && !ShiftKeyPressed)
		{
			GetDlgItem(IDC_IMPORT_FILTER)->SetFocus();
			Result = true;
		}
		else if (1 == Cell.row && 1 == Cell.col && ShiftKeyPressed)
		{
			GetDlgItem(IDC_CLIPPING_ON)->SetFocus();
			Result = true;
		}
		else if (-1 == Cell.row && -1 == Cell.col)
		{
			SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
			for (int i = Selection.GetMinRow(); i <= Selection.GetMaxRow(); i++)
			{
				for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
				{
					if (m_Grid.IsCellSelected(i, j))
					{
						m_Grid.SetFocusCell(i, j);
						m_Grid.SetSelectedRange(i, j, i, j);
						Result = TRUE;
						//Have found fist selected cell so exit loops
						i = Selection.GetMaxRow();
						j = Selection.GetMaxCol();
					}
				}
			}

		}

		setKernelFromControl();
	}

	return Result;
}

void Ranking2FilterDlg::recalculateKernel(int Width, int Height)
{
	initializeKernel(Width, Height);

	SvGcl::GV_ITEM Item;

	int MiddleRow = (m_KernelHeight + 1) / 2;
	int MiddleCol = (m_KernelWidth + 1) / 2;

	for (int row = 0; row <= m_KernelHeight; row++)
	{
		for (int col = 0; col <= m_KernelWidth; col++)
		{
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
			Item.row = row;
			Item.col = col;
			Item.crBkClr = CLR_DEFAULT;


			if (0 == row && 0 == col)
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
				Item.strText = HeaderCell;
				m_Grid.SetItem(&Item);
			}
			else if (0 == row)
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format(_T("%d"), col);
				m_Grid.SetItem(&Item);
			}
			else if (0 == col)
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
				Item.strText.Format(_T("%d"), row);
				m_Grid.SetItem(&Item);
			}
			else
			{
				//Check if cells are in the middle row and column of the kernel
				if (MiddleRow == row || MiddleCol == col)
				{
					Item.crBkClr = KERNEL_CENTER_COLOR;
				}
				int ValueIndex = (row - 1)*m_KernelWidth + (col - 1);
				long Value(1);
				std::vector<long>::iterator Iter = m_KernelArray.begin() + ValueIndex;
				if (Iter != m_KernelArray.end())
				{
					Value = *Iter;
				}

				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format(_T(""));
				//We need to use the using here because the macro RUNTIME_CLASS cannot handle namespaces
				using namespace SvGcl;
				m_Grid.SetItem(&Item);
				m_Grid.SetCellType(row, col, RUNTIME_CLASS(CGridCellCheck));
				SvGcl::CGridCellCheck* cell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, col));
				cell->SetCheck(1 == Value);
			}
		}
	}
	updateEditCellandStatus();
}

void Ranking2FilterDlg::initializeKernel(int Width, int Height)
{
	long SourceKernelWidth(m_KernelWidth);
	long SourceKernelHeight(m_KernelHeight);

	if (0 == Width)
	{
		m_KernelWidth = SvDef::cStandardKernelSize;
	}
	else if (-1 != Width)
	{
		m_KernelWidth = Width;
	}

	if (0 == Height)
	{
		m_KernelHeight = SvDef::cStandardKernelSize;
	}
	else if (-1 != Height)
	{
		m_KernelHeight = Height;
	}

	m_Grid.SetColumnCount(cHeaderSize + m_KernelWidth);
	for (int i = 0; i < cHeaderSize + m_KernelWidth; i++)
	{
		if (0 == i)
		{
			m_Grid.SetColumnWidth(i, HeaderColumnSize);
		}
		else
		{
			m_Grid.SetColumnWidth(i, CellSize);
		}
	}

	m_Grid.SetRowCount(cHeaderSize + m_KernelHeight);
	m_Grid.SetRowHeight(0, CellSize);

	//Deselect when changing size etc..
	SvGcl::CCellRange CellSelection;
	m_Grid.SetSelectedRange(CellSelection);
	SvGcl::CCellID CellFocus;
	m_Grid.SetFocusCell(CellFocus);

	int CurrentSelection = (m_KernelWidth - 1) / 2;
	if (m_WidthCtrl.GetCurSel() != CurrentSelection && CurrentSelection < m_WidthCtrl.GetCount())
	{
		m_WidthCtrl.SetCurSel(CurrentSelection);
	}
	CurrentSelection = (m_KernelHeight - 1) / 2;
	if (m_HeightCtrl.GetCurSel() != CurrentSelection && CurrentSelection < m_HeightCtrl.GetCount())
	{
		m_HeightCtrl.SetCurSel(CurrentSelection);
	}

	copyKernel(SourceKernelWidth, SourceKernelHeight);
}

void Ranking2FilterDlg::copyKernel(long SourceKernelWidth, long SourceKernelHeight)
{
	//Copy kernel only if the source and destination are different in size
	if (m_KernelWidth != SourceKernelWidth || m_KernelHeight != SourceKernelHeight)
	{
		std::vector<long> KernelArrayCopy {m_KernelArray};
		m_KernelArray.assign(m_KernelWidth*m_KernelHeight, 1);

		if (m_KernelArray.size() > KernelArrayCopy.size())
		{
			long OffsetWidth = (m_KernelWidth - SourceKernelWidth) / 2;
			long OffsetHeight = (m_KernelHeight - SourceKernelHeight) / 2;
			for (int i = 0; i < SourceKernelHeight; i++)
			{
				long CopyIndex = (i + OffsetHeight)*m_KernelWidth + OffsetWidth;
				std::vector<long>::iterator IterKernel = m_KernelArray.begin() + CopyIndex;
				if (IterKernel != m_KernelArray.end() && IterKernel + SourceKernelWidth - 1 != m_KernelArray.end())
				{
					std::vector<long>::iterator IterCopy = KernelArrayCopy.begin() + i*SourceKernelWidth;
					if (IterCopy != KernelArrayCopy.end() && IterCopy + SourceKernelWidth - 1 != KernelArrayCopy.end())
					{
						std::copy(IterCopy, IterCopy + SourceKernelWidth, IterKernel);
					}
				}
			}
		}
		else
		{
			long OffsetWidth = (SourceKernelWidth - m_KernelWidth) / 2;
			long OffsetHeight = (SourceKernelHeight - m_KernelHeight) / 2;
			for (int i = 0; i < m_KernelHeight; i++)
			{
				long CopyIndex = i*m_KernelWidth;
				std::vector<long>::iterator IterKernel = m_KernelArray.begin() + CopyIndex;
				if (IterKernel != m_KernelArray.end() && IterKernel + m_KernelWidth - 1 != m_KernelArray.end())
				{
					std::vector<long>::iterator IterCopy = KernelArrayCopy.begin() + (i + OffsetHeight)*SourceKernelWidth + OffsetWidth;
					if (IterCopy != KernelArrayCopy.end() && IterCopy + OffsetWidth - 1 != KernelArrayCopy.end())
					{
						std::copy(IterCopy, IterCopy + m_KernelWidth, IterKernel);
					}
				}
			}
		}
	}
}

void Ranking2FilterDlg::updateGridSelection()
{
	bool value = false;
	bool first = true;

	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	for (int row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++)
	{
		for (int col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++)
		{
			if (m_Grid.IsCellSelected(row, col))
			{
				SvGcl::CGridCellCheck* cell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, col));
				if (first)
				{
					value = (TRUE != cell->GetCheck()); //switch from true to false or in the other way
					first = false;
				}
				cell->SetCheck(value);
				int EditIndex = (row - 1)*m_KernelWidth + (col - 1);
				std::vector<long>::iterator Iter = m_KernelArray.begin() + EditIndex;
				if (Iter != m_KernelArray.end())
				{
					*Iter = value ? 1 : 0;
				}
			}
		}
	}
	m_Grid.Refresh();

	updateEditCellandStatus();
}

void Ranking2FilterDlg::updateEditCellandStatus()
{
	int Row = 0;
	int Col = 0;
	int CellCount = 0;

	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	for (int i = Selection.GetMinRow(); i <= Selection.GetMaxRow(); i++)
	{
		for (int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++)
		{
			if (m_Grid.IsCellSelected(i, j))
			{
				if (CellCount == 0)
				{
					Row = i;
					Col = j;
				}
				CellCount++;
			}
		}
	}
	if (0 == CellCount)
	{
		m_GridStatus = _T("");
	}
	else if (1 == CellCount)
	{
		m_GridStatus.Format(StatusGridSingleCell, Row, Col, (Row - 1)*m_KernelWidth + Col);
	}
	else
	{
		m_GridStatus.Format(StatusGridMultiCell, CellCount);
	}
}

void Ranking2FilterDlg::isDataValid() const
{
	//Kernel array and kernel height and width must match
	if (m_KernelArray.size() != m_KernelWidth*m_KernelHeight)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_KernelArray.size()));
		msgList.push_back(SvUl::Format(_T("%d"), m_KernelWidth));
		msgList.push_back(SvUl::Format(_T("%d"), m_KernelHeight));
		SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_DataInvalidKernelSize, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10228);
		throw message;
	}

	//Check that the Kernel Width and Height are odd and between 1 and MaxKernelSize
	if (1 != m_KernelWidth % 2 || 1 > m_KernelWidth || SvDef::cMaxKernelSize < m_KernelWidth)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_KernelWidth));
		msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
		SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_DataInvalidKernelWidth, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10228);
		throw message;
	}
	if (1 != m_KernelHeight % 2 || 1 > m_KernelHeight || SvDef::cMaxKernelSize < m_KernelHeight)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_KernelHeight));
		msgList.push_back(SvUl::Format(_T("%d"), SvDef::cMaxKernelSize));
		SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_DataInvalidKernelHeight, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10228);
		throw message;
	}
}

bool Ranking2FilterDlg::doesControlHaveFocus(UINT ControlID) const
{
	bool Result(false);

	CWnd* pFocusWnd = GetFocus();
	if (nullptr != pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
	{
		Result = ControlID == GetFocus()->GetDlgCtrlID();
	}
	return Result;
}

void Ranking2FilterDlg::enableCells()
{
	m_ctlRankingRank.ResetContent();
	m_ctlRankingRank.SetItemData(m_ctlRankingRank.AddString(_T("MEDIAN")), SVValueMedian);

	for (long j = 0; j < m_KernelWidth * m_KernelHeight; j++)
	{
		std::string Temp = SvUl::Format(_T("%02d"), j + 1);
		m_ctlRankingRank.SetItemData(m_ctlRankingRank.AddString(Temp.c_str()), j + 1);
	}

	if (SVValueMedian == m_RankingRank)
	{
		m_ctlRankingRank.SelectString(-1, _T("MEDIAN"));
	}// end if
	else
	{
		std::string Temp = SvUl::Format("%02d", m_RankingRank);
		m_ctlRankingRank.SelectString(-1, Temp.c_str());
	}// end else

}// end EnableCells

void Ranking2FilterDlg::setKernelFromControl()
{
	assert(m_KernelHeight*m_KernelWidth == m_KernelArray.size());
	for (int col = 1; col <= m_KernelWidth; col++)
	{
		for (int row = 1; row <= m_KernelHeight; row++)
		{
			SvGcl::CGridCellCheck* cell = dynamic_cast<SvGcl::CGridCellCheck*>(m_Grid.GetCell(row, col));
			assert(nullptr != cell);
			if (nullptr != cell)
			{
				bool value = (TRUE == cell->GetCheck());

				int EditIndex = (row - 1)*m_KernelWidth + (col - 1);
				std::vector<long>::iterator Iter = m_KernelArray.begin() + EditIndex;
				if (Iter != m_KernelArray.end())
				{
					*Iter = value ? 1 : 0;
				}
			}
		}
	}
}

#pragma endregion Protected Methods
}  //namespace SvOg
