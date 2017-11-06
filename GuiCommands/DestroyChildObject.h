//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for Destroy child object.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

namespace SvCmd
{
	struct DestroyChildObject : public boost::noncopyable
	{
		enum FlagEnum
		{
			Flag_None = 0,
			Flag_SetDefaultInputs = 1,
			Flag_ResetInspection = 2,
			Flag_SetDefaultInputs_And_ResetInspection = 3
		};

		//************************************
		/// \param rTaskObjectListID [in] GUID of the task-object-list-object.
		/// \param rObjectID [in] GUID of the object which should be destroyed.
		/// \param flags [in] Define if after destroying object the default inputs are set and/or inspection are reset.
		//************************************
		DestroyChildObject(const GUID& rTaskObjectListID, const GUID& rObjectID, FlagEnum flag = Flag_None) 
			: m_InstanceID(rTaskObjectListID), m_ObjectID(rObjectID), m_flag(flag) {}

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::ITaskObjectListClass *pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(m_InstanceID));
			SvOi::ITaskObject *pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(m_ObjectID));
			if( nullptr != pTaskObjectList && nullptr != pObject )
			{
				DWORD flag = 0;
				switch(m_flag)
				{
					case Flag_SetDefaultInputs:
						flag = SvDef::SVMFSetDefaultInputs;
						break;
					case Flag_ResetInspection:
						flag = SvDef::SVMFResetInspection;
						break;
					case Flag_SetDefaultInputs_And_ResetInspection:
						flag = SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection;
						break;
					default:
						flag = 0;
						break;
				}
				pTaskObjectList->DestroyChild(*pObject, flag);
			}
			else
			{
				hr = SvStl::Err_10024_DestroyChildObject_InvalidParameter;
			}
			return hr;
		}
		bool empty() const { return false; }

	private:
		GUID m_InstanceID;
		GUID m_ObjectID;
		FlagEnum m_flag;
	};
} //namespace SvCmd
