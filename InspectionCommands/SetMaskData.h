//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the Command for setting the Mask Data.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IMask.h"
#pragma endregion Includes

//This comment is to avoid that the IObjectClass include is marked as not required due to a function return value

#pragma region Declarations
#pragma endregion Declarations

namespace SvCmd
{

	class SetMaskDataFunctor
	{
	public:
		explicit SetMaskDataFunctor(uint32_t objectID, HGLOBAL hGlobal)
			: m_InstanceID(objectID), m_hGlobal(hGlobal) {};
		
		HRESULT operator() ()
		{
			HRESULT hr = S_OK;

			SvOi::IMask* pObject = dynamic_cast<SvOi::IMask*>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				pObject->SetMaskData(m_hGlobal);
			}
			else
			{
				hr = E_POINTER;
			}
			return hr;
		}
		

	private:
		uint32_t m_InstanceID;
		HGLOBAL m_hGlobal;
	};


} //namespace SvCmd
