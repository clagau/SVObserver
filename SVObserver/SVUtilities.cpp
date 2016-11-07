//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesClass
//* .File Name       : $Workfile:   SVUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUtilities.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVUtilitiesCustomizeDialog.h"
#include "SVUtilityArgumentDialog.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVUtilityIniClass.h"
#include "SVObserver.h"
#include "SVStatusLibrary/MessageManager.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const TCHAR* const cUtilityKey				= _T( "HKEY_CURRENT_USER\\Software\\%s\\%s\\Utilities");
const TCHAR* const cUtilityNr				= _T("Utility%d");
const TCHAR* const cDisplayName				= _T("DisplayName");
const TCHAR* const cCommandName				= _T("Command");
const TCHAR* const cArgumentsName			= _T("Arguments");
const TCHAR* const cWorkingDirectoryName	= _T("Working Directory");
const TCHAR* const cPromptName				= _T("Prompt For Arguments");
const TCHAR* const cMenuIDName				= _T("Menu Id");
const TCHAR* const cGeneralName				= _T("General");
const TCHAR* const cHighestUtilityIndex		= _T("HighestUtilityIndex");

typedef struct UtilityInfoTag
{
	BOOL    bUtilityFound;
	CString szUtilityName;
	CString szCommand;
	CString szArguments;
	CString szWorkingDirectory;
	BOOL    bPromptForArguments;
	UINT    uiId;
} UTILITYINFO, *PUTILITYINFO;
#pragma endregion Declarations

SVUtilitiesClass::SVUtilitiesClass()
{
	m_UtilityKey = SvUl_SF::Format(cUtilityKey, AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName );
}

SVUtilitiesClass::~SVUtilitiesClass()
{
}

void SVUtilitiesClass::RunUtility(SVSecurityManager* pAccess, UINT uiUtilityId)
{
	SVRegistryClass reg( m_UtilityKey.c_str() );
	UTILITYINFO utilInfo;
	SVUtilityArgumentDialogClass dlg;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());

	utilInfo.bUtilityFound = FALSE;
	utilInfo.uiId = uiUtilityId;

	if ( ID_EXTRAS_UTILITIES_LIMIT != uiUtilityId )
	{
		std::map<UINT, SVUtilityIniClass>::iterator iter;

		iter = pApp->m_UtilityMenu.find(uiUtilityId);
		if ( iter != pApp->m_UtilityMenu.end() )
		{
			SVUtilityIniClass l_Struct;
			l_Struct = iter->second;

		  if ( 0 == l_Struct.m_csPromptForArguments.Left(1).CompareNoCase(_T("Y")) )
		  {
			  utilInfo.bPromptForArguments = true;
		  }
		  else
		  {
			  utilInfo.bPromptForArguments = false;
		  }
		  utilInfo.bUtilityFound = true;
		  utilInfo.szArguments = l_Struct.m_csArguments;
		  utilInfo.szCommand = l_Struct.m_csCommand;
		  utilInfo.szUtilityName = l_Struct.m_csDisplayName;
		  utilInfo.szWorkingDirectory = l_Struct.m_csWorkingDirectory;
	  }
  }

	//note: need to prompt for arguments!
	if (utilInfo.bUtilityFound)
	{
		if (utilInfo.bPromptForArguments)
		{
			if (IDOK == dlg.DoModal())
			{
				utilInfo.szArguments.Empty();
				utilInfo.szArguments = dlg.mszArguments;
			}
		}

		if( pAccess->SVCreateProcess(utilInfo.szCommand, utilInfo.szWorkingDirectory, utilInfo.szArguments ) )
		{
			SVStringArray msgList;
			msgList.push_back(SVString(utilInfo.szUtilityName));
			msgList.push_back(SVString(utilInfo.szCommand));
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnableStart_Utility, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10237 );
		}
	}
}

BOOL SVUtilitiesClass::SetupUtilities(CMenu *pMenu)
{
	SVUtilitiesCustomizeDialogClass dlg;

	ClearMenu (pMenu);

	dlg.mszUtilityKey = m_UtilityKey.c_str();
	if ( IDOK == dlg.DoModal() )
	{	
		UpdateIni();
	}
	return LoadMenu(pMenu);
}

