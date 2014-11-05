//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Custom2 Filter dialog
//* .File Name       : $Workfile:   Custom2FilterDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   05 Nov 2014 05:05:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <fstream>
#include <limits.h>
#include "Custom2FilterDlg.h"
#include "Custom2Filter.h"
#include "SVObserver.h"
#include "SVLibrary/SVFileDialog.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVXMLLibrary/SVXML2TreeConverter.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int HeaderSize = 1;
static const int CellSize = 40;
static const int HeaderColumnSize = 60;
static const int MaxCellCharSize = 3;
static const COLORREF KERNEL_CENTER_COLOR = RGB( 240, 240, 240 );

static const TCHAR HeaderCell[] = _T("Row / Column");
static const TCHAR NoCommonValue[] = _T("###");
static const TCHAR* const SumSelections[] = {_T("Total Sum"), _T("Positive Sum"), _T("Negative Sum"), _T("Absolute Sum") };
static const TCHAR StatusGridSingleCell[] = _T("Row: %d  Col: %d  Index: %d");
static const TCHAR StatusGridMultiCell[] = _T("Count: %d  Sum: %d");
static const TCHAR CustomFilterExportFileFilters[] = _T("Custom Filter Export Files (*.xcf)|*.xcf||");
static const TCHAR CustomFilterExportFileExt[] = _T("xcf");

static const TCHAR ExportFailed[] = _T("Custom filter export failed");
static const TCHAR ImportFailed[] = _T("The Custom Filter file [%s] could not be imported.");
static const TCHAR XmlFormatInvalid[] = _T("\r\nThe XML format is invalid.");
static const TCHAR VersionMismatch[] = _T("\r\nVersion mismatch.");
static const TCHAR ImportFailedDataInvalid[] = _T("Custom filter import failed, Kernel data is invalid.\r\n");
static const TCHAR DataInvalidKernelSize[] = _T("The Kernel array size [%d] does not match the size of the Kernel Width [%d] and Height [%d]");
static const TCHAR DataInvalidKernelWidth[] = _T("The Kernel Width is not valid [%d], it must be odd and be between the limits including 1 and %d");
static const TCHAR DataInvalidKernelHeight[] = _T("The Kernel Height is not valid [%d], it must be odd and be between the limits including 1 and %d");
static const TCHAR DataInvalidNormalizationFactor[] = _T("The Normalization Factor is not allowed to be 0 or negative");
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(Custom2FilterDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_KERNELWIDTH, &Custom2FilterDlg::OnCbnSelchangeKernelWidth)
	ON_CBN_SELCHANGE(IDC_KERNELHEIGHT, &Custom2FilterDlg::OnCbnSelchangeKernelHeight)
	ON_CBN_SELCHANGE(IDC_SUM_SELECTOR, &Custom2FilterDlg::OnCbnSelchangeSumSelector)
	ON_BN_CLICKED(IDC_APPLY_SUM, &Custom2FilterDlg::OnBnClickedApplySum)
	ON_EN_CHANGE(IDC_NORM_FACTOR, &Custom2FilterDlg::OnEnChangeNormilizationFactor)
	ON_EN_CHANGE(IDC_EDIT_CELL, &Custom2FilterDlg::OnEnChangeEditCell)
	ON_EN_SETFOCUS(IDC_EDIT_CELL, &Custom2FilterDlg::OnEnSetfocusEditCell)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, &Custom2FilterDlg::OnGridSelChanged)
	ON_BN_CLICKED(IDC_IMPORT_FILTER, &Custom2FilterDlg::OnBnClickedImportFilter)
	ON_BN_CLICKED(IDC_EXPORT_FILTER, &Custom2FilterDlg::OnBnClickedExportFilter)
	ON_BN_CLICKED(IDOK, &Custom2FilterDlg::OnBnMfcStandardOk)
	ON_BN_CLICKED(ID_OK, &Custom2FilterDlg::OnBnClickedOk)
END_MESSAGE_MAP()

#pragma region Constructor
Custom2FilterDlg::Custom2FilterDlg( CWnd* pParent )
	: CDialog( Custom2FilterDlg::IDD, pParent )
	, m_KernelWidth( 0 )
	, m_KernelHeight( 0 )
	, m_KernelSum( 0 )
	, m_NormalizationFactor( 1 )
	, m_NormilizationResult( _T("") )
	, m_GridEditMode( false )
	, m_AbsoluteValue( FALSE )
	, m_ClippingEnabled( FALSE )
	, m_EditCell( _T("") )
	, m_GridStatus( _T("") )
{
}

