//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RegistryAccess
//* .File Name       : $Workfile:   RegistryAccess.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:08:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RegistryAccess.h"

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

void RegistryAccess::InitRegistry(LPCTSTR pKey)
{
	DWORD dwDisposition = 0L;
	std::string keyString = (nullptr != pKey) ? pKey : std::string();
	std::string fullKeyString = keyString;
	HKEY hBaseKey = HKEY_LOCAL_MACHINE;
	LONG lResult;
	TCHAR szFname[_MAX_FNAME], szPath[_MAX_PATH];

	m_keyString = keyString;

	// check if first node of key is an HKEY Value
	if (fullKeyString.substr(0, 17) == SV_HKEY_CLASSES_ROOT)
	{
		hBaseKey = HKEY_CLASSES_ROOT;
		fullKeyString.erase(0, 18);
	}
	else
	{
		if (fullKeyString.substr(0, 17) == SV_HKEY_CURRENT_USER)
		{
			hBaseKey = HKEY_CURRENT_USER;
			fullKeyString.erase(0, 18);
		}
		else
		{
			if (fullKeyString.substr(0, 18) == SV_HKEY_LOCAL_MACHINE)
			{
				hBaseKey = HKEY_LOCAL_MACHINE;
				fullKeyString.erase(0, 19);
			}
			else
			{
				if (fullKeyString.substr(0, 10) == SV_HKEY_USERS)
				{
					hBaseKey = HKEY_USERS;
					fullKeyString.erase(0, 11);
				}
				else
				{
					if (fullKeyString.substr(0, strlen(SV_HKEY_PERFORMANCE_DATA)) == SV_HKEY_PERFORMANCE_DATA)
					{
						hBaseKey = HKEY_PERFORMANCE_DATA;
						fullKeyString.erase(0, strlen(SV_HKEY_PERFORMANCE_DATA)+1);
					}
					else
					{
						m_keyString = SV_HKEY_LOCAL_MACHINE;
						m_keyString += SV_BACKSLASH;
						m_keyString += SV_ROOT_KEY;

						fullKeyString = SV_ROOT_KEY;

						if (keyString.substr(0, 1) != SV_BACKSLASH)
						{
							fullKeyString += SV_BACKSLASH;
							m_keyString += SV_BACKSLASH;
							if (GetModuleFileName (nullptr, szPath, _MAX_PATH))
							{
								_tsplitpath (szPath, nullptr, nullptr, szFname, nullptr);
								fullKeyString += szFname;
								fullKeyString += SV_BACKSLASH;
								m_keyString += szFname;
								m_keyString += SV_BACKSLASH;
							}
						}
						fullKeyString += keyString;
						m_keyString += keyString;
					}
				}
			}
		}
	}

	mhKey = (HKEY) nullptr;

	if (SvUl::Right(m_keyString, 1) == SV_BACKSLASH)
		m_keyString.erase(m_keyString.size() - 1);

	lResult = RegOpenKeyEx (hBaseKey,
		fullKeyString.c_str(),
		static_cast< DWORD >( 0 ),
		KEY_ALL_ACCESS,
		&mhKey );

	if( lResult == ERROR_ACCESS_DENIED)
	{
		lResult = RegOpenKeyEx (hBaseKey,
			fullKeyString.c_str(),
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
				fullKeyString.c_str(),
				static_cast< DWORD >( 0 ),
				nullptr,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				static_cast< SECURITY_ATTRIBUTES* >( nullptr ),
				&mhKey,
				&dwDisposition ) )
			{
				mhKey = static_cast< HKEY >( nullptr );
				SvStl::MessageManager Exception(SvStl::MsgType::Data);
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED, fullKeyString.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}

			mbCreatedNewKey = true;
		}
		else
		{
			mhKey = (HKEY) nullptr;
			SvStl::MessageManager Exception(SvStl::MsgType::Data );
			Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED, fullKeyString.c_str(), SvStl::SourceFileParams(StdMessageParams) );
			Exception.Throw();
		}
	}
}

RegistryAccess::RegistryAccess(LPCTSTR keyString)
{
  InitRegistry (keyString);
}

RegistryAccess::RegistryAccess()
{
  InitRegistry( _T("") ); 
}

RegistryAccess& RegistryAccess::operator=(const RegistryAccess &svrRight)
{
	if (mhKey)
		RegCloseKey (mhKey);

	InitRegistry( svrRight.m_keyString.c_str() );
	return (*this);
}

RegistryAccess::~RegistryAccess()
{
	if (mhKey)
		RegCloseKey (mhKey);
}

bool RegistryAccess::GetRegistryValue( LPCTSTR szValueName, std::string& szValue)
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

bool RegistryAccess::SetRegistryValue( LPCTSTR szValueName, LPCTSTR szValue )
{
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
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
  }
}

bool RegistryAccess::GetRegistryValue( LPCTSTR szValueName, DWORD *pdwValue)
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

bool RegistryAccess::SetRegistryValue( LPCTSTR szValueName, DWORD dwValue )
{
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
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}
}

