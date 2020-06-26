//*****************************************************************************
/// \copyright (c) 2018,2018 by Jan Kaniewski, Keith J. Jones
/// \file ArchiveExtractCallback.cpp
/// All Rights Reserved 
/// Author           : Jan Kaniewski, Keith J. Jones
/// License          : Public Domain License
/// From             : https://github.com/keithjjones/7zip-cpp
//*****************************************************************************
/// Archive extract callback class which is used by the archive interface
/// These files have been adapted to Seidenader requirements
//******************************************************************************

// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/7zip/UI/Client7z/Client7z.cpp
#include "StdAfx.h"
#include <ShlObj.h>
#include <comdef.h>
#include "ArchiveExtractCallback.h"
#include "PropVariant.h"
#include "OutStreamWrapper.h"

constexpr GUID cICryptoGetTextPassword =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00}};
constexpr GUID cIArchiveExtractCallback =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x20, 0x00, 0x00}};
constexpr LPCTSTR cEmptyFileAlias = _T("[Content]");


ArchiveExtractCallback::ArchiveExtractCallback( const CComPtr<IInArchive>& rArchiveHandler, const std::string& rDirectory, std::vector<std::string>& rUnzippedFiles)
	: m_ArchiveHandler{rArchiveHandler}
	, m_OutputFolder{rDirectory}
	, m_rUnzippedFiles{rUnzippedFiles}
{
}

ArchiveExtractCallback::~ArchiveExtractCallback()
{
}

STDMETHODIMP ArchiveExtractCallback::QueryInterface( REFIID iid, void** ppvObject )
{
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = reinterpret_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cIArchiveExtractCallback )
	{
		*ppvObject = static_cast< IArchiveExtractCallback* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == cICryptoGetTextPassword )
	{
		*ppvObject = static_cast< ICryptoGetTextPassword* >( this );
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveExtractCallback::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &m_refCount ) );
}

