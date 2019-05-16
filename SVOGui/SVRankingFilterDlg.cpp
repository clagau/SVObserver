//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilterDlg
//* .File Name       : $Workfile:   SVRankingFilterDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:12:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "SVRankingFilterDlg.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	static const std::vector<GUID> CellValueGuidList
	{
		SVRankingFilterCell01Guid, SVRankingFilterCell02Guid, SVRankingFilterCell03Guid, SVRankingFilterCell04Guid, SVRankingFilterCell05Guid,
		SVRankingFilterCell06Guid, SVRankingFilterCell07Guid, SVRankingFilterCell08Guid, SVRankingFilterCell09Guid, SVRankingFilterCell10Guid,
		SVRankingFilterCell11Guid, SVRankingFilterCell12Guid, SVRankingFilterCell13Guid, SVRankingFilterCell14Guid, SVRankingFilterCell15Guid,
		SVRankingFilterCell16Guid, SVRankingFilterCell17Guid, SVRankingFilterCell18Guid, SVRankingFilterCell19Guid, SVRankingFilterCell20Guid,
		SVRankingFilterCell21Guid, SVRankingFilterCell22Guid, SVRankingFilterCell23Guid, SVRankingFilterCell24Guid, SVRankingFilterCell25Guid,
		SVRankingFilterCell26Guid, SVRankingFilterCell27Guid, SVRankingFilterCell28Guid, SVRankingFilterCell29Guid, SVRankingFilterCell30Guid,
		SVRankingFilterCell31Guid, SVRankingFilterCell32Guid, SVRankingFilterCell33Guid, SVRankingFilterCell34Guid, SVRankingFilterCell35Guid,
		SVRankingFilterCell36Guid, SVRankingFilterCell37Guid, SVRankingFilterCell38Guid, SVRankingFilterCell39Guid, SVRankingFilterCell40Guid,
		SVRankingFilterCell41Guid, SVRankingFilterCell42Guid, SVRankingFilterCell43Guid, SVRankingFilterCell44Guid, SVRankingFilterCell45Guid,
		SVRankingFilterCell46Guid, SVRankingFilterCell47Guid, SVRankingFilterCell48Guid, SVRankingFilterCell49Guid
	};

	SVRankingFilterDlg::SVRankingFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent) : CDialog(SVRankingFilterDlg::IDD, pParent)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values{ SvOg::BoundValues{ rInspectionID, rFilterID } }
	{
		//{{AFX_DATA_INIT(SVRankingFilterDlg)
		m_lRankingWidth = 3;
		m_lRankingHeight = 3;
		//}}AFX_DATA_INIT
		for( long l = 0; l < 49; l++ )
		{
			m_lRankingCells[l] = 1;
		}// end for

	}// end ctor

	SVRankingFilterDlg::~SVRankingFilterDlg()
	{
	}

	HRESULT SVRankingFilterDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData( TRUE ); // get data from dialog

		m_Values.Set<long>(SVRankingFilterRankingWidthGuid, m_lRankingWidth);
		m_Values.Set<long>(SVRankingFilterRankingHeightGuid, m_lRankingHeight);
		m_Values.Set<long>(SVRankingFilterRankingRankGuid, m_lRankingRank);

		for( long j = 0; S_OK == l_hrOk && j < m_lRankingWidth; j++ )
		{
			for( long k = 0;S_OK == l_hrOk && k < m_lRankingHeight; k++ )
			{
				long l_lKernelIndex = k * m_lRankingWidth + j;
				long l_lDialogIndex = ( k + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( j + ( ( 7 - m_lRankingWidth ) / 2 ) );

				m_Values.Set<long>( CellValueGuidList[l_lKernelIndex], m_lRankingCells[ l_lDialogIndex ] );
			}// end for
		}// end for

		m_Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

		UpdateData( FALSE );

		return l_hrOk;
	}

	void SVRankingFilterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVRankingFilterDlg)
		DDX_Control(pDX, IDC_KERNELWIDTH, m_ctlRankingWidth);
		DDX_Control(pDX, IDC_KERNELHEIGHT, m_ctlRankingHeight);
		DDX_Control(pDX, IDC_RANKINGRANK, m_ctlRankingRank);
		DDX_Text(pDX, IDC_KERNELWIDTH, m_lRankingWidth);
		DDX_Text(pDX, IDC_KERNELHEIGHT, m_lRankingHeight);
		//}}AFX_DATA_MAP
		for( long l = 0; l < 49; l++ )
		{
			DDX_Text(pDX, IDC_CELL1 + l, m_lRankingCells[l]);
			DDV_MinMaxLong(pDX, m_lRankingCells[l], 0, 1);
		}// end for
	}


	BEGIN_MESSAGE_MAP(SVRankingFilterDlg, CDialog)
		//{{AFX_MSG_MAP(SVRankingFilterDlg)
		ON_LBN_SELCHANGE(IDC_KERNELWIDTH, OnSelchangeRankingWidth)
		ON_LBN_SELCHANGE(IDC_KERNELHEIGHT, OnSelchangeRankingHeight)
		ON_LBN_SELCHANGE(IDC_RANKINGRANK, OnSelchangeRankingRank)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVRankingFilterDlg message handlers

	BOOL SVRankingFilterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		
		std::string Temp;
		
		m_Values.Init();
		
		m_lRankingWidth = m_Values.Get<long>(SVRankingFilterRankingWidthGuid);
		Temp = SvUl::Format( "%d", m_lRankingWidth );
		m_ctlRankingWidth.SelectString( -1, Temp.c_str() );

		m_lRankingHeight = m_Values.Get<long>(SVRankingFilterRankingHeightGuid);
		Temp = SvUl::Format( "%d", m_lRankingHeight );
		m_ctlRankingHeight.SelectString( -1, Temp.c_str() );

		m_lRankingRank = m_Values.Get<long>(SVRankingFilterRankingRankGuid);
		m_ctlRankingRank.SetCurSel(m_lRankingRank);

		EnableCells();

		for( long h = 0; h < m_lRankingWidth; h++ )
		{
			for( long i = 0; i < m_lRankingHeight; i++ )
			{
				long l_lKernelIndex = i * m_lRankingWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lRankingWidth ) / 2 ) );
				long lValue = m_Values.Get<long>(CellValueGuidList[l_lKernelIndex]);
				m_lRankingCells[ l_lDialogIndex ] = ( lValue == 1 ? 1 : 0 );
			}// end for
		}// end for

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}// end OnInitDialog

	void SVRankingFilterDlg::OnSelchangeRankingWidth() 
	{
		long lIndex = m_ctlRankingWidth.GetCurSel();

		if( LB_ERR != lIndex )
		{
			UpdateData();
			EnableCells();
		}// end if

	}// end OnSelchangeRankingWidth

	void SVRankingFilterDlg::OnSelchangeRankingHeight() 
	{
		long lIndex = m_ctlRankingHeight.GetCurSel();

		if( LB_ERR != lIndex )
		{
			UpdateData();
			EnableCells();
		}// end if

	}// end OnSelchangeRankingHeight

	void SVRankingFilterDlg::OnSelchangeRankingRank() 
	{
		long lIndex = m_ctlRankingRank.GetCurSel();

		if( LB_ERR != lIndex )
		{
			m_lRankingRank = static_cast<long>(m_ctlRankingRank.GetItemData( lIndex ));
		}// end if

	}// end OnSelchangeRankingRank

	void SVRankingFilterDlg::EnableCells()
	{
		for(long j = 0; j < 7; j++ )
		{
			for( long k = 0; k < 7; k++ )
			{
				if( j - ( ( 7 - m_lRankingWidth  ) / 2 ) < m_lRankingWidth  && j - ( ( 7 - m_lRankingWidth  ) / 2 ) >= 0 &&
					k - ( ( 7 - m_lRankingHeight ) / 2 ) < m_lRankingHeight && k - ( ( 7 - m_lRankingHeight ) / 2 ) >= 0 )

				{
					std::string Name;
					Name = SvUl::Format( _T( "Cell %d" ), ( k - ( ( 7 - m_lRankingHeight ) / 2 ) ) * m_lRankingWidth + ( j - ( ( 7 - m_lRankingWidth  ) / 2 ) ) + 1 );
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

		m_ctlRankingRank.ResetContent();
		m_ctlRankingRank.SetItemData( m_ctlRankingRank.AddString( _T( "MEDIAN" ) ), SVValueMedian );

		for(long j = 0; j < m_lRankingWidth * m_lRankingHeight; j++ )
		{
			std::string Temp = SvUl::Format( _T("%02d"), j + 1 );
			m_ctlRankingRank.SetItemData( m_ctlRankingRank.AddString( Temp.c_str() ), j + 1 );
		}

		if( SVValueMedian == m_lRankingRank )
		{
			m_ctlRankingRank.SelectString( -1, _T("MEDIAN") );
		}// end if
		else
		{
			std::string Temp = SvUl::Format( "%02d", m_lRankingRank );
			m_ctlRankingRank.SelectString( -1, Temp.c_str() );
		}// end else

	}// end EnableCells

	void SVRankingFilterDlg::OnOK() 
	{
		SetInspectionData();

		CDialog::OnOK();
	}
}  //namespace SvOg

