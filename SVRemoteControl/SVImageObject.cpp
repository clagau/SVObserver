//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageObject
//* .File Name       : $Workfile:   SVImageObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   02 Sep 2014 10:37:00  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <limits>
#include "SVImageObject.h"
#include "FreeImage\FreeImage.h"
#include "SVImageConvertorGDI.h"
#include "EventLog.h"
#include "WebsocketLibrary/RunRequest.inl"
#pragma endregion Includes

#pragma comment(lib,"gdiplus")

#define INET_E_NO_SESSION		_HRESULT_TYPEDEF_(0x800C0003L)
#define INET_E_NO_IMAGE_SOCKET 	_HRESULT_TYPEDEF_(0x83760006L)

STDMETHODIMP SVImageObject::get_Image(IPictureDisp** pVal)
{
	HRESULT hr = S_OK;
	if (m_length == 0)
	{
		hr = FetchImage();
	}
	if (SUCCEEDED(hr))
	{
		HGLOBAL hg = ::GlobalAlloc(GHND, m_length);
		if (hg)
		{
			memcpy(::GlobalLock(hg), m_DIB.get(), m_length);
			::GlobalUnlock(hg);
			CComPtr<IStream> stream;
			hr = ::CreateStreamOnHGlobal(hg, TRUE, &stream);
			// Create IPictureDisp from IStream
			if (SUCCEEDED(hr))
			{
				hr = ::OleLoadPicture(stream, m_length, FALSE, __uuidof(IPictureDisp), (LPVOID *)pVal);
			}
		}
		else
		{
			hr = E_OUTOFMEMORY;
			SVLOG(hr);
		}
	}

	return hr;
}

STDMETHODIMP SVImageObject::get_Name(BSTR* pVal)
{
	ATL::CComBSTR tmp;
	tmp.Attach(*pVal);
	tmp = m_name;
	*pVal = tmp.Detach();
	return S_OK;
}

STDMETHODIMP SVImageObject::put_Name(BSTR newVal)
{
	m_name = newVal;
	return S_OK;
}

STDMETHODIMP SVImageObject::get_Status(LONG* pVal)
{
	*pVal = m_status;
	return S_OK;
}

STDMETHODIMP SVImageObject::put_Status(LONG newVal)
{
	m_status = newVal;
	return S_OK;
}

STDMETHODIMP SVImageObject::get_TriggerCount(LONG* pVal)
{
	*pVal = m_trigger;
	return S_OK;
}

STDMETHODIMP SVImageObject::put_TriggerCount(LONG newVal)
{
	m_trigger = newVal;
	return S_OK;
}

STDMETHODIMP SVImageObject::put_Image(IPictureDisp*)
{
	return E_NOTIMPL;
}

STDMETHODIMP SVImageObject::get_ImageFormat(SVImageFormatsEnum* pVal)
{
	*pVal = m_format;
	return S_OK;
}

STDMETHODIMP SVImageObject::put_ImageFormat(SVImageFormatsEnum newVal)
{
	m_format = newVal;
	if (m_format & fetch)
	{
		//@TODO:  BRW - Had to disable this to avoid "Problem setting image format."
		//return FetchImage();
	}
	return S_OK;
}

