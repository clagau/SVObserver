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
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString gmszUtilityKey;
CString gmszCommandValueName;
CString gmszArgumentsValueName;
CString gmszWorkingDirectoryValueName;
CString gmszPromptValueName;
CString gmszIdValueName;

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVUtilitiesClass::SVUtilitiesClass()
{
	gmszUtilityKey.Format( _T( "HKEY_CURRENT_USER\\Software\\%s\\%s\\Utilities" ),
		AfxGetApp()->m_pszRegistryKey,
		AfxGetApp()->m_pszAppName );

	gmszCommandValueName = _T("Command");
	gmszArgumentsValueName = _T("Arguments");
	gmszWorkingDirectoryValueName = _T("Working Directory");
	gmszPromptValueName = _T("Prompt For Arguments");
	gmszIdValueName = _T("Menu Id");
}

SVUtilitiesClass::~SVUtilitiesClass()
{
}

BOOL CALLBACK FindUtilityById (CString &szKeyName, LPVOID pVoid)
{
	CString szKey;
	PUTILITYINFO pUtilInfo = (PUTILITYINFO) pVoid;

	szKey.Format (_T("%s\\%s"), gmszUtilityKey, szKeyName);

	SVRegistryClass reg(szKey);
	DWORD dwId, dwPromptForArguments;

	if (reg.GetRegistryValue (gmszIdValueName, &dwId))
	{
		if (dwId == pUtilInfo->uiId)
		{
			SVString l_Command;
			SVString l_Arguments;
			SVString l_WorkingDirectory;

			pUtilInfo->uiId = (UINT) dwId;
			pUtilInfo->szUtilityName = szKeyName;
			reg.GetRegistryValue( gmszCommandValueName, l_Command );
			reg.GetRegistryValue( gmszArgumentsValueName, l_Arguments );
			reg.GetRegistryValue( gmszWorkingDirectoryValueName, l_WorkingDirectory );
			reg.GetRegistryValue( gmszPromptValueName, &dwPromptForArguments );

			pUtilInfo->bPromptForArguments = (BOOL) dwPromptForArguments;
			pUtilInfo->bUtilityFound = TRUE;
			pUtilInfo->szCommand = l_Command.c_str();
			pUtilInfo->szArguments = l_Arguments.c_str();
			pUtilInfo->szWorkingDirectory = l_WorkingDirectory.c_str();

			return false;
		}
	}
	return true;
}

void SVUtilitiesClass::RunUtility(SVSecurityManager* pAccess, UINT uiUtilityId)
{
	SVRegistryClass reg(gmszUtilityKey);
	UTILITYINFO utilInfo;
	SVUtilityArgumentDialogClass dlg;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnableStart_Utility, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10237 );
		}
	}
}

BOOL SVUtilitiesClass::SetupUtilities(CMenu *pMenu)
{
	SVUtilitiesCustomizeDialogClass dlg;

	ClearMenu (pMenu);

	dlg.mszUtilityKey = gmszUtilityKey;
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
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	if( !( pApp->m_UtilityMenu.empty() ) )
	{
		pApp->m_UtilityMenu.clear();
	}
	
	l_iHighestIndex = UtilityIni.GetValueInt(_T("General"), _T("HighestUtilityIndex"), 0);

	for ( int i = 0; (i < l_iHighestIndex) && bRet; i++ )
	{
		SVUtilityIniClass l_Struct;
		Stanza.Format(_T("Utility%d"), i+1);
		UtilName = UtilityIni.GetValueString(Stanza, _T("DisplayName"), _T("") );
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
			Value = UtilityIni.GetValueString(Stanza, _T("Command"), _T(""));
			l_Struct.m_csCommand = Value.c_str();

			UtilityIni.GetValueString(Stanza, _T("Arguments"), _T(""));
			l_Struct.m_csArguments = Value.c_str();

			UtilityIni.GetValueString(Stanza, _T("WorkingDirectory"), _T(""));
			l_Struct.m_csWorkingDirectory = Value.c_str();

			UtilityIni.GetValueString(Stanza, _T("PromptForArguments"), _T(""));
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
		Stanza.Format(_T("Utility%d"), iUtilityIndex);
		Value = UtilityIni.GetValueString( Stanza, _T("DisplayName"), _T("") );
		if ( !Value.empty() )
		{
			//found a utility with the current index. put into map
			l_iProcessedUtility++;
			l_Struct.m_csDisplayName = Value.c_str();
			Value = UtilityIni.GetValueString( Stanza, _T("Command"), _T("") );
			l_Struct.m_csCommand = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, _T("Arguments"), _T("") );
			l_Struct.m_csArguments = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, _T("WorkingDirectory"), _T("") );
			l_Struct.m_csWorkingDirectory = Value.c_str();

			Value =  UtilityIni.GetValueString( Stanza, _T("PromptForArguments"), _T("") );
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
		Stanza.Format(_T("Utility%d"), i);

		iter = pApp->m_UtilityMenu.find((UINT)(iId + i-1));
		if ( iter != pApp->m_UtilityMenu.end() )
		{
			l_Struct = iter->second;
		
			UtilityIni.SetValueString( Stanza, _T("DisplayName"), static_cast<LPCTSTR> (l_Struct.m_csDisplayName) );
			UtilityIni.SetValueString( Stanza, _T("Command"), static_cast<LPCTSTR> (l_Struct.m_csCommand) );
			UtilityIni.SetValueString( Stanza, _T("Arguments"), static_cast<LPCTSTR> (l_Struct.m_csArguments) );
			UtilityIni.SetValueString(Stanza, _T("WorkingDirectory"), static_cast<LPCTSTR> (l_Struct.m_csWorkingDirectory) );
			UtilityIni.SetValueString(Stanza, _T("PromptForArguments"), static_cast<LPCTSTR> (l_Struct.m_csPromptForArguments) );
		}
	}

	return true;
}

BOOL SVUtilitiesClass::UpdateIni()
{
	BOOL bRet = TRUE;
	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	SvOml::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

	int iMapSize = static_cast<int>(pApp->m_UtilityMenu.size());

	iter = pApp->m_UtilityMenu.begin();

	int iCnt = 0;
	while ( iter != pApp->m_UtilityMenu.end() )
	{
		iCnt++;
		CString Stanza;
		Stanza.Format(_T("Utility%d"), iCnt);
		l_Struct = iter->second;

		//update ini entries for each utility
		UtilityIni.SetValueString( Stanza, _T("DisplayName"), l_Struct.m_csDisplayName );
		UtilityIni.SetValueString( Stanza, _T("Command"), l_Struct.m_csCommand );
		UtilityIni.SetValueString( Stanza, _T("Arguments"), l_Struct.m_csArguments );
		UtilityIni.SetValueString( Stanza, _T("WorkingDirectory"), l_Struct.m_csWorkingDirectory );
		UtilityIni.SetValueString( Stanza, _T("PromptForArguments"), l_Struct.m_csPromptForArguments );

		++iter;
	}
	return bRet;
}
