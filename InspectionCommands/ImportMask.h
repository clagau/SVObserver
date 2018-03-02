//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Importing a mask.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IMask.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ImportMask
	{
	public:
		ImportMask(const ImportMask&) = delete;
		ImportMask& operator=(const ImportMask&) = delete;

		ImportMask(const SVGUID& rObjectID, const std::string& rFilename) : m_InstanceID(rObjectID), m_Filename(rFilename) {};

		virtual ~ImportMask() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				hr = pObject->Import(m_Filename);
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
		std::string m_Filename;
	};
} //namespace SvCmd