//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the ringbuffer indexes.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IRingBufferTool.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct SetRingbufferIndexValue: public boost::noncopyable
		{
			SetRingbufferIndexValue(const GUID& ringToolID, int index, const SVString & value) : m_InstanceID(ringToolID), m_index(index), m_valueString(value) {}
	
			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::IRingBufferTool* pTool = dynamic_cast<SvOi::IRingBufferTool *>(SvOi::getObject(m_InstanceID));
				if (pTool)
				{
					hr = pTool->setImageIndex(m_index, m_valueString);
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			
		private:
			GUID m_InstanceID;
			int m_index;
			SVString m_valueString;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
