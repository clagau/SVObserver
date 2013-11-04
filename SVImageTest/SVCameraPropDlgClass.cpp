// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPropDlgClass.cpp
// * .File Name       : $Workfile:   SVCameraPropDlgClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:24  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImageTest.h"
#include "SVCameraPropDlgClass.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVCameraPropDlgClass::SVCameraPropDlgClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVCameraPropDlgClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVCameraPropDlgClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psvDigitizers = NULL;
	m_ulHandle = NULL;
}

void SVCameraPropDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVCameraPropDlgClass)
	DDX_Control(pDX, IDC_LIST1, m_svListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVCameraPropDlgClass, CDialog)
	//{{AFX_MSG_MAP(SVCameraPropDlgClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVCameraPropDlgClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect crPLSize;

	m_svListCtrl.GetClientRect(crPLSize);
	
	int l_iLabelWidth = (crPLSize.Width() - 25) / 2;
	int l_iValueWidth = crPLSize.Width() - 25 - l_iLabelWidth;
	
	m_svListCtrl.InsertColumn(0, "Name", LVCFMT_LEFT, l_iLabelWidth);
	m_svListCtrl.InsertColumn(1, "Value", LVCFMT_CENTER, l_iValueWidth);
	

	// Start - Add Camera Parameters

	if( m_psvDigitizers != NULL )
	{
		VARIANT l_varList;

		::VariantInit( &l_varList );

		m_psvDigitizers->ParameterGetList( m_ulHandle, &l_varList );

		if( ( l_varList.vt & VT_ARRAY ) == VT_ARRAY )
		{
			long lLBound, lUBound;
			SafeArrayGetLBound(l_varList.parray, 1, &lLBound);
			SafeArrayGetUBound(l_varList.parray, 1, &lUBound);

			int l_iIndex = -1;

			// iterate safearray
			for ( long i=lLBound; i <= lUBound; i++)
			{
				int l_iParameterID = 0;
				int l_iParameterTypeID = 0;

				BSTR l_bstrName = NULL;
				VARIANT l_varValue;

				::VariantInit( &l_varValue );

				::SafeArrayGetElement(l_varList.parray, &i, &l_iParameterID);

				if( m_psvDigitizers->ParameterGetName( m_ulHandle, l_iParameterID, &l_bstrName ) == S_OK )
				{
					m_psvDigitizers->ParameterGetValue( m_ulHandle, l_iParameterID, &l_iParameterTypeID, &l_varValue );

					CString l_csName;

					l_csName = l_bstrName;

					l_iIndex = m_svListCtrl.InsertItem( l_iIndex + 1, l_csName );
					
					if ( l_iIndex > -1 )
					{
						CString l_csData;

						switch( l_varValue.vt )
						{
							case VT_I4:
							{
								l_csData.Format( "%d", l_varValue.lVal );
								break;
							}
							case VT_UI4:
							{
								l_csData.Format( "0x%04X", l_varValue.ulVal );
								break;
							}
							case VT_R4:
							{
								l_csData.Format( "%f", l_varValue.fltVal );
								break;
							}
							case VT_R8:
							{
								l_csData.Format( "%lf", l_varValue.dblVal );
								break;
							}
							case VT_BSTR:
							{
								l_csData = l_varValue.bstrVal;
								break;
							}
						}

						m_svListCtrl.SetItemText( l_iIndex, 1, l_csData );
						m_svListCtrl.SetItemData( l_iIndex, l_iParameterID );
					}

					::VariantClear( &l_varValue );
				}

				::SysFreeString( l_bstrName );
			}
		}

		::VariantClear( &l_varList );
	}
	// End - Add Camera Parameters

	if (m_svListCtrl.GetItemCount() > 0)
	{
		int iItem = m_svListCtrl.GetNextItem(-1, LVNI_FOCUSED);
		if (iItem == -1)
		{
			m_svListCtrl.SetItemState(0, LVNI_FOCUSED, LVNI_FOCUSED);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVCameraPropDlgClass::OnOK() 
{
	//*
	int l_iCount = m_svListCtrl.GetItemCount();

	for( int i = 0; i < l_iCount; i++ )
	{
		CString l_csData = m_svListCtrl.GetItemText( i, 1 );
		long l_lParameterID = m_svListCtrl.GetItemData( i );
		long l_lParameterTypeID = 0;

		VARIANT l_varValue;

		::VariantInit( &l_varValue );

		l_varValue.vt = VT_UI4;
		l_varValue.lVal = atol( l_csData );

		m_psvDigitizers->ParameterSetValue( m_ulHandle, l_lParameterID, l_lParameterTypeID, &l_varValue );
	}
	//*/
	
	CDialog::OnOK();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVCameraPropDlgClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:08:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 11:42:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Jun 2009 08:31:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revsied to show more variant types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Mar 2008 09:39:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 12:23:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