Custom2FilterDlg::~Custom2FilterDlg()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT Custom2FilterDlg::SetInspectionData()
{
	HRESULT Result = S_OK;

	Custom2Filter *pCustom2Filter = getCustom2Filter();

	if( pCustom2Filter != NULL )
	{
		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getKernelWidth(), m_KernelWidth );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getKernelHeight(), m_KernelHeight );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getNormalizationFactor(), m_NormalizationFactor );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getAbsoluteValue(), m_AbsoluteValue );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getClippingEnabled(),  m_ClippingEnabled );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequest( &pCustom2Filter->getKernelArray(), m_KernelArray.begin(), m_KernelArray.end() );
		}

		if( Result == S_OK )
		{
			Result = AddInputRequestMarker();
		}

		if( Result == S_OK )
		{
			Result = RunOnce( pCustom2Filter->GetTool() );
		}
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void Custom2FilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_KERNELHEIGHT, m_HeightCtrl);
	DDX_Control(pDX, IDC_KERNELWIDTH, m_WidthCtrl);
	DDX_Control(pDX, IDC_SUM_SELECTOR, m_SumSelectorCtrl);
	DDX_Text(pDX, IDC_KERNEL_SUM, m_KernelSum);
	DDX_Text(pDX, IDC_NORM_FACTOR, m_NormalizationFactor);
	DDX_Text(pDX, IDC_NORM_RESULT, m_NormilizationResult);
	DDX_Check(pDX, IDC_ABSOLUTE_VALUE, m_AbsoluteValue);
	DDX_Check(pDX, IDC_CLIPPING_ON, m_ClippingEnabled);
	DDX_Text(pDX, IDC_EDIT_CELL, m_EditCell);
	DDX_Text(pDX, IDC_GRID_STATUS, m_GridStatus);
}

BOOL Custom2FilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initializeFilter();

	SVString Entry;
	for( int i=1; i <= Custom2Filter::MaxKernelSize; i++ )
	{
		//Check that its an odd value
		if( 1 == (i % 2) )
		{
			Entry.Format(_T("%d"), i);
			m_WidthCtrl.AddString( Entry.c_str() );
			m_HeightCtrl.AddString( Entry.c_str() );
		}
	}

	for( int i=0; i < LastSumType; i++ )
	{
		m_SumSelectorCtrl.AddString( SumSelections[i] );
	}

	m_Grid.SetFixedRowCount( HeaderSize );
	m_Grid.SetFixedColumnCount( HeaderSize );
	m_Grid.SetRowResize( FALSE );
	m_Grid.SetColumnResize( FALSE );
	m_Grid.AllowReorderColumn( false );
	m_Grid.EnableDragAndDrop( FALSE );
	m_Grid.SetEditable( FALSE );

	int CurrentSelection = 0;
	if( CurrentSelection < m_SumSelectorCtrl.GetCount() )
	{
		m_SumSelectorCtrl.SetCurSel( CurrentSelection );
	}

	recalculateKernel( m_KernelWidth,  m_KernelHeight );

	UpdateData( FALSE );
	return TRUE;
}

