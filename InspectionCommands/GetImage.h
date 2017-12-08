//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Image as an IPictureDisp.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <afxctl.h>
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetImage
	{
	public:
		GetImage(const GetImage&) = delete;
		GetImage& operator=(const GetImage&) = delete;

		/// Constructor to get an image with a SVGUID.
		/// \param rObjectID [in] The SVGUID of the requested Image.
		GetImage(const SVGUID& rObjectID) : m_InstanceID(rObjectID)
			, m_Name()
			, m_ParentID(SV_GUID_NULL)
			, m_Width(0)
			, m_Height(0)
		{};

		/// Constructor to get an image from a taskObject with a name.
		/// \param rName [in] Name of the image.
		/// \param rParentID [in] The SVGUID of the taskObject which is the parent of the image.
		GetImage(const std::string& rName, const SVGUID& rParentID) : m_InstanceID(SV_GUID_NULL)
			, m_Name(rName)
			, m_ParentID(rParentID)
			, m_Width(0)
			, m_Height(0)
		{};

		virtual ~GetImage() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			SvOi::MatroxImageSmartHandlePtr data;

			if (SV_GUID_NULL != m_InstanceID)
			{
				SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(m_InstanceID));
				if (pImage)
				{
					data = pImage->getImageData();
				}
			}
			else if ( !m_Name.empty() && SV_GUID_NULL != m_ParentID )
			{
				SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(m_ParentID));
				if (nullptr != pObject)
				{
					if ( !pObject->getSpecialImage(m_Name, data) )
					{
						return E_FAIL;
					}
				}
				else
				{
					return E_POINTER;
				}
			}
			else
			{
				return E_POINTER;
			}
			
			SvOi::IMatroxImageData* pImageData = data.get();
			if (nullptr != pImageData && !pImageData->empty())
			{
				m_Width = abs(pImageData->getBitmapInfo().GetWidth());
				m_Height = abs(pImageData->getBitmapInfo().GetHeight());
				HBITMAP hBitmap = pImageData->GetHBitmap();
				if (nullptr != hBitmap)
				{
					//convert the hbitmap to an IPictureDisp for the activeX-control.
					CPictureHolder pic;
					BOOL bRet = pic.CreateFromBitmap(hBitmap);
					if (bRet)
					{
						m_picture = pic.GetPictureDispatch();
					}
					else
					{
						hr = E_HANDLE;
					}
				}
				else
				{
					hr = E_HANDLE;
				}
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		IPictureDisp* Image() const { return m_picture; }
		int getWidth() const { return m_Width; }
		int getHeight() const { return m_Height; }

	private:
		CComPtr<IPictureDisp> m_picture;
		SVGUID m_InstanceID;
		SVGUID m_ParentID;
		std::string m_Name;
		int m_Width;
		int m_Height;
	};
} //namespace SvCmd
