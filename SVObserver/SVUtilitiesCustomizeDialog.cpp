//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesCustomizeDialogClass
//* .File Name       : $Workfile:   SVUtilitiesCustomizeDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUtilitiesCustomizeDialog.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVObserver.h"
#include "SVUtilityIniClass.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* Globals */
DWORD gmdwLastId;
CString gmszUtilityKeyStr;
CString gmszIdValueNameStr;

SVUtilitiesCustomizeDialogClass::SVUtilitiesCustomizeDialogClass(CWnd* pParent /*=nullptr*/)
	: CDialog(SVUtilitiesCustomizeDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVUtilitiesCustomizeDialogClass)
	mszArguments = _T("");
	mszCommand = _T("");
	mszMenuText = _T("");
	mbPromptForArguments = FALSE;
	mszWorkingDirectory = _T("");
	//}}AFX_DATA_INIT
}

SVUtilitiesCustomizeDialogClass::~SVUtilitiesCustomizeDialogClass()
{
}

void SVUtilitiesCustomizeDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVUtilitiesCustomizeDialogClass)
	DDX_Text(pDX, IDC_CUSTOMIZE_ARGUMENTS, mszArguments);
	DDX_Text(pDX, IDC_CUSTOMIZE_COMMAND, mszCommand);
	DDX_Text(pDX, IDC_CUSTOMIZE_MENUTEXT, mszMenuText);
	DDX_Check(pDX, IDC_CUSTOMIZE_PROMPTFORARGUMENTS, mbPromptForArguments);
	DDX_Text(pDX, IDC_CUSTOMIZE_WORKINGDIRECTORY, mszWorkingDirectory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVUtilitiesCustomizeDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVUtilitiesCustomizeDialogClass)
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

/////////////////////////////////////////////////////////////////////////////
// SVUtilitiesCustomizeDialogClass message handlers
BOOL CALLBACK LoadComboBox (CString &szKeyName, LPVOID pVoid)
{
	CString szKey;

	szKey.Format (_T("%s\\%s"), gmszUtilityKeyStr, szKeyName);
	SVRegistryClass reg(szKey);

	reg.SetRegistryValue (gmszIdValueNameStr, gmdwLastId++);

	CComboBox *pBox = (CComboBox *) pVoid;
	pBox->AddString (szKeyName);

	return TRUE;
}

BOOL SVUtilitiesCustomizeDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	mszCommandValueName = _T("Command");
	mszArgumentsValueName = _T("Arguments");
	mszWorkingDirectoryValueName = _T("Working Directory");
	mszPromptValueName = _T("Prompt For Arguments");
	mszIdValueName = _T("Menu Id");

	gmszIdValueNameStr = mszIdValueName;
	gmszUtilityKeyStr = mszUtilityKey;
	gmdwLastId = ID_EXTRAS_UTILITIES_BASE;

	SVRegistryClass reg(mszUtilityKey); // Note: mszUtilityKey MUST be set in calling routine before DoModal() is called!
	CComboBox *pBox;

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	//load the combobox with the info in the map.

	std::map<UINT, SVUtilityIniClass>::iterator iter;

	iter = pApp->m_UtilityMenu.begin();

	while ( iter != pApp->m_UtilityMenu.end() )
	{
		SVUtilityIniClass l_Struct;
		l_Struct = iter->second;
		pBox->AddString(l_Struct.m_csDisplayName);

		++iter;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVUtilitiesCustomizeDialogClass::OnSelEndOkCustomizeMenuText() 
{
	CString szKey;
	CComboBox *pBox;
	int iCurSel;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	iCurSel = pBox->GetCurSel ();

	if (iCurSel > -1)
	{
		pBox->GetLBText (iCurSel, mszMenuText);

		std::map<UINT, SVUtilityIniClass>::iterator iter;

		iter = pApp->m_UtilityMenu.begin();

		BOOL bDone = FALSE;

		while ( (iter != pApp->m_UtilityMenu.end()) && !bDone )
		{
			SVUtilityIniClass l_Struct;
			l_Struct = iter->second;
			if ( l_Struct.m_csDisplayName == mszMenuText)
			{
				mszCommand = l_Struct.m_csCommand;
				mszArguments = l_Struct.m_csArguments;
				mszWorkingDirectory = l_Struct.m_csWorkingDirectory;
				if ( l_Struct.m_csPromptForArguments.Left(1).CompareNoCase("Y") == 0 )
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

void SVUtilitiesCustomizeDialogClass::OnEditChangeCustomizeMenuText() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialogClass::SetApplyState(BOOL bEnabled)
{
	CButton *pApply;

	pApply = (CButton *) GetDlgItem (IDC_APPLY);
	pApply->EnableWindow (bEnabled);
	SetDeleteState (!bEnabled);
}

void SVUtilitiesCustomizeDialogClass::OnChangeCustomizeCommand() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialogClass::OnChangeCustomizeArguments() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialogClass::OnChangeCustomizeWorkingDirectory() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialogClass::OnCustomizePromptForArguments() 
{
	SetApplyState (TRUE);
}

void SVUtilitiesCustomizeDialogClass::SetDeleteState(BOOL bEnabled)
{
	CButton *pDelete;

	pDelete = (CButton *) GetDlgItem (IDC_CUSTOMIZE_DELETE);
	pDelete->EnableWindow (bEnabled);
}

void SVUtilitiesCustomizeDialogClass::OnApply() 
{
	CString szKey;
	CComboBox *pBox;

	UpdateData (TRUE);
	if (!mszMenuText.IsEmpty ())
	{

		//check to see if it is already in the map, if not add it
		SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
		std::map<UINT, SVUtilityIniClass>::iterator iter;

		iter = pApp->m_UtilityMenu.begin();
		BOOL bFound = FALSE;

		while ( (iter != pApp->m_UtilityMenu.end()) && !bFound )
		{
			SVUtilityIniClass l_Struct;
			l_Struct = iter->second;
			if ( l_Struct.m_csDisplayName == mszMenuText)
			{
				l_Struct.m_csArguments = mszArguments;
				l_Struct.m_csCommand = mszCommand;
				l_Struct.m_csWorkingDirectory = mszWorkingDirectory;
				if ( mbPromptForArguments )
				{
					l_Struct.m_csPromptForArguments = "Y";
				}
				else
				{
					l_Struct.m_csPromptForArguments = "N";
				}
				iter->second = l_Struct;
				bFound = TRUE;
			}
			++iter;
		}
		if ( !bFound )
		{

			pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
			pBox->AddString (mszMenuText);

			//must be a new utility
			std::map<UINT, SVUtilityIniClass>::reverse_iterator riter;
			SVUtilityIniClass l_Struct;
			riter = pApp->m_UtilityMenu.rbegin();
			UINT uintVal = riter->first;
			l_Struct.m_csArguments = mszArguments;
			l_Struct.m_csCommand = mszCommand;
			l_Struct.m_csDisplayName = mszMenuText;
			if ( mbPromptForArguments )
			{
				l_Struct.m_csPromptForArguments = "Y";
			}
			else
			{
				l_Struct.m_csPromptForArguments = "N";
			}
			l_Struct.m_csWorkingDirectory = mszWorkingDirectory;

			//add new utility to map
			pApp->m_UtilityMenu[uintVal+1] = l_Struct;

			SvOml::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

			int iCnt = UtilityIni.GetValueInt("General","HighestUtilityIndex",0);
			iCnt++;
			UtilityIni.SetValueInt("General","HighestUtilityIndex",iCnt);
			CString Stanza;
			Stanza.Format("Utility%d",iCnt);
			UtilityIni.SetValueString( Stanza, _T("DisplayName"), l_Struct.m_csDisplayName );
			UtilityIni.SetValueString( Stanza, _T("Command"), l_Struct.m_csCommand );
			UtilityIni.SetValueString( Stanza, _T("Arguments"), l_Struct.m_csArguments );
			UtilityIni.SetValueString( Stanza, _T("WorkingDirectory"), l_Struct.m_csWorkingDirectory );
			UtilityIni.SetValueString( Stanza, _T("PromptForArguments"), l_Struct.m_csPromptForArguments );
		}
	}
	SetApplyState (FALSE);

	if (mszMenuText.IsEmpty ())
		SetDeleteState (FALSE);
}

void SVUtilitiesCustomizeDialogClass::OnOK() 
{
	CButton *pApply;

	pApply = (CButton *) GetDlgItem (IDC_APPLY);
	if (pApply->IsWindowEnabled ())
		OnApply();

	CDialog::OnOK();
}

void SVUtilitiesCustomizeDialogClass::OnCustomizeDelete() 
{
	CString szKey;
	CComboBox *pBox;

	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	SvOml::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

	UpdateData (TRUE);

	pBox = (CComboBox *) GetDlgItem (IDC_CUSTOMIZE_MENUTEXT);
	pBox->DeleteString (pBox->FindStringExact (0, mszMenuText));

	szKey.Format (_T("%s\\%s"), mszUtilityKey, mszMenuText);

	CString Stanza;
	int iUtlCnt = 0;

	BOOL bFound = FALSE;

	while ( !bFound )
	{
		SVString Value;
		iUtlCnt++;
		Stanza.Format(_T("Utility%d"), iUtlCnt);
		Value = UtilityIni.GetValueString( Stanza, _T("DisplayName"), _T("") );

		if ( Value == static_cast<LPCTSTR> (mszMenuText) )
		{
			//found value.  delete from INI
			UtilityIni.SetValueString( Stanza, nullptr, _T("") );
			int iVal = UtilityIni.GetValueInt( _T("General"), _T("HighestUtilityIndex"), 0);
			UtilityIni.SetValueInt( _T("General"), _T("HighestUtilityIndex"), (iVal-1) );
			bFound = TRUE;

			//find and delete from map
			iter = pApp->m_UtilityMenu.begin();
			BOOL bDelete = FALSE;

			while ( (!bDelete) && (iter != pApp->m_UtilityMenu.end()) )
			{
				l_Struct = iter->second;
				if ( l_Struct.m_csDisplayName == mszMenuText )
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
	mszArguments.Empty ();
	mszCommand.Empty ();
	mszMenuText.Empty ();
	mszWorkingDirectory.Empty ();
	muiId = 0;


	SetApplyState (FALSE);
	SetDeleteState (FALSE);
	UpdateData (FALSE);
}

void SVUtilitiesCustomizeDialogClass::OnCustomizeCommandSelect() 
{
	SVFileNameClass	svfncCommand;

	svfncCommand.SetFileType(SV_DEFAULT_FILE_TYPE);
	if (svfncCommand.SelectFile())
	{
		mszCommand = svfncCommand.GetFullFileName();
		SetApplyState(TRUE);
		UpdateData(FALSE);
	}
}

void SVUtilitiesCustomizeDialogClass::OnCustomizeDirectorySelect() 
{
	SVFileNameClass	svfncWorkingDirectory;

	svfncWorkingDirectory.SetFileType(SV_DEFAULT_FILE_TYPE);

	if (svfncWorkingDirectory.SelectPath())
	{
		mszWorkingDirectory = svfncWorkingDirectory.GetPathName();
		SetApplyState (TRUE);
		UpdateData (FALSE);
	}
}

void SVUtilitiesCustomizeDialogClass::OnKillfocusCustomizeMenutext() 
{
	GetDlgItem(IDC_CUSTOMIZE_MENUTEXT)->GetWindowText(mszMenuText);
	UpdateData (FALSE);
}
