//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Allowed Image List.
//******************************************************************************
#pragma once

#pragma region Includes
#include "AllowedImageFunc.h"
#include "GetAvailableObjects.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetAllowedImageList : public boost::noncopyable
		{
			GetAllowedImageList(const GUID& rObjectID, const SVObjectTypeInfoStruct& typeInfo, const GUID& rTaskObjectID, SVObjectSubTypeEnum subType)
			: m_command(rObjectID, typeInfo, AllowedImageFunc(rTaskObjectID, subType)) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = m_command.Execute();
				return hr;
			}
			bool empty() const { return false; }
			const SvUl::NameGuidList& AvailableObjects() const { return m_command.AvailableObjects(); }

		private:
			GetAvailableObjects m_command;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
