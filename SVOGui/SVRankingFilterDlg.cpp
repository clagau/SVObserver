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
#include <boost/assign/list_of.hpp> 
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVRankingFilterDlg.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;

namespace Seidenader
{
	namespace SVOGui
	{
		static const std::string WidthTag("Width");
		static const std::string HeidghtTag("Height");
		static const std::string RankValueTag("RankValue");
		static const std::string CellValueTag("CellValue");

		static const std::vector<std::string> CellValueTagList = 
			boost::assign::list_of(CellValueTag+"1") (CellValueTag+"2") (CellValueTag+"3") (CellValueTag+"4") (CellValueTag+"5") (CellValueTag+"6") (CellValueTag+"7")
									(CellValueTag+"8")  (CellValueTag+"9")  (CellValueTag+"10") (CellValueTag+"11") (CellValueTag+"12") (CellValueTag+"13") (CellValueTag+"14")
									(CellValueTag+"15") (CellValueTag+"16") (CellValueTag+"17") (CellValueTag+"18") (CellValueTag+"19") (CellValueTag+"20") (CellValueTag+"21")
									(CellValueTag+"22") (CellValueTag+"23") (CellValueTag+"24") (CellValueTag+"25") (CellValueTag+"26") (CellValueTag+"27") (CellValueTag+"28")
									(CellValueTag+"29") (CellValueTag+"30") (CellValueTag+"31") (CellValueTag+"32") (CellValueTag+"33") (CellValueTag+"34") (CellValueTag+"35")
									(CellValueTag+"36") (CellValueTag+"37") (CellValueTag+"38") (CellValueTag+"39") (CellValueTag+"40") (CellValueTag+"41") (CellValueTag+"42")
									(CellValueTag+"43") (CellValueTag+"44") (CellValueTag+"45") (CellValueTag+"46") (CellValueTag+"47") (CellValueTag+"48") (CellValueTag+"49");

