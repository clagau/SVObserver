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
#include "SVUtilitiesCustomizeDialog.h"
#include "SVMFCControls/SVUtilityArgumentDialog.h"
#include "SVLibrary/SVOIniClass.h"
#include "SVOCore/SVUtilityIniClass.h"
#include "SVObserver.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const TCHAR* const cUtilityNr				= _T("Utility%d");
const TCHAR* const cDisplayName				= _T("DisplayName");
const TCHAR* const cCommandName				= _T("Command");
const TCHAR* const cArgumentsName			= _T("Arguments");
const TCHAR* const cWorkingDirectoryName	= _T("WorkingDirectory");
const TCHAR* const cPromptName				= _T("PromptForArguments");
const TCHAR* const cMenuIDName				= _T("Menu Id");
const TCHAR* const cGeneralName				= _T("General");
const TCHAR* const cHighestUtilityIndex		= _T("HighestUtilityIndex");

typedef struct UtilityInfoTag
{
	BOOL		m_UtilityFound;
	SVString	m_UtilityName;
	SVString	m_Command;
	SVString	m_Arguments;
	SVString	m_WorkingDirectory;
	BOOL		m_PromptForArguments;
	UINT		m_ID;
} UTILITYINFO, *PUTILITYINFO;
#pragma endregion Declarations

SVUtilitiesClass::SVUtilitiesClass()
{
}

SVUtilitiesClass::~SVUtilitiesClass()
{
}

void SVUtilitiesClass::RunUtility(SVSecurityManager* pAccess, UINT uiUtilityId)
{
	UTILITYINFO utilInfo;
	SVUtilityArgumentDialogClass dlg;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());

	utilInfo.m_UtilityFound = FALSE;
	utilInfo.m_ID = uiUtilityId;

	if ( ID_EXTRAS_UTILITIES_LIMIT != uiUtilityId )
	{
		std::map<UINT, SVUtilityIniClass>::iterator iter;

		iter = pApp->m_UtilityMenu.find(uiUtilityId);
		if ( iter != pApp->m_UtilityMenu.end() )
		{
			SVUtilityIniClass l_Struct;
			l_Struct = iter->second;

		  if ( 0 == SvUl_SF::CompareNoCase( SvUl_SF::Left( l_Struct.m_PromptForArguments, 1), SVString( _T("Y") )) )
		  {
			  utilInfo.m_PromptForArguments = true;
		  }
		  else
		  {
			  utilInfo.m_PromptForArguments = false;
		  }
		  utilInfo.m_UtilityFound = true;
		  utilInfo.m_Arguments = l_Struct.m_Arguments;
		  utilInfo.m_Command = l_Struct.m_Command;
		  utilInfo.m_UtilityName = l_Struct.m_DisplayName;
		  utilInfo.m_WorkingDirectory = l_Struct.m_WorkingDirectory;
	  }
  }

	//note: need to prompt for arguments!
	if (utilInfo.m_UtilityFound)
	{
		if (utilInfo.m_PromptForArguments)
		{
			if (IDOK == dlg.DoModal())
			{
				utilInfo.m_Arguments.clear();
				utilInfo.m_Arguments = dlg.mszArguments;
			}
		}

		if( pAccess->SVCreateProcess(utilInfo.m_Command.c_str(), utilInfo.m_WorkingDirectory.c_str(), utilInfo.m_Arguments.c_str() ) )
		{
			SVStringVector msgList;
			msgList.push_back(SVString(utilInfo.m_UtilityName));
			msgList.push_back(SVString(utilInfo.m_Command));
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnableStart_Utility, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10237 );
		}
	}
}