bool RegistryAccess::GetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector)
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

bool RegistryAccess::SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector )
{
	 LONG lResult = RegSetValueEx( mhKey,
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
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}
}

bool RegistryAccess::SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector, DWORD dwType, DWORD dwLength )
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
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_SET_REGISTRY_VALUE_FAILED, szValueName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}
}

bool RegistryAccess::DeleteKey()
{
	if (ERROR_SUCCESS == RegDeleteKey (mhKey, _T("")))
	{
		mhKey = nullptr;
		return true;
	}
	
	return false;
}

bool RegistryAccess::CreatedNewKey()
{
	return mbCreatedNewKey;
}

bool RegistryAccess::SetShadowFileName(LPCTSTR szFileName)
{
	return SetRegistryValue( SV_SHADOWFILE, szFileName );
}

bool RegistryAccess::GetShadowFileName(std::string & szShadowFile)
{
	return GetRegistryValue( SV_SHADOWFILE, szShadowFile );
}

bool RegistryAccess::GetDefaultShadowFileName(std::string & szShadowFile)
{
	TCHAR szPath[_MAX_PATH];
	std::string keyString;

	if (GetModuleFileName (nullptr, szPath, _MAX_PATH))
	{
		TCHAR szFname[_MAX_FNAME];
		_tsplitpath (szPath, nullptr, nullptr, szFname, nullptr);
		_tgetcwd (szPath, _MAX_PATH);

		szShadowFile = szPath;
		szShadowFile += SV_BACKSLASH;
		szShadowFile += szFname;
		szShadowFile += SV_UNDERLINE;
		keyString = m_keyString;
		SvUl::searchAndReplace (keyString, SV_BACKSLASH, SV_UNDERLINE);
		szShadowFile += keyString;
		szShadowFile += SV_SHADOWFILEEXT;
		return true;
	}

	return false;
}

bool RegistryAccess::Import()
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

bool RegistryAccess::Import(LPCTSTR szFileName)
{
	FILE *pFile;
	bool rc = false;
	TCHAR cNewLine = _T('0');

	if (nullptr != (pFile = _tfopen (szFileName, _T("r"))))
	{
		if (_ftscanf (pFile, SV_SHADOWFILEHEADER _T("%c"), &cNewLine) &&
			cNewLine == _T('\n'))
		{
			rc = ImportKeys (pFile);
			fclose (pFile);
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Data );
			Exception.setMessage( SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE, szFileName, SvStl::SourceFileParams(StdMessageParams) );
			Exception.Throw();
		}
	}
	else
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FAILED, szFileName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	return rc;
}

bool RegistryAccess::ImportKeys(FILE * pFile)
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
				RegistryAccess reg(szName.c_str());
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
				SvStl::MessageManager Exception(SvStl::MsgType::Data );
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

int RegistryAccess::GetImportString(FILE * pFile, std::string& rName, SVByteVector& rValueVector, DWORD * pdwType)
{
	TCHAR tBuffer;
	int i;

	// skip white space characters
	for (tBuffer = static_cast<TCHAR> (_fgettc(pFile)); 
		 !feof (pFile) && !ferror (pFile) && _istspace (tBuffer); tBuffer = static_cast<TCHAR> (_fgettc(pFile)));

	if (feof (pFile))
		return SV_ISEOF;

	if (ferror (pFile))
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	switch (tBuffer)
	{
	case _T('[') :
		// read the key name
		for (rName.clear();
			(_T('\n') != (tBuffer = static_cast<TCHAR> (_fgettc(pFile)))) && !feof (pFile) && !ferror (pFile);
			rName += tBuffer);

		if (ferror (pFile))
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Data );
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
			(_T('"') != (tBuffer = static_cast<TCHAR> (_fgettc(pFile)))) && !feof (pFile) && !ferror (pFile);
			)
		{
			if (tBuffer == _T('\\'))
			{
				tBuffer = static_cast<TCHAR> (_fgettc(pFile));
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
			// cppcheck-suppress invalidScanfArgType_int ; this became visible when the class RegistryAccess was renamed: cppcheck-suppress (and this problem) should be removed when possible
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
}

bool RegistryAccess::Export()
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

bool RegistryAccess::Export(LPCTSTR szFileName)
{
	FILE *pFile;
	bool rc = false;

	if (nullptr != (pFile = _tfopen (szFileName, _T("w"))))
	{
		_ftprintf (pFile, _T("%s\n\n"), SV_SHADOWFILEHEADER);
		if (true == (rc = ExportKeys (pFile)))
		{
			SetShadowFileName (szFileName);
		}
		fclose (pFile);
	}
	else
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data );
		Exception.setMessage( SVMSG_LIB_REGISTRY_EXPORT_FAILED, szFileName, SvStl::SourceFileParams(StdMessageParams) );
		Exception.Throw();
	}

	return rc;
}

