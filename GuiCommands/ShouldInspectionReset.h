//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for return if the inspection should be reset.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes


namespace Seidenader
{
	namespace GuiCommand
	{
		struct ShouldInspectionReset : public boost::noncopyable
		{
			ShouldInspectionReset(const GUID& rObjectID) 
				: m_InstanceID(rObjectID), m_shouldResetInspection(false) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ISVFilter *pFilter = dynamic_cast<SvOi::ISVFilter*>(SvOi::getObject(m_InstanceID));
				if( nullptr != pFilter )
				{
					m_shouldResetInspection = pFilter->shouldResetInspection( );
				}
				else
				{
					hr = SvOi::Err_10025_ShouldResetInspection_InvalidParameter;
				}
				return hr;
			}
			bool empty() const { return false; }
			bool shouldResetInspection() const {return m_shouldResetInspection; }

		private:
			GUID m_InstanceID;
			bool m_shouldResetInspection;
		};
	}
}