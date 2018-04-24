//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// COM object for Images
//******************************************************************************
#pragma once
#pragma region Includes
#include "resource.h"       // main symbols
#include "SVRemoteControl.h"
#include <boost/shared_array.hpp>
#include "WebsocketLibrary/clientservice.h"
#include "SVProtobuf/SVRC.h"
#pragma endregion Includes


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

typedef boost::shared_array<BYTE> bytes;

namespace gdi = Gdiplus;

// SVImageObject

class ATL_NO_VTABLE SVImageObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<SVImageObject, &CLSID_SVImageObject>,
	public IDispatchImpl<ISVImageObject, &IID_ISVImageObject, &LIBID_SVRemoteControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	SVImageObject(){}

	DECLARE_REGISTRY_RESOURCEID(IDR_SVIMAGEOBJECT)


	BEGIN_COM_MAP(SVImageObject)
		COM_INTERFACE_ENTRY(ISVImageObject)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		status = 0;
		trigger = 0;
		len = 0;
		format = SVImageFormatsEnum::BMP;
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_Status)(LONG* pVal);
	STDMETHOD(put_Status)(LONG newVal);
	STDMETHOD(get_TriggerCount)(LONG* pVal);
	STDMETHOD(put_TriggerCount)(LONG newVal);
	STDMETHOD(get_ImageFormat)(SVImageFormatsEnum* pVal);
	STDMETHOD(put_ImageFormat)(SVImageFormatsEnum newVal);

	STDMETHOD(GetImage)(VARIANT_BOOL overlays, DOUBLE zoom, SVImageFormatsEnum format, VARIANT* result);
	STDMETHOD(SetImage)(VARIANT image);



	void SetOverlays(VARIANT bsOverlays);
	void SetDIB(bytes dib) { DIB.swap(dib); }
	void SetLen(ULONG l) { len = l; }

	const std::string & GetUrl() const { return url; }
	void SetUrl(const std::string & u) { url = u; }
	void SetClientService(SvWsl::ClientServicePointer&   rpClientService);
	void SetImageId(const SvPb::CurImageId& CurImageId);
private:
	STDMETHOD(get_Image)(IPictureDisp** pVal);
	STDMETHOD(put_Image)(IPictureDisp * newVal);
	STDMETHOD(get_DIB)(VARIANT* pVal);
	STDMETHOD(put_DIB)(VARIANT newVal);
	STDMETHOD(get_PNG)(VARIANT* pVal);

	HRESULT SaveBitmap(gdi::Bitmap & bmp);
	HRESULT FetchImage();

	//::CComPtr<IPictureDisp> m_image;
	CComBSTR name;
	std::string url;
	bytes DIB;
	LONG status = 0;
	LONG trigger = 0;
	ULONG len = 0;
	SVImageFormatsEnum format = BMP;
	SvPb::CurImageId m_CurImId;
	SvWsl::ClientServicePointer*  m_pClientService = nullptr;
};

OBJECT_ENTRY_AUTO(__uuidof(SVImageObject), SVImageObject)

