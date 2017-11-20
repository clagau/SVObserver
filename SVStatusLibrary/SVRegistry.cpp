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

#pragma region Includes
#include "stdafx.h"
#include "SVRegistry.h"

#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "MessageContainer.h"
#include "MessageManager.h"
#pragma endregion Includes

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
	std::string szKey = p_szKey;
	std::string szFullKey = p_szKey;
	std::string szTemp;
	HKEY hBaseKey = HKEY_LOCAL_MACHINE;
	LONG lResult;
	TCHAR szFname[_MAX_FNAME], szPath[_MAX_PATH];

	mszKey = (nullptr != p_szKey) ? p_szKey : std::string();

	// check if first node of key is an HKEY Value
	if (szFullKey.substr(0, 17) == SV_HKEY_CLASSES_ROOT)
	{
		hBaseKey = HKEY_CLASSES_ROOT;
		szFullKey.erase(0, 18);
	}
	else
	{
		if (szFullKey.substr(0, 17) == SV_HKEY_CURRENT_USER)
		{
			hBaseKey = HKEY_CURRENT_USER;
			szFullKey.erase(0, 18);
		}
		else
		{
			if (szFullKey.substr(0, 18) == SV_HKEY_LOCAL_MACHINE)
			{
				hBaseKey = HKEY_LOCAL_MACHINE;
				szFullKey.erase(0, 19);
			}
			else
			{
				if (szFullKey.substr(0, 10) == SV_HKEY_USERS)
				{
					hBaseKey = HKEY_USERS;
					szFullKey.erase(0, 11);
				}
				else
				{
					if (szFullKey.substr(0, 20) == SV_HKEY_PERFORMANCE_DATA)
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

						if (szKey.substr(0, 1) != SV_BACKSLASH)
						{
							szFullKey += SV_BACKSLASH;
							mszKey += SV_BACKSLASH;
							if (GetModuleFileName (nullptr, szPath, _MAX_PATH))
							{
								_tsplitpath (szPath, nullptr, nullptr, szFname, nullptr);
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

	mhKey = (HKEY) nullptr;

	if (SvUl::Right(mszKey, 1) == SV_BACKSLASH)
		mszKey.erase(mszKey.size() - 1);

	lResult = RegOpenKeyEx (hBaseKey,
		szFullKey.c_str(),
		static_cast< DWORD >( 0 ),
		KEY_ALL_ACCESS,
		&mhKey );

	if( lResult == ERROR_ACCESS_DENIED)
	{
		lResult = RegOpenKeyEx (hBaseKey,
			szFullKey.c_str(),
			static_cast< DWORD >( 0 ),
			KEY_READ,
			&mhKey );
	}
	
	if( lResult == ERROR_SUCCESS )
	{
		mbCreatedNewKey = false;
	}
	else
	{
		if( lResult == ERROR_FILE_NOT_FOUND )
		{
			if( ERROR_SUCCESS != RegCreateKeyEx ( hBaseKey,
				szFullKey.c_str(),
				static_cast< DWORD >( 0 ),
				_T( "" ),
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				static_cast< SECURITY_ATTRIBUTES* >( nullptr ),
				&mhKey,
				&dwDisposition ) )
			{
				mhKey = static_cast< HKEY >( nullptr );
				SvStl::MessageMgrStd Exception( SvStl::DataOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED, szFullKey.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}

			mbCreatedNewKey = true;
		}
		else
		{
			mhKey = (HKEY) nullptr;
			SvStl::MessageMgrStd Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED, szFullKey.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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

	InitRegistry( svrRight.mszKey.c_str() );
	return (*this);
}

SVRegistryClass::~SVRegistryClass()
{
	if (mhKey)
		RegCloseKey (mhKey);
}

bool SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, std::string& szValue)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) nullptr,
		&dwType,
		nullptr,
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
					(DWORD *) nullptr,
					&dwType,
					pBuff,
					&dwSize))
				{
					szValue = (LPCTSTR)pBuff;
					delete [] pBuff;
					return true;
				}
				else
				{
					delete [] pBuff;
					return false;
				}
			}
		}
	}
	return false;
}

