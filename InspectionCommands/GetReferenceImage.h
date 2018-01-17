//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Reference Image for the Mask.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <afxctl.h>
#include "ObjectInterfaces/IMask.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/BitmapHelper.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetReferenceImage
	{
	public:
		GetReferenceImage(const GetReferenceImage&) = delete;
		GetReferenceImage& operator=(const GetReferenceImage&) = delete;

		GetReferenceImage(const SVGUID& rObjectID) : m_InstanceID(rObjectID) {};

		virtual ~GetReferenceImage() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pMask = dynamic_cast<SvOi::IMask *>(SvOi::getObject(m_InstanceID));
			if (pMask)
			{
				SvOi::SVImageBufferHandlePtr pImageData = pMask->GetReferenceImage();

				if (nullptr != pImageData && !pImageData->empty())
				{
					SVBitmapInfo dibInfo;
					BYTE* pMilBuffer(nullptr);
					//copy the image buffer, as the source doesn't have a DIB
					SVMatroxBuffer newBuffer;
					const SVMatroxBuffer& oldBuffer = pImageData->GetBuffer();

					HRESULT l_Code = SVMatroxBufferInterface::Create(newBuffer, oldBuffer);
					if (S_OK == l_Code)
					{
						l_Code = SVMatroxBufferInterface::CopyBuffer(newBuffer, oldBuffer);
						if (S_OK == l_Code)
						{
							l_Code = SVMatroxBufferInterface::GetBitmapInfo(dibInfo, newBuffer);
						}
						if (S_OK == l_Code)
						{
							l_Code = SVMatroxBufferInterface::GetHostAddress(&pMilBuffer, newBuffer);
						}
					}

					if (nullptr != pMilBuffer && !dibInfo.empty())
					{
						HBITMAP hBitmap = SvUl::CreateDIBitmap(*dibInfo.GetBitmapInfo(), pMilBuffer);
						if (nullptr != hBitmap)
						{
							//convert the hbitmap to an IPictureDisp.
							CPictureHolder Picture;
							if ( Picture.CreateFromBitmap(hBitmap))
							{
								m_picture = Picture.GetPictureDispatch();
							}
							else
							{
								hr = E_HANDLE;
							}
						}
					}
					newBuffer.clear();
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
