// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVVersionInfo
// * .File Name       : $Workfile:   SVVersionInfo.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:51:44  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVVersionInfo.h"

#pragma comment(lib, "Version")
//##ModelId=3A2E3CBC0203
int SVVersionInfo::m_curlang   = 0;

//##ModelId=3A2E3CBD0198
SVVersionInfo::SVVersionInfo()
{
	m_pVersionInfo = NULL;				// raw version info data 
	m_pTranslation = NULL;

	GetAppVersionInfo(); 
}


//////////////////
// Destroy: delete version info
//

//##ModelId=3A2E3CBD0197
SVVersionInfo::~SVVersionInfo()
{
	ReleaseVersionInfo();
}


//##ModelId=3A2E3CBD0199
void SVVersionInfo::ReleaseVersionInfo()
{
	if (m_pVersionInfo)
	{
		delete [] m_pVersionInfo;
		m_pVersionInfo = NULL;
	}
	
	if (m_pTranslation)
	{
		delete [] m_pTranslation;
		m_pTranslation = NULL;
	}
	m_languages = 0;
}


//////////////////
// Get file version info for a given module
// Allocates storage for all info, fills "this" with
// VS_FIXEDFILEINFO, and sets codepage.
//

//##ModelId=3A2E3CBD0191
BOOL SVVersionInfo::GetAppVersionInfo()
{
	HGLOBAL hGlobal;
	DWORD		len;

	memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));
	ReleaseVersionInfo();
	
	CWinApp* app = AfxGetApp();
	HRSRC hRsrc1 = FindResource(app->m_hInstance,
															MAKEINTRESOURCE(VS_VERSION_INFO),
															RT_VERSION);
	if (!hRsrc1)
		return FALSE;

	LPVOID	lpvi;
	UINT		iLen;

	hGlobal = LoadResource(app->m_hInstance, hRsrc1);
	len     = SizeofResource(app->m_hInstance, hRsrc1);
	m_pVersionInfo = new BYTE[len];	// allocate version info
	memcpy(m_pVersionInfo, hGlobal, len);

	if (!::VerQueryValue(m_pVersionInfo, _T("\\"), &lpvi, &iLen))
		return FALSE;
	
	// copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
	*(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpvi;
	
	// Get translation info
	// Note: VerQueryValue could return a value > 4, in which case
	// mulitple languages are supported and VerQueryValue returns an
	// array of langID/codepage pairs and you have to decide which to use.
	if (::VerQueryValue(m_pVersionInfo,
											_T("\\VarFileInfo\\Translation"),
											&lpvi, &iLen) && iLen >= 4)
	{
		m_languages = iLen >> 2;
		m_pTranslation = new TRANSLATION[m_languages];
		memcpy(m_pTranslation, lpvi, iLen);
		if (m_curlang < 0 || !(m_curlang < m_languages))
			m_curlang = 0;
	}
	
	return dwSignature == VS_FFI_SIGNATURE;
}


//////////////////
// Get string file info.
// Key name is something like "CompanyName".
// returns the value as a CString.
//

//##ModelId=3A2E3CBD018F
CString SVVersionInfo::GetValue(LPCTSTR lpKeyName)
{
	CString sVal;

	if ( m_curlang < 0 || !(m_curlang < m_languages))
		m_curlang = 0;

	if (m_pVersionInfo)
	{
		// To get a string value must pass query in the form
		//
		//    "\StringFileInfo\<langID><codepage>\keyname"
		//
		// where <lang-codepage> is the languageID concatenated with the
		// code page, in hex. Wow.
		//
		CString query;
		query.Format(	_T("\\StringFileInfo\\%04x%04x\\%s"),
									m_pTranslation[m_curlang].langID,
									m_pTranslation[m_curlang].charset,
									lpKeyName);
		
		LPCTSTR pVal;
		UINT iLenVal;
		if (::VerQueryValue(m_pVersionInfo, (LPTSTR)(LPCTSTR)query, (LPVOID*)&pVal, &iLenVal))
		{
			sVal = pVal;
		}
	}
	return sVal;
}


