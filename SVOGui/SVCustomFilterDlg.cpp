//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilterDlg
//* .File Name       : $Workfile:   SVCustomFilterDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:04:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVCustomFilterDlg.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr char* WidthTag = "Width";
	constexpr char* HeidghtTag = "Height";
	constexpr char* ClippingTag = "IsClipping";
	constexpr char* AbsoluteValueTag = "AbsoluteValue";
	constexpr char* TransformationTag = "TransformationFactor";
	constexpr char* CellValueTag = "CellValue";

	SVCustomFilterDlg::SVCustomFilterDlg(uint32_t inspectionId, uint32_t filterId, CWnd* pParent) :
		CDialog(SVCustomFilterDlg::IDD, pParent)
		, m_filterID(filterId)
		, m_InspectionID(inspectionId)
		, m_values{ SvOg::BoundValues{ inspectionId, filterId } }
	{
		//{{AFX_DATA_INIT(SVCustomFilterDlg)
		m_bAbsoluteValue = TRUE;
		m_bClippingOn = TRUE;
		m_lTransformationFactor = 1;
		m_lKernelWidth = 3;
		m_lKernelHeight = 3;
		//}}AFX_DATA_INIT
		for( long l = 0; l < 49; l++ )
		{
			m_lKernelCells[l] = 0;
		}// end for
	}

	SVCustomFilterDlg::~SVCustomFilterDlg()
	{
	}

	HRESULT SVCustomFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		m_values.Set<long>(SvPb::FilterKernelWidthEId, m_lKernelWidth);
		m_values.Set<long>(SvPb::FilterKernelHeightEId, m_lKernelHeight);
		m_values.Set<bool>(SvPb::CustomFilterClippingEId, m_bClippingOn ? true : false);
		m_values.Set<bool>(SvPb::CustomFilterAbsoluteEId, m_bAbsoluteValue ? true : false);
		m_values.Set<long>(SvPb::CustomFilterTransformEId, m_lTransformationFactor);

		for( long j = 0; S_OK == l_hrOk && j < m_lKernelWidth; j++ )
		{
			for (long k = 0; S_OK == l_hrOk && k < m_lKernelHeight; k++)
			{
				long l_lKernelIndex = k * m_lKernelWidth + j;
				long l_lDialogIndex = (k + ((7 - m_lKernelHeight) / 2)) * 7 + (j + ((7 - m_lKernelWidth) / 2));

				m_values.Set<long>(SvPb::FilterCellEId + l_lKernelIndex, m_lKernelCells[l_lDialogIndex]);
			}// end for

		}// end for

		m_values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVCustomFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVCustomFilterDlg)
		DDX_Control(pDX, IDC_KERNELWIDTH, m_ctlKernelWidth);
		DDX_Control(pDX, IDC_KERNELHEIGHT, m_ctlKernelHeight);
		DDX_Check(pDX, IDC_ABSOLUTE_VALUE, m_bAbsoluteValue);
		DDX_Check(pDX, IDC_CLIPPING_ON, m_bClippingOn);
		DDX_Text(pDX, IDC_TRANSFORMATION_FACTOR, m_lTransformationFactor);
		DDX_Text(pDX, IDC_KERNELWIDTH, m_lKernelWidth);
		DDX_Text(pDX, IDC_KERNELHEIGHT, m_lKernelHeight);
		DDV_MinMaxLong(pDX, m_lTransformationFactor, 1, 2401);
		//}}AFX_DATA_MAP
		for( long l = 0; l < 49; l++ )
		{
			DDX_Text(pDX, IDC_CELL1 + l, m_lKernelCells[l]);
			DDV_MinMaxLong(pDX, m_lKernelCells[l], -49, 49);
		}// end for
	}


	BEGIN_MESSAGE_MAP(SVCustomFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVCustomFilterDlg)
		ON_LBN_SELCHANGE(IDC_KERNELWIDTH, OnSelchangeKernelWidth)
		ON_LBN_SELCHANGE(IDC_KERNELHEIGHT, OnSelchangeKernelHeight)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVCustomFilterDlg message handlers

	BOOL SVCustomFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		std::string Temp;

		m_values.Init();
		m_lKernelWidth = m_values.Get<long>(SvPb::FilterKernelWidthEId);
		Temp = SvUl::Format( "%d", m_lKernelWidth );
		m_ctlKernelWidth.SelectString( -1, Temp.c_str() );

		m_lKernelHeight = m_values.Get<long>(SvPb::FilterKernelHeightEId);
		Temp = SvUl::Format( "%d", m_lKernelHeight );
		m_ctlKernelHeight.SelectString( -1, Temp.c_str() );

		EnableCells();

		m_bClippingOn = m_values.Get<bool>(SvPb::CustomFilterClippingEId);
		m_bAbsoluteValue = m_values.Get<bool>(SvPb::CustomFilterAbsoluteEId);
		m_lTransformationFactor = m_values.Get<long>(SvPb::CustomFilterTransformEId);

		for( long h = 0; h < m_lKernelWidth; h++ )
		{
			for( long i = 0; i < m_lKernelHeight; i++ )
			{
				long l_lKernelIndex = i * m_lKernelWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lKernelHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lKernelWidth ) / 2 ) );

				m_lKernelCells[ l_lDialogIndex ] = m_values.Get<long>(SvPb::FilterCellEId + l_lKernelIndex);
			}// end for
		}// end for

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVCustomFilterDlg::OnSelchangeKernelWidth() 
	{
		long lIndex = m_ctlKernelWidth.GetCurSel();

		if( LB_ERR != lIndex )
		{
			UpdateData();
			EnableCells();
		}// end if

	}// end OnSelchangeKernelWidth

	void SVCustomFilterDlg::OnSelchangeKernelHeight() 
	{
		long lIndex = m_ctlKernelHeight.GetCurSel();

		if( LB_ERR != lIndex )
		{
			UpdateData();
			EnableCells();
		}// end if

	}// end OnSelchangeKernelHeight

	void SVCustomFilterDlg::EnableCells()
	{
		for( long j = 0; j < 7; j++ )
		{
			for( long k = 0; k < 7; k++ )
			{
				if( j - ( ( 7 - m_lKernelWidth  ) / 2 ) < m_lKernelWidth  && j - ( ( 7 - m_lKernelWidth  ) / 2 ) >= 0 &&
					k - ( ( 7 - m_lKernelHeight ) / 2 ) < m_lKernelHeight && k - ( ( 7 - m_lKernelHeight ) / 2 ) >= 0 )

				{
					std::string Name;
					Name = SvUl::Format( _T( "Cell %d" ), ( k - ( ( 7 - m_lKernelHeight ) / 2 ) ) * m_lKernelWidth + ( j - ( ( 7 - m_lKernelWidth  ) / 2 ) ) + 1 );
					GetDlgItem( IDC_STATIC_CELL1 + k * 7 + j )->SetWindowText( Name.c_str() );
					GetDlgItem( IDC_CELL1 + k * 7 + j )->EnableWindow( TRUE );
				}// end if
				else
				{
					GetDlgItem( IDC_STATIC_CELL1 + k * 7 + j )->SetWindowText( _T( "N/A" ) );
					GetDlgItem( IDC_CELL1 + k * 7 + j )->EnableWindow( FALSE );
				}// end else

			}// end for

		}// end for

	}// end EnableCells

	void SVCustomFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	void SVCustomFilterDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}
}  //namespace SvOg

