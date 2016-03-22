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

#pragma once

//Moved to precompiled header: #include <shlwapi.h>

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

