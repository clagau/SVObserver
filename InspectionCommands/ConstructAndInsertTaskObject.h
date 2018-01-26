//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Command for construct an taskObject and insert it to a taskObjectList.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\IObjectAppClass.h"
#include "Definitions/ObjectDefines.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class ConstructAndInsertTaskObject
	{
	public:
		ConstructAndInsertTaskObject(const ConstructAndInsertTaskObject&) = delete;
		ConstructAndInsertTaskObject& operator=(const ConstructAndInsertTaskObject&) = delete;

		ConstructAndInsertTaskObject(const SVGUID& rTaskObjectID, const SVGUID& rClassID, int pos)
			: m_InstanceID(rTaskObjectID), m_ClassID(rClassID), m_pos(pos) {};

		virtual ~ConstructAndInsertTaskObject() {};

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
					if( !pObjectApp->CreateChildObject(*pObject, SvDef::SVMFResetObject ) )
					{
						hr = SvStl::Err_10021_InsertTaskObject_CreateObjectFailed;

						// Remove it from the Tool TaskObjectList ( Destruct it )
						const SVGUID& rObjectID = pObject->GetUniqueObjectID();
						if( GUID_NULL != rObjectID )
						{
							pTaskObjectList->Delete( rObjectID.ToGUID() );
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
				hr = SvStl::Err_10020_InsertTaskObject_InvalidParameter;
			}
			return hr;
		}
		bool empty() const { return false; }

	private:
		SVGUID m_InstanceID;
		SVGUID m_ClassID;
		int m_pos;
	};
} //namespace SvCmd