STDMETHODIMP_(ULONG) ArchiveExtractCallback::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &m_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP ArchiveExtractCallback::SetTotal( UInt64  )
{
	//	- SetTotal is never called for ZIP and 7z formats
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetCompleted( const UInt64*)
{
	//Callback Event calls
	/*
	NB:
	- For ZIP format SetCompleted only called once per 1000 files in central directory and once per 100 in local ones.
	- For 7Z format SetCompleted is never called.
	*/
	//Don't call this directly, it will be called per file which is more consistent across archive types
	//TODO: incorporate better progress tracking
	//m_callback->OnProgress(m_absPath, *completeValue);
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::GetStream( UInt32 index, ISequentialOutStream** ppOutStream, Int32 askExtractMode )
{
	try
	{
		// Retrieve all the various properties for the file at this index.
		GetPropertyFilePath( index );
		if ( askExtractMode != NArchive::NExtract::NAskMode::kExtract )
		{
			return S_OK;
		}

		GetPropertyAttributes( index );
		GetPropertyIsDir( index );
		GetPropertyModifiedTime( index );
		GetPropertySize( index );
	}
	catch ( _com_error& ex )
	{
		return ex.Error();
	}

	if (m_OutputFolder.empty() || m_OutputFolder[m_OutputFolder.size() - 1] == _T('\\'))
	{
		m_absPath = m_OutputFolder + m_relPath;
	}
	else
	{
		m_absPath = m_OutputFolder + _T("\\") + m_relPath;
	}

	if ( m_isDir )
	{
		// Creating the directory here supports having empty directories.
		SHCreateDirectoryEx(nullptr, m_absPath.c_str(), nullptr);
		*ppOutStream = nullptr;
		return S_OK;
	}

	std::string Folder = GetPath( m_absPath );
	SHCreateDirectoryEx(nullptr, Folder.c_str(), nullptr);
	
	m_rUnzippedFiles.emplace_back(m_absPath);

	CComPtr<IStream> pFileStream;
	_bstr_t wFileName{m_absPath.c_str()};
	SHCreateStreamOnFileEx(wFileName, STGM_CREATE | STGM_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &pFileStream);
	if (nullptr ==  pFileStream)
	{
		m_absPath.clear();
		return HRESULT_FROM_WIN32( GetLastError() );
	}

	CComPtr<OutStreamWrapper> wrapperStream = new OutStreamWrapper(pFileStream);
	*ppOutStream = wrapperStream.Detach();

	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::PrepareOperation( Int32  )
{
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetOperationResult( Int32  )
{
	if ( m_absPath.empty() )
	{
		return S_OK;
	}

	if ( m_hasModifiedTime )
	{
		HANDLE fileHandle = CreateFile( m_absPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( fileHandle != INVALID_HANDLE_VALUE )
		{
			SetFileTime( fileHandle, NULL, NULL, &m_modifiedTime );
			CloseHandle( fileHandle );
		}
	}

	if ( m_hasAttrib )
	{
		SetFileAttributes( m_absPath.c_str(), m_attrib );
	}

	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::CryptoGetTextPassword( BSTR* )
{
	// TODO: support passwords
	return E_ABORT;
}

void ArchiveExtractCallback::GetPropertyFilePath( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = m_ArchiveHandler->GetProperty( index, kpidPath, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		m_relPath = cEmptyFileAlias;
	}
	else if ( prop.vt != VT_BSTR )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		_bstr_t bstr = prop.bstrVal;
#ifdef _UNICODE
		m_relPath = bstr;
#else
		m_relPath = std::string{bstr};
#endif
	}
}

void ArchiveExtractCallback::GetPropertyAttributes( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = m_ArchiveHandler->GetProperty( index, kpidAttrib, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		m_attrib = 0;
		m_hasAttrib = false;
	}
	else if ( prop.vt != VT_UI4 )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		m_attrib = prop.ulVal;
		m_hasAttrib = true;
	}
}

void ArchiveExtractCallback::GetPropertyIsDir( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = m_ArchiveHandler->GetProperty( index, kpidIsDir, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		m_isDir = false;
	}
	else if ( prop.vt != VT_BOOL )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		m_isDir = prop.boolVal != VARIANT_FALSE;
	}
}

void ArchiveExtractCallback::GetPropertyModifiedTime( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = m_ArchiveHandler->GetProperty( index, kpidMTime, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	if ( prop.vt == VT_EMPTY )
	{
		m_hasModifiedTime = false;
	}
	else if ( prop.vt != VT_FILETIME )
	{
		_com_issue_error( E_FAIL );
	}
	else
	{
		m_modifiedTime = prop.filetime;
		m_hasModifiedTime = true;
	}
}

void ArchiveExtractCallback::GetPropertySize( UInt32 index )
{
	CPropVariant prop;
	HRESULT hr = m_ArchiveHandler->GetProperty( index, kpidSize, &prop );
	if ( hr != S_OK )
	{
		_com_issue_error( hr );
	}

	switch ( prop.vt )
	{
	case VT_EMPTY:
		m_hasNewFileSize = false;
		return;
	case VT_UI1: 
		m_newFileSize = prop.bVal;
		break;
	case VT_UI2:
		m_newFileSize = prop.uiVal;
		break;
	case VT_UI4:
		m_newFileSize = prop.ulVal;
		break;
	case VT_UI8:
		m_newFileSize = (UInt64)prop.uhVal.QuadPart;
		break;
	default:
		_com_issue_error( E_FAIL );
	}

	m_hasNewFileSize = true;
}

std::string ArchiveExtractCallback::GetPath(const std::string& rFilePath)
{
	// Find the last "\" or "/" in the string and return it and everything before it.
	size_t index = rFilePath.rfind(_T('\\'));
	size_t index2 = rFilePath.rfind(_T('/'));

	if (index2 != std::string::npos && index2 > index)
	{
		index = index2;
	}

	if (index == std::string::npos)
	{
		// No path sep.
		return std::string();
	}
	else if (index + 1 >= rFilePath.size())
	{
		// Last char is path sep, the whole thing is a path.
		return rFilePath;
	}
	else
	{
		return rFilePath.substr(0, index + 1);
	}
}
