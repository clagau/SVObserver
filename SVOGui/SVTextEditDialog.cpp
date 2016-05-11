//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTextEditDialog
//* .File Name       : $Workfile:   SVTextEditDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:36:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVTextEditDialog.h"

namespace Seidenader { namespace SVOGui
{
	#pragma region Declarations
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif

	BEGIN_MESSAGE_MAP(SVTextEditDialog, CDialog)
	END_MESSAGE_MAP()

	IMPLEMENT_DYNAMIC(SVTextEditDialog, CDialog)
	#pragma endregion Declarations

	#pragma region Constructor
	SVTextEditDialog::SVTextEditDialog( LPCTSTR Text,  CWnd* pParent /*=nullptr*/)
		: CDialog(SVTextEditDialog::IDD, pParent)
		, m_Text( Text )
	{

	}

	SVTextEditDialog::~SVTextEditDialog()
	{
	}
	#pragma endregion Constructor

	#pragma region Private Methods
	void SVTextEditDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_TEXT_EDIT, m_Text);
	}

	BOOL SVTextEditDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		return true;
	}
	#pragma endregion Private Methods
} /* namespace SVOGui */ } /* namespace Seidenader */

