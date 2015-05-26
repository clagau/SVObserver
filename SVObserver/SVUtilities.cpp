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

#include "stdafx.h"
#include "SVUtilities.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVUtilitiesCustomizeDialog.h"
#include "SVUtilityArgumentDialog.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVUtilityIniClass.h"
#include "SVObserver.h"

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
      pUtilInfo->szCommand = l_Command.ToString();
      pUtilInfo->szArguments = l_Arguments.ToString();
      pUtilInfo->szWorkingDirectory = l_WorkingDirectory.ToString();

      return FALSE;
    }
  }
  return TRUE;
}

void SVUtilitiesClass::RunUtility(SVSecurityManager* pAccess, UINT uiUtilityId)
{
  SVRegistryClass reg(gmszUtilityKey);
  UTILITYINFO utilInfo;
  SVUtilityArgumentDialogClass dlg;
  SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

  utilInfo.bUtilityFound = FALSE;
  utilInfo.uiId = uiUtilityId;
  
  if ( uiUtilityId == ID_EXTRAS_UTILITIES_LIMIT )
  {
	  // do the firmware stuff...
	  utilInfo.bUtilityFound = TRUE;
	  utilInfo.szArguments = pApp->m_csFirmwareArguments;
	  utilInfo.szCommand = pApp->m_csFirmwareCommand;
	  utilInfo.szWorkingDirectory = pApp->m_csFirmwareWorkingDir;
	  utilInfo.bPromptForArguments = FALSE;
  }
  else
  {
	  std::map<UINT, SVUtilityIniClass>::iterator iter;

	  iter = pApp->m_UtilityMenu.find(uiUtilityId);
	  if ( iter != pApp->m_UtilityMenu.end() )
	  {
		  SVUtilityIniClass l_Struct;
		  l_Struct = iter->second;

		  if ( l_Struct.m_csPromptForArguments.Left(1).CompareNoCase("Y")== 0 )
		  {
			  utilInfo.bPromptForArguments = TRUE;
		  }
		  else
		  {
			  utilInfo.bPromptForArguments = FALSE;
		  }
		  utilInfo.bUtilityFound = TRUE;
		  utilInfo.szArguments = l_Struct.m_csArguments;
		  utilInfo.szCommand = l_Struct.m_csCommand;
		  utilInfo.szUtilityName = l_Struct.m_csDisplayName;
		  utilInfo.szWorkingDirectory = l_Struct.m_csWorkingDirectory;
	  }
  }

  BOOL bUpdateFirmware = pApp->getShowUpdateFirmwareInMenu();

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
      CString msg;
      msg.Format (_T("Unable to start %s\n(%s).\n\nCheck Utility Properties."),
                  utilInfo.szUtilityName,
                  utilInfo.szCommand);
      ::MessageBox (HWND_DESKTOP, msg, "Failure", MB_OK);
    }
  }
}

BOOL SVUtilitiesClass::SetupUtilities(CMenu *pMenu)
{
  SVUtilitiesCustomizeDialogClass dlg;

  ClearMenu (pMenu);

  dlg.mszUtilityKey = gmszUtilityKey;
  if ( dlg.DoModal () == IDOK )
  {	
	UpdateIni();
  }
  return LoadMenu (pMenu);
}

BOOL SVUtilitiesClass::ClearMenu(CMenu *pMenu)
{
  while (pMenu->GetMenuItemCount () > 2)
  {
    pMenu->RemoveMenu (2, MF_BYPOSITION);
  }
  return TRUE;
}

BOOL CALLBACK LoadMenuItem (CString &szKeyName, LPVOID pVoid)
{
  CMenu *pMenu = (CMenu *) pVoid;
  CString szKey;
  DWORD dwId;

  szKey.Format (_T("%s\\%s"), gmszUtilityKey, szKeyName);

  SVRegistryClass reg(szKey);

  if (reg.GetRegistryValue (gmszIdValueName, &dwId))
  {
    pMenu->AppendMenu(MF_STRING, (UINT) dwId, szKeyName);
  }

  int iCount = pMenu->GetMenuItemCount();
  return TRUE;
}

BOOL SVUtilitiesClass::LoadMenu(CMenu *pMenu)
{
	if ( !LoadMenuFromINI(pMenu) )
	{
		CleanupIni();
		LoadMenuFromINI(pMenu);
	}
  return TRUE;
}

