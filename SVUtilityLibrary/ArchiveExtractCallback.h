//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file ArchiveExtractCallback.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Archive extract callback class which is used by the archive interface
/// These files have been adapted to Körber Pharma Inspection GmbH requirements
//******************************************************************************

// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/7zip/UI/Client7z/Client7z.cpp
#pragma once
#include <string>
#include <vector>
#include "IArchive.h"
#include "IPassword.h"

//This comment is to avoid that the IPassword PASSWORD_INTERFACE include is marked as not required due to a macro

class ArchiveExtractCallback : public IArchiveExtractCallback, public ICryptoGetTextPassword
{
public:

	ArchiveExtractCallback( const CComPtr<IInArchive>& rArchiveHandler, const std::string& rOutputFolder, std::vector<std::string>& rUnzippedFiles);
	virtual ~ArchiveExtractCallback();

	STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IProgress
	STDMETHOD(SetTotal)( UInt64 size );
	STDMETHOD(SetCompleted)( const UInt64* completeValue );

	// IArchiveExtractCallback
	STDMETHOD(GetStream)( UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode );
	STDMETHOD(PrepareOperation)( Int32 askExtractMode );
	STDMETHOD(SetOperationResult)( Int32 resultEOperationResult );

	// ICryptoGetTextPassword
	STDMETHOD(CryptoGetTextPassword)( BSTR* password );

private:

	void GetPropertyFilePath( UInt32 index );
	void GetPropertyAttributes( UInt32 index );
	void GetPropertyIsDir( UInt32 index );
	void GetPropertyModifiedTime( UInt32 index );
	void GetPropertySize( UInt32 index );
	std::string GetPath(const std::string& rFilePath);

private:
	long m_refCount{0};
	CComPtr<IInArchive> m_ArchiveHandler;
	std::string m_OutputFolder;
	std::vector<std::string>& m_rUnzippedFiles;

	std::string m_relPath;
	std::string m_absPath;
	bool m_isDir;

	bool m_hasAttrib;
	UInt32 m_attrib;

	bool m_hasModifiedTime;
	FILETIME m_modifiedTime;

	bool m_hasNewFileSize;
	UInt64 m_newFileSize;
};