BOOL SVUtilitiesClass::ClearMenu(CMenu *pMenu)
{
	while (pMenu->GetMenuItemCount () > 2)
	{
		pMenu->RemoveMenu (2, MF_BYPOSITION);
	}
	return true;
}

BOOL SVUtilitiesClass::LoadMenu(CMenu *pMenu)
{
	if ( !LoadMenuFromINI(pMenu) )
	{
		CleanupIni();
		LoadMenuFromINI(pMenu);
	}
	return true;
}

CMenu *SVUtilitiesClass::FindSubMenuByName(CMenu *pMenu, CString &szName)
{
	if (nullptr != pMenu)
	{
		int iLimit = pMenu->GetMenuItemCount ();
		for (int i = 0; i < iLimit; i++)
		{
			CString szMenuText;
			pMenu->GetMenuString(i, szMenuText, MF_BYPOSITION);
			if (szMenuText == szName)
			{
				return pMenu->GetSubMenu(i);
			}
			else
			{
				if (pMenu->GetSubMenu(i))
				{
					CMenu* pMatchingMenu = FindSubMenuByName (pMenu->GetSubMenu(i), szName);
					if (pMatchingMenu)
					{
						return pMatchingMenu;
					}
				}
			}
		}
	}
	return nullptr;
}

BOOL SVUtilitiesClass::LoadMenuFromINI(CMenu *pMenu)
{
	BOOL bRet = true;
	SvOml::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );
	int l_iHighestIndex =0;
	int iId = ID_EXTRAS_UTILITIES_BASE;

	ClearMenu(pMenu);

	SVString UtilName;
	CString Stanza;
	LPTSTR pKeyName = nullptr;
	DWORD dwId = 0;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());

	if( !( pApp->m_UtilityMenu.empty() ) )
	{
		pApp->m_UtilityMenu.clear();
	}
	
	l_iHighestIndex = UtilityIni.GetValueInt( cGeneralName, cHighestUtilityIndex, 0 );

	for ( int i = 0; (i < l_iHighestIndex) && bRet; i++ )
	{
		SVUtilityIniClass l_Struct;
		Stanza.Format( cUtilityNr, i+1 );
		UtilName = UtilityIni.GetValueString(Stanza,cDisplayName, _T("") );
		if ( UtilName.empty() )
		{
			//not found, set bRet = FALSE - will cause a clean to happen
			bRet = false;
		}
		else
		{
			iId++;
			l_Struct.m_csDisplayName = UtilName.c_str();

			SVString Value;
			Value = UtilityIni.GetValueString(Stanza, cCommandName, _T(""));
			l_Struct.m_csCommand = Value.c_str();

			Value = UtilityIni.GetValueString(Stanza, cArgumentsName, _T(""));
			l_Struct.m_csArguments = Value.c_str();

			Value = UtilityIni.GetValueString(Stanza, cWorkingDirectoryName, _T(""));
			l_Struct.m_csWorkingDirectory = Value.c_str();

			Value = UtilityIni.GetValueString(Stanza, cPromptName, _T(""));
			l_Struct.m_csPromptForArguments = Value.c_str();
			dwId = iId;

			pApp->m_UtilityMenu[(UINT)dwId] = l_Struct;

			pMenu->AppendMenu (MF_STRING, (UINT) dwId, UtilName.c_str());
		}
	}
	return bRet;
}

