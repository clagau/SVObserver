//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file ArchiveUpdateCallback.h
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Archive update callback class which is used by the archive interface
/// These files have been adapted to Körber Pharma Inspection GmbH requirements
//******************************************************************************

// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/7zip/UI/Client7z/Client7z.cpp
#pragma once

#include <string>
#include <vector>
#include <sys/stat.h>
#include "IArchive.h"
#include "ICoder.h"
#include "IPassword.h"

//This comment is to avoid that the ICoder CODER_INTERFACE include is marked as not required due to a macro
//This comment is to avoid that the IPassword PASSWORD_INTERFACE include is marked as not required due to a macro

class ArchiveUpdateCallback : public IArchiveUpdateCallback, public ICryptoGetTextPassword2, public ICompressProgressInfo
{
public:

	explicit ArchiveUpdateCallback(const std::vector<std::string>& rFiles, const std::string& rFolderPrefix = std::string());
	virtual ~ArchiveUpdateCallback();

	STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IProgress
	STDMETHOD(SetTotal)( UInt64 size );
	STDMETHOD(SetCompleted)( const UInt64* completeValue );

	// IArchiveUpdateCallback
	STDMETHOD(GetUpdateItemInfo)( UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive );
	STDMETHOD(GetProperty)( UInt32 index, PROPID propID, PROPVARIANT* value );
	STDMETHOD(GetStream)( UInt32 index, ISequentialInStream** inStream );
	STDMETHOD(SetOperationResult)( Int32 operationResult );

	// ICryptoGetTextPassword2
	STDMETHOD(CryptoGetTextPassword2)( Int32* passwordIsDefined, BSTR* password );

	// ICompressProgressInfo
	STDMETHOD(SetRatioInfo)( const UInt64* inSize, const UInt64* outSize );

private:
	void checkFileInfoList();
	FILETIME ConvertTime(const __time64_t& rTime);

	long m_refCount{0};
	std::string m_FolderPrefix;
	const std::vector<std::string>& m_rFiles;
	std::vector<struct _stat> m_FileInfoVector;
};
