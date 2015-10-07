//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRegistry
//* .File Name       : $Workfile:   SVRegistry.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:08:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVRegistry.h"

#include "SVMessage/SVMessage.h"

#include "MessageContainer.h"
#include "MessageManager.h"

#define SV_HKEY_CLASSES_ROOT _T("HKEY_CLASSES_ROOT")
#define SV_HKEY_CURRENT_USER _T("HKEY_CURRENT_USER")
#define SV_HKEY_LOCAL_MACHINE _T("HKEY_LOCAL_MACHINE")
#define SV_HKEY_USERS _T("HKEY_USERS")
#define SV_HKEY_PERFORMANCE_DATA _T("HKEY_PERFORMANCE_DATA")
#define SV_ROOT_KEY _T("Software\\SVResearch") // Maybe should change to -> _T("Software\\Seidenader Vision Inc.")
#define SV_BACKSLASH _T("\\")
#define SV_BACKSLASHCHAR _T('\\')
#define SV_SHADOWFILE _T("SVShadowFile")
#define SV_SHADOWFILEEXT _T(".reg")
#define SV_SHADOWFILEHEADER _T("REGEDIT4")
#define SV_UNDERLINE _T("_")

// return codes for GetImportString()
#define SV_ISERROR -1
#define SV_ISEOF 0
#define SV_ISKEY 1
#define SV_ISVALUE 2
#define SV_ISGARBAGE 3

void SVRegistryClass::InitRegistry(LPCTSTR p_szKey)
{
	DWORD dwDisposition = 0L;
	SVString szKey = p_szKey;
	SVString szFullKey = p_szKey;
	SVString szTemp;
	HKEY hBaseKey = HKEY_LOCAL_MACHINE;
	LONG lResult;
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
	TCHAR szFname[_MAX_FNAME], szPath[_MAX_PATH];

	mszKey = p_szKey;

	// check if first node of key is an HKEY Value
	if (szFullKey.Left(17) == SV_HKEY_CLASSES_ROOT)
	{
		hBaseKey = HKEY_CLASSES_ROOT;
		szFullKey.erase(0, 18);
	}
	else
	{
		if (szFullKey.Left(17) == SV_HKEY_CURRENT_USER)
		{
			hBaseKey = HKEY_CURRENT_USER;
			szFullKey.erase(0, 18);
		}
		else
		{
			if (szFullKey.Left(18) == SV_HKEY_LOCAL_MACHINE)
			{
				hBaseKey = HKEY_LOCAL_MACHINE;
				szFullKey.erase(0, 19);
			}
			else
			{
				if (szFullKey.Left(10) == SV_HKEY_USERS)
				{
					hBaseKey = HKEY_USERS;
					szFullKey.erase(0, 11);
				}
				else
				{
					if (szFullKey.Left(20) == SV_HKEY_PERFORMANCE_DATA)
					{
						hBaseKey = HKEY_PERFORMANCE_DATA;
						szFullKey.erase(0, 21);
					}
					else
					{
						mszKey = SV_HKEY_LOCAL_MACHINE;
						mszKey += SV_BACKSLASH;
						mszKey += SV_ROOT_KEY;

						szFullKey = SV_ROOT_KEY;

						if (szKey.Left(1) != SV_BACKSLASH)
						{
							szFullKey += SV_BACKSLASH;
							mszKey += SV_BACKSLASH;
							if (GetModuleFileName (NULL, szPath, _MAX_PATH))
							{
								_tsplitpath (szPath, NULL, NULL, szFname, NULL);
								szFullKey += szFname;
								szFullKey += SV_BACKSLASH;
								mszKey += szFname;
								mszKey += SV_BACKSLASH;
							}
						}
						szFullKey += szKey;
						mszKey += szKey;
					}
				}
			}
		}
	}

	mhKey = (HKEY) NULL;

	if (mszKey.Right(1) == SV_BACKSLASH)
		mszKey.erase(mszKey.size() - 1);

	lResult = RegOpenKeyEx (hBaseKey,
		szFullKey.ToString(),
		static_cast< DWORD >( 0 ),
		KEY_ALL_ACCESS,
		&mhKey );

	if( lResult == ERROR_ACCESS_DENIED)
	{
		lResult = RegOpenKeyEx (hBaseKey,
			szFullKey.ToString(),
			static_cast< DWORD >( 0 ),
			KEY_READ,
			&mhKey );
	}
	
	if( lResult == ERROR_SUCCESS )
	{
		mbCreatedNewKey = FALSE;
	}
	else
	{
		if( lResult == ERROR_FILE_NOT_FOUND )
		{
			if( ERROR_SUCCESS != RegCreateKeyEx ( hBaseKey,
				szFullKey.ToString(),
				static_cast< DWORD >( NULL ),
				_T( "" ),
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				static_cast< SECURITY_ATTRIBUTES* >( NULL ),
				&mhKey,
				&dwDisposition ) )
			{
				mhKey = static_cast< HKEY >( NULL );
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED, szFullKey.ToString(), StdMessageParams );
				Exception.Throw();
			}

			mbCreatedNewKey = TRUE;
		}
		else
		{
			mhKey = (HKEY) NULL;
			Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED, szFullKey.ToString(), StdMessageParams );
			Exception.Throw();
		}
	}
}

