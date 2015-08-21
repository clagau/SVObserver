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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPackedFile.cpp_v  $
 * 
 *    Rev 1.3   10 Nov 2014 16:51:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  970
 * SCR Title:  GetConfig and PutConfig cause a crash when there is not enough disk space
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised PackFiles to catch CFileException by pointer instead of value.
 * Revised UnpackFiles to catch CFileException by pointer instead of value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Feb 2014 16:36:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed SEC-part from method UnPackFiles.
 * Changed name of public method GetSecFilePath to getConfigFilePath.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 10:02:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:37:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Feb 2013 10:52:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all methods to remove forced CString type requirements for calling methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Oct 2010 08:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2009 10:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Nov 2002 14:47:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add the compare for .SVX  in the UnPackFiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jun 2001 13:34:40   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added the GetSecFilePath function to allow retreival of the SEC file path after unpacking the files.
 * Modified the UnPackFiles function to add a paramter that specifies the path that the files will be
 * unpacked to. 
 * Modified all CFile::Open calls to use the CFile::shareDenyNone access permission.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Feb 2001 16:34:46   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create a non-unicode version to operate with svobserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Feb 2001 08:56:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  113
 * SCR Title:  SVIMServer fails if path of destination file does not exist
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified to create directories if they don't already exist
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2000 14:38:00   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  22
 * SCR Title:  PLC Error/Alarm Management
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added error codes to report file errors 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Apr 05 2000 09:10:06   mike
 * Initial revision.
*/