BOOL Custom2FilterDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL Result( FALSE );

	if( doesControlHaveFocus( IDC_GRID ) )
	{
		bool isNumeric = '0' <= pMsg->wParam && '9' >= pMsg->wParam;
		//Minus sign only when it is the first character
		if( !m_GridEditMode && '-' == pMsg->wParam )
		{
			isNumeric = true;
		}

		if(	WM_CHAR == pMsg->message )
		{
			if( isNumeric )
			{
				if( !m_GridEditMode )
				{
					m_EditCell.Empty();
					m_GridEditMode = true;
				}

				int MaxCellCharacters( MaxCellCharSize );
				//Check if cell value not to large
				if( !m_EditCell.IsEmpty() && '-' == m_EditCell[0] )
				{
					MaxCellCharacters++;
				}

				if( m_EditCell.GetLength() < MaxCellCharacters )
				{
					m_EditCell += static_cast<TCHAR> (pMsg->wParam);
					updateGridSelection( m_EditCell );
					UpdateData( FALSE );
					Result = TRUE;
				}
				if( m_EditCell.GetLength() >= MaxCellCharacters )
				{
					m_GridEditMode = false;
				}
			}
			else
			{
				m_GridEditMode = false;
				checkCellsValid();
			}
		}
		else if( WM_KEYDOWN == pMsg->message )
		{
			bool hasPositionChanged = VK_PRIOR <= pMsg->wParam && VK_DOWN >= pMsg->wParam || VK_RETURN == pMsg->wParam;
			if( hasPositionChanged )
			{
				m_GridEditMode = false;
				checkCellsValid();
			}

			if( VK_TAB == pMsg->wParam )
			{
				bool ShiftKeyPressed( false );
				m_GridEditMode = false;
				checkCellsValid();

				ShiftKeyPressed = 0 != HIBYTE( GetKeyState( VK_SHIFT ) );
				SvGcl::CCellID Cell = m_Grid.GetFocusCell();
				if( m_KernelHeight == Cell.row && m_KernelWidth == Cell.col && !ShiftKeyPressed)
				{
					GetDlgItem( IDC_IMPORT_FILTER )->SetFocus();
					Result = TRUE;
				}
				else if( 1 == Cell.row && 1 == Cell.col && ShiftKeyPressed)
				{
					GetDlgItem( IDC_CLIPPING_ON )->SetFocus();
					Result = TRUE;
				}
			}
		}
	}

	if( !Result )
	{
		Result = CDialog::PreTranslateMessage(pMsg);
	}

	return Result;
}

void Custom2FilterDlg::OnCbnSelchangeKernelWidth()
{
	recalculateKernel( m_WidthCtrl.GetCurSel() * 2 + 1, -1 );
	UpdateData( FALSE );
}

void Custom2FilterDlg::OnCbnSelchangeKernelHeight()
{
	recalculateKernel( -1, m_HeightCtrl.GetCurSel() * 2 + 1 );
	UpdateData( FALSE );
}

void Custom2FilterDlg::OnCbnSelchangeSumSelector()
{
	addKernelSum();
	UpdateData( FALSE );
}

void Custom2FilterDlg::OnBnClickedApplySum()
{
	UpdateData( TRUE );
	if( 0 == m_KernelSum )
	{
		AfxMessageBox( DataInvalidNormalizationFactor );
	}
	else
	{
		//Only apply the absolute value for the Normalization Factor
		m_NormalizationFactor = ::abs( m_KernelSum );
		calculateNormalizationResult();
		UpdateData( FALSE );
	}
}

void Custom2FilterDlg::OnEnChangeNormilizationFactor()
{
	UpdateData( TRUE );
	if( 0 == m_NormalizationFactor )
	{
		AfxMessageBox( DataInvalidNormalizationFactor );
		m_NormalizationFactor = 1;
	}
	calculateNormalizationResult();
	UpdateData( FALSE );
}

void Custom2FilterDlg::OnEnChangeEditCell()
{
	SVString EditCell( m_EditCell );

	UpdateData( TRUE );
	
	int MaxCellCharacters( MaxCellCharSize );
	
	//Check if cell value not to large
	if( !m_EditCell.IsEmpty() && '-' == m_EditCell[0] )
	{
		MaxCellCharacters++;
	}

	if( m_EditCell.GetLength() <= MaxCellCharacters )
	{
		updateGridSelection( m_EditCell );
	}
	else
	{
		m_EditCell = EditCell.c_str();
	}

	UpdateData( FALSE );
}

void Custom2FilterDlg::OnEnSetfocusEditCell()
{
	//Only when focus received via Tab key not mouse
	if( 0 == HIBYTE( GetKeyState( VK_LBUTTON ) ) )
	{
		SvGcl::CCellID CellFocus;
		SvGcl::CCellRange CellSelection;
		//Instead of the edit cell we want the first or last cell of the grid to have the selection
		if( 0 == HIBYTE( GetKeyState( VK_SHIFT ) ) )
		{
			CellFocus.row = 1;
			CellFocus.col = 1;
			CellSelection.SetMinRow( 1 );
			CellSelection.SetMaxRow( 1 );
			CellSelection.SetMinCol( 1 );
			CellSelection.SetMaxCol( 1 );
		}
		else
		{
			CellFocus.row = m_KernelHeight;
			CellFocus.col = m_KernelWidth;
			CellSelection.SetMinRow( m_KernelHeight );
			CellSelection.SetMaxRow( m_KernelHeight );
			CellSelection.SetMinCol( m_KernelWidth );
			CellSelection.SetMaxCol( m_KernelWidth );
		}
		m_Grid.SetFocus();
		m_Grid.SetFocusCell( CellFocus );
		m_Grid.SetSelectedRange( CellSelection );
	}
}

