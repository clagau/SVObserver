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

SVRemoteIOAdjustDialog::SVRemoteIOAdjustDialog(CWnd* pParent /*=NULL*/)
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

	if( m_IOName.GetSafeHwnd() != NULL )
	{
		m_IOName.SetWindowText( p_szName );
	}
}

void SVRemoteIOAdjustDialog::SetIOValue( const _variant_t& p_rValue )
{
	m_Value = p_rValue;

	m_Value.ChangeType( VT_R8 );

	if( m_ValueEditControl.GetSafeHwnd() != NULL )
	{
		_bstr_t l_Value = m_Value;

		m_ValueEditControl.SetWindowText( l_Value );
	}
}

void SVRemoteIOAdjustDialog::GetIOValue( _variant_t& p_rValue ) const
{
	p_rValue.Clear();

	if( m_ValueEditControl.GetSafeHwnd() != NULL )
	{
		CString l_Value;

		m_ValueEditControl.GetWindowText( l_Value );

		p_rValue = static_cast< LPCTSTR >( l_Value );
	}
	else
	{
		p_rValue = m_Value;
	}

	p_rValue.ChangeType( VT_R8 );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteIOAdjustDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:49:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Oct 2010 11:20:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.  Added new dialog to edit the Remote Input Value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/