// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : modulename
// * .File Name       : $Workfile:   SVPackedFile.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   10 Nov 2014 16:51:36  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPackedFile.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL SVPackedFile::PackFiles( const CString& szFile, const CString& szPackedFile )
{
	CFileException FileException;
	SvStl::MessageMgrNoDisplay Exception( SvStl::DataOnly );
	TCHAR szMessage[80], szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	HANDLE hFindFile;
	WIN32_FIND_DATAW FindData;
	DWORD dwPackedFileVersion = SV_PACKEDFILE_VERSION;
	CFile PackedFile, SourceFile;
	CString szSourceFile, szWildCard, szPath;
	BYTE Buffer[1024];
	UINT CountRead, NumFiles = 0, PathLen;

	USES_CONVERSION;

	if (!(PackedFile.Open (szPackedFile, CFile::shareDenyNone | CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &FileException)))
	{
		FileException.GetErrorMessage (szMessage, sizeof (szMessage));
		Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
		Exception.Throw();
	}

	try
	{
		PackedFile.Write (&dwPackedFileVersion, sizeof (DWORD));
	}
	catch (CFileException e)
	{
		e.GetErrorMessage (szMessage, sizeof (szMessage));
		PackedFile.Close();
		Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
		Exception.Throw();
	}

	_tsplitpath (szFile, szDrive, szDir, szFName, szExt);
	szWildCard = szDrive;
	szWildCard += szDir;
	szWildCard += _T("*.*");

	hFindFile = FindFirstFileW (T2W((TCHAR *)(LPCTSTR)szWildCard), &FindData);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		PackedFile.Close ();
		return FALSE;
	}

	szPath = szDrive;
	szPath += szDir;
	do
	{
		if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_tsplitpath (W2T((WCHAR *)(FindData.cFileName)), szDrive, szDir, szFName, szExt);
			if (_tcscmp (szExt, _T(".svf")))
			{
				szSourceFile = szPath;
				szSourceFile += W2T((WCHAR *)(FindData.cFileName));
				try
				{
					PackedFile.Write (&FindData, sizeof (WIN32_FIND_DATAW));
					PathLen = (szPath.GetLength() * sizeof (WCHAR)) + sizeof (WCHAR);
					PackedFile.Write (&PathLen, sizeof (PathLen));
					PackedFile.Write (T2W((TCHAR *)(LPCTSTR)szPath), PathLen);
				}
				catch (CFileException* pe)
				{
					pe->GetErrorMessage (szMessage, sizeof (szMessage));
					pe->Delete();
					PackedFile.Close();
					FindClose (hFindFile);
					Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
					Exception.Throw();
				}

				if (SourceFile.Open (szSourceFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, &FileException))
				{
					try
					{
						while (CountRead = SourceFile.Read (Buffer, sizeof(Buffer)))
						{
							PackedFile.Write(Buffer, CountRead);
						}
					}
					catch (CFileException* pe)
					{
						pe->GetErrorMessage (szMessage, sizeof (szMessage));
						pe->Delete();
						PackedFile.Close();
						SourceFile.Close();
						FindClose (hFindFile);
						Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
						Exception.Throw();
					}
					SourceFile.Close();
					NumFiles++;
				}
				else
				{
					PackedFile.Close();
					FindClose (hFindFile);
					Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, nullptr, StdMessageParams );
					Exception.Throw();
				}
			}
		}
	} while (FindNextFileW (hFindFile, &FindData));

	PackedFile.Close();
	FindClose (hFindFile);

	return (BOOL) NumFiles;
}

BOOL SVPackedFile::UnPackFiles( const CString& szPackedFile, const CString& szUnPackDir )
{
	WIN32_FIND_DATAW FindData;
	CFile PackedFile, SourceFile;
	CFileException FileException;
	DWORD dwPackedFileVersion;
	TCHAR szMessage[80];
	CString szPath;
	SvStl::MessageMgrNoDisplay Exception( SvStl::DataOnly );
	UINT CountRead, PathLen, BytesRead;
	BYTE Buffer[_MAX_PATH * sizeof (TCHAR)];
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	USES_CONVERSION;

	m_configFilePath.Empty();

	if (PackedFile.Open (szPackedFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, &FileException))
	{
		try
		{
			if (PackedFile.Read (&dwPackedFileVersion, sizeof (DWORD)) == sizeof (DWORD))
			{
				switch (dwPackedFileVersion)
				{
					case 1 :
						while (sizeof (WIN32_FIND_DATAW) == (CountRead = PackedFile.Read (&FindData, sizeof (WIN32_FIND_DATAW))))
						{
							PackedFile.Read (&PathLen, sizeof (PathLen));
							memset (Buffer, 0, _MAX_PATH * sizeof (TCHAR));
							PackedFile.Read (Buffer, PathLen);
							if(szUnPackDir.IsEmpty())
								{
							szPath.Format (_T("%s%s"), W2T((WCHAR*)Buffer), W2T((WCHAR*)(FindData.cFileName)));
								}
							else
								{
								szPath.Format (_T("%s\\%s"), szUnPackDir, W2T((WCHAR*)(FindData.cFileName)));
								}

							_tsplitpath (szPath, szDrive, szDir, szFName, szExt);
							if( _tcscmp(szExt, _T(".svx")) == 0 )
							{
								m_configFilePath = szPath;
							}

							CreateDirectoryW ((WCHAR *) Buffer, NULL);

							if (SourceFile.Open (szPath, CFile::shareDenyNone | CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &FileException))
							{
								for ( ; FindData.nFileSizeHigh; --FindData.nFileSizeHigh)
								{
									for (BytesRead = 0; BytesRead < MAXDWORD ; BytesRead += CountRead)
									{
										CountRead = PackedFile.Read( Buffer, std::min( ( MAXDWORD - BytesRead ), static_cast< UINT >( sizeof( Buffer ) ) ) );
										SourceFile.Write (Buffer, CountRead);
									}
								}
								for ( ; FindData.nFileSizeLow; FindData.nFileSizeLow -= CountRead)
								{
									CountRead = PackedFile.Read( Buffer, std::min< UINT >( FindData.nFileSizeLow, sizeof( Buffer ) ) );
									SourceFile.Write( Buffer, CountRead );
								}
								SourceFile.Close ();
							}
							else
							{
								FileException.GetErrorMessage (szMessage, sizeof (szMessage));
								PackedFile.Close();
								Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
								Exception.Throw();
							}
						}
						PackedFile.Close();
						return TRUE;
						break;

					default :
						PackedFile.Close();
						return FALSE;
						break;
				}
			}
		}
		catch (CFileException* pe)
		{
			pe->GetErrorMessage (szMessage, sizeof (szMessage));
			pe->Delete();
			if (SourceFile.m_hFile != CFile::hFileNull)
			{
				SourceFile.Close();
			}
			PackedFile.Close();
			Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, StdMessageParams );
			Exception.Throw();
		}
		PackedFile.Close();
	}

	return FALSE;
}

SVPackedFile::SVPackedFile()
{
}

SVPackedFile::~SVPackedFile()
{
}

const CString& SVPackedFile::getConfigFilePath() const
{
	return m_configFilePath;
}