//////////////////
// Get string file info.
// Key name is something like "CompanyName".
// returns the value as a CString.
//

//##ModelId=3A2E3CBD018D
CString SVVersionInfo::GetValueFromResource(LPCTSTR lpKeyName)
{
	TRANSLATION	translation;
	CString			sVal;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc     = FindResource(	hInst,
																	MAKEINTRESOURCE(VS_VERSION_INFO),
																	RT_VERSION);

	if (hRsrc)
	{
		HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
		DWORD		len     = SizeofResource(hInst, hRsrc);
		LPVOID	lpvi    = NULL;
		UINT		iLen    = 0;

		if (::VerQueryValue(hGlobal, _T("\\"), &lpvi, &iLen))
		{
			if (::VerQueryValue(hGlobal,
					_T("\\VarFileInfo\\Translation"), &lpvi, &iLen) && iLen >= 4)
			{
				translation = *(LPTRANSLATION)lpvi;
			}
		}

		if (dwSignature == VS_FFI_SIGNATURE)
		{
			CString query;
			query.Format(	_T("\\StringFileInfo\\%04x%04x\\%s"),
										translation.langID,
										translation.charset,
										lpKeyName);
			LPCTSTR pVal;
			UINT iLenVal;
			if (::VerQueryValue(hGlobal, (LPTSTR)(LPCTSTR)query, (LPVOID*)&pVal, &iLenVal))
			{
				sVal = pVal;
			}
		}
	}
	
	return sVal;
}


//////////////////
// Get string file info.
// Key name is something like "CompanyName".
// returns the value as a CString.
//

//##ModelId=3A2E3CBD018B
CString SVVersionInfo::GetProductString(void)
{
	CString sVal;

	CString sProductName = GetValueFromResource(_T("ProductName"));
	if (!sProductName.IsEmpty())
	{
		if (LOWORD(dwProductVersionLS))
		{
			sVal.Format(_T("%s %d.%d Beta %d"),
									sProductName,
									HIWORD(dwProductVersionMS),
									LOWORD(dwProductVersionMS),
									LOWORD(dwProductVersionLS));
		}
		else
		{
			sVal.Format(_T("%s %d.%d"),
									sProductName,
									HIWORD(dwProductVersionMS),
									LOWORD(dwProductVersionMS));
		}
	}

	return sVal;
}


//##ModelId=3A2E3CBD0192
void SVVersionInfo::SetLanguage(int langID)
{
	m_curlang = 0;
	for (int i=0; i<m_languages; i++)
	{
		if (m_pTranslation[i].langID == langID)
		{
			m_curlang = i;
			break;
		}
	}
}


/////////////////
// Get DLL Version by calling DLL's DllGetVersion proc
//
//##ModelId=3A2E3CBD0158
BOOL SVVersionInfo::DllGetVersion(LPCTSTR ModuleName, DLLVERSIONINFO& dvi)
{
	HINSTANCE   hinstDll;
	HRESULT     hr = -1;
	
	// Load the DLL.
	hinstDll = LoadLibrary(ModuleName);
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
	Sleep(0);
	
	if (hinstDll)
  {
		DLLGETVERSIONPROC pDllGetVersion;
		
		/*
		You must get this function explicitly because the DLL might not implement 
		the function. Depending upon the DLL, the lack of implementation of the 
		function may be a version marker in itself.
		*/
		pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
		
		if (pDllGetVersion)
		{
      ZeroMemory(&dvi, sizeof(dvi));
      dvi.cbSize = sizeof(dvi);
			
      hr = (*pDllGetVersion)(&dvi);
		}
		
		FreeLibrary(hinstDll);
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
	}

	return (SUCCEEDED(hr));
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVVersionInfo.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Oct 2007 15:25:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Sleep after LoadLibrary and FreeLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Dec 2000 12:10:52   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added model id's
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Dec 2000 12:50:36   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  58
 * SCR Title:  Add Version control to SVFocus Splash screen
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added define for Beta version
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2000 09:23:12   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added new classes to the library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Oct 2000 09:40:18   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  58
 * SCR Title:  Add Version control to SVFocus Splash screen
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/