SVRegistryClass::SVRegistryClass(LPCTSTR szKey)
{
  InitRegistry (szKey);
}

SVRegistryClass::SVRegistryClass()
{
  InitRegistry( _T("") ); 
}

SVRegistryClass& SVRegistryClass::operator=(const SVRegistryClass &svrRight)
{
	if (mhKey)
		RegCloseKey (mhKey);

	InitRegistry( svrRight.mszKey.ToString() );
	return (*this);
}

SVRegistryClass::~SVRegistryClass()
{
	if (mhKey)
		RegCloseKey (mhKey);
}

BOOL SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, SVString& szValue)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) NULL,
		&dwType,
		NULL,
		&dwSize))
	{
		if (dwType == REG_SZ || dwType == REG_BINARY || dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ)
		{
			LPBYTE pBuff = new BYTE[dwSize];
			if (pBuff)
			{
				// get the data
				if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
					szValueName,
					(DWORD *) NULL,
					&dwType,
					pBuff,
					&dwSize))
				{
					szValue = (LPCTSTR)pBuff;
					delete [] pBuff;
					return TRUE;
				}
				else
				{
					delete [] pBuff;
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}

BOOL SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, LPCTSTR szValue )
{
	DWORD dwType = 0L;

	if( ERROR_SUCCESS == RegSetValueEx( mhKey,
		szValueName,
		static_cast< DWORD >( NULL ),
		REG_SZ,
		( BYTE* )( szValue ),  //BRW - This conversion only works when a C-style cast is used. [2007]
		( DWORD )( strlen( szValue ) * sizeof( TCHAR ) ) ) )  //BRW - This conversion only works when a C-style cast is used. [2007]
	{
		return TRUE;
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, StdMessageParams );
		Exception.Throw();
  }

	return FALSE;
}

BOOL SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, DWORD *pdwValue)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) NULL,
		&dwType,
		NULL,
		&dwSize))
	{
		if (dwType == REG_DWORD)
		{
			// get the data
			if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
				szValueName,
				(DWORD *) NULL,
				&dwType,
				(LPBYTE) pdwValue,
				&dwSize))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

BOOL SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, DWORD dwValue )
{
	DWORD dwType = 0L, dwSize = 0L;

	if( ERROR_SUCCESS == RegSetValueEx( mhKey,
		szValueName,
		static_cast< DWORD >( NULL ),
		REG_DWORD,
		reinterpret_cast< BYTE* >( &dwValue ),
		static_cast< DWORD >( sizeof( DWORD ) ) ) )
	{
		return TRUE;
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, StdMessageParams );
		Exception.Throw();
	}
	return FALSE;
}

BOOL SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, SVByteArray& baValue)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) NULL,
		&dwType,
		NULL,
		&dwSize))
	{
		// set byte array size
		baValue.SetSize( (int) dwSize);

		// get the data
		if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
			szValueName,
			(DWORD *) NULL,
			&dwType,
			(LPBYTE) baValue.GetData(),
			&dwSize))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, SVByteArray& baValue )
{
	DWORD dwType = 0L;
	DWORD dwSize = 0L;
	LONG lResult = 0L;

	lResult = RegSetValueEx( mhKey,
		szValueName,
		static_cast< DWORD >( NULL ),
		REG_BINARY,
		baValue.GetData(),
		static_cast< DWORD >( baValue.GetSize() ) );

	if( lResult == ERROR_SUCCESS )
	{
		return TRUE;
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, StdMessageParams );
		Exception.Throw();
	}

	return FALSE;
}

