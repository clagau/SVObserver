//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file ArchiveUpdateCallback.cpp
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Archive update callback class which is used by the archive interface
/// These files have been adapted to Seidenader requirements
//******************************************************************************

// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/7zip/UI/Client7z/Client7z.cpp
#include "StdAfx.h"
#include <sys/types.h>
#include "ArchiveUpdateCallback.h"
#include "PropVariant.h"
#include "InStreamWrapper.h"

static const LONGLONG cTimeConvertOffset = 116444736000000000;

ArchiveUpdateCallback::ArchiveUpdateCallback(const std::vector<std::string>& rFiles, const std::string& rFolderPrefix)
	: m_rFiles {rFiles}
	, m_FolderPrefix {rFolderPrefix}
{
	if(!m_FolderPrefix.empty())
	{
		//Make sure Folder Prefix has ending backslash
		if('\\' != m_FolderPrefix[m_FolderPrefix.size()-1])
		{
			m_FolderPrefix += '\\';
		}
	}
}

ArchiveUpdateCallback::~ArchiveUpdateCallback()
{
}

STDMETHODIMP ArchiveUpdateCallback::QueryInterface(REFIID iid, void** ppvObject)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvObject = reinterpret_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_IArchiveUpdateCallback)
	{
		*ppvObject = static_cast<IArchiveUpdateCallback*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICryptoGetTextPassword2)
	{
		*ppvObject = static_cast<ICryptoGetTextPassword2*>(this);
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICompressProgressInfo)
	{
		*ppvObject = static_cast<ICompressProgressInfo*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::AddRef()
{
	return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::Release()
{
	ULONG res = static_cast<ULONG>(InterlockedDecrement(&m_refCount));
	if (res == 0)
	{
		delete this;
	}
	return res;
}

STDMETHODIMP ArchiveUpdateCallback::SetTotal(UInt64 size)
{
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::SetCompleted(const UInt64* completeValue)
{
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetUpdateItemInfo(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive)
{
	// Setting info for Create mode (vs. Append mode).
	// TODO: support append mode
	if (newData != NULL)
	{
		*newData = 1;
	}

	if (newProperties != NULL)
	{
		*newProperties = 1;
	}

	if (indexInArchive != NULL)
	{
		*indexInArchive = static_cast<UInt32>(-1); // TODO: UInt32.Max
	}

	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
{
	CPropVariant prop;

	if (propID == kpidIsAnti)
	{
		prop = false;
		prop.Detach(value);
		return S_OK;
	}

	checkFileInfoList();

	if (index >= m_rFiles.size() || index >= m_FileInfoVector.size())
	{
		return E_INVALIDARG;
	}
	const std::string& rFile = m_rFiles.at(index);
	const struct _stat& rFileInfo = m_FileInfoVector.at(index);

	switch (propID)
	{
		case kpidPath:
		{
			std::string RelativePath;
			//When no folder prefix then zip only the files with no path
			if(m_FolderPrefix.empty())
			{
				std::string::size_type Pos = rFile.rfind('\\');
				if (std::string::npos != Pos)
				{
					Pos++;
					RelativePath = rFile.substr(Pos, rFile.size() - Pos);
				}
			}
			else
			{
				if (m_FolderPrefix.size() < rFile.size())
				{
					if (m_FolderPrefix == rFile.substr(0, m_FolderPrefix.size()))
					{
						RelativePath = rFile.substr(m_FolderPrefix.size(), rFile.size() - m_FolderPrefix.size());
					}
				}
			}
			_bstr_t bstrPath{RelativePath.c_str()};
			prop = bstrPath.Detach();
			break;
		}
		case kpidIsDir:
			prop = (rFileInfo.st_mode & _S_IFDIR) ? true : false;
			break;
		case kpidSize:
			prop = static_cast<unsigned long long> (rFileInfo.st_size);
			break;
		case kpidAttrib:
			//No attributes
			break;
		case kpidCTime:
			prop = ConvertTime(rFileInfo.st_ctime);
			break;
		case kpidATime:
			prop = ConvertTime(rFileInfo.st_atime);
			break;
		case kpidMTime:
			prop = ConvertTime(rFileInfo.st_mtime);
			break;
	}

	prop.Detach(value);
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream** inStream)
{
	if (index >= m_rFiles.size())
	{
		return E_INVALIDARG;
	}

	const std::string& rFile = m_rFiles.at(index);

	struct _stat FileInfo;
	if (0 != _stat(rFile.c_str(), &FileInfo))
	{
		return E_FAIL;
	}
	if (FileInfo.st_mode & _S_IFDIR)
	{
		return S_OK;
	}

	CComPtr<IStream> fileStream;

	_bstr_t wFile{rFile.c_str()};
	if (FAILED(SHCreateStreamOnFileEx(wFile, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &fileStream)))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	CComPtr<InStreamWrapper> wrapperStream = new InStreamWrapper(fileStream);
	*inStream = wrapperStream.Detach();

	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::SetOperationResult(Int32 operationResult)
{
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::CryptoGetTextPassword2(Int32* passwordIsDefined, BSTR* password)
{
	// TODO: support passwords
	*passwordIsDefined = 0;
	*password = SysAllocString(L"");
	return *password != 0 ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP ArchiveUpdateCallback::SetRatioInfo(const UInt64* inSize, const UInt64* outSize)
{
	return S_OK;
}

void ArchiveUpdateCallback::checkFileInfoList()
{
	if (m_rFiles.size() != m_FileInfoVector.size())
	{
		m_FileInfoVector.clear();
		m_FileInfoVector.reserve(m_rFiles.size());

		for (const auto& rFileName : m_rFiles)
		{
			struct _stat FileInfo;
			if (0 == _stat(rFileName.c_str(), &FileInfo))
			{
				m_FileInfoVector.emplace_back(FileInfo);
			}
		}
	}
}

FILETIME ArchiveUpdateCallback::ConvertTime(const __time64_t& rTime)
{
	FILETIME fileTime;

	LONGLONG llDate = Int32x32To64(rTime, 10000000) + cTimeConvertOffset;
	fileTime.dwLowDateTime = static_cast<DWORD> (llDate);
	fileTime.dwHighDateTime = static_cast<DWORD> (llDate >> 32);

	return fileTime;
}