void Custom2FilterDlg::OnGridSelChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	updateEditCellandStatus();
	m_GridEditMode = false;
	UpdateData( FALSE );
}

void Custom2FilterDlg::OnBnClickedImportFilter()
{
	bool FullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	SVFileDialog FileDlg( TRUE, FullAccess, CustomFilterExportFileExt, _T(""), dwFlags, CustomFilterExportFileFilters, this );

	if( FileDlg.DoModal() == IDOK )
	{
		SVString Message;
		
		try
		{
			SVMaterialsTree LocalTree;
			SVXMLMaterialsTree Tree( LocalTree );
			SVXMLClass Xml;

			::CoInitialize(NULL);
			HRESULT Result = Xml.Initialize( 0, 0 );
			if( S_OK == Result )
			{
				_bstr_t RevisionHistory;
				_bstr_t FilePath( FileDlg.GetPathName() );

				Xml.PreserveWhitespace( true );

				Result = Xml.CopyXMLFileToDOM( FilePath, RevisionHistory.GetAddress() );
				if ( S_OK == Result )
				{
					Result = SVXML2TreeConverter::CopyToTree( Xml, Tree, L"Base", false );

					if ( S_OK == Result )
					{
						SVXMLMaterialsTree::SVBranchHandle Branch;
						if ( SVNavigateTreeClass::GetItemBranch( Tree, CTAG_ENVIRONMENT, NULL, Branch ) )
						{
							_variant_t Value;
							//At this moment in time we just check that the version number tag is in the file but do not worry which version saved the file
							if ( SVNavigateTreeClass::GetItem( Tree, CTAG_VERSION_NUMBER, Branch, Value ) )
							{
								if( readCustom2FilterBranch( Tree, Message ) )
								{
									recalculateKernel( m_KernelWidth, m_KernelHeight );
									UpdateData( FALSE );
								}
								else
								{
									if( Message.empty() )
									{
										Message.Format( ImportFailed, FileDlg.GetPathName() );
										Message += XmlFormatInvalid;
									}
								}
							}
							else
							{
								Message.Format( ImportFailed, FileDlg.GetPathName() );
								Message += VersionMismatch;
							}
						}
						else
						{
							Message.Format( ImportFailed, FileDlg.GetPathName() );
							Message += XmlFormatInvalid;
						}
					}
				}
			}
			::CoUninitialize();
			if( S_OK != Result )
			{
				Message.Format( ImportFailed, FileDlg.GetPathName() );
				Message += XmlFormatInvalid;
			}
		}
		catch ( ... )
		{
			Message.Format( ImportFailed, FileDlg.GetPathName() );
		}
		if( !Message.empty() )
		{
			AfxMessageBox( Message.c_str() );
		}
	}
}

void Custom2FilterDlg::OnBnClickedExportFilter()
{
	UpdateData( TRUE );
	
	bool FullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	SVFileDialog FileDlg(FALSE, FullAccess, CustomFilterExportFileExt, _T(""), dwFlags, CustomFilterExportFileFilters, this );
	if( FileDlg.DoModal() == IDOK )
	{
		try
		{

			std::ofstream FileStream;
			FileStream.open( FileDlg.GetPathName(), std::ofstream::binary );
			if( FileStream.is_open() )
			{
				SVObjectXMLWriter XmlWriter( FileStream );

				SVString rootNodeName( _T( "Custom2_Filter_Export") );
				XmlWriter.WriteRootElement( rootNodeName );
				XmlWriter.WriteSchema();

				_variant_t xmlnsValue;
				_variant_t Value;

				xmlnsValue.SetString( "x-schema:#SVR00001" );

				Value.SetString( "SV_BASENODE" );

				XmlWriter.StartElement( _T("Base") );
				XmlWriter.ElementAttribute( _T("xmlns"), xmlnsValue );
				XmlWriter.ElementAttribute( _T("Type"), Value );

				Value.Clear();
				Value = TheSVObserverApp.getCurrentVersion();

				XmlWriter.StartElement( CTAG_ENVIRONMENT  );
				XmlWriter.WriteAttribute( CTAG_VERSION_NUMBER, Value );
				XmlWriter.EndElement();
				CString Label;
				Label.LoadString( IDS_CLASSNAME_CUSTOM2FILTER );
				XmlWriter.StartElement( SVString( Label ) );

				writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH, m_KernelWidth );
				writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT, m_KernelHeight );
				writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM, m_NormalizationFactor );
				writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE, m_AbsoluteValue );
				writeElement( XmlWriter, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING, m_ClippingEnabled );

				Label.LoadString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
				XmlWriter.StartElement( SVString( Label ) );
				SVVariantList KernelArray;
				LongArray::iterator Iter( m_KernelArray.begin() );
				while( m_KernelArray.end() != Iter )
				{
					KernelArray.push_back( *Iter );
					++Iter;
				}
				XmlWriter.WriteAttribute( _T("Element"), KernelArray );

				XmlWriter.EndElement();// end of Kernel
				XmlWriter.EndElement();// end of Custom2 Filter

				XmlWriter.EndElement(); // end of BaseNode
				XmlWriter.EndElement(); // end of Root Element

				FileStream.close();
			}
		}
		catch( ... )
		{
			AfxMessageBox( ExportFailed );
		}
	}
}

void Custom2FilterDlg::OnBnClickedOk()
{
	UpdateData( TRUE );

	SetInspectionData();

	CDialog::OnOK();
}

void Custom2FilterDlg::initializeFilter()
{
	Custom2Filter *pFilter = getCustom2Filter();

	if( nullptr != pFilter )
	{
		pFilter->getKernelWidth().GetValue( m_KernelWidth );
		pFilter->getKernelHeight().GetValue( m_KernelHeight );
		pFilter->getNormalizationFactor().GetValue( m_NormalizationFactor );
		pFilter->getAbsoluteValue().GetValue( m_AbsoluteValue );
		pFilter->getClippingEnabled().GetValue( m_ClippingEnabled );

		m_KernelArray.clear();
		pFilter->getKernelArray().GetValues( m_KernelArray );
	}
}

Custom2Filter* Custom2FilterDlg::getCustom2Filter()
{
	return dynamic_cast<Custom2Filter*>( GetTaskObject() );
}

void Custom2FilterDlg::recalculateKernel( int Width, int Height )
{
	initializeKernel( Width, Height );

	GV_ITEM Item;

	int MiddleRow = (m_KernelHeight + 1) / 2;
	int MiddleCol = (m_KernelWidth + 1) / 2;

	for( int i=0; i <= m_KernelHeight; i++ )
	{
		for( int j=0; j <= m_KernelWidth; j++ )
		{
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
			Item.row = i;
			Item.col = j;
			Item.crBkClr = CLR_DEFAULT;


			if( 0 == i  && 0 == j )
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
				Item.strText = HeaderCell;
			}
			else if( 0 == i )
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format( _T("%d"), j );
			}
			else if ( 0 == j )
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
				Item.strText.Format( _T("%d"), i );
			}
			else 
			{
				//Check if cells are in the middle row and column of the kernel
				if( MiddleRow  == i || MiddleCol == j )
				{
					Item.crBkClr = KERNEL_CENTER_COLOR;
				}
				int ValueIndex = (i - 1)*m_KernelWidth + (j - 1);
				long Value( 1 );
				LongArray::iterator Iter = m_KernelArray.begin() + ValueIndex;
				if( Iter != m_KernelArray.end() )
				{
					Value = *Iter;
				}

				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format( _T("%d"), Value);
			}

			m_Grid.SetItem( &Item );
		}
	}
	addKernelSum();
	updateEditCellandStatus();
}