bool SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, LPCTSTR szValue )
{
	DWORD dwType = 0L;

	if( ERROR_SUCCESS == RegSetValueEx( mhKey,
		szValueName,
		0,
		REG_SZ,
		( BYTE* )( szValue ),  //BRW - This conversion only works when a C-style cast is used. [2007]
		( DWORD )( strlen( szValue ) * sizeof( TCHAR ) ) ) )  //BRW - This conversion only works when a C-style cast is used. [2007]
	{
		return true;
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
  }

	return false;
}

bool SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, DWORD *pdwValue)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) nullptr,
		&dwType,
		nullptr,
		&dwSize))
	{
		if (dwType == REG_DWORD)
		{
			// get the data
			if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
				szValueName,
				(DWORD *) nullptr,
				&dwType,
				(LPBYTE) pdwValue,
				&dwSize))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

bool SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, DWORD dwValue )
{
	DWORD dwType = 0L, dwSize = 0L;

	if( ERROR_SUCCESS == RegSetValueEx( mhKey,
		szValueName,
		0,
		REG_DWORD,
		reinterpret_cast< BYTE* >( &dwValue ),
		static_cast< DWORD >( sizeof( DWORD ) ) ) )
	{
		return true;
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}
	return false;
}

bool SVRegistryClass::GetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector)
{
	DWORD dwType = 0L, dwSize = 0L;

	// determine the size of the buffer necessary for the data
	if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
		szValueName,
		(DWORD *) nullptr,
		&dwType,
		nullptr,
		&dwSize))
	{
		// set byte array size
		rValueVector.resize(dwSize);

		// get the data
		if (ERROR_SUCCESS == RegQueryValueEx (mhKey,
			szValueName,
			(DWORD *) nullptr,
			&dwType,
			(LPBYTE) rValueVector.data(),
			&dwSize))
		{
			return true;
		}
	}
	return false;
}

bool SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector )
{
	DWORD dwType = 0L;
	DWORD dwSize = 0L;
	LONG lResult = 0L;

	lResult = RegSetValueEx( mhKey,
		szValueName,
		0,
		REG_BINARY,
		rValueVector.data(),
		static_cast< DWORD >( rValueVector.size() ) );

	if( lResult == ERROR_SUCCESS )
	{
		return true;
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	return false;
}

bool SVRegistryClass::SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector, DWORD dwType, DWORD dwLength )
{
	LONG lResult;

	lResult = RegSetValueEx( mhKey,
		szValueName,
		0,
		dwType,
		rValueVector.data(),
		dwLength );

	if( lResult == ERROR_SUCCESS )
	{
		return true;
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}
	return false;
}

bool SVRegistryClass::SaveKey(LPCTSTR p_szHive)
{
	LONG dwResult;

	AdjustPrivileges (SE_BACKUP_NAME);

	_tunlink( p_szHive );

	dwResult = RegSaveKey( mhKey, p_szHive, nullptr );

	return (ERROR_SUCCESS == dwResult);
}

bool SVRegistryClass::ReplaceKey(LPCTSTR p_szHive)
{
	WIN32_FIND_DATA wfd;
	LONG dwResult;

	AdjustPrivileges (SE_RESTORE_NAME);

	if (INVALID_HANDLE_VALUE == FindFirstFile (p_szHive, &wfd))
		SaveKey (p_szHive);

	dwResult = RegRestoreKey (mhKey, p_szHive, 0L);

	return true;
}

bool SVRegistryClass::AdjustPrivileges(TCHAR *pszPrivilege)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES NewState;
	LUID luid;

	if (LookupPrivilegeValue (nullptr, pszPrivilege, &luid))
	{
		NewState.PrivilegeCount = 1;
		NewState.Privileges[0].Luid = luid;
		NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (OpenProcessToken (GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			return AdjustTokenPrivileges(hToken, false, &NewState, 0, nullptr, nullptr) ? true : false;
		}
	}

	return false;
}

