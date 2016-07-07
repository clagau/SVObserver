//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the output rectangle of an image.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetOutputRectangle: public boost::noncopyable
		{
			GetOutputRectangle(const GUID& rObjectID) : m_InstanceID(rObjectID) {}

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
			GUID m_InstanceID;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