BOOL SVUtilitiesClass::CleanupIni()
{  //this function will cleanup the utility ini file.  
	int l_iHighestIndex = 0;

	CString Stanza;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());
	CString csIniFile = SvStl::GlobalPath::Inst().GetSVUtilityIniPath() ;
	SvOml::SVOINIClass UtilityIni( static_cast<LPCTSTR> (csIniFile) );

	CStdioFile file;

	//read each line.  For each Stanza that is [Utility  add 1 to number of actual
	if ( file.Open(csIniFile, CFile::modeRead | CFile::shareDenyNone ) )
	{
		CString sLine;
		while (file.ReadString(sLine))
		{
			if ( !sLine.IsEmpty() )
			{
				if ( sLine.GetAt(0) == _T('[') )
				{
					if ( 0 == sLine.Left(3).Compare(_T("[Ut")) )
					{
						l_iHighestIndex++;
					}
				}
			}
		}
		file.Close();
	}

	pApp->m_UtilityMenu.clear();

	//find all utilites in the INI file and store into the deque

	BOOL bDone = false;
	int iUtilityIndex = 0;
	SVString Value;
	int l_iProcessedUtility = 0;
	int iId = ID_EXTRAS_UTILITIES_BASE;

	while ( !bDone )
	{
		SVUtilityIniClass l_Struct;
		iUtilityIndex++;
		Stanza.Format( cUtilityNr, iUtilityIndex);
		Value = UtilityIni.GetValueString( Stanza, cDisplayName, _T("") );
		if ( !Value.empty() )
		{
			//found a utility with the current index. put into map
			l_iProcessedUtility++;
			l_Struct.m_csDisplayName = Value.c_str();
			Value = UtilityIni.GetValueString( Stanza, cCommandName, _T("") );
			l_Struct.m_csCommand = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, cArgumentsName, _T("") );
			l_Struct.m_csArguments = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, cWorkingDirectoryName, _T("") );
			l_Struct.m_csWorkingDirectory = Value.c_str();

			Value =  UtilityIni.GetValueString( Stanza, cPromptName, _T("") );
			l_Struct.m_csPromptForArguments = Value.c_str();

			//add struct to the map
			pApp->m_UtilityMenu[(UINT)iId] = l_Struct;
			iId++;
		}
		if ( l_iProcessedUtility == l_iHighestIndex )
		{
			bDone = TRUE;
		}
	}

	//destroy contents of ini file.
	FILE *fp;
	if ( nullptr != (fp = fopen(SvStl::GlobalPath::Inst().GetSVUtilityIniPath(), "w")) )
	{
		fclose(fp);
	}

	//write out ini file

	UtilityIni.SetValueInt( _T("General"), _T("HighestUtilityIndex"), l_iHighestIndex );

	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;

	iId = ID_EXTRAS_UTILITIES_BASE;

	for ( int i = 1; i <= l_iHighestIndex; i++ )
	{
		CString Stanza;
		Stanza.Format( cUtilityNr, i );

		iter = pApp->m_UtilityMenu.find((UINT)(iId + i-1));
		if ( iter != pApp->m_UtilityMenu.end() )
		{
			l_Struct = iter->second;
		
			UtilityIni.SetValueString( Stanza, cDisplayName, static_cast<LPCTSTR> (l_Struct.m_csDisplayName) );
			UtilityIni.SetValueString( Stanza, cCommandName, static_cast<LPCTSTR> (l_Struct.m_csCommand) );
			UtilityIni.SetValueString( Stanza, cArgumentsName, static_cast<LPCTSTR> (l_Struct.m_csArguments) );
			UtilityIni.SetValueString( Stanza, cWorkingDirectoryName, static_cast<LPCTSTR> (l_Struct.m_csWorkingDirectory) );
			UtilityIni.SetValueString( Stanza, cPromptName, static_cast<LPCTSTR> (l_Struct.m_csPromptForArguments) );
		}
	}

	return true;
}

BOOL SVUtilitiesClass::UpdateIni()
{
	BOOL bRet = TRUE;
	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());
	SvOml::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

	int iMapSize = static_cast<int>(pApp->m_UtilityMenu.size());

	iter = pApp->m_UtilityMenu.begin();

	int iCnt = 0;
	while ( iter != pApp->m_UtilityMenu.end() )
	{
		iCnt++;
		CString Stanza;
		Stanza.Format( cUtilityNr, iCnt);
		l_Struct = iter->second;

		//update ini entries for each utility
		UtilityIni.SetValueString( Stanza, cDisplayName, l_Struct.m_csDisplayName );
		UtilityIni.SetValueString( Stanza, cCommandName, l_Struct.m_csCommand );
		UtilityIni.SetValueString( Stanza, cArgumentsName, l_Struct.m_csArguments );
		UtilityIni.SetValueString( Stanza, cWorkingDirectoryName, l_Struct.m_csWorkingDirectory );
		UtilityIni.SetValueString( Stanza, cPromptName, l_Struct.m_csPromptForArguments );

		++iter;
	}
	return bRet;
}