void Custom2FilterDlg::initializeKernel( int Width, int Height )
{
	long SourceKernelWidth( m_KernelWidth);
	long SourceKernelHeight( m_KernelHeight);

	if( 0 == Width )
	{
		m_KernelWidth = Custom2Filter::StandardKernelSize;
	}
	else if( -1 != Width )
	{
		m_KernelWidth = Width;
	}

	if(0 == Height)
	{
		m_KernelHeight = Custom2Filter::StandardKernelSize;
	}
	else if( -1 != Height )
	{
		m_KernelHeight = Height;
	}

	m_Grid.SetColumnCount( HeaderSize +  m_KernelWidth );
	for( int i=0; i < HeaderSize + m_KernelWidth; i++ )
	{
		if( 0 == i )
		{
			m_Grid.SetColumnWidth( i, HeaderColumnSize );
		}
		else
		{
			m_Grid.SetColumnWidth( i, CellSize );
		}
	}

	m_Grid.SetRowCount( HeaderSize + m_KernelHeight );
	m_Grid.SetRowHeight( 0, CellSize );

	//Deselect when changing size etc..
	SvGcl::CCellRange CellSelection;
	m_Grid.SetSelectedRange( CellSelection );
	SvGcl::CCellID CellFocus;
	m_Grid.SetFocusCell( CellFocus );

	int CurrentSelection = (m_KernelWidth - 1) / 2;
	if( m_WidthCtrl.GetCurSel() != CurrentSelection && CurrentSelection < m_WidthCtrl.GetCount() )
	{
		m_WidthCtrl.SetCurSel( CurrentSelection );
	}
	CurrentSelection = (m_KernelHeight - 1) / 2;
	if( m_HeightCtrl.GetCurSel() != CurrentSelection && CurrentSelection < m_HeightCtrl.GetCount() )
	{
		m_HeightCtrl.SetCurSel( CurrentSelection );
	}

	copyKernel( SourceKernelWidth, SourceKernelHeight );
}

void Custom2FilterDlg::copyKernel( long SourceKernelWidth, long SourceKernelHeight )
{
	//Copy kernel only if the source and destination are different in size
	if( m_KernelWidth != SourceKernelWidth || m_KernelHeight != SourceKernelHeight)
	{
		LongArray KernelArrayCopy;
		KernelArrayCopy = m_KernelArray;

		m_KernelArray.clear();
		m_KernelArray.assign( m_KernelWidth*m_KernelHeight, 1);

		if( m_KernelArray.size() > KernelArrayCopy.size() )
		{
			long OffsetWidth = (m_KernelWidth - SourceKernelWidth) / 2;
			long OffsetHeight = (m_KernelHeight - SourceKernelHeight) / 2;
			for( int i=0; i < SourceKernelHeight; i++ )
			{
				long CopyIndex = (i + OffsetHeight)*m_KernelWidth + OffsetWidth;
				LongArray::iterator IterKernel = m_KernelArray.begin() + CopyIndex;
				if( IterKernel != m_KernelArray.end() && IterKernel + SourceKernelWidth - 1 != m_KernelArray.end() )
				{
					LongArray::iterator IterCopy = KernelArrayCopy.begin() + i*SourceKernelWidth;
					if( IterCopy != KernelArrayCopy.end() && IterCopy + SourceKernelWidth - 1 != KernelArrayCopy.end() )
					{
						std::copy( IterCopy, IterCopy + SourceKernelWidth, IterKernel );
					}
				}
			}
		}
		else
		{
			long OffsetWidth = (SourceKernelWidth - m_KernelWidth) / 2;
			long OffsetHeight = (SourceKernelHeight - m_KernelHeight) / 2;
			for( int i=0; i < m_KernelHeight; i++ )
			{
				long CopyIndex = i*m_KernelWidth;
				LongArray::iterator IterKernel = m_KernelArray.begin() + CopyIndex;
				if( IterKernel != m_KernelArray.end() && IterKernel + m_KernelWidth - 1 != m_KernelArray.end() )
				{
					LongArray::iterator IterCopy = KernelArrayCopy.begin() + (i + OffsetHeight)*SourceKernelWidth + OffsetWidth;
					if( IterCopy != KernelArrayCopy.end() && IterCopy + OffsetWidth - 1 != KernelArrayCopy.end() )
					{
						std::copy( IterCopy, IterCopy + m_KernelWidth, IterKernel );
					}
				}
			}
		}
	}
}