		/////////////////////////////////////////////////////////////////////////////
		// SVRankingFilterDlg dialog
		SVRankingFilterDlg::SVRankingFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent): 
	CDialog(SVRankingFilterDlg::IDD, pParent)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
		(WidthTag, SVRankingFilterRankingWidthGuid)
		(HeidghtTag, SVRankingFilterRankingHeightGuid)
		(RankValueTag, SVRankingFilterRankingRankGuid)
		(CellValueTagList[0],  SVRankingFilterCell01Guid)
		(CellValueTagList[1],  SVRankingFilterCell02Guid)
		(CellValueTagList[2],  SVRankingFilterCell03Guid)
		(CellValueTagList[3],  SVRankingFilterCell04Guid)
		(CellValueTagList[4],  SVRankingFilterCell05Guid)
		(CellValueTagList[5],  SVRankingFilterCell06Guid)
		(CellValueTagList[6],  SVRankingFilterCell07Guid)
		(CellValueTagList[7],  SVRankingFilterCell08Guid)
		(CellValueTagList[8],  SVRankingFilterCell09Guid)
		(CellValueTagList[9],  SVRankingFilterCell10Guid)
		(CellValueTagList[10], SVRankingFilterCell11Guid)
		(CellValueTagList[11], SVRankingFilterCell12Guid)
		(CellValueTagList[12], SVRankingFilterCell13Guid)
		(CellValueTagList[13], SVRankingFilterCell14Guid)
		(CellValueTagList[14], SVRankingFilterCell15Guid)
		(CellValueTagList[15], SVRankingFilterCell16Guid)
		(CellValueTagList[16], SVRankingFilterCell17Guid)
		(CellValueTagList[17], SVRankingFilterCell18Guid)
		(CellValueTagList[18], SVRankingFilterCell19Guid)
		(CellValueTagList[19], SVRankingFilterCell20Guid)
		(CellValueTagList[20], SVRankingFilterCell21Guid)
		(CellValueTagList[21], SVRankingFilterCell22Guid)
		(CellValueTagList[22], SVRankingFilterCell23Guid)
		(CellValueTagList[23], SVRankingFilterCell24Guid)
		(CellValueTagList[24], SVRankingFilterCell25Guid)
		(CellValueTagList[25], SVRankingFilterCell26Guid)
		(CellValueTagList[26], SVRankingFilterCell27Guid)
		(CellValueTagList[27], SVRankingFilterCell28Guid)
		(CellValueTagList[28], SVRankingFilterCell29Guid)
		(CellValueTagList[29], SVRankingFilterCell30Guid)
		(CellValueTagList[30], SVRankingFilterCell31Guid)
		(CellValueTagList[31], SVRankingFilterCell32Guid)
		(CellValueTagList[32], SVRankingFilterCell33Guid)
		(CellValueTagList[33], SVRankingFilterCell34Guid)
		(CellValueTagList[34], SVRankingFilterCell35Guid)
		(CellValueTagList[35], SVRankingFilterCell36Guid)
		(CellValueTagList[36], SVRankingFilterCell37Guid)
		(CellValueTagList[37], SVRankingFilterCell38Guid)
		(CellValueTagList[38], SVRankingFilterCell39Guid)
		(CellValueTagList[39], SVRankingFilterCell40Guid)
		(CellValueTagList[40], SVRankingFilterCell41Guid)
		(CellValueTagList[41], SVRankingFilterCell42Guid)
		(CellValueTagList[42], SVRankingFilterCell43Guid)
		(CellValueTagList[43], SVRankingFilterCell44Guid)
		(CellValueTagList[44], SVRankingFilterCell45Guid)
		(CellValueTagList[45], SVRankingFilterCell46Guid)
		(CellValueTagList[46], SVRankingFilterCell47Guid)
		(CellValueTagList[47], SVRankingFilterCell48Guid)
		(CellValueTagList[48], SVRankingFilterCell49Guid)))
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

		m_Values.Set<long>(WidthTag, m_lRankingWidth);
		m_Values.Set<long>(HeidghtTag, m_lRankingHeight);
		m_Values.Set<long>(RankValueTag, m_lRankingRank);

		for( long j = 0; S_OK == l_hrOk && j < m_lRankingWidth; j++ )
		{
			for( long k = 0;S_OK == l_hrOk && k < m_lRankingHeight; k++ )
			{
				long l_lKernelIndex = k * m_lRankingWidth + j;
				long l_lDialogIndex = ( k + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( j + ( ( 7 - m_lRankingWidth ) / 2 ) );

				m_Values.Set<long>( CellValueTagList[l_lKernelIndex], m_lRankingCells[ l_lDialogIndex ] );
			}// end for
		}// end for

		m_Values.Commit(true);

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
		
		CString strTemp;
		
		m_Values.Init();
		
		m_lRankingWidth = m_Values.Get<long>(WidthTag);
		strTemp.Format( "%d", m_lRankingWidth );
		m_ctlRankingWidth.SelectString( -1, strTemp );

		m_lRankingHeight = m_Values.Get<long>(HeidghtTag);
		strTemp.Format( "%d", m_lRankingHeight );
		m_ctlRankingHeight.SelectString( -1, strTemp );

		m_lRankingRank = m_Values.Get<long>(RankValueTag);
		m_ctlRankingRank.SetCurSel(m_lRankingRank);

		EnableCells();

		for( long h = 0; h < m_lRankingWidth; h++ )
		{
			for( long i = 0; i < m_lRankingHeight; i++ )
			{
				long l_lKernelIndex = i * m_lRankingWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lRankingHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lRankingWidth ) / 2 ) );
				long lValue = m_Values.Get<long>(CellValueTagList[l_lKernelIndex]);
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

		if( SVValueMedian == m_lRankingRank )
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
	}  //end namespace SVOGUI
}  //end namespace Seidenader

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
