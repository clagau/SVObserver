// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVVersionInfo.h
// * .File Name       : $Workfile:   SVVersionInfo.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:51:48  $
// ******************************************************************************

#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <shlwapi.h>

#pragma comment(lib, "shlwapi")

//////////////////
// SVVersionInfo version info about a module.
// To use:
//
// SVVersionInfo ver
// if (ver.GetFileVersionInfo(_T("mymodule")))
// {
//		// info is in ver, you can call GetValue to get variable info like
//		CString s = ver.GetValue(_T("CompanyName"));
// }
//
// You can also call the static fn DllGetVersion to get DLLVERSIONINFO.
//

class SVVersionInfo : public VS_FIXEDFILEINFO
{
protected:
	BYTE* m_pVersionInfo;	// all version info

	typedef struct tagTRANSLATION
	{
		WORD langID;			// language ID
		WORD charset;			// character set (code page)
	} TRANSLATION, *LPTRANSLATION;

	LPTRANSLATION	m_pTranslation;
	int						m_languages;
	static int		m_curlang;

protected:
	void ReleaseVersionInfo();

public:
	SVVersionInfo();
	virtual ~SVVersionInfo();

	void		SetLanguage(int langID);
	BOOL		GetAppVersionInfo();
	CString	GetValue(LPCTSTR lpKeyName);
	CString	GetValueFromResource(LPCTSTR lpKeyName);
	CString	GetProductString(void);

	inline CString GetComments(void)					{ return GetValue(_T("Comments")); }
	inline CString GetCompanyName(void)				{ return GetValue(_T("CompanyName")); }
	inline CString GetFileDescription(void)		{ return GetValue(_T("FileDescription")); }
	inline CString GetFileVersion(void)				{ return GetValue(_T("FileVersion")); }
	inline CString GetInternalName(void)			{ return GetValue(_T("InternalName")); }
	inline CString GetLegalCopyright(void)		{ return GetValue(_T("LegalCopyright")); }
	inline CString GetLegalTrademarks(void)		{ return GetValue(_T("LegalTrademarks")); }
	inline CString GetOriginalFilename(void)	{ return GetValue(_T("OriginalFilename")); }
	inline CString GetPrivateBuild(void)			{ return GetValue(_T("PrivateBuild")); }
	inline CString GetProductName(void)				{ return GetValue(_T("ProductName")); }
	inline CString GetProductVersion(void)		{ return GetValue(_T("ProductVersion")); }
	inline CString GetSpecialBuild(void)			{ return GetValue(_T("SpecialBuild")); }

	static BOOL DllGetVersion(LPCTSTR ModuleName, DLLVERSIONINFO& dvi);
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVVersionInfo.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   22 Oct 2010 08:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Dec 2000 12:10:50   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added model id's
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2000 09:23:04   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added new classes to the library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Oct 2000 09:40:16   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  58
 * SCR Title:  Add Version control to SVFocus Splash screen
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
