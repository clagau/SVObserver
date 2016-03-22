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
//Moved to precompiled header: #include <boost/assign/list_of.hpp> 
#include "SVCustomFilterDlg.h"
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
		static const std::string ClippingTag("IsClipping");
		static const std::string AbsoluteValueTag("AbsoluteValue");
		static const std::string TransformationTag("TransformationFactor");
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
		// SVCustomFilterDlg dialog
	SVCustomFilterDlg::SVCustomFilterDlg(const SVGUID& rInspectionID, const SVGUID& rFilterID, CWnd* pParent): 
		CDialog(SVCustomFilterDlg::IDD, pParent)
		,m_filterID(rFilterID)
		,m_rInspectionID(rInspectionID)
		, m_Values(SvOg::BoundValues(rInspectionID, rFilterID, boost::assign::map_list_of
		(WidthTag, SVCustomFilterKernelWidthGuid)
		(HeidghtTag, SVCustomFilterKernelHeightGuid)
		(ClippingTag, SVCustomFilterClippingGuid)
		(AbsoluteValueTag, SVCustomFilterAbsoluteGuid)
		(TransformationTag, SVCustomFilterTransformGuid)
		(CellValueTagList[0],  SVCustomFilterCell01Guid)
		(CellValueTagList[1],  SVCustomFilterCell02Guid)
		(CellValueTagList[2],  SVCustomFilterCell03Guid)
		(CellValueTagList[3],  SVCustomFilterCell04Guid)
		(CellValueTagList[4],  SVCustomFilterCell05Guid)
		(CellValueTagList[5],  SVCustomFilterCell06Guid)
		(CellValueTagList[6],  SVCustomFilterCell07Guid)
		(CellValueTagList[7],  SVCustomFilterCell08Guid)
		(CellValueTagList[8],  SVCustomFilterCell09Guid)
		(CellValueTagList[9],  SVCustomFilterCell10Guid)
		(CellValueTagList[10], SVCustomFilterCell11Guid)
		(CellValueTagList[11], SVCustomFilterCell12Guid)
		(CellValueTagList[12], SVCustomFilterCell13Guid)
		(CellValueTagList[13], SVCustomFilterCell14Guid)
		(CellValueTagList[14], SVCustomFilterCell15Guid)
		(CellValueTagList[15], SVCustomFilterCell16Guid)
		(CellValueTagList[16], SVCustomFilterCell17Guid)
		(CellValueTagList[17], SVCustomFilterCell18Guid)
		(CellValueTagList[18], SVCustomFilterCell19Guid)
		(CellValueTagList[19], SVCustomFilterCell20Guid)
		(CellValueTagList[20], SVCustomFilterCell21Guid)
		(CellValueTagList[21], SVCustomFilterCell22Guid)
		(CellValueTagList[22], SVCustomFilterCell23Guid)
		(CellValueTagList[23], SVCustomFilterCell24Guid)
		(CellValueTagList[24], SVCustomFilterCell25Guid)
		(CellValueTagList[25], SVCustomFilterCell26Guid)
		(CellValueTagList[26], SVCustomFilterCell27Guid)
		(CellValueTagList[27], SVCustomFilterCell28Guid)
		(CellValueTagList[28], SVCustomFilterCell29Guid)
		(CellValueTagList[29], SVCustomFilterCell30Guid)
		(CellValueTagList[30], SVCustomFilterCell31Guid)
		(CellValueTagList[31], SVCustomFilterCell32Guid)
		(CellValueTagList[32], SVCustomFilterCell33Guid)
		(CellValueTagList[33], SVCustomFilterCell34Guid)
		(CellValueTagList[34], SVCustomFilterCell35Guid)
		(CellValueTagList[35], SVCustomFilterCell36Guid)
		(CellValueTagList[36], SVCustomFilterCell37Guid)
		(CellValueTagList[37], SVCustomFilterCell38Guid)
		(CellValueTagList[38], SVCustomFilterCell39Guid)
		(CellValueTagList[39], SVCustomFilterCell40Guid)
		(CellValueTagList[40], SVCustomFilterCell41Guid)
		(CellValueTagList[41], SVCustomFilterCell42Guid)
		(CellValueTagList[42], SVCustomFilterCell43Guid)
		(CellValueTagList[43], SVCustomFilterCell44Guid)
		(CellValueTagList[44], SVCustomFilterCell45Guid)
		(CellValueTagList[45], SVCustomFilterCell46Guid)
		(CellValueTagList[46], SVCustomFilterCell47Guid)
		(CellValueTagList[47], SVCustomFilterCell48Guid)
		(CellValueTagList[48], SVCustomFilterCell49Guid)))
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

		m_Values.Set<long>(WidthTag, m_lKernelWidth);
		m_Values.Set<long>(HeidghtTag, m_lKernelHeight);
		m_Values.Set<bool>(ClippingTag, TRUE == m_bClippingOn);
		m_Values.Set<bool>(AbsoluteValueTag, TRUE == m_bAbsoluteValue);
		m_Values.Set<long>(TransformationTag, m_lTransformationFactor);

		for( long j = 0; S_OK == l_hrOk && j < m_lKernelWidth; j++ )
		{
			for( long k = 0; S_OK == l_hrOk && k < m_lKernelHeight; k++ )
			{
				long l_lKernelIndex = k * m_lKernelWidth + j;
				long l_lDialogIndex = ( k + ( ( 7 - m_lKernelHeight ) / 2 ) ) * 7 + ( j + ( ( 7 - m_lKernelWidth ) / 2 ) );

				m_Values.Set<long>( CellValueTagList[l_lKernelIndex], m_lKernelCells[ l_lDialogIndex ] );
			}// end for

		}// end for

		m_Values.Commit(true);

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

		CString strTemp;

		m_Values.Init();
		m_lKernelWidth = m_Values.Get<long>(WidthTag);
		strTemp.Format( "%d", m_lKernelWidth );
		m_ctlKernelWidth.SelectString( -1, strTemp );

		m_lKernelHeight = m_Values.Get<long>(HeidghtTag);
		strTemp.Format( "%d", m_lKernelHeight );
		m_ctlKernelHeight.SelectString( -1, strTemp );

		EnableCells();

		m_bClippingOn = m_Values.Get<bool>(ClippingTag);
		m_bAbsoluteValue = m_Values.Get<bool>(AbsoluteValueTag);
		m_lTransformationFactor = m_Values.Get<long>(TransformationTag);

		for( long h = 0; h < m_lKernelWidth; h++ )
		{
			for( long i = 0; i < m_lKernelHeight; i++ )
			{
				long l_lKernelIndex = i * m_lKernelWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lKernelHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lKernelWidth ) / 2 ) );

				m_lKernelCells[ l_lDialogIndex ] = m_Values.Get<long>(CellValueTagList[l_lKernelIndex]);
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
		CString strName;

		for( long j = 0; j < 7; j++ )
		{
			for( long k = 0; k < 7; k++ )
			{
				if( j - ( ( 7 - m_lKernelWidth  ) / 2 ) < m_lKernelWidth  && j - ( ( 7 - m_lKernelWidth  ) / 2 ) >= 0 &&
					k - ( ( 7 - m_lKernelHeight ) / 2 ) < m_lKernelHeight && k - ( ( 7 - m_lKernelHeight ) / 2 ) >= 0 )

				{
					strName.Format( _T( "Cell %d" ), ( k - ( ( 7 - m_lKernelHeight ) / 2 ) ) * m_lKernelWidth + ( j - ( ( 7 - m_lKernelWidth  ) / 2 ) ) + 1 );
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
	}  //end namespace SVOGUI
}  //end namespace Seidenader

