//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesCustomizeDialog
//* .File Name       : $Workfile:   SVUtilitiesCustomizeDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUtilitiesCustomizeDialog.h"
#include "SVObserver.h"
#include "SVUtilities.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVLibrary/SVUtilityIniClass.h"
#include "SVLibrary/SVOIniClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVUtilitiesCustomizeDialog::SVUtilitiesCustomizeDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(SVUtilitiesCustomizeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVUtilitiesCustomizeDialog)
	m_Arguments = _T("");
	m_Command = _T("");
	m_MenuText = _T("");
	mbPromptForArguments = FALSE;
	m_WorkingDirectory = _T("");
	m_pUtilityClass = nullptr;

	//}}AFX_DATA_INIT
}

SVUtilitiesCustomizeDialog::~SVUtilitiesCustomizeDialog()
{
}

void SVUtilitiesCustomizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVUtilitiesCustomizeDialog)
	DDX_Text(pDX, IDC_CUSTOMIZE_ARGUMENTS, m_Arguments);
	DDX_Text(pDX, IDC_CUSTOMIZE_COMMAND, m_Command);
	DDX_Text(pDX, IDC_CUSTOMIZE_MENUTEXT, m_MenuText);
	DDX_Check(pDX, IDC_CUSTOMIZE_PROMPTFORARGUMENTS, mbPromptForArguments);
	DDX_Text(pDX, IDC_CUSTOMIZE_WORKINGDIRECTORY, m_WorkingDirectory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVUtilitiesCustomizeDialog, CDialog)
	//{{AFX_MSG_MAP(SVUtilitiesCustomizeDialog)
	ON_CBN_SELENDOK(IDC_CUSTOMIZE_MENUTEXT, OnSelEndOkCustomizeMenuText)
	ON_CBN_EDITCHANGE(IDC_CUSTOMIZE_MENUTEXT, OnEditChangeCustomizeMenuText)
	ON_EN_CHANGE(IDC_CUSTOMIZE_COMMAND, OnChangeCustomizeCommand)
	ON_EN_CHANGE(IDC_CUSTOMIZE_ARGUMENTS, OnChangeCustomizeArguments)
	ON_EN_CHANGE(IDC_CUSTOMIZE_WORKINGDIRECTORY, OnChangeCustomizeWorkingDirectory)
	ON_BN_CLICKED(IDC_CUSTOMIZE_PROMPTFORARGUMENTS, OnCustomizePromptForArguments)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_CUSTOMIZE_DELETE, OnCustomizeDelete)
	ON_BN_CLICKED(IDC_CUSTOMIZE_COMMAND_SELECT, OnCustomizeCommandSelect)
	ON_BN_CLICKED(IDC_CUSTOMIZE_DIRECTORY_SELECT, OnCustomizeDirectorySelect)
	ON_CBN_KILLFOCUS(IDC_CUSTOMIZE_MENUTEXT, OnKillfocusCustomizeMenutext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVUtilitiesCustomizeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox *pBox;

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	//load the combobox with the info in the map.

	std::map<UINT, SvLib::SVUtilityIniClass>::const_iterator iter( pApp->m_UtilityMenu.begin() );
	while ( iter != pApp->m_UtilityMenu.end() )
	{
		pBox->AddString( iter->second.m_DisplayName.c_str() );

		++iter;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVUtilitiesCustomizeDialog::OnSelEndOkCustomizeMenuText() 
{
	CComboBox *pBox;
	int iCurSel;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	iCurSel = pBox->GetCurSel ();

	if (iCurSel > -1)
	{
		pBox->GetLBText (iCurSel, m_MenuText);

		std::map<UINT, SvLib::SVUtilityIniClass>::const_iterator iter;

		iter = pApp->m_UtilityMenu.begin();

		BOOL bDone = FALSE;

		while ( (iter != pApp->m_UtilityMenu.end()) && !bDone )
		{
			const SvLib::SVUtilityIniClass& rUtilityStruct( iter->second );
			if ( rUtilityStruct.m_DisplayName == m_MenuText.GetString() )
			{
				m_Command = rUtilityStruct.m_Command.c_str();
				m_Arguments = rUtilityStruct.m_Arguments.c_str();
				m_WorkingDirectory = rUtilityStruct.m_WorkingDirectory.c_str();
				if( 0 == SvUl::CompareNoCase( SvUl::Left( rUtilityStruct.m_PromptForArguments, 1 ), std::string( _T("Y") ) ) )
				{
					mbPromptForArguments = TRUE;
				}
				else
				{
					mbPromptForArguments = FALSE;
				}

				//fill in the info
				bDone = TRUE;
			}
			++iter;
		}
		SetApplyState (FALSE);
		UpdateData (FALSE);
	}
}

void SVUtilitiesCustomizeDialog::OnEditChangeCustomizeMenuText() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialog::SetApplyState(BOOL bEnabled)
{
	CButton *pApply;

	pApply = (CButton *) GetDlgItem (IDC_APPLY);
	pApply->EnableWindow (bEnabled);
	SetDeleteState (!bEnabled);
}

void SVUtilitiesCustomizeDialog::OnChangeCustomizeCommand() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialog::OnChangeCustomizeArguments() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialog::OnChangeCustomizeWorkingDirectory() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialog::OnCustomizePromptForArguments() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialog::SetDeleteState(BOOL bEnabled)
{
	CButton *pDelete;

	pDelete = (CButton *) GetDlgItem (IDC_CUSTOMIZE_DELETE);
	pDelete->EnableWindow (bEnabled);
}

void SVUtilitiesCustomizeDialog::OnApply() 
{
	CComboBox *pBox;

	UpdateData (TRUE);
	if (!m_MenuText.IsEmpty ())
	{

		//check to see if it is already in the map, if not add it
		SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
		std::map<UINT, SvLib::SVUtilityIniClass>::iterator iter;

		iter = pApp->m_UtilityMenu.begin();
		BOOL bFound = FALSE;

		while ( (iter != pApp->m_UtilityMenu.end()) && !bFound )
		{
			SvLib::SVUtilityIniClass& rUtilityStruct( iter->second );
			
			if ( rUtilityStruct.m_DisplayName == m_MenuText.GetString() )
			{
				rUtilityStruct.m_Arguments = m_Arguments;
				rUtilityStruct.m_Command = m_Command;
				rUtilityStruct.m_WorkingDirectory = m_WorkingDirectory;
				if ( mbPromptForArguments )
				{
					rUtilityStruct.m_PromptForArguments = _T("Y");
				}
				else
				{
					rUtilityStruct.m_PromptForArguments = _T("N");
				}
				iter->second = rUtilityStruct;
				bFound = TRUE;
			}
			++iter;
		}
		if ( !bFound )
		{

			pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
			pBox->AddString (m_MenuText);

			//must be a new utility
			std::map<UINT, SvLib::SVUtilityIniClass>::reverse_iterator riter;
			SvLib::SVUtilityIniClass UtilityStruct;
			riter = pApp->m_UtilityMenu.rbegin();
			UINT uintVal = riter->first;
			UtilityStruct.m_Arguments = m_Arguments;
			UtilityStruct.m_Command = m_Command;
			UtilityStruct.m_DisplayName = m_MenuText;
			if ( mbPromptForArguments )
			{
				UtilityStruct.m_PromptForArguments = "Y";
			}
			else
			{
				UtilityStruct.m_PromptForArguments = "N";
			}
			UtilityStruct.m_WorkingDirectory = m_WorkingDirectory;

			//add new utility to map
			pApp->m_UtilityMenu[uintVal+1] = UtilityStruct;

			SvLib::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

			int iCnt = UtilityIni.GetValueInt(_T("General"), _T("HighestUtilityIndex"), 0);
			iCnt++;
			UtilityIni.SetValueInt( _T("General"), _T("HighestUtilityIndex"),iCnt);
			std::string Text = SvUl::Format(_T("Utility%d"),iCnt);
			UtilityIni.SetValueString( Text.c_str(), _T("DisplayName"), UtilityStruct.m_DisplayName.c_str() );
			UtilityIni.SetValueString( Text.c_str(), _T("Command"), UtilityStruct.m_Command.c_str() );
			UtilityIni.SetValueString( Text.c_str(), _T("Arguments"), UtilityStruct.m_Arguments.c_str() );
			UtilityIni.SetValueString( Text.c_str(), _T("WorkingDirectory"), UtilityStruct.m_WorkingDirectory.c_str() );
			UtilityIni.SetValueString( Text.c_str(), _T("PromptForArguments"), UtilityStruct.m_PromptForArguments.c_str() );
		}
	}
	SetApplyState(false);

	if (m_MenuText.IsEmpty ())
	{
		SetDeleteState (false);
	}
}

void SVUtilitiesCustomizeDialog::OnOK() 
{
	CButton *pApply;

	pApply = (CButton *) GetDlgItem (IDC_APPLY);
	if (pApply->IsWindowEnabled ())
		OnApply();

	CDialog::OnOK();
}

void SVUtilitiesCustomizeDialog::OnCustomizeDelete() 
{
	CComboBox *pBox;

	SvLib::SVUtilityIniClass l_Struct;
	std::map<UINT, SvLib::SVUtilityIniClass>::iterator iter;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	SvLib::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

	UpdateData (TRUE);

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	pBox->DeleteString (pBox->FindStringExact (0, m_MenuText));

	int iUtlCnt = 0;

	BOOL bFound = FALSE;

	while ( !bFound )
	{
		std::string Value;
		iUtlCnt++;
		std::string Text = SvUl::Format(_T("Utility%d"), iUtlCnt);
		Value = UtilityIni.GetValueString( Text.c_str(), _T("DisplayName"), _T("") );

		if ( Value == m_MenuText.GetString() )
		{
			//found value.  delete from INI
			UtilityIni.SetValueString( Text.c_str(), nullptr, _T("") );
			int iVal = UtilityIni.GetValueInt( _T("General"), _T("HighestUtilityIndex"), 0);
			UtilityIni.SetValueInt( _T("General"), _T("HighestUtilityIndex"), (iVal-1) );
			bFound = TRUE;

			//find and delete from map
			iter = pApp->m_UtilityMenu.begin();
			BOOL bDelete = FALSE;

			while ( (!bDelete) && (iter != pApp->m_UtilityMenu.end()) )
			{
				l_Struct = iter->second;
				if ( l_Struct.m_DisplayName == m_MenuText.GetString() )
				{
					iter = pApp->m_UtilityMenu.erase(iter);
					bDelete = TRUE;
				}
				else
				{
					++iter;
				}
			}

			if ( bDelete )
			{
				//call UtilitiesClass to cleanup the ini
				m_pUtilityClass->CleanupIni();
			}
		}
	}



	mbPromptForArguments = FALSE;
	m_Arguments.Empty ();
	m_Command.Empty ();
	m_MenuText.Empty ();
	m_WorkingDirectory.Empty ();

	SetApplyState (FALSE);
	SetDeleteState (FALSE);
	UpdateData (FALSE);
}

void SVUtilitiesCustomizeDialog::OnCustomizeCommandSelect() 
{
	SVFileNameClass	svfncCommand;

	svfncCommand.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncCommand.SelectFile())
	{
		m_Command = svfncCommand.GetFullFileName().c_str();
		SetApplyState(TRUE);
		UpdateData(FALSE);
	}
}

void SVUtilitiesCustomizeDialog::OnCustomizeDirectorySelect() 
{
	SVFileNameClass	svfncWorkingDirectory;

	svfncWorkingDirectory.SetFileType(SV_DEFAULT_FILE_TYPE);

	if (svfncWorkingDirectory.SelectPath())
	{
		m_WorkingDirectory = svfncWorkingDirectory.GetPathName().c_str();
		SetApplyState (TRUE);
		UpdateData (FALSE);
	}
}

void SVUtilitiesCustomizeDialog::OnKillfocusCustomizeMenutext() 
{
	GetDlgItem(IDC_CUSTOMIZE_MENUTEXT)->GetWindowText(m_MenuText);
	UpdateData (FALSE);
}
