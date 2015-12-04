//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Auxiliary Source image.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetAuxSourceImage: public boost::noncopyable
		{
			GetAuxSourceImage(const GUID& rObjectID) : m_InstanceID(rObjectID) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;
				
				SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(m_InstanceID));
				if (pTool)
				{
					m_SourceImage = pTool->getAuxSourceImage();
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			SvUl::NameGuidPair SourceImage() const { return m_SourceImage; }

		private:
			SvUl::NameGuidPair m_SourceImage;
			GUID m_InstanceID;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