CMenu *SVUtilitiesClass::FindSubMenuByName(CMenu *pMenu, CString &szName)
{
  CString szMenuText;
  int i, iLimit;
  CMenu *pMatchingMenu;
  
  if (pMenu == NULL)
      return (CMenu*) NULL;
  
  iLimit = pMenu->GetMenuItemCount ();
  for (i = 0; i < iLimit; i++)
  {
    pMenu->GetMenuString (i, szMenuText, MF_BYPOSITION);
    if (szMenuText == szName)
    {
      return pMenu->GetSubMenu (i);
    }
    else
    {
      if (pMenu->GetSubMenu (i))
      {
        pMatchingMenu = FindSubMenuByName (pMenu->GetSubMenu (i), szName);
        if (pMatchingMenu)
        {
          return pMatchingMenu;
        }
      }
    }
  }
  return (CMenu *) NULL;
}

BOOL SVUtilitiesClass::LoadMenuFromINI(CMenu *pMenu)
{
	BOOL bRet = TRUE;
	SVOINIClass l_svIni;
	int l_iHighestIndex =0;
	int iId = ID_EXTRAS_UTILITIES_BASE;

	ClearMenu(pMenu);

	BSTR l_bstrVal = NULL;
	CString csString;
	CString csStanza;
	LPTSTR pKeyName = NULL;
	DWORD dwId = 0;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	if( !( pApp->m_UtilityMenu.empty() ) )
	{
		pApp->m_UtilityMenu.clear();
	}
	
	CString csIniName = "C:\\SVObserver\\bin\\SVUtility.ini";

	l_iHighestIndex = l_svIni.GetValueInt("General","HighestUtilityIndex",0,csIniName);

	for ( int i = 0; (i < l_iHighestIndex) && bRet; i++ )
	{
		SVUtilityIniClass l_Struct;
		csStanza.Format("Utility%d",i+1);
		l_svIni.GetValue(csStanza, "DisplayName","",&l_bstrVal, csIniName);
		csString = l_bstrVal;
		
		if ( csString.IsEmpty() )
		{
			//not found, set bRet = FALSE - will cause a clean to happen
			bRet = FALSE;
		}
		else
		{
			iId++;
			l_Struct.m_csDisplayName = csString;

			l_svIni.GetValue(csStanza, "Command", "", &l_bstrVal, csIniName);
			l_Struct.m_csCommand = l_bstrVal;

			l_svIni.GetValue(csStanza, "Arguments","",&l_bstrVal, csIniName);
			l_Struct.m_csArguments = l_bstrVal;

			l_svIni.GetValue(csStanza, "WorkingDirectory","",&l_bstrVal, csIniName);
			l_Struct.m_csWorkingDirectory = l_bstrVal;

			l_svIni.GetValue(csStanza, "PromptForArguments", "", &l_bstrVal, csIniName);
			l_Struct.m_csPromptForArguments = l_bstrVal;
			dwId = iId;
			::SysFreeString( l_bstrVal );
			l_bstrVal = NULL;

			pApp->m_UtilityMenu[(UINT)dwId] = l_Struct;

			pMenu->AppendMenu (MF_STRING, (UINT) dwId, csString);
		}
		
	}

	//check to see if the Update Firmware should be added to the menu
	if ( bRet )
	{
		if ( !pApp->m_csFirmwareCommand.IsEmpty() )
		{
			if ( pApp->getShowUpdateFirmwareInMenu() )
			{
				iId++;
				dwId = iId;
				pMenu->AppendMenu(MF_SEPARATOR,(UINT)dwId);
				csString = "Update Firmware";
				dwId = ID_EXTRAS_UTILITIES_LIMIT;
				pMenu->AppendMenu (MF_STRING, (UINT) dwId, csString);
			}
		}
	}
	return bRet;
}

