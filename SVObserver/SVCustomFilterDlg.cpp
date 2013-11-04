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

#include "stdafx.h"
#include "SVCustomFilterDlg.h"

#include "SVCustomFilters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVCustomFilterDlg dialog


SVCustomFilterDlg::SVCustomFilterDlg(): 
		CDialog(SVCustomFilterDlg::IDD)
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

SVCustomFilterClass *SVCustomFilterDlg::GetCustomFilter()
{
	return dynamic_cast<SVCustomFilterClass *>( GetTaskObject() );
}

HRESULT SVCustomFilterDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData( TRUE ); // get data from dialog

	SVCustomFilterClass *l_pCustomFilter = GetCustomFilter();

	if( l_pCustomFilter != NULL )
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pCustomFilter->m_lvoKernelWidth), m_lKernelWidth );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pCustomFilter->m_lvoKernelHeight), m_lKernelHeight );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pCustomFilter->m_bvoClippingOn), m_bClippingOn );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pCustomFilter->m_bvoAbsoluteValue), m_bAbsoluteValue );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &(l_pCustomFilter->m_lvoTransformationFactor), m_lTransformationFactor );
		}

		for( long j = 0; l_hrOk == S_OK && j < m_lKernelWidth; j++ )
		{
			for( long k = 0; l_hrOk == S_OK && k < m_lKernelHeight; k++ )
			{
				long l_lKernelIndex = k * m_lKernelWidth + j;
				long l_lDialogIndex = ( k + ( ( 7 - m_lKernelHeight ) / 2 ) ) * 7 + ( j + ( ( 7 - m_lKernelWidth ) / 2 ) );

				l_hrOk = AddInputRequest( l_pCustomFilter->m_plvoKernelCells[ l_lKernelIndex ], m_lKernelCells[ l_lDialogIndex ] );
			}// end for

		}// end for

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( l_pCustomFilter->GetTool() );
		}

		UpdateData( FALSE );
	}
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

	SVCustomFilterClass *l_pFilter = GetCustomFilter();

	if( l_pFilter != NULL )
	{
		l_pFilter->m_lvoKernelWidth.GetValue( m_lKernelWidth );
		strTemp.Format( "%d", m_lKernelWidth );
		m_ctlKernelWidth.SelectString( -1, strTemp );

		l_pFilter->m_lvoKernelHeight.GetValue( m_lKernelHeight );
		strTemp.Format( "%d", m_lKernelHeight );
		m_ctlKernelHeight.SelectString( -1, strTemp );

		EnableCells();

		l_pFilter->m_bvoClippingOn.GetValue( m_bClippingOn );
		l_pFilter->m_bvoAbsoluteValue.GetValue( m_bAbsoluteValue );
		l_pFilter->m_lvoTransformationFactor.GetValue( m_lTransformationFactor );
		
		for( long h = 0; h < m_lKernelWidth; h++ )
		{
			for( long i = 0; i < m_lKernelHeight; i++ )
			{
				long l_lKernelIndex = i * m_lKernelWidth + h;
				long l_lDialogIndex = ( i + ( ( 7 - m_lKernelHeight ) / 2 ) ) * 7 + ( h + ( ( 7 - m_lKernelWidth ) / 2 ) );

				l_pFilter->m_plvoKernelCells[ l_lKernelIndex ]->GetValue( m_lKernelCells[ l_lDialogIndex ] );
			}// end for
		}// end for
	}

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVCustomFilterDlg::OnSelchangeKernelWidth() 
{
	long lIndex = m_ctlKernelWidth.GetCurSel();

	if( lIndex != LB_ERR )
	{
		UpdateData();
		EnableCells();
	}// end if

}// end OnSelchangeKernelWidth

void SVCustomFilterDlg::OnSelchangeKernelHeight() 
{
	long lIndex = m_ctlKernelHeight.GetCurSel();

	if( lIndex != LB_ERR )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCustomFilterDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:04:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Aug 2005 09:31:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Aug 2005 13:17:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:35:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Nov 2003 14:11:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 10:16:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
*/