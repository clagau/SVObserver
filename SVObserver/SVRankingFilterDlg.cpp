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


#include "stdafx.h"
#include "SVRankingFilterDlg.h"

#include "SVRankingFilters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRankingFilterDlg dialog


SVRankingFilterDlg::SVRankingFilterDlg(): 
		CDialog(SVRankingFilterDlg::IDD)
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

SVRankingFilterClass *SVRankingFilterDlg::GetRankingFilter()
{
	return dynamic_cast<SVRankingFilterClass *>( GetTaskObject() );
}

HRESULT SVRankingFilterDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData( TRUE ); // get data from dialog

	SVRankingFilterClass *l_pRankingFilter = GetRankingFilter();

	if( l_pRankingFilter != NULL )
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pRankingFilter->m_lvoRankingWidth), m_lRankingWidth );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pRankingFilter->m_lvoRankingHeight), m_lRankingHeight );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pRankingFilter->m_lvoRankingRank), m_lRankingRank );
		}

		for( long j = 0; l_hrOk == S_OK && j < m_lRankingWidth; j++ )
		{
			for( long k = 0; l_hrOk == S_OK && k < m_lRankingHeight; k++ )
			{
				long l_lKernelIndex = k * m_lRankingWidth + j;
				long l_lDialogIndex = ( k + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( j + ( ( 7 - m_lRankingWidth ) / 2 ) );

				l_hrOk = AddInputRequest( l_pRankingFilter->m_plvoRankingCells[ l_lKernelIndex ], m_lRankingCells[ l_lDialogIndex ] );
			}// end for
		}// end for

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( l_pRankingFilter->GetTool() );
		}

		UpdateData( FALSE );
	}

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

	SVRankingFilterClass *l_pRankingFilter = GetRankingFilter();

	if( l_pRankingFilter != NULL )
	{
		CString strTemp;

		l_pRankingFilter->m_lvoRankingWidth.GetValue( m_lRankingWidth );
		strTemp.Format( "%d", m_lRankingWidth );
		m_ctlRankingWidth.SelectString( -1, strTemp );

		l_pRankingFilter->m_lvoRankingHeight.GetValue( m_lRankingHeight );
		strTemp.Format( "%d", m_lRankingHeight );
		m_ctlRankingHeight.SelectString( -1, strTemp );

		l_pRankingFilter->m_lvoRankingRank.GetValue( m_lRankingRank );
		m_ctlRankingRank.SetCurSel(m_lRankingRank);


		EnableCells();

		
		for( long h = 0; h < m_lRankingWidth; h++ )
		{
			for( long i = 0; i < m_lRankingHeight; i++ )
			{
				long lValue = 0;
				long l_lKernelIndex = i * m_lRankingWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lRankingWidth ) / 2 ) );

				l_pRankingFilter->m_plvoRankingCells[ l_lKernelIndex ]->GetValue( lValue );
				m_lRankingCells[ l_lDialogIndex ] = ( lValue == 1 ? 1 : 0 );
			}// end for
		}// end for
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}// end OnInitDialog

void SVRankingFilterDlg::OnSelchangeRankingWidth() 
{
	long lIndex = m_ctlRankingWidth.GetCurSel();

	if( lIndex != LB_ERR )
	{
		UpdateData();
		EnableCells();
	}// end if

}// end OnSelchangeRankingWidth

void SVRankingFilterDlg::OnSelchangeRankingHeight() 
{
	long lIndex = m_ctlRankingHeight.GetCurSel();

	if( lIndex != LB_ERR )
	{
		UpdateData();
		EnableCells();
	}// end if

}// end OnSelchangeRankingHeight

void SVRankingFilterDlg::OnSelchangeRankingRank() 
{
	long lIndex = m_ctlRankingRank.GetCurSel();

	if( lIndex != LB_ERR )
	{
		m_lRankingRank = static_cast<long>(m_ctlRankingRank.GetItemData( lIndex ));
	}// end if

}// end OnSelchangeRankingRank

void SVRankingFilterDlg::EnableCells()
{
	CString strName;
	CString strTemp;
	long j( 0 );

	for( j = 0; j < 7; j++ )
	{
		for( long k = 0; k < 7; k++ )
		{
			if( j - ( ( 7 - m_lRankingWidth  ) / 2 ) < m_lRankingWidth  && j - ( ( 7 - m_lRankingWidth  ) / 2 ) >= 0 &&
				k - ( ( 7 - m_lRankingHeight ) / 2 ) < m_lRankingHeight && k - ( ( 7 - m_lRankingHeight ) / 2 ) >= 0 )
				
			{
				strName.Format( _T( "Cell %d" ), ( k - ( ( 7 - m_lRankingHeight ) / 2 ) ) * m_lRankingWidth + ( j - ( ( 7 - m_lRankingWidth  ) / 2 ) ) + 1 );
				GetDlgItem( IDC_STATIC_CELL1 + k * 7 + j )->SetWindowText( strName );
				GetDlgItem( IDC_CELL1 + k * 7 + j )->EnableWindow( TRUE );
			}// end if
			else
			{
				strName.Format( _T( "N/A" ) );
				GetDlgItem( IDC_STATIC_CELL1 + k * 7 + j )->SetWindowText( strName );
				GetDlgItem( IDC_CELL1 + k * 7 + j )->EnableWindow( FALSE );
			}// end else
			
		}// end for

	}// end for

	m_ctlRankingRank.ResetContent();
	m_ctlRankingRank.SetItemData( m_ctlRankingRank.AddString( _T( "MEDIAN" ) ), SVValueMedian );

	for( j = 0; j < m_lRankingWidth * m_lRankingHeight; j++ )
	{
		strTemp.Format( "%02d", j + 1 );
		m_ctlRankingRank.SetItemData( m_ctlRankingRank.AddString( strTemp ), j + 1 );
	}// end for

	if( m_lRankingRank == SVValueMedian )
	{
		strTemp.Format( "MEDIAN" );
		m_ctlRankingRank.SelectString( -1, strTemp );
	}// end if
	else
	{
		strTemp.Format( "%02d", m_lRankingRank );
		m_ctlRankingRank.SelectString( -1, strTemp );
	}// end else

}// end EnableCells

void SVRankingFilterDlg::OnOK() 
{
	SetInspectionData();

	CDialog::OnOK();
}// end OnOK

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRankingFilterDlg.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:39:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jul 2007 15:44:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2007 14:46:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Sep 2005 13:46:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem with Rank box not filling in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Aug 2005 13:21:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:09:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Nov 2003 14:40:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
