//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for construct an taskObject and insert it to a taskObjectList.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ObjectDefines.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct ConstructAndInsertTaskObject : public boost::noncopyable
		{
			ConstructAndInsertTaskObject(const GUID& rTaskObjectID, const GUID& rClassID, int pos) 
				: m_InstanceID(rTaskObjectID), m_ClassID(rClassID), m_pos(pos) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(m_InstanceID));
				SvOi::IObjectAppClass* pObjectApp = dynamic_cast<SvOi::IObjectAppClass*>(pTaskObjectList);

				SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::ConstructObject( m_ClassID ));
				SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*>(pTaskObject);
				
				if( nullptr != pTaskObjectList && nullptr != pTaskObject && nullptr != pObjectApp && nullptr != pObject)
				{
					pTaskObjectList->InsertAt( m_pos, *pTaskObject );

					// And last - Create (initialize) it
					if( ! pObject->is_Created() )
					{
						// And finally try to create the child object...
						if( SVMR_SUCCESS != pObjectApp->CreateChildObject(*pObject, SVMFResetObject ) )
						{
							hr = SvOi::Err_10021_InsertTaskObject_CreateObjectFailed;

							// Remove it from the Tool TaskObjectList ( Destruct it )
							GUID objectID = pObject->GetUniqueObjectID();
							if( SV_GUID_NULL != objectID )
							{
								pTaskObjectList->Delete( objectID );
							}
							else
							{
								delete pTaskObject;
							}
						}
					}
				}
				else
				{
					hr = SvOi::Err_10020_InsertTaskObject_InvalidParameter;
				}
				return hr;
			}
			bool empty() const { return false; }

		private:
			GUID m_InstanceID;
			GUID m_ClassID;
			int m_pos;
		};
	}
}