bool RegistryAccess::ExportKeys(FILE * pFile)
{
	bool rc = true;

	_ftprintf (pFile, _T("[%s]\n"), m_keyString.c_str());

	if (ExportValues (pFile))
	{
		_ftprintf (pFile, _T("\n"));

		DWORD dwcSubKeys;
		DWORD dwcbMaxSubKeyLen;
		RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, &dwcSubKeys, &dwcbMaxSubKeyLen, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

		for (DWORD dwIndex = 0; dwIndex < dwcSubKeys; dwIndex++)
		{
			DWORD dwcbBuffSize = dwcbMaxSubKeyLen + 1;
			TCHAR* pszKey = new TCHAR[dwcbBuffSize];
			LONG lResult= RegEnumKeyEx (mhKey, dwIndex, pszKey, &dwcbBuffSize, nullptr, nullptr, nullptr, nullptr);
			std::string keyString = pszKey;
			delete [] pszKey;

			if (lResult == ERROR_SUCCESS)
			{
				try
				{
					keyString.insert (0, SV_BACKSLASH);
					keyString.insert(0, m_keyString.c_str());
					RegistryAccess reg(keyString.c_str());
					rc &= reg.ExportKeys (pFile);
					keyString.clear();
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
				SvStl::MessageManager Exception(SvStl::MsgType::Data );
				Exception.setMessage( SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED, keyString.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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

bool RegistryAccess::ExportValues(FILE * pFile)
{
	bool rc = true;
	DWORD dwcValues, dwcbValueMax, dwcbNameMax;
	DWORD dwType;
	std::string szComma;

	RegQueryInfoKey (mhKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwcValues, &dwcbNameMax, &dwcbValueMax, nullptr, nullptr);

	if (dwcValues)
	{
		DWORD dwcbName = dwcbNameMax + 1;
		DWORD dwcbValue = dwcbValueMax;

		TCHAR* pszName = new TCHAR[dwcbName];
		BYTE* pData = (BYTE *) new char[dwcbValue];

		for (DWORD dwIndex = 0; dwIndex < dwcValues; dwIndex++)
		{
			dwcbName = dwcbNameMax + 1;
			dwcbValue = dwcbValueMax;
			LONG lResult= RegEnumValue (mhKey, dwIndex, pszName, &dwcbName, nullptr, &dwType, pData, &dwcbValue);

			if (lResult == ERROR_SUCCESS)
			{
				LONG lLineLen = _ftprintf (pFile, _T("\""));
				for (int i = 0; (i < (int) dwcbName) && pszName[i]; i++)
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
				{
					TCHAR* ptData = (TCHAR *)pData;
					lLineLen += _ftprintf(pFile, _T("\""));
					dwcbValue /= sizeof(TCHAR);
					for (int i = 0; (i < (int)dwcbValue) && ptData[i]; i++)
					{
						switch (ptData[i])
						{
							case SV_BACKSLASHCHAR:
								lLineLen += _ftprintf(pFile, _T("%c%c"), SV_BACKSLASHCHAR, SV_BACKSLASHCHAR);
								break;

							case _T('"'):
								lLineLen += _ftprintf(pFile, _T("%c%c"), SV_BACKSLASHCHAR, _T('"'));
								break;

							default:
								if (ptData[i] > _T('\xff'))
								{
									lLineLen += _ftprintf(pFile, _T("?"));
								}
								else
								{
									lLineLen += _ftprintf(pFile, _T("%c"), ptData[i]);
								}
						}
					}
					lLineLen += _ftprintf(pFile, _T("\"\n"));
				}
					break;

				case REG_DWORD :
					// cppcheck-suppress invalidPrintfArgType_uint ; this became visible when the class RegistryAccess was renamed: cppcheck-suppress (and this problem) should be removed when possible
					lLineLen += _ftprintf (pFile, _T("dword:%08.8x\n"), *((DWORD *) pData));
					break;

				case REG_BINARY :
					lLineLen += _ftprintf (pFile, _T("hex:"));
					szComma = _T(""); 
					for (int i = 0; i < (int) dwcbValue; i++)
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
				{
					TCHAR* ptData = (TCHAR *)pData;
					lLineLen += _ftprintf(pFile, _T("hex(%lu):"), dwType);
					szComma = _T("");
					dwcbValue /= sizeof(TCHAR);
					for (int i = 0; i < (int)dwcbValue; i++)
					{
						lLineLen += _ftprintf(pFile, _T("%s"), szComma.c_str());
						szComma = _T(",");

						if (lLineLen > 77)
						{
							_ftprintf(pFile, _T("\\\n  "));
							lLineLen = 2;
						}

						lLineLen += _ftprintf(pFile, _T("%02.2x"), ptData[i]);
					}
					_ftprintf(pFile, _T("\n"));
				}
					break;
				}
			}
			else
			{
				rc = false;
				SvStl::MessageManager Exception(SvStl::MsgType::Data );
				Exception.setMessage( SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED, m_keyString.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				Exception.Throw();
			}
		}
		delete [] pData;
		delete [] pszName;
	}

	return rc;
}

bool RegistryAccess::GetRegistryValue(DWORD dwIndex, std::string& szValueName, std::string& szValue, LPDWORD pdwType)
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