BOOL SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, SVByteArray& baValue, DWORD dwType, DWORD dwLength )
{
	LONG lResult;

	lResult = RegSetValueEx( mhKey,
		szValueName,
		static_cast< DWORD >( NULL ),
		dwType,
		baValue.GetData(),
		dwLength );

	if( lResult == ERROR_SUCCESS )
	{
		return TRUE;
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, StdMessageParams );
		Exception.Throw();
	}
	return FALSE;
}

BOOL SVRegistryClass::SaveKey(LPCTSTR p_szHive)
{
	LONG dwResult;

	AdjustPrivileges (SE_BACKUP_NAME);

	_tunlink( p_szHive );

	dwResult = RegSaveKey( mhKey, p_szHive, NULL );

	return (ERROR_SUCCESS == dwResult);
}

BOOL SVRegistryClass::ReplaceKey(LPCTSTR p_szHive)
{
	WIN32_FIND_DATA wfd;
	LONG dwResult;

	AdjustPrivileges (SE_RESTORE_NAME);

	if (INVALID_HANDLE_VALUE == FindFirstFile (p_szHive, &wfd))
		SaveKey (p_szHive);

	dwResult = RegRestoreKey (mhKey, p_szHive, 0L);

	return TRUE;
}

BOOL SVRegistryClass::AdjustPrivileges(TCHAR *pszPrivilege)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES NewState;
	LUID luid;

	if (LookupPrivilegeValue (NULL, pszPrivilege, &luid))
	{
		NewState.PrivilegeCount = 1;
		NewState.Privileges[0].Luid = luid;
		NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (OpenProcessToken (GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			return AdjustTokenPrivileges (hToken, FALSE, &NewState, NULL, NULL, NULL);
		}
	}

	return FALSE;
}