void Custom2FilterDlg::addKernelSum()
{
	m_KernelSum = 0;
	SumType CurrentSum = static_cast<SumType> (m_SumSelectorCtrl.GetCurSel());
	for( int i=HeaderSize; i <= m_Grid.GetRowCount() ; i++ )
	{
		for( int j=HeaderSize; j <= m_Grid.GetColumnCount(); j++ )
		{
			long CellValue = ::atol( m_Grid.GetItemText( i, j ) );
			switch( CurrentSum )
			{
			case TotalSum:
				{
					m_KernelSum += CellValue;
				}
				break;
			case PositiveSum:
				{
					if( CellValue > 0 )
					{
						m_KernelSum += CellValue;
					}
				}
				break;
			case NegativeSum:
				{
					if( CellValue < 0 )
					{
						m_KernelSum += CellValue;
					}
				}
				break;
			case AbsoluteSum:
				{
					m_KernelSum += ::abs( CellValue );
				}
				break;
			}
		}
	}
	//When sum has changed recalculate the normalization result
	calculateNormalizationResult();
}

void Custom2FilterDlg::calculateNormalizationResult()
{
	if( 0 != m_NormalizationFactor )
	{
		double NormilizationResult = static_cast<double> (m_KernelSum) / static_cast<double> (m_NormalizationFactor);
		m_NormilizationResult.Format( _T("%0.2f"), NormilizationResult );
	}
}

void Custom2FilterDlg::updateGridSelection( const CString& rValue )
{
	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	for( int i = Selection.GetMinRow(); i <= Selection.GetMaxRow(); i++ )
	{
		for( int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++ )
		{
			if(m_Grid.IsCellSelected( i, j ) )
			{
				m_Grid.SetItemText( i, j, rValue );
				int EditIndex = (i - 1)*m_KernelWidth + (j - 1);
				LongArray::iterator Iter = m_KernelArray.begin() + EditIndex;
				if( Iter != m_KernelArray.end() )
				{
					*Iter = atol( rValue );
				}
			}
		}
	}
	m_Grid.Refresh();

	addKernelSum();
	updateEditCellandStatus();
}

void Custom2FilterDlg::updateEditCellandStatus()
{
	CString MarkedCellsValue;
	int Row = 0;
	int Col = 0;
	int CellCount = 0;
	long MarkedSum = 0;


	SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
	for( int i = Selection.GetMinRow(); i <= Selection.GetMaxRow(); i++ )
	{
		for( int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++ )
		{
			if( m_Grid.IsCellSelected( i, j ) )
			{
				CString Value;
				Value = m_Grid.GetItemText( i, j );

				if( Value != MarkedCellsValue )
				{
					if( MarkedCellsValue.IsEmpty() )
					{
						MarkedCellsValue = Value;
					}
					else
					{
						MarkedCellsValue = NoCommonValue;
					}
				}

				if( CellCount == 0 )
				{
					Row = i;
					Col = j;
				}
				MarkedSum += atol( Value );
				CellCount++;
			}
		}
	}
	if( 0 == CellCount )
	{
		m_GridStatus = _T("");
	}
	else if( 1 == CellCount )
	{
		m_GridStatus.Format( StatusGridSingleCell, Row, Col, (Row - 1)*m_KernelWidth + Col );
	}
	else
	{
		m_GridStatus.Format( StatusGridMultiCell, CellCount, MarkedSum );
	}

	m_EditCell = MarkedCellsValue;
}

void Custom2FilterDlg::checkCellsValid()
{
	if( !m_GridEditMode )
	{
		for( int i = 1; i <= m_KernelHeight; i++ )
		{
			for( int j = 1; j <= m_KernelWidth; j++ )
			{
				CString Value;
				Value = m_Grid.GetItemText( i, j );
				//Check if during input only a minus was entered (invalid) set it to 0
				if( _T("-") == Value )
				{
					Value = _T("0");
					m_Grid.SetItemText( i, j, Value );
				}
			}
		}
	}
}

