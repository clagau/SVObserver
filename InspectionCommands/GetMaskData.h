//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Mask Data.
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

	class GetMaskDataFunctor
	{
	public:
		explicit GetMaskDataFunctor(uint32_t objectID) : m_InstanceID(objectID) {};

		HGLOBAL operator()()
		{
			SvOi::IMask* pObject = dynamic_cast<SvOi::IMask*>(SvOi::getObject(m_InstanceID));
			if (pObject)
			{
				return  pObject->GetMaskData();
			}
			else
			{
				throw DWORD(E_POINTER);
			}

		};

	private:
		uint32_t m_InstanceID;

	};




} //namespace SvCmd