void SVRegistryClass::EnumKeys(PFKEYENUMPROC pKeyEnumProc, LPVOID pUserData)
{
	SVString szKey;
	DWORD dwIndex;
	DWORD dwcbBuffSize;
	DWORD dwcSubKeys;
	DWORD dwcbMaxSubKeyLen;
	FILETIME ftLastWriteTime;
	LONG lResult;

	RegQueryInfoKey (mhKey, NULL, NULL, NULL, &dwcSubKeys, &dwcbMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);

	if (dwcSubKeys)
	{
		for (lResult = ERROR_SUCCESS, dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
		{
			dwcbBuffSize = dwcbMaxSubKeyLen + 1;
			TCHAR* pszKey = new TCHAR[dwcbBuffSize];
			lResult = RegEnumKeyEx (mhKey, dwIndex, pszKey, &dwcbBuffSize, NULL, NULL, NULL, &ftLastWriteTime);
			
			if (lResult == ERROR_SUCCESS)
				szKey = pszKey;

			delete [] pszKey;

			if (lResult == ERROR_SUCCESS)
			{
				if (!(*pKeyEnumProc) (szKey.ToString(), pUserData))
				{
					lResult = ERROR_NO_MORE_ITEMS;
				}
			}
		}
	}
}

BOOL SVRegistryClass::DeleteKey()
{
	if (ERROR_SUCCESS == RegDeleteKey (mhKey, _T("")))
	{
		mhKey = NULL;
		return TRUE;
	}
	
	return FALSE;
}

BOOL SVRegistryClass::CreatedNewKey()
{
	return mbCreatedNewKey;
}

SVRegistryClass * SVRegistryClass::OpenSubKey( LPCTSTR p_szSubKey )
{
	SVString szKey;
	SVString szSubKey = p_szSubKey;

	szKey = mszKey;
	if (szSubKey.Left(1) != _T("\\"))
		szKey += _T("\\");
	szKey += szSubKey;
	return new SVRegistryClass( szKey.ToString() );
}

BOOL SVRegistryClass::DeleteValue(LPCTSTR p_szValueName)
{
	return (ERROR_SUCCESS == RegDeleteValue (mhKey, p_szValueName));
}

BOOL SVRegistryClass::SetShadowFileName(LPCTSTR szFileName)
{
	return SetRegistryValue( SV_SHADOWFILE, szFileName );
}

BOOL SVRegistryClass::GetShadowFileName(SVString & szShadowFile)
{
	return GetRegistryValue( SV_SHADOWFILE, szShadowFile );
}

BOOL SVRegistryClass::GetDefaultShadowFileName(SVString & szShadowFile)
{
	TCHAR szFname[_MAX_FNAME], szPath[_MAX_PATH];
	SVString szKey;

	if (GetModuleFileName (NULL, szPath, _MAX_PATH))
	{
		_tsplitpath (szPath, NULL, NULL, szFname, NULL);
		_tgetcwd (szPath, _MAX_PATH);

		szShadowFile = szPath;
		szShadowFile += SV_BACKSLASH;
		szShadowFile += szFname;
		szShadowFile += SV_UNDERLINE;
		szKey = mszKey;
		szKey.replace (SV_BACKSLASH, SV_UNDERLINE);
		szShadowFile += szKey;
		szShadowFile += SV_SHADOWFILEEXT;
		return TRUE;
	}

	return FALSE;
}

BOOL SVRegistryClass::Import()
{
	SVString szShadowFile;

	if (GetShadowFileName (szShadowFile))
	{
		return Import (szShadowFile.ToString());
	}
	else
	{
		if (GetDefaultShadowFileName (szShadowFile))
		{
			return Import (szShadowFile.ToString());
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL SVRegistryClass::Import(LPCTSTR szFileName)
{
	FILE *pFile;
	BOOL rc = FALSE;
	TCHAR cNewLine = _T('0');

	if (pFile = _tfopen (szFileName, _T("r")))
	{
		if (_ftscanf (pFile, SV_SHADOWFILEHEADER _T("%c"), &cNewLine) &&
			cNewLine == _T('\n'))
		{
			rc = ImportKeys (pFile);
			fclose (pFile);
		}
		else
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE, szFileName, StdMessageParams );
		}
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FAILED, szFileName, StdMessageParams );
	}

	return rc;
}

BOOL SVRegistryClass::ImportKeys(FILE * pFile)
{
	BOOL rc = TRUE;
	SVString szName;
	SVByteArray baValue;
	DWORD dwType;
	int iResult;

	for (iResult = GetImportString (pFile, szName, baValue, &dwType);
		(iResult != SV_ISEOF) && (iResult != SV_ISERROR);
		iResult = GetImportString (pFile, szName, baValue, &dwType))
	{
		switch (iResult)
		{
		case SV_ISKEY :
			{
				SVRegistryClass reg(szName.ToString());
				reg.ImportKeys (pFile);
			}
			break;

		case SV_ISVALUE :
			SetRegistryValue( szName.ToString(),
				baValue,
				dwType,
				static_cast< DWORD >( baValue.GetSize() ) );
			break;

		case SV_ISGARBAGE :
			{
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_EXPECTED_KEY, nullptr, StdMessageParams );
			}
			break;

		default :
			break;
		}
	}
	
	return rc;
}

int SVRegistryClass::GetImportString(FILE * pFile, SVString & szName, SVByteArray & baValue, DWORD * pdwType)
{
	TCHAR tBuffer;
	int i;

	// skip white space characters
	for (tBuffer = _fgettc (pFile); 
		!feof (pFile) && !ferror (pFile) && _istspace (tBuffer); 
		tBuffer = _fgettc (pFile));

	if (feof (pFile))
		return SV_ISEOF;

	if (ferror (pFile))
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR, nullptr, StdMessageParams );
		return SV_ISERROR;
	}

	switch (tBuffer)
	{
	case _T('[') :
		// read the key name
		for (szName.clear();
			(_T('\n') != (tBuffer = _fgettc (pFile))) && !feof (pFile) && !ferror (pFile);
			szName += tBuffer);

		if (ferror (pFile))
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR, nullptr, StdMessageParams );
			return SV_ISERROR;
		}

		if (szName.rfind (_T(']')) == -1)
		{
			return SV_ISGARBAGE;
		}

		szName.erase(szName.rfind (_T(']')));

		return SV_ISKEY;
		break;

	case _T('"') :
		// get the value name
		for (szName.clear();
			(_T('"') != (tBuffer = _fgettc (pFile))) && !feof (pFile) && !ferror (pFile);
			)
		{
			if (tBuffer == _T('\\'))
			{
				tBuffer = _fgettc (pFile);
				if (ferror (pFile) || feof (pFile) || tBuffer == _T('\n'))
					return SV_ISERROR;
			}
			szName += tBuffer;
		}
		// skip past the '='
		_ftscanf (pFile, _T(" ="));
		// get the data type
		if (_ftscanf (pFile, _T(" hex%c"), &tBuffer))
		{
			if (tBuffer == _T(':'))
			{
				*pdwType = REG_BINARY;
			}
			else
			{
				if (!_ftscanf (pFile, _T("%d):"), pdwType))
				{
					return SV_ISGARBAGE;
				}
			}
			// get the binary data
			baValue.RemoveAll();
			for (i = 0; _ftscanf (pFile, _T(" %x"), &tBuffer); i++)
			{
				baValue.Add((BYTE) tBuffer);
				_ftscanf (pFile, _T("%c"), tBuffer);
				if (tBuffer == _T(','))
				{
					_ftscanf (pFile, _T("\\")); // remove any line continuations...
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			// try to read dword data
			baValue.RemoveAll();
			if (_ftscanf (pFile, _T(" dword:%x"), pdwType))
			{
				for( i = 0; i < sizeof( DWORD ); i++ )
				{
					baValue.Add( ( ( BYTE* ) pdwType )[ i ] );
				}
				*pdwType = REG_DWORD;
			}
			else
			{
				// maybe it's string data...
				if (_ftscanf (pFile, _T(" %c"), &tBuffer))
				{
					if (tBuffer == _T('"'))
					{
						*pdwType = REG_SZ;
						// get the string data
						baValue.RemoveAll();
							
						for (i = 0; _ftscanf (pFile, _T(" %c"), &tBuffer); i++)
						{
							if (tBuffer == _T('\\'))
							{
								_ftscanf (pFile, _T(" %c"), &tBuffer);
							}
							else
							{
								if (tBuffer == _T('"'))
								{
									break;
								}
							}
							baValue.Add((BYTE) tBuffer);
						}
					}
					else
					{
						// couldn't determine type
						return SV_ISGARBAGE;
					}
				}
			}
		}
			
		return SV_ISVALUE;
		break;

	default :
		return SV_ISGARBAGE;
		break;
	}

	return SV_ISERROR;
}