BOOL SVUtilitiesClass::CleanupIni()
{  //this function will cleanup the utility ini file.  
	SVOINIClass l_svIni;
	int l_iHighestIndex =0;

	BSTR l_bstrVal = NULL;
	CString csString;
	CString csStanza;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	CString csIniFile = "C:\\SVObserver\\bin\\SVUtility.ini";

	CStdioFile file;

	//read each line.  For each Stanza that is [Utility  add 1 to number of actual
	if ( file.Open(csIniFile, CFile::modeRead | CFile::shareDenyNone ) )
	{
		CString sLine;
		while (file.ReadString(sLine))
		{
			if ( !sLine.IsEmpty() )
			{
				if ( sLine.GetAt(0) == '[' )
				{
					if ( sLine.Left(3).Compare("[Ut")== 0 )
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

	BOOL bDone = FALSE;
	int iUtilityIndex = 0;
	CString csValue;
	int l_iProcessedUtility = 0;
	int iId = ID_EXTRAS_UTILITIES_BASE;

	while ( !bDone )
	{
		l_bstrVal = NULL;
		SVUtilityIniClass l_Struct;
		iUtilityIndex++;
		csStanza.Format("Utility%d",iUtilityIndex);
		l_svIni.GetValue(csStanza,"DisplayName","",&l_bstrVal,csIniFile);
		csValue = l_bstrVal;
		::SysFreeString( l_bstrVal );
		l_bstrVal = NULL;
		if ( !csValue.IsEmpty() )
		{
			//found a utility with the current index. put into map
			l_iProcessedUtility++;
			l_Struct.m_csDisplayName = csValue;
			l_svIni.GetValue(csStanza,"Command","",&l_bstrVal, csIniFile);
			l_Struct.m_csCommand = l_bstrVal;
			::SysFreeString( l_bstrVal );
			l_bstrVal = NULL;

			l_svIni.GetValue(csStanza,"Arguments","",&l_bstrVal, csIniFile);
			l_Struct.m_csArguments = l_bstrVal;
			::SysFreeString( l_bstrVal );
			l_bstrVal = NULL;

			l_svIni.GetValue(csStanza,"WorkingDirectory","",&l_bstrVal, csIniFile);
			l_Struct.m_csWorkingDirectory = l_bstrVal;
			::SysFreeString( l_bstrVal );
			l_bstrVal = NULL;

			l_svIni.GetValue(csStanza,"PromptForArguments","",&l_bstrVal,csIniFile);
			l_Struct.m_csPromptForArguments = l_bstrVal;
			::SysFreeString( l_bstrVal );
			l_bstrVal = NULL;

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
	if ( (fp = fopen("C:\\SVObserver\\bin\\SVUtility.ini","w")) != NULL )
	{
		fclose(fp);
	}

	//write out ini file

	l_svIni.SetValue("General","HighestUtilityIndex",l_iHighestIndex,csIniFile);

	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;


	iId = ID_EXTRAS_UTILITIES_BASE;

	for ( int i = 1; i <= l_iHighestIndex; i++ )
	{
		CString csStanza;
		csStanza.Format("Utility%d",i);

		iter = pApp->m_UtilityMenu.find((UINT)(iId+i-1));
		if ( iter != pApp->m_UtilityMenu.end() )
		{
			l_Struct = iter->second;
		
			l_svIni.SetValue(csStanza,"DisplayName",l_Struct.m_csDisplayName,csIniFile);
			l_svIni.SetValue(csStanza,"Command",l_Struct.m_csCommand,csIniFile);
			l_svIni.SetValue(csStanza,"Arguments",l_Struct.m_csArguments,csIniFile);
			l_svIni.SetValue(csStanza,"WorkingDirectory",l_Struct.m_csWorkingDirectory,csIniFile);
			l_svIni.SetValue(csStanza,"PromptForArguments",l_Struct.m_csPromptForArguments,csIniFile);
		}
	}

	return TRUE;
}

BOOL SVUtilitiesClass::UpdateIni()
{
	BOOL bRet = TRUE;
	SVUtilityIniClass l_Struct;
	std::map<UINT,SVUtilityIniClass>::iterator iter;
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	SVOINIClass l_svIni;
	CString csIniName = "C:\\SVObserver\\bin\\SVUtility.ini";

	int iMapSize = static_cast<int>(pApp->m_UtilityMenu.size());

	iter = pApp->m_UtilityMenu.begin();

	int iCnt = 0;
	while ( iter != pApp->m_UtilityMenu.end() )
	{
		iCnt++;
		CString csStanza;
		csStanza.Format("Utility%d",iCnt);
		l_Struct = iter->second;

		//update ini entries for each utility
		l_svIni.SetValue(csStanza,"DisplayName",l_Struct.m_csDisplayName, csIniName);
		l_svIni.SetValue(csStanza,"Command",l_Struct.m_csCommand, csIniName);
		l_svIni.SetValue(csStanza,"Arguments",l_Struct.m_csArguments,csIniName);
		l_svIni.SetValue(csStanza,"WorkingDirectory",l_Struct.m_csWorkingDirectory,csIniName);
		l_svIni.SetValue(csStanza,"PromptForArguments",l_Struct.m_csPromptForArguments,csIniName);

		++iter;
	}
	return bRet;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUtilities.cpp_v  $
 * 
 *    Rev 1.2   31 Jan 2014 17:16:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 08:24:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:56:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   04 Oct 2012 11:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Sep 2011 16:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   30 Aug 2010 13:53:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   16 Dec 2009 13:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   03 Sep 2009 10:44:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   31 Oct 2007 08:31:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SysfreeString to LoadMenuFromIni and CleanupIni functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   21 Jun 2007 15:04:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   04 May 2007 12:23:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with UpdateFirmware. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   04 Apr 2007 09:02:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   took out an else statement that did nothing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   02 Mar 2007 06:27:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the utilities to use the ini files instead of the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Jun 2005 13:16:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified RunUtility to use SVSecurity create process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 17:18:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 14:54:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added return for invalid menu in FindSubMenuByName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Oct 2001 18:57:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
