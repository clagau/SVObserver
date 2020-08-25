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
//Moved to precompiled header: #include <algorithm>
#include "SVPackedFile.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPackedFile::SVPackedFile()
{
}

SVPackedFile::~SVPackedFile()
{
}

bool SVPackedFile::UnPackFiles( LPCTSTR PackedFileName, LPCTSTR UnPackDir /* = nullptr*/ )
{
	WIN32_FIND_DATAW FindData;
	CFile PackedFile, SourceFile;
	CFileException FileException;
	DWORD dwPackedFileVersion;
	TCHAR szMessage[80];
	std::string Path;
	SvStl::MessageManager Exception( SvStl::MsgType::Data);
	UINT CountRead, PathLen, BytesRead;
	BYTE Buffer[_MAX_PATH * sizeof (TCHAR)];
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	USES_CONVERSION;

	m_configFilePath.clear();

	if (PackedFile.Open (PackedFileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary, &FileException))
	{
		try
		{
			if (PackedFile.Read (&dwPackedFileVersion, sizeof (DWORD)) == sizeof (DWORD))
			{
				switch (dwPackedFileVersion)
				{
					case SV_PACKEDFILE_VERSION :
						while (sizeof (WIN32_FIND_DATAW) == (CountRead = PackedFile.Read (&FindData, sizeof (WIN32_FIND_DATAW))))
						{
							PackedFile.Read (&PathLen, sizeof (PathLen));
							memset (Buffer, 0, _MAX_PATH * sizeof (TCHAR));
							PackedFile.Read (Buffer, PathLen);
							if( nullptr == UnPackDir )
							{
								Path = SvUl::Format (_T("%s%s"), W2T(reinterpret_cast<wchar_t*> (Buffer) ), W2T( FindData.cFileName));
							}
							else
							{
								Path = SvUl::Format (_T("%s\\%s"), UnPackDir, W2T( FindData.cFileName));
							}

							_tsplitpath (Path.c_str(), szDrive, szDir, szFName, szExt);
							if( _tcscmp(szExt, SvDef::cConfigExtension) == 0 )
							{
								m_configFilePath = Path;
							}

							CreateDirectoryW (reinterpret_cast<wchar_t*> (Buffer), nullptr);

							if (SourceFile.Open (Path.c_str(), CFile::shareDenyNone | CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &FileException))
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
								Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, SvStl::SourceFileParams(StdMessageParams) );
								Exception.Throw();
							}
						}
						PackedFile.Close();
						return true;
						break;

					default :
						PackedFile.Close();
						return false;
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
			Exception.setMessage( SVMSG_LIB_PACKFILE_IO_ERROR, szMessage, SvStl::SourceFileParams(StdMessageParams) );
			Exception.Throw();
		}
		PackedFile.Close();
	}

	return false;
}