BOOL SVRegistryClass::Export()
{
	SVString szShadowFile;

	if (GetShadowFileName (szShadowFile))
	{
		return Export (szShadowFile.ToString());
	}
	else
	{
		if (GetDefaultShadowFileName (szShadowFile))
		{
			return Export (szShadowFile.ToString());
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL SVRegistryClass::Export(LPCTSTR szFileName)
{
	FILE *pFile;
	BOOL rc = FALSE;

	if (pFile = _tfopen (szFileName, _T("w")))
	{
		_ftprintf (pFile, _T("%s\n\n"), SV_SHADOWFILEHEADER);
		if (rc = ExportKeys (pFile))
		{
			SetShadowFileName (szFileName);
		}
		fclose (pFile);
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_EXPORT_FAILED, szFileName, StdMessageParams );
	}

	return rc;
}

BOOL SVRegistryClass::ExportKeys(FILE * pFile)
{
	BOOL rc = TRUE;
	SVString szKey;
	DWORD dwIndex;
	DWORD dwcSubKeys;
	DWORD dwcbMaxSubKeyLen;
	DWORD dwcbBuffSize;
	LONG lResult;

	_ftprintf (pFile, _T("[%s]\n"), mszKey);

	if (ExportValues (pFile))
	{
		_ftprintf (pFile, _T("\n"));
		RegQueryInfoKey (mhKey, NULL, NULL, NULL, &dwcSubKeys, &dwcbMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);

		for (dwIndex = 0; dwIndex < dwcSubKeys; dwIndex++)
		{
			dwcbBuffSize = dwcbMaxSubKeyLen + 1;
			TCHAR* pszKey = new TCHAR[dwcbBuffSize];
			lResult= RegEnumKeyEx (mhKey, dwIndex, pszKey, &dwcbBuffSize, NULL, NULL, NULL, NULL);
			szKey = pszKey;
			delete [] pszKey;

			if (lResult == ERROR_SUCCESS)
			{
				try
				{
					szKey.insert (0, SV_BACKSLASH);
					szKey.insert(0, mszKey.ToString());
					SVRegistryClass reg(szKey.ToString());
					rc &= reg.ExportKeys (pFile);
					szKey.clear();
				}
				catch ( SvStl::MessageContainer& rSvE )
				{
					rc = FALSE;
					rSvE.logMessage();
				}
			}
			else
			{
				rc = FALSE;
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED, szKey.ToString(), StdMessageParams );
			}
		}
	}
	else
	{
		rc = FALSE;
	}

	return rc;
}

BOOL SVRegistryClass::ExportValues(FILE * pFile)
{
	BOOL rc = TRUE;
	TCHAR *pszName, *ptData;
	BYTE *pData;
	DWORD dwIndex;
	DWORD dwcValues, dwcbValueMax, dwcbNameMax;
	DWORD dwcbName, dwcbValue, dwType;
	LONG lResult;
	LONG lLineLen;
	int i;
	SVString szComma;

	RegQueryInfoKey (mhKey, NULL, NULL, NULL, NULL, NULL, NULL, &dwcValues, &dwcbNameMax, &dwcbValueMax, NULL, NULL);

	if (dwcValues)
	{
		dwcbName = dwcbNameMax + 1;
		dwcbValue = dwcbValueMax;

		pszName = new TCHAR[dwcbName];
		pData = (BYTE *) new char[dwcbValue];

		for (dwIndex = 0; dwIndex < dwcValues; dwIndex++)
		{
			dwcbName = dwcbNameMax + 1;
			dwcbValue = dwcbValueMax;
			lResult= RegEnumValue (mhKey, dwIndex, pszName, &dwcbName, NULL, &dwType, pData, &dwcbValue);

			if (lResult == ERROR_SUCCESS)
			{
				lLineLen = _ftprintf (pFile, _T("\""));
				for (i = 0; (i < (int) dwcbName) && pszName[i]; i++)
				{
					switch (pszName[i])
					{
					case SV_BACKSLASHCHAR :
						lLineLen += _ftprintf (pFile, _T("%c%c"), SV_BACKSLASHCHAR, SV_BACKSLASHCHAR);
						break;

					case _T('"') :
						lLineLen += _ftprintf (pFile, _T("%c%c"), SV_BACKSLASHCHAR, _T('"'));
						break;

					default :
						if (pszName[i] > _T('\xff'))
						{
							lLineLen += _ftprintf (pFile, _T("?"));
						}
						else
						{
							lLineLen += _ftprintf (pFile, _T("%c"), pszName[i]);
						}
					}
				}
				lLineLen += _ftprintf (pFile, _T("\"="));

				switch (dwType)
				{
				case REG_SZ :
					ptData = (TCHAR *) pData;
					lLineLen += _ftprintf (pFile, _T("\""));
					dwcbValue /= sizeof (TCHAR);
					for (i = 0; (i < (int) dwcbValue) && ptData[i]; i++)
					{
						switch (ptData[i])
						{
						case SV_BACKSLASHCHAR :
							lLineLen += _ftprintf (pFile, _T("%c%c"), SV_BACKSLASHCHAR, SV_BACKSLASHCHAR);
							break;

						case _T('"') :
							lLineLen += _ftprintf (pFile, _T("%c%c"), SV_BACKSLASHCHAR, _T('"'));
							break;

						default :
							if (ptData[i] > _T('\xff'))
							{
								lLineLen += _ftprintf (pFile, _T("?"));
							}
							else
							{
								lLineLen += _ftprintf (pFile, _T("%c"), ptData[i]);
							}
						}
					}
					lLineLen += _ftprintf (pFile, _T("\"\n"));
					break;

				case REG_DWORD :
					lLineLen += _ftprintf (pFile, _T("dword:%08.8x\n"), *((DWORD *) pData));
					break;

				case REG_BINARY :
					lLineLen += _ftprintf (pFile, _T("hex:"));
					szComma = _T(""); 
					for (i = 0; i < (int) dwcbValue; i++)
					{
						lLineLen += _ftprintf( pFile, _T( "%s" ), szComma.ToString() );
						szComma = _T(",");

						if (lLineLen > 77)
						{
							_ftprintf (pFile, _T("\\\n  "));
							lLineLen = 2;
						}

						lLineLen += _ftprintf (pFile, _T("%02.2x"), pData[i]);
					}
					_ftprintf (pFile, _T("\n"));
					break;

				default :
					ptData = (TCHAR *) pData;
					lLineLen += _ftprintf (pFile, _T("hex(%d):"), dwType);
					szComma = _T("");
					dwcbValue /= sizeof (TCHAR);
					for (i = 0; i < (int) dwcbValue; i++)
					{
						lLineLen += _ftprintf( pFile, _T( "%s" ), szComma.ToString() );
						szComma = _T(",");

						if (lLineLen > 77)
						{
							_ftprintf (pFile, _T("\\\n  "));
							lLineLen = 2;
						}

						lLineLen += _ftprintf (pFile, _T("%02.2x"), ptData[i]);
					}
					_ftprintf (pFile, _T("\n"));
					break;
				}
			}
			else
			{
				rc = FALSE;
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED, mszKey.ToString(), StdMessageParams );
			}
		}
		delete [] pData;
		delete [] pszName;
	}

	return rc;
}

