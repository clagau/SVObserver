//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Mask Image.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <afxctl.h>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IMask.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVMFCControls\SVBitmap.h"
#pragma endregion Includes

namespace SvCmd
{
	struct GetMaskImage: public boost::noncopyable
	{
		GetMaskImage(const GUID& rObjectID) : m_InstanceID(rObjectID) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pMask = dynamic_cast<SvOi::IMask *>(SvOi::getObject(m_InstanceID));
			if (pMask)
			{
				SvOi::MatroxImageSmartHandlePtr data = pMask->GetMaskImage();
				SvOi::IMatroxImageData *imageData = data.get();
				if( nullptr != imageData && !imageData->empty())
				{
					SVBitmapInfo dibInfo = imageData->getBitmapInfo();
					BYTE* pMilBuffer = static_cast< BYTE* >( imageData->getBufferAddress() );

					if (nullptr != pMilBuffer && !dibInfo.empty())
					{
						SVBitmap bitmap;
						HRESULT hr = bitmap.LoadDIBitmap(dibInfo.GetBitmapInfo(), pMilBuffer);
				
						if (S_OK == hr)
						{
							//convert the hbitmap to an IPictureDisp.
							CPictureHolder pic;
							BOOL bRet = pic.CreateFromBitmap(static_cast<HBITMAP>(bitmap.Detach()));
							if (bRet)
							{
								m_picture = pic.GetPictureDispatch();
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
		GUID m_InstanceID;
	};
} //namespace SvCmd
