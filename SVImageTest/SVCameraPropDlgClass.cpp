// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPropDlgClass.cpp
// * .File Name       : $Workfile:   SVCameraPropDlgClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:58  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "CameraLibrary\SVDeviceParams.h"
#include "SVImageTest.h"
#include "SVCameraPropDlgClass.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVCameraPropDlgClass::SVCameraPropDlgClass(CWnd* pParent /*=nullptr*/)
	: CDialog(SVCameraPropDlgClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVCameraPropDlgClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psvDigitizers = nullptr;
	m_triggerchannel = 0;
}

SVCameraPropDlgClass::~SVCameraPropDlgClass()
{
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

	if( nullptr != m_psvDigitizers )
	{
		VARIANT l_varList;

		::VariantInit( &l_varList );

		m_psvDigitizers->ParameterGetList( m_triggerchannel, &l_varList );

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

				BSTR l_bstrName = nullptr;
				VARIANT l_varValue;

				::VariantInit( &l_varValue );

				::SafeArrayGetElement(l_varList.parray, &i, &l_iParameterID);

				if( S_OK == m_psvDigitizers->ParameterGetName( m_triggerchannel, l_iParameterID, &l_bstrName ) )
				{
					m_psvDigitizers->ParameterGetValue( m_triggerchannel, l_iParameterID, &l_iParameterTypeID, &l_varValue );

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
	int l_iCount = m_svListCtrl.GetItemCount();

	for( int i = 0; i < l_iCount; i++ )
	{
		CString l_csData = m_svListCtrl.GetItemText( i, 1 );
		long l_lParameterID = static_cast<long>(m_svListCtrl.GetItemData( i ));
		long l_lParameterTypeID = 0;

		VARIANT l_varValue;

		::VariantInit( &l_varValue );

		l_varValue.vt = VT_UI4;
		l_varValue.lVal = atol( l_csData );

		m_psvDigitizers->ParameterSetValue( m_triggerchannel, l_lParameterID, l_lParameterTypeID, &l_varValue );
	}
	
	CDialog::OnOK();
}