BOOL SVRegistryClass::GetRegistryValue(DWORD dwIndex, SVString& szValueName, SVString& szValue, LPDWORD pdwType)
{
	BOOL rc = FALSE;
	DWORD dwcbNameMax, dwcbValueMax, dwcbName, dwcbValue;
	
	RegQueryInfoKey (mhKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwcbNameMax, &dwcbValueMax, NULL, NULL);

	TCHAR* pName = new TCHAR[++dwcbNameMax];
	TCHAR* pValue = new TCHAR[dwcbValueMax];

	dwcbName = dwcbNameMax;
	dwcbValue = dwcbValueMax;

	if (ERROR_SUCCESS == RegEnumValue (mhKey, dwIndex, pName, &dwcbName, NULL, pdwType, reinterpret_cast<BYTE *>(pValue), &dwcbValue))
	{
		memset ((void *) pName, 0, dwcbNameMax * sizeof (TCHAR));
		memset ((void *) pValue, 0, dwcbValueMax * sizeof (TCHAR));

		szValueName = pName;
		szValue = pValue;
		rc = TRUE;
	}

	delete [] pName;
	delete [] pValue;

	return rc;
}

DWORD SVRegistryClass::NumSubKeys()
{
	DWORD dwNumSubKeys;

	RegQueryInfoKey (mhKey, NULL, NULL, NULL, &dwNumSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	return dwNumSubKeys;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\SVRegistry.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 10:08:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:43:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2012 09:43:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with the usage of the wrong delete functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2010 08:16:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Feb 2001 16:23:42   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jul 2000 07:24:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  23
 * SCR Title:  Bug fixes 1.0
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Fixed a destructor failure caused by DeleteKey () method of SVRegistryClass. When a key is deleted from the registry, it is also closed making the handle to the key (mhKey) invalid. Attempting to close the key again in the destructor caused a memory leak. The Delete () method, then, needed to set mhKey to NULL upon successful deletion of a key.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Jul 2000 14:27:06   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  13
 * SCR Title:  Stage 2: SVFocus Configuration Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added NumSubKeys function and GetRegistryValue() overloaded to retrieve values by index instead of name.
 * 
 * This is in support of configuring security settings for SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2000 09:22:34   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  13
 * SCR Title:  Stage 2: SVFocus Configuration Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added functions for registry import/export to support SVFocusConfig (note: import not complete).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 May 2000 17:16:40   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated error codes.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Apr 2000 16:58:16   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  12
 * SCR Title:  Stage 2: Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Updated the SVException::Format () function to read the message string from the message dll. This change required a modification to SVRegistryClass to permit opening the registry key "HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\EventLog\Application"
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:20   mike
 * Initial revision.
*/
