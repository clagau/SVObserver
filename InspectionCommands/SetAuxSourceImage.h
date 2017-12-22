//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Auxiliary Source image.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

namespace SvCmd
{
	class SetAuxSourceImage
	{
	public:
		SetAuxSourceImage(const SetAuxSourceImage&) = delete;
		SetAuxSourceImage& operator=(const SetAuxSourceImage&) = delete;
		
		SetAuxSourceImage(const SVGUID& rObjectID, const SVGUID& rImageID) : m_InstanceID(rObjectID), m_ImageID(rImageID) {};

		virtual ~SetAuxSourceImage() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(m_InstanceID));
			if (pTool)
			{
				hr = pTool->setAuxSourceImage(m_ImageID);
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }

	private:
		SVGUID m_InstanceID;
		SVGUID m_ImageID;
	};
} //namespace SvCmd
