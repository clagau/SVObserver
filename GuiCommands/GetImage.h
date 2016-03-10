//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Image as an IPictureDisp.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <atlbase.h>
#include <afxctl.h>
#include <Guiddef.h>
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetImage: public boost::noncopyable
		{
			GetImage(const GUID& rObjectID) : m_InstanceID(rObjectID) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(m_InstanceID));
				if (pImage)
				{
					SvOi::MatroxImageSmartHandlePtr data = pImage->getImageData();
					SvOi::IMatroxImageData* pImageData = data.get();
					if (nullptr != pImageData && !pImageData->empty())
					{
						HBITMAP hBitmap = pImageData->GetHBitmap();
						if (hBitmap)
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
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
