//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the output rectangle of an image.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetOutputRectangle
	{
	public:
		GetOutputRectangle(const GetOutputRectangle&) = delete;
		GetOutputRectangle& operator=(const GetOutputRectangle&) = delete;

		GetOutputRectangle(const SVGUID& rObjectID) : m_InstanceID(rObjectID) {};

		virtual ~GetOutputRectangle() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(m_InstanceID));
			if (pImage)
			{
				m_rect = pImage->GetOutputRectangle();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const RECT& getRectangle() const { return m_rect; }

	private:
		RECT m_rect;
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
