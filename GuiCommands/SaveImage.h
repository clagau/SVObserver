//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Saving an Image.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ISVImage.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace SvCmd
{
	struct SaveImage : public boost::noncopyable
	{
		SaveImage(const GUID& rObjectID, const SVString& rFilename) 
			: m_InstanceID(rObjectID), m_Filename(rFilename) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ISVImage * pObject = dynamic_cast<SvOi::ISVImage *>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				hr = pObject->Save(m_Filename);
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
		SVString m_Filename;
	};
} //namespace SvCmd
