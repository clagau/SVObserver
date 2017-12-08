//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Available Auxiliary Source Images.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	struct GetAvailableAuxSourceImages
	{
	public:
		GetAvailableAuxSourceImages(const GetAvailableAuxSourceImages&) = delete;
		GetAvailableAuxSourceImages& operator=(const GetAvailableAuxSourceImages&) = delete;

		GetAvailableAuxSourceImages(const SVGUID& rObjectID) : m_InstanceID(rObjectID) {};

		virtual ~GetAvailableAuxSourceImages() {};


		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			
			SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(m_InstanceID));
			if (pTool)
			{
				m_list = pTool->getAvailableAuxSourceImages();
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvUl::NameGuidList& AvailableImages() const { return m_list; }

	private:
		SvUl::NameGuidList m_list;
		SVGUID m_InstanceID;
	};
} //namespace SvCmd
