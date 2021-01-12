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
#include "SVCameraPropDlgClass.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "SVUtilityLibrary/StringHelper.h"
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
	m_digitizerHandle = 0;
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
		_variant_t varList = m_psvDigitizers->ParameterGetList( m_digitizerHandle);

		if( (varList.vt & VT_ARRAY ) == VT_ARRAY )
		{
			long index{ -1L };
			long lLBound{ 0L }; 
			long lUBound{ 0L };
			SafeArrayGetLBound(varList.parray, 1, &lLBound);
			SafeArrayGetUBound(varList.parray, 1, &lUBound);

			// iterate safearray
			for ( long i=lLBound; i <= lUBound; i++)
			{
				int parameterID = 0;

				::SafeArrayGetElement(varList.parray, &i, &parameterID);
				_variant_t name = m_psvDigitizers->ParameterGetName(m_digitizerHandle, parameterID);
				if(VT_EMPTY != name.vt)
				{
					_variant_t varValue = m_psvDigitizers->ParameterGetValue( m_digitizerHandle, parameterID);

					std::string parameterName{ SvUl::createStdString(name)};

					index = m_svListCtrl.InsertItem(index + 1, parameterName.c_str() );
					
					if ( index > -1 )
					{
						std::string Data;

						switch(varValue.vt)
						{
							case VT_I4:
							{
								Data = SvUl::Format( _T("%d"), varValue.lVal );
								break;
							}
							case VT_UI4:
							{
								Data = SvUl::Format( _T("0x%04X"), varValue.ulVal );
								break;
							}
							case VT_R4:
							{
								Data = SvUl::Format( _T("%f"), varValue.fltVal );
								break;
							}
							case VT_R8:
							{
								Data = SvUl::Format( _T("%lf"), varValue.dblVal );
								break;
							}
							case VT_BSTR:
							{
								Data = SvUl::createStdString(varValue);
								break;
							}
						}

						m_svListCtrl.SetItemText(index, 1, Data.c_str());
						m_svListCtrl.SetItemData(index, parameterID );
					}
				}
			}
		}
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
		std::string Data = m_svListCtrl.GetItemText( i, 1 );
		long parameterID = static_cast<long>(m_svListCtrl.GetItemData( i ));
		_variant_t value = atol(Data.c_str());

		m_psvDigitizers->ParameterSetValue( m_digitizerHandle, parameterID, value);
	}
	
	CDialog::OnOK();
}

