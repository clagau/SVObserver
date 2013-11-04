// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CPlc_interfaceApp
// * .File Name       : $Workfile:   plc_interface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:33:08  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPlc_interfaceApp

BEGIN_MESSAGE_MAP(CPlc_interfaceApp, CWinApp)
	//{{AFX_MSG_MAP(CPlc_interfaceApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlc_interfaceApp construction

CPlc_interfaceApp::CPlc_interfaceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
   while (1)
   {
      

      break;
   }

}

CPlc_interfaceApp::~CPlc_interfaceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
   while (1)
   {
	   ::FreeLibrary( msvhCurrentResourceDll );
		msvhCurrentResourceDll=NULL;
      break;
   }
}

BOOL CPlc_interfaceApp::InitInstance ()
{
   long      lErr;

   lErr = 0;


   while (1)
   {
	   if (!AfxSocketInit())
	   {
		   AfxMessageBox(_T("Sockets not defined"));
		   return FALSE;
	   }


      msvhCurrentResourceDll = 
         LoadLibrary (_T("PLCEnglish.dll"));

      if (msvhCurrentResourceDll == NULL)
      {
		  AfxMessageBox(_T("PLCEnglish.dll Failed to load"));
         lErr = -((long) GetLastError ());
         break;
      }


      AfxSetResourceHandle (msvhCurrentResourceDll);

      break;
   }



   if (lErr < 0)
      return FALSE;
   else
      return TRUE;
}

long CPlc_interfaceApp::SetLanguage(SVLanguage aLanguage)
{
  HINSTANCE hNewResourceDll;
//  SVRegistryClass reg(_T("\\"));

  switch (aLanguage)
  {
    default :
    case USEnglish :
      hNewResourceDll = LoadLibrary (_T("PLCEnglish.dll"));
      break;

    case UKEnglish :
      hNewResourceDll = LoadLibrary (_T("PLCUKEnglish.dll"));
      break;

    case French :
      hNewResourceDll = LoadLibrary (_T("PLCFrench.dll"));
      break;

    case German :
      hNewResourceDll = LoadLibrary (_T("PLCGerman.dll"));
      break;

    case Italian :
      hNewResourceDll = LoadLibrary (_T("PLCItalian.dll"));
      break;

    case Portuguese :
      hNewResourceDll = LoadLibrary (_T("PLCPortugese.dll"));
      break;
  }

  if (!hNewResourceDll)
  {
    if (aLanguage != USEnglish)
    {
      hNewResourceDll = LoadLibrary (_T("PLCEnglish.dll"));
      msvCurrentLanguage = USEnglish;
    }
    return FALSE;
  }
  else
  {
    msvCurrentLanguage = aLanguage;

    if (msvhCurrentResourceDll)
    {
      FreeLibrary (msvhCurrentResourceDll);
    }
    AfxSetResourceHandle (hNewResourceDll);
    msvhCurrentResourceDll = hNewResourceDll;
    msvCurrentLanguage = aLanguage;
//    reg.SetRegistryValue (mszLanguageName, (DWORD) mCurrentLanguage);
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlc_interfaceApp object

CPlc_interfaceApp theApp;

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\plc_interface.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:33:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:55:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:04   mike
 * Initial revision.
*/