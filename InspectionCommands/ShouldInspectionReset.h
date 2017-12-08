//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for return if the inspection should be reset.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	struct ShouldInspectionReset
	{
		ShouldInspectionReset(const SVGUID& rObjectID) 
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
				hr = SvStl::Err_10025_ShouldResetInspection_InvalidParameter;
			}
			return hr;
		}
		bool empty() const { return false; }
		bool shouldResetInspection() const {return m_shouldResetInspection; }

	private:
		SVGUID m_InstanceID;
		bool m_shouldResetInspection;
	};
} //namespace SvCmd