BOOL SVUtilitiesClass::SetupUtilities(CMenu *pMenu)
{
	SVUtilitiesCustomizeDialogClass dlg;

	ClearMenu (pMenu);

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

CMenu *SVUtilitiesClass::FindSubMenuByName(CMenu *pMenu, LPCTSTR Name)
{
	if (nullptr != pMenu)
	{
		int iLimit = pMenu->GetMenuItemCount ();
		for (int i = 0; i < iLimit; i++)
		{
			CString MenuText;
			pMenu->GetMenuString(i, MenuText, MF_BYPOSITION);
			if (MenuText == Name)
			{
				return pMenu->GetSubMenu(i);
			}
			else
			{
				if (pMenu->GetSubMenu(i))
				{
					CMenu* pMatchingMenu = FindSubMenuByName (pMenu->GetSubMenu(i), Name);
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
	SvLib::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );
	int l_iHighestIndex =0;
	int iId = ID_EXTRAS_UTILITIES_BASE;

	ClearMenu(pMenu);

	SVString UtilName;
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
		SVString Text = SvUl_SF::Format( cUtilityNr, i+1 );
		UtilName = UtilityIni.GetValueString( Text.c_str(), cDisplayName, _T("") );
		if ( UtilName.empty() )
		{
			//not found, set bRet = FALSE - will cause a clean to happen
			bRet = false;
		}
		else
		{
			iId++;
			l_Struct.m_DisplayName = UtilName.c_str();

			SVString Value;
			Value = UtilityIni.GetValueString(Text.c_str(), cCommandName, _T(""));
			l_Struct.m_Command = Value.c_str();

			Value = UtilityIni.GetValueString(Text.c_str(), cArgumentsName, _T(""));
			l_Struct.m_Arguments = Value.c_str();

			Value = UtilityIni.GetValueString(Text.c_str(), cWorkingDirectoryName, _T(""));
			l_Struct.m_WorkingDirectory = Value.c_str();

			Value = UtilityIni.GetValueString(Text.c_str(), cPromptName, _T(""));
			l_Struct.m_PromptForArguments = Value.c_str();
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
	SVString IniFile = SvStl::GlobalPath::Inst().GetSVUtilityIniPath() ;
	SvLib::SVOINIClass UtilityIni( IniFile.c_str() );

	CStdioFile file;

	//read each line.  For each Stanza that is [Utility  add 1 to number of actual
	if ( file.Open(IniFile.c_str(), CFile::modeRead | CFile::shareDenyNone ) )
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
			l_Struct.m_DisplayName = Value.c_str();
			Value = UtilityIni.GetValueString( Stanza, cCommandName, _T("") );
			l_Struct.m_Command = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, cArgumentsName, _T("") );
			l_Struct.m_Arguments = Value.c_str();

			Value = UtilityIni.GetValueString( Stanza, cWorkingDirectoryName, _T("") );
			l_Struct.m_WorkingDirectory = Value.c_str();

			Value =  UtilityIni.GetValueString( Stanza, cPromptName, _T("") );
			l_Struct.m_PromptForArguments = Value.c_str();

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

	std::map<UINT,SVUtilityIniClass>::const_iterator Iter;

	iId = ID_EXTRAS_UTILITIES_BASE;

	for( int i = 1; i <= l_iHighestIndex; i++ )
	{
		SVString Text = SvUl_SF::Format( cUtilityNr, i );

		Iter = pApp->m_UtilityMenu.find((UINT) (iId + i-1));
		if( pApp->m_UtilityMenu.end() != Iter )
		{
			const SVUtilityIniClass rUtilityStruct( Iter->second );
		
			UtilityIni.SetValueString( Stanza, cDisplayName, rUtilityStruct.m_DisplayName.c_str() );
			UtilityIni.SetValueString( Stanza, cCommandName, rUtilityStruct.m_Command.c_str() );
			UtilityIni.SetValueString( Stanza, cArgumentsName, rUtilityStruct.m_Arguments.c_str() );
			UtilityIni.SetValueString( Stanza, cWorkingDirectoryName, rUtilityStruct.m_WorkingDirectory.c_str() );
			UtilityIni.SetValueString( Stanza, cPromptName, rUtilityStruct.m_PromptForArguments.c_str() );
		}
	}

	return true;
}

BOOL SVUtilitiesClass::UpdateIni()
{
	BOOL bRet = TRUE;
	SVObserverApp* pApp = static_cast<SVObserverApp*> (AfxGetApp());
	SvLib::SVOINIClass UtilityIni( SvStl::GlobalPath::Inst().GetSVUtilityIniPath() );

	int iMapSize = static_cast<int>(pApp->m_UtilityMenu.size());

	std::map<UINT,SVUtilityIniClass>::const_iterator Iter( pApp->m_UtilityMenu.begin() );

	int iCnt = 0;
	while ( pApp->m_UtilityMenu.end() != Iter  )
	{
		iCnt++;
		SVString Text = SvUl_SF::Format( cUtilityNr, iCnt);
		const SVUtilityIniClass& rUtilityStruct( Iter->second );

		//update ini entries for each utility
		UtilityIni.SetValueString( Text.c_str(), cDisplayName, rUtilityStruct.m_DisplayName.c_str() );
		UtilityIni.SetValueString( Text.c_str(), cCommandName, rUtilityStruct.m_Command.c_str() );
		UtilityIni.SetValueString( Text.c_str(), cArgumentsName, rUtilityStruct.m_Arguments.c_str() );
		UtilityIni.SetValueString( Text.c_str(), cWorkingDirectoryName, rUtilityStruct.m_WorkingDirectory.c_str() );
		UtilityIni.SetValueString( Text.c_str(), cPromptName, rUtilityStruct.m_PromptForArguments.c_str() );

		++Iter;
	}
	return bRet;
}
