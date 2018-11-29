//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Zip helper
//* .File Name       : $Workfile:   ZipHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 04:37:14  $
//* ----------------------------------------------------------------------------
//* This class is used to create zip files and unzip files
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ZipHelper.h"
#include "7zTypes.h"
#include "LoadDll.h"
#include "io.h"
#include "IArchive.h"
#include "PropVariant.h"
#include "OutStreamWrapper.h"
#include "InStreamWrapper.h"
#include "ArchiveUpdateCallback.h"
#include "ArchiveExtractCallback.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR* const cCreateObjectFunction = _T("CreateObject");

// CLSIDs for 7z
static const GUID CLSID_CFormatZip =			{0x23170F69, 0x40C1, 0x278A, {0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}};
static const GUID CLSID_CFormat7z =				{0x23170F69, 0x40C1, 0x278A, {0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00}};
static const GUID IID_ISequentialInStream =		{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00}};
static const GUID IID_ISequentialOutStream =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00}};
static const GUID IID_IInStream =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00}};
static const GUID IID_IOutStream =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00}};
static const GUID IID_IStreamGetSize =			{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00}};
static const GUID IID_ICompressProgressInfo =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00}};
static const GUID IID_ICryptoGetTextPassword =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00}};
static const GUID IID_ICryptoGetTextPassword2 = {0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00}};
static const GUID IID_ISetProperties =			{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00}};
static const GUID IID_IArchiveExtractCallback = {0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x20, 0x00, 0x00}};
static const GUID IID_IArchiveUpdateCallback =	{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x00, 0x00}};
static const GUID IID_IOutArchive =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0xA0, 0x00, 0x00}};
static const GUID IID_IInArchive =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x00}};
//Unzip formats supported in the order of their priority
static const GUID UnzipFormats[] = {CLSID_CFormat7z, CLSID_CFormatZip};
#pragma endregion Declarations

namespace SvUl
{
bool makeZipFile( const std::string& rZipFileName, const SvDef::StringVector& rZipFiles, const std::string& rFolderPrefix, bool DeleteSourceFiles )
{
	bool Result{false};

	if( 0 != rZipFiles.size() )
	{
		HINSTANCE hZipInstance{nullptr};
		LoadDll::Instance().getDll(ZipDll, hZipInstance);
		if(nullptr != hZipInstance)
		{
			CComPtr<IOutArchive> pArchive;
			CreateObjectFunc pCreateObject = reinterpret_cast<CreateObjectFunc> (::GetProcAddress(hZipInstance, cCreateObjectFunction));
		
			if(nullptr != pCreateObject)
			{
				if(S_OK == pCreateObject(&CLSID_CFormat7z, &IID_IOutArchive, reinterpret_cast<void **> (&pArchive)))
				{
					CComPtr< ISetProperties > pSetProperties;
					pArchive->QueryInterface(IID_ISetProperties, reinterpret_cast<void**>(&pSetProperties));
		
					if(nullptr != pSetProperties)
					{
						const UInt32 PropertyCount = 1;
						const wchar_t* PropertyNames[PropertyCount] = {L"x"};
						CPropVariant PropertyValues[PropertyCount] = {static_cast<UInt32> (CompressionFormat::SevenZip)};
		
						if(S_OK == pSetProperties->SetProperties(PropertyNames, PropertyValues, PropertyCount))
						{
							CComPtr<IStream> pFileStream{nullptr};
							_bstr_t wFileName{rZipFileName.c_str()};
							if (S_OK == SHCreateStreamOnFileEx(wFileName, STGM_CREATE | STGM_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &pFileStream))
							{
								CComPtr<OutStreamWrapper> pOutputFile = new OutStreamWrapper(pFileStream);
								CComPtr<ArchiveUpdateCallback> pUpdateCallback = new ArchiveUpdateCallback(rZipFiles, rFolderPrefix);
								if(S_OK == pArchive->UpdateItems(pOutputFile, static_cast<UInt32> (rZipFiles.size()), pUpdateCallback))
								{
									Result = true;
								}
							}
						}
					}
					pArchive.Release();
				}
			}
		}
		//Zip file must be successful before deleting
		if (Result && DeleteSourceFiles)
		{
			for(const auto& rEntry : rZipFiles)
			{
				::DeleteFile(rEntry.c_str());
			}
		}
	}
	return Result;
}

bool unzipAll( const std::string& rZipFileName, const std::string& rDestinationFolder, SvDef::StringVector& rUnzippedFiles )
{
	bool Result{false};

	if (!rZipFileName.empty() && !rDestinationFolder.empty())
	{
		HINSTANCE hZipInstance {nullptr};
		LoadDll::Instance().getDll(ZipDll, hZipInstance);
		if (nullptr != hZipInstance)
		{
			CComPtr<IInArchive> pArchive;
			CreateObjectFunc pCreateObject = reinterpret_cast<CreateObjectFunc> (::GetProcAddress(hZipInstance, cCreateObjectFunction));

			if (nullptr != pCreateObject)
			{
				//Tries to unzip the supported formats until one works
				for (const auto& rUnzipFormat : UnzipFormats)
				{
					if (S_OK == pCreateObject(&rUnzipFormat, &IID_IInArchive, reinterpret_cast<void **> (&pArchive)))
					{
						CComPtr<IStream> pFileStream;
						_bstr_t wFileName {rZipFileName.c_str()};
						if (S_OK == SHCreateStreamOnFileEx(wFileName, STGM_READ, FILE_ATTRIBUTE_NORMAL, false, nullptr, &pFileStream))
						{
							CComPtr<InStreamWrapper> pInputFile = new InStreamWrapper(pFileStream);
							if (S_OK == pArchive->Open(pInputFile, 0, nullptr))
							{
								CComPtr<ArchiveExtractCallback> pExtractCallback = new ArchiveExtractCallback(pArchive, rDestinationFolder, rUnzippedFiles);
								if (S_OK == pArchive->Extract(nullptr, -1, false, pExtractCallback))
								{
									Result = true;
									break;
								}
							}
						}
					}
					pArchive->Close();
					pArchive.Release();
				}
			}
		}
	}
	
	return Result;
}
} // namespace SvUl

