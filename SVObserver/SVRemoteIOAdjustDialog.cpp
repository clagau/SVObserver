//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteIOAdjustDialog
//* .File Name       : $Workfile:   SVRemoteIOAdjustDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:49:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteIOAdjustDialog.h"

IMPLEMENT_DYNAMIC(SVRemoteIOAdjustDialog, CDialog)

BEGIN_MESSAGE_MAP(SVRemoteIOAdjustDialog, CDialog)
END_MESSAGE_MAP()

SVRemoteIOAdjustDialog::SVRemoteIOAdjustDialog(CWnd* pParent /*=nullptr*/)
: CDialog(SVRemoteIOAdjustDialog::IDD, pParent)
, m_Name()
, m_Value()
, m_EditValue( 0.0 )
{
}

SVRemoteIOAdjustDialog::~SVRemoteIOAdjustDialog()
{
}

void SVRemoteIOAdjustDialog::SetIOName( LPCTSTR p_szName )
{
	m_Name = p_szName;

	if( nullptr != m_IOName.GetSafeHwnd() )
	{
		m_IOName.SetWindowText( p_szName );
	}
}

void SVRemoteIOAdjustDialog::SetIOValue( const _variant_t& p_rValue )
{
	m_Value = p_rValue;

	m_Value.ChangeType( VT_R8 );

	if( nullptr != m_ValueEditControl.GetSafeHwnd() )
	{
		_bstr_t l_Value{ m_Value };

		m_ValueEditControl.SetWindowText( l_Value );
	}
}

void SVRemoteIOAdjustDialog::GetIOValue( _variant_t& rValue ) const
{
	rValue.Clear();

	if( nullptr != m_ValueEditControl.GetSafeHwnd() )
	{
		CString Text;

		m_ValueEditControl.GetWindowText( Text );

		rValue.SetString( Text );
	}
	else
	{
		rValue = m_Value;
	}

	rValue.ChangeType( VT_R8 );
}

void SVRemoteIOAdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMOTE_IONAME, m_IOName);
	DDX_Control(pDX, IDC_VALUE_EDIT, m_ValueEditControl);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_EditValue);
	DDV_MinMaxDouble(pDX, m_EditValue, -1.7E308 , 1.7E308);
}

BOOL SVRemoteIOAdjustDialog::OnInitDialog()
{
	BOOL l_Status = CDialog::OnInitDialog();

	if( l_Status )
	{
		_bstr_t l_Value;
		
		if( m_Value.vt == VT_EMPTY )
		{
			l_Value = _T( "" );
		}
		else
		{
			l_Value = m_Value;
		}

		m_IOName.SetWindowText( m_Name );

		m_ValueEditControl.SetWindowText( l_Value );
	}

	return l_Status;
}

void SVRemoteIOAdjustDialog::OnOK()
{
	if( UpdateData( TRUE ) )
	{
		m_Value = m_EditValue;

		CDialog::OnOK();
	}
}