STDMETHODIMP SVImageObject::get_PNG(VARIANT* pVal)
{
	BOOL ok = FALSE;
	BYTE * png = 0;
	HRESULT hr = S_OK;
	if (m_length == 0)
	{
		hr = FetchImage();
	}
	if (SUCCEEDED(hr))
	{
		// open a memory stream for the source
		FIMEMORY* hSrcMem = FreeImage_OpenMemory(const_cast<unsigned char *>(m_DIB.get()), m_length);
		if (hSrcMem)
		{
			// get the file type
			FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hSrcMem, 0);

			// load the bitmap from the memory stream
			FIBITMAP* dib = FreeImage_LoadFromMemory(fif, hSrcMem, 0);
			if (dib)
			{
				unsigned int bpp = FreeImage_GetBPP(dib);

				// open a memory stream
				FIMEMORY* hDstMem = FreeImage_OpenMemory();
				if (hDstMem)
				{
					// Bit depth of 16 not supported
					// Bit depth of 32, BMP has alpha channel of 0, so it's completely transparent
					if (16 == bpp || 32 == bpp)
					{
						FIBITMAP* convertedDIB = FreeImage_ConvertTo24Bits(dib);
						// encode and save the image to the memory
						ok = FreeImage_SaveToMemory(FIF_PNG, convertedDIB, hDstMem, PNG_Z_BEST_SPEED);

						FreeImage_Unload(convertedDIB);
					}
					else
					{
						// encode and save the image to the memory
						ok = FreeImage_SaveToMemory(FIF_PNG, dib, hDstMem, PNG_Z_BEST_SPEED);
					}

					if (ok)
					{
						FreeImage_SeekMemory(hDstMem, 0, SEEK_SET);

						// write the compressed image
						unsigned char* pDst = nullptr;
						unsigned long dstLen = 0;
						ok = FreeImage_AcquireMemory(hDstMem, &pDst, &dstLen);
						if (ok && pDst)
						{
							SAFEARRAY* pArr = ::SafeArrayCreateVector(::VT_UI1, 0, dstLen);
							if (pArr)
							{
								::SafeArrayAccessData(pArr, reinterpret_cast<void**>(&png));
								memcpy(png, pDst, dstLen);
								::SafeArrayUnaccessData(pArr);
								CComVariant var(pArr);
								var.Detach(pVal);
								::SafeArrayDestroy(pArr);
							}
							else
							{
								ok = false;
							}
						}
						else if (!pDst)
						{
							ok = false;
						}
					}
					FreeImage_CloseMemory(hDstMem);
				}
				FreeImage_Unload(dib);
			}
			FreeImage_CloseMemory(hSrcMem);
		}
	}

	if (SUCCEEDED(hr) && !ok)
	{
		hr = CO_E_CONVERSIONFAILED;
	}
	SVLOG(hr);

	return hr;
}

HRESULT SVImageObject::SaveBitmap(gdi::Bitmap & bmp)
{
	CComPtr<IStream> stream;
	HRESULT l_Status = ::CreateStreamOnHGlobal(NULL, TRUE, &stream);
	SVLOG(l_Status);
	if (SUCCEEDED(l_Status))
	{
		CLSID cid;
		// get class id for image/bmp
		bmp.Save(stream, &cid);
		HGLOBAL hg;
		SVLOG((l_Status = ::GetHGlobalFromStream(stream, &hg)));
		m_length = static_cast<ULONG>(GlobalSize(hg));
		bytes tmp(reinterpret_cast<BYTE *>(new char[m_length]));
		void * glo = ::GlobalLock(hg);
		memcpy(tmp.get(), glo, m_length);
		::GlobalUnlock(hg);
		m_DIB.swap(tmp);
	}
	return l_Status;
}

STDMETHODIMP SVImageObject::GetImage(VARIANT_BOOL overlays, DOUBLE zoom, SVImageFormatsEnum format, VARIANT* result)
{
	if (!overlays && (abs(zoom - 1.0) < std::numeric_limits<double>::epsilon()))
	{
		switch (format & Mask)
		{
			case IDisp:
			{
				IPictureDisp* idisp(nullptr);
				HRESULT hr = get_Image(&idisp);
				SVLOG(hr);
				if (SUCCEEDED(hr))
				{
					CComVariant var(idisp);
					var.Detach(result);
				}
				return hr;
			}
			case BMP:
				return get_DIB(result);
			case PNG:
				return get_PNG(result);
		}
	}
	return E_NOTIMPL;
}

STDMETHODIMP SVImageObject::get_DIB(VARIANT* pVal)
{
	HRESULT hr = S_OK;
	if (m_length == 0)
	{
		hr = FetchImage();
	}
	if (SUCCEEDED(hr))
	{
		BYTE * buff = 0;
		SAFEARRAY * arr = ::SafeArrayCreateVector(::VT_UI1, 0, m_length);
		::SafeArrayAccessData(arr, reinterpret_cast<void**>(&buff));
		BYTE * dib = m_DIB.get();
		if (!dib)
		{
			hr = INET_E_DOWNLOAD_FAILURE;
			SVLOG(hr);
		}
		else
		{
			memcpy(buff, dib, m_length);
		}
		::SafeArrayUnaccessData(arr);
		if (SUCCEEDED(hr))
		{
			CComVariant var(arr);
			var.Detach(pVal);
		}
		::SafeArrayDestroy(arr);
	}
	return hr;
}

STDMETHODIMP SVImageObject::put_DIB(VARIANT)
{
	return E_NOTIMPL;
}

STDMETHODIMP SVImageObject::SetImage(VARIANT image)
{
	HRESULT l_Status = S_OK;

	if ((image.vt == VT_DISPATCH) && (image.pdispVal != NULL))
	{
		CComQIPtr<IPictureDisp> l_PictureDispPtr;

		l_Status = image.pdispVal->QueryInterface(&l_PictureDispPtr);

		SVLOG(l_Status);

		if (l_PictureDispPtr.p != NULL)
		{
			CComQIPtr<IPicture> l_PicturePtr;
			l_Status = l_PictureDispPtr->QueryInterface(&l_PicturePtr);
			SVLOG(l_Status);

			if (l_Status == S_OK && l_PicturePtr.p != NULL)
			{
				CComPtr<IStream> stream;
				l_Status = ::CreateStreamOnHGlobal(NULL, TRUE, &stream);
				SVLOG(l_Status);

				if (SUCCEEDED(l_Status))
				{
					l_PicturePtr->SaveAsFile(stream, false, NULL);

					HGLOBAL hg;
					l_Status = ::GetHGlobalFromStream(stream, &hg);
					SVLOG(l_Status);
					m_length = static_cast<ULONG>(GlobalSize(hg));
					bytes tmp(reinterpret_cast<BYTE *>(new char[m_length]));
					void * glo = ::GlobalLock(hg);
					memcpy(tmp.get(), glo, m_length);
					::GlobalUnlock(hg);
					m_DIB.swap(tmp);
				}
			}
		}
	}
	else if ((image.vt == (VT_ARRAY | VT_UI1)) || (image.vt == (VT_ARRAY | VT_I1)))
	{
		SAFEARRAY * arr = image.parray;
		::SafeArrayGetUBound(arr, 1, reinterpret_cast<LONG *>(&m_length));
		BYTE * src = 0;
		::SafeArrayAccessData(arr, reinterpret_cast<void**>(&src));
		HGLOBAL hg = ::GlobalHandle(src);
		if (hg != NULL)
		{
			m_length = static_cast<LONG>(GlobalSize(hg));
			bytes tmp(reinterpret_cast<BYTE *>(new char[m_length]));
			void * glo = ::GlobalLock(hg);
			memcpy(tmp.get(), glo, m_length);
			::GlobalUnlock(hg);
			m_DIB.swap(tmp);
		}
		else
		{
			l_Status = ::GetLastError();
			SVLOG(l_Status);
		}
		::SafeArrayUnaccessData(arr);
	}
	else
	{
		l_Status = E_INVALIDARG;
		SVLOG(l_Status);
	}

	return l_Status;
}




HRESULT SVImageObject::FetchImage()
{
	HRESULT hr = INET_E_DOWNLOAD_FAILURE;;
	SetLen(0);
	try
	{
		if (nullptr != m_pClientService && m_pClientService->get())
		{
			SvPb::GetImageFromIdRequest request;
			request.mutable_id()->set_inspectionid(m_CurrentImageId.inspectionid());
			request.mutable_id()->set_imageindex(m_CurrentImageId.imageindex());
			request.mutable_id()->set_trid(m_CurrentImageId.trid());

			auto resp = SvWsl::runRequest(*(m_pClientService->get()), &SvWsl::SVRCClientService::GetImageFromId, std::move(request)).get();


			if (resp.imagedata().rgbdata().length() > 0)
			{
				BYTE *buff = new BYTE[resp.imagedata().rgbdata().length()];
				memcpy(buff, resp.imagedata().rgbdata().c_str(), resp.imagedata().rgbdata().length());
				boost::shared_array<BYTE> b(buff);
				SetLen((ULONG)resp.imagedata().rgbdata().length());
				SetDIB(b);
				hr = S_OK;
			}
		}
		else if (m_pClientService == nullptr)
		{
			hr = INET_E_NO_IMAGE_SOCKET;
		}
		else
		{
			hr = INET_E_NO_SESSION;
		}

	}
	catch (std::exception & /*ex*/)
	{
		m_DIB.reset();
		m_length = 0;
		hr = INET_E_DOWNLOAD_FAILURE;
		SVLOG(hr);
	}
	return hr;
}

void SVImageObject::SetOverlays(VARIANT)
{

}

void SVImageObject::SetClientService(SvWsl::SVRCClientServicePtr& rpSvrcClientService)
{
	m_pClientService = &rpSvrcClientService;
}
void SVImageObject::SetImageId(const SvPb::ImageId& CurImageId)
{
	m_CurrentImageId = CurImageId;
}