void SVRegistryClass::EnumKeys(PFKEYENUMPROC pKeyEnumProc, LPVOID pUserData)
{
	std::string szKey;
	DWORD dwIndex;
	DWORD dwcbBuffSize;
	DWORD dwcSubKeys;
	DWORD dwcbMaxSubKeyLen;
	FILETIME ftLastWriteTime;
	LONG lResult;

	RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, &dwcSubKeys, &dwcbMaxSubKeyLen, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

	if (dwcSubKeys)
	{
		for (lResult = ERROR_SUCCESS, dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
		{
			dwcbBuffSize = dwcbMaxSubKeyLen + 1;
			TCHAR* pszKey = new TCHAR[dwcbBuffSize];
			lResult = RegEnumKeyEx (mhKey, dwIndex, pszKey, &dwcbBuffSize, nullptr, nullptr, nullptr, &ftLastWriteTime);
			
			if (lResult == ERROR_SUCCESS)
				szKey = pszKey;

			delete [] pszKey;

			if (lResult == ERROR_SUCCESS)
			{
				if (!(*pKeyEnumProc) (szKey.c_str(), pUserData))
				{
					lResult = ERROR_NO_MORE_ITEMS;
				}
			}
		}
	}
}

bool SVRegistryClass::DeleteKey()
{
	if (ERROR_SUCCESS == RegDeleteKey (mhKey, _T("")))
	{
		mhKey = nullptr;
		return true;
	}
	
	return false;
}

bool SVRegistryClass::CreatedNewKey()
{
	return mbCreatedNewKey;
}

SVRegistryClass * SVRegistryClass::OpenSubKey( LPCTSTR p_szSubKey )
{
	std::string szKey;
	std::string szSubKey = p_szSubKey;

	szKey = mszKey;
	if (szSubKey.substr(0,1) != _T("\\"))
	{
		szKey += _T("\\");
	}
	szKey += szSubKey;
	return new SVRegistryClass( szKey.c_str() );
}

bool SVRegistryClass::DeleteValue(LPCTSTR p_szValueName)
{
	return (ERROR_SUCCESS == RegDeleteValue (mhKey, p_szValueName));
}

bool SVRegistryClass::SetShadowFileName(LPCTSTR szFileName)
{
	return SetRegistryValue( SV_SHADOWFILE, szFileName );
}

bool SVRegistryClass::GetShadowFileName(std::string & szShadowFile)
{
	return GetRegistryValue( SV_SHADOWFILE, szShadowFile );
}

bool SVRegistryClass::GetDefaultShadowFileName(std::string & szShadowFile)
{
	TCHAR szFname[_MAX_FNAME], szPath[_MAX_PATH];
	std::string szKey;

	if (GetModuleFileName (nullptr, szPath, _MAX_PATH))
	{
		_tsplitpath (szPath, nullptr, nullptr, szFname, nullptr);
		_tgetcwd (szPath, _MAX_PATH);

		szShadowFile = szPath;
		szShadowFile += SV_BACKSLASH;
		szShadowFile += szFname;
		szShadowFile += SV_UNDERLINE;
		szKey = mszKey;
		SvUl::searchAndReplace (szKey, SV_BACKSLASH, SV_UNDERLINE);
		szShadowFile += szKey;
		szShadowFile += SV_SHADOWFILEEXT;
		return true;
	}

	return false;
}

bool SVRegistryClass::Import()
{
	std::string szShadowFile;

	if (GetShadowFileName (szShadowFile))
	{
		return Import (szShadowFile.c_str());
	}
	else
	{
		if (GetDefaultShadowFileName (szShadowFile))
		{
			return Import (szShadowFile.c_str());
		}
		else
		{
			return false;
		}
	}
}

bool SVRegistryClass::Import(LPCTSTR szFileName)
{
	FILE *pFile;
	bool rc = false;
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
			SvStl::MessageMgrStd Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE, szFileName, SvStl::SourceFileParams(StdMessageParams) );
			Exception.Throw();
		}
	}
	else
	{
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FAILED, szFileName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	return rc;
}