bool Custom2FilterDlg::readCustom2FilterBranch( SVXMLMaterialsTree& rTree, SVString& rMessage )
{
	SVXMLMaterialsTree::SVBranchHandle Branch;
	CString Label;
	bool Result(false);

	Label.LoadString( IDS_CLASSNAME_CUSTOM2FILTER );
	if ( SVNavigateTreeClass::GetItemBranch( rTree, Label, NULL, Branch ) )
	{
		//Save current values in case of a restore
		long KernelWidth( m_KernelWidth );
		long KernelHeight( m_KernelHeight );
		long NormalizationFactor( m_NormalizationFactor );
		BOOL AbsoluteValue( m_AbsoluteValue );
		BOOL ClippingEnabled( m_ClippingEnabled );
		LongArray KernelArray;
		_variant_t Value;

		m_KernelWidth = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELWIDTH );
		m_KernelHeight = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_KERNELHEIGHT );
		m_NormalizationFactor = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_TRANSFORM );
		m_AbsoluteValue = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_ABSOLUTE );
		m_ClippingEnabled = readElement( rTree, Branch, IDS_OBJECTNAME_CUSTOMFILTER_CLIPPING );

		Label.LoadString( IDS_OBJECTNAME_CUSTOMFILTER_KERNELCELL );
		SVTreeType::SVBranchHandle Elements = NULL;
		if ( SVNavigateTreeClass::GetItemBranch( rTree, Label, Branch, Elements ) )
		{
			SVTreeType::SVLeafHandle Leaf;

			rTree.GetFirstLeaf( Elements, Leaf );

			KernelArray.clear();
			m_KernelArray.swap( KernelArray );

			while ( S_OK == rTree.IsValidLeaf( Elements, Leaf ) )
			{
				rTree.GetLeafData( Leaf, Value );
				m_KernelArray.push_back( Value );
				rTree.GetNextLeaf( Elements, Leaf );
			}
		}
		SVString DataInvalidMessage;

		//This checks that the most important values of the kernel are valid
		if( isDataValid( DataInvalidMessage ) )
		{
			Result = true;
		}
		else
		{
			rMessage =  ImportFailedDataInvalid;
			rMessage += DataInvalidMessage;

			//Need to restore the previous values before the import
			m_KernelWidth = KernelWidth;
			m_KernelHeight = KernelHeight;
			m_NormalizationFactor = NormalizationFactor;
			m_AbsoluteValue = AbsoluteValue;
			m_ClippingEnabled = ClippingEnabled;
			m_KernelArray.swap( KernelArray );
		}
	}

	return Result;
}


variant_t Custom2FilterDlg::readElement( SVXMLMaterialsTree& rTree, const SVXMLMaterialsTree::SVBranchHandle& rBranch, UINT ResourceID ) const
{
	variant_t Value;

	CString Label;
	Label.LoadString( ResourceID );
	if( !SVNavigateTreeClass::GetItem( rTree, Label, rBranch, Value ) )
	{
		Value.Clear();
	}
	return Value;
}

void Custom2FilterDlg::writeElement( SVObjectXMLWriter& rXmlWriter, UINT ResourceID, variant_t Value )
{
	CString Label;
	Label.LoadString( ResourceID );
	rXmlWriter.WriteAttribute( SVString( Label ), Value);
}

bool Custom2FilterDlg::isDataValid( SVString& rMessage ) const
{
	bool Result( true );

	//Kernel array and kernel height and width must match
	if( m_KernelArray.size() != m_KernelWidth*m_KernelHeight )
	{
		rMessage.Format(DataInvalidKernelSize, m_KernelArray.size(), m_KernelWidth, m_KernelHeight);
		Result = false;
	}

	//Check that the Kernel Width and Height are odd and between 1 and MaxKernelSize
	if( 1 != m_KernelWidth % 2 || 1 > m_KernelWidth || Custom2Filter::MaxKernelSize < m_KernelWidth )
	{
		rMessage.Format( DataInvalidKernelWidth, m_KernelWidth,  Custom2Filter::MaxKernelSize);
		Result = false;
	}
	if( 1 != m_KernelHeight % 2 || 1 > m_KernelHeight || Custom2Filter::MaxKernelSize < m_KernelHeight )
	{
		rMessage.Format( DataInvalidKernelHeight, m_KernelHeight,  Custom2Filter::MaxKernelSize);
		Result = false;
	}
	//Normalization Factor is not allowed to be 0 or negative
	if( 0 >= m_NormalizationFactor )
	{
		rMessage = DataInvalidNormalizationFactor;
		Result = false;
	}

	return Result;
}

bool Custom2FilterDlg::doesControlHaveFocus( UINT ControlID ) const
{
	bool Result( false );

	CWnd* pFocusWnd =  GetFocus();
	if( nullptr != pFocusWnd && ::IsWindow( pFocusWnd->m_hWnd ) )
	{
		Result = ControlID == GetFocus()->GetDlgCtrlID();
	}
	return Result;
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Custom2FilterDlg.cpp_v  $
 * 
 *    Rev 1.1   05 Nov 2014 05:05:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Bugfix: Cell input problem
 * Kernel update problem in Tool Adjust dialog
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Oct 2014 11:15:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */