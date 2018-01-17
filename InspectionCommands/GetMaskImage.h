//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Mask Image.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <afxctl.h>
#include "ObjectInterfaces/IMask.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/BitmapHelper.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetMaskImage
	{
	public:
		GetMaskImage(const GetMaskImage&) = delete;
		GetMaskImage& operator=(const GetMaskImage&) = delete;

		GetMaskImage(const SVGUID& rObjectID) : m_InstanceID(rObjectID) {};

		virtual ~GetMaskImage() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pMask = dynamic_cast<SvOi::IMask *>(SvOi::getObject(m_InstanceID));
			if (pMask)
			{
				SvOi::SVImageBufferHandlePtr imageData = pMask->GetMaskImage();
				if( nullptr != imageData && !imageData->empty())
				{
					SVBitmapInfo dibInfo = imageData->GetBitmapInfo();
					BYTE* pMilBuffer = static_cast< BYTE* >( imageData->GetBufferAddress() );

					if (nullptr != pMilBuffer && !dibInfo.empty())
					{
						HBITMAP hBitmap = SvUl::CreateDIBitmap(*dibInfo.GetBitmapInfo(), pMilBuffer);
						if (nullptr != hBitmap)
						{
							//convert the hbitmap to an IPictureDisp.
							CPictureHolder Picture;
							if (Picture.CreateFromBitmap(hBitmap))
							{
								m_picture = Picture.GetPictureDispatch();
							}
							else
							{
								hr = E_HANDLE;
							}
						}
					}
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

	private:
		CComPtr<IPictureDisp> m_picture;
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