bool SVRegistryClass::ImportKeys(FILE * pFile)
{
	bool rc = true;
	std::string szName;
	SVByteVector ValueVector;
	DWORD dwType;
	int iResult;

	for (iResult = GetImportString (pFile, szName, ValueVector, &dwType);
		(iResult != SV_ISEOF) && (iResult != SV_ISERROR);
		iResult = GetImportString (pFile, szName, ValueVector, &dwType))
	{
		switch (iResult)
		{
		case SV_ISKEY :
			{
				SVRegistryClass reg(szName.c_str());
				reg.ImportKeys (pFile);
			}
			break;

		case SV_ISVALUE :
			SetRegistryValue( szName.c_str(),
				ValueVector,
				dwType,
				static_cast< DWORD >( ValueVector.size() ) );
			break;

		case SV_ISGARBAGE :
			{
				SvStl::MessageMgrStd Exception( SvStl::DataOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_EXPECTED_KEY, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}
			break;

		default :
			break;
		}
	}
	
	return rc;
}

int SVRegistryClass::GetImportString(FILE * pFile, std::string& rName, SVByteVector& rValueVector, DWORD * pdwType)
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
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	switch (tBuffer)
	{
	case _T('[') :
		// read the key name
		for (rName.clear();
			(_T('\n') != (tBuffer = _fgettc (pFile))) && !feof (pFile) && !ferror (pFile);
			rName += tBuffer);

		if (ferror (pFile))
		{
			SvStl::MessageMgrStd Exception( SvStl::DataOnly );
			Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			Exception.Throw();
		}

		if( std::string::npos == rName.rfind (_T(']')))
		{
			return SV_ISGARBAGE;
		}

		rName.erase(rName.rfind (_T(']')));

		return SV_ISKEY;
		break;

	case _T('"') :
		// get the value name
		for (rName.clear();
			(_T('"') != (tBuffer = _fgettc (pFile))) && !feof (pFile) && !ferror (pFile);
			)
		{
			if (tBuffer == _T('\\'))
			{
				tBuffer = _fgettc (pFile);
				if (ferror (pFile) || feof (pFile) || tBuffer == _T('\n'))
					return SV_ISERROR;
			}
			rName += tBuffer;
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
			rValueVector.clear();
			for (i = 0; _ftscanf (pFile, _T(" %hhx"), &tBuffer); i++)
			{
				rValueVector.push_back((BYTE) tBuffer);
				_ftscanf (pFile, _T("%c"), &tBuffer);
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
			rValueVector.clear();
			if (_ftscanf (pFile, _T(" dword:%x"), pdwType))
			{
				for( i = 0; i < sizeof( DWORD ); i++ )
				{
					rValueVector.push_back( ( ( BYTE* ) pdwType )[ i ] );
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
						rValueVector.clear();
							
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
							rValueVector.push_back((BYTE) tBuffer);
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

bool SVRegistryClass::Export()
{
	std::string szShadowFile;

	if (GetShadowFileName (szShadowFile))
	{
		return Export (szShadowFile.c_str());
	}
	else
	{
		if (GetDefaultShadowFileName (szShadowFile))
		{
			return Export (szShadowFile.c_str());
		}
		else
		{
			return false;
		}
	}
}

bool SVRegistryClass::Export(LPCTSTR szFileName)
{
	FILE *pFile;
	bool rc = false;

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
		SvStl::MessageMgrStd Exception( SvStl::DataOnly );
		Exception.setMessage( SVMSG_LIB_REGISTRY_EXPORT_FAILED, szFileName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	return rc;
}

bool SVRegistryClass::ExportKeys(FILE * pFile)
{
	bool rc = true;
	std::string szKey;
	DWORD dwIndex;
	DWORD dwcSubKeys;
	DWORD dwcbMaxSubKeyLen;
	DWORD dwcbBuffSize;
	LONG lResult;

	_ftprintf (pFile, _T("[%s]\n"), mszKey.c_str());

	if (ExportValues (pFile))
	{
		_ftprintf (pFile, _T("\n"));
		RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, &dwcSubKeys, &dwcbMaxSubKeyLen, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

		for (dwIndex = 0; dwIndex < dwcSubKeys; dwIndex++)
		{
			dwcbBuffSize = dwcbMaxSubKeyLen + 1;
			TCHAR* pszKey = new TCHAR[dwcbBuffSize];
			lResult= RegEnumKeyEx (mhKey, dwIndex, pszKey, &dwcbBuffSize, nullptr, nullptr, nullptr, nullptr);
			szKey = pszKey;
			delete [] pszKey;

			if (lResult == ERROR_SUCCESS)
			{
				try
				{
					szKey.insert (0, SV_BACKSLASH);
					szKey.insert(0, mszKey.c_str());
					SVRegistryClass reg(szKey.c_str());
					rc &= reg.ExportKeys (pFile);
					szKey.clear();
				}
				catch ( SvStl::MessageContainer& rSvE )
				{
					rc = false;
					rSvE.logMessage();
				}
			}
			else
			{
				rc = false;
				SvStl::MessageMgrStd Exception( SvStl::DataOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED, szKey.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}
		}
	}
	else
	{
		rc = false;
	}

	return rc;
}

bool SVRegistryClass::ExportValues(FILE * pFile)
{
	bool rc = true;
	TCHAR *pszName, *ptData;
	BYTE *pData;
	DWORD dwIndex;
	DWORD dwcValues, dwcbValueMax, dwcbNameMax;
	DWORD dwcbName, dwcbValue, dwType;
	LONG lResult;
	LONG lLineLen;
	int i;
	std::string szComma;

	RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwcValues, &dwcbNameMax, &dwcbValueMax, nullptr, nullptr);

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
			lResult= RegEnumValue (mhKey, dwIndex, pszName, &dwcbName, nullptr, &dwType, pData, &dwcbValue);

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
						lLineLen += _ftprintf( pFile, _T( "%s" ), szComma.c_str() );
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
						lLineLen += _ftprintf( pFile, _T( "%s" ), szComma.c_str() );
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
				rc = false;
				SvStl::MessageMgrStd Exception( SvStl::DataOnly );
				Exception.setMessage( SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED, mszKey.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}
		}
		delete [] pData;
		delete [] pszName;
	}

	return rc;
}

bool SVRegistryClass::GetRegistryValue(DWORD dwIndex, std::string& szValueName, std::string& szValue, LPDWORD pdwType)
{
	bool rc = false;
	DWORD dwcbNameMax, dwcbValueMax, dwcbName, dwcbValue;
	
	RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwcbNameMax, &dwcbValueMax, nullptr, nullptr);

	TCHAR* pName = new TCHAR[++dwcbNameMax];
	TCHAR* pValue = new TCHAR[dwcbValueMax];

	dwcbName = dwcbNameMax;
	dwcbValue = dwcbValueMax;

	if (ERROR_SUCCESS == RegEnumValue (mhKey, dwIndex, pName, &dwcbName, nullptr, pdwType, reinterpret_cast<BYTE *>(pValue), &dwcbValue))
	{
		memset ((void *) pName, 0, dwcbNameMax * sizeof (TCHAR));
		memset ((void *) pValue, 0, dwcbValueMax * sizeof (TCHAR));

		szValueName = pName;
		szValue = pValue;
		rc = true;
	}

	delete [] pName;
	delete [] pValue;

	return rc;
}

DWORD SVRegistryClass::NumSubKeys()
{
	DWORD dwNumSubKeys;

	RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, &dwNumSubKeys, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	return dwNumSubKeys;
}

