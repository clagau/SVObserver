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
constexpr GUID CLSID_CFormatZip =			{0x23170F69, 0x40C1, 0x278A, {0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00}};
constexpr GUID CLSID_CFormat7z =			{0x23170F69, 0x40C1, 0x278A, {0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00}};
constexpr GUID cISetProperties =			{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00}};
constexpr GUID cIOutArchive =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0xA0, 0x00, 0x00}};
constexpr GUID cIInArchive =				{0x23170F69, 0x40C1, 0x278A, {0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x00}};
//Unzip formats supported in the order of their priority
constexpr GUID UnzipFormats[] = {CLSID_CFormat7z, CLSID_CFormatZip};
#pragma endregion Declarations

namespace SvUl
{
bool makeZipFile( const std::string& rZipFileName, const SvDef::StringVector& rZipFiles, const std::string& rFolderPrefix, bool DeleteSourceFiles )
{
	bool Result{false};

	if(!rFolderPrefix.empty())
	{
		//Check that all files to be zipped have the folder prefix
		for(const auto rFile : rZipFiles)
		{
			if(0 != rFile.find(rFolderPrefix))
			{
				return false;
			}
		}
	}

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
				if(S_OK == pCreateObject(&CLSID_CFormat7z, &cIOutArchive, reinterpret_cast<void **> (&pArchive)))
				{
					CComPtr< ISetProperties > pSetProperties;
					pArchive->QueryInterface(cISetProperties, reinterpret_cast<void**>(&pSetProperties));
		
					if(nullptr != pSetProperties)
					{
						const UInt32 PropertyCount = 1;
						const wchar_t* PropertyNames[PropertyCount] = {L"x"};
						PropVariant PropertyValues[PropertyCount] = { PropVariant{static_cast<UInt32> (CompressionFormat::SevenZip)} };
		
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
					if (S_OK == pCreateObject(&rUnzipFormat, &cIInArchive, reinterpret_cast<void **> (&pArchive)))
					{
						CComPtr<IStream> pFileStream;
						_bstr_t wFileName {rZipFileName.c_str()};
						if (S_OK == SHCreateStreamOnFileEx(wFileName, STGM_READ, FILE_ATTRIBUTE_NORMAL, false, nullptr, &pFileStream))
						{
							CComPtr<InStreamWrapper> pInputFile = new InStreamWrapper(pFileStream);
							if (S_OK == pArchive->Open(pInputFile, 0, nullptr))
							{
								CComPtr<ArchiveExtractCallback> pExtractCallback = new ArchiveExtractCallback(pArchive, rDestinationFolder, rUnzippedFiles);
								if (S_OK == pArchive->Extract(nullptr, static_cast<UInt32> (-1), false, pExtractCallback))
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

