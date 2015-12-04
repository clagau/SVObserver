//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Connected Objects based on the type.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <Guiddef.h>
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		struct GetConnectedObjects: public boost::noncopyable
		{
			GetConnectedObjects(const GUID& rObjectID, const SVObjectTypeInfoStruct& typeInfo, int maxRequested) : m_InstanceID(rObjectID), m_typeInfo(typeInfo), m_maxRequested(maxRequested) {}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = S_OK;

				SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
				if (pObject)
				{
					if (SVImageObjectType == m_typeInfo.ObjectType)
					{
						SVInObjectInfoStruct* psvImageInfo(nullptr);
						SVInObjectInfoStruct* psvLastImageInfo(nullptr);

						SvOi::ITaskObject* pTaskObject  = dynamic_cast<SvOi::ITaskObject *>(pObject);
						while (!psvImageInfo && S_OK == pTaskObject->FindNextInputImageInfo(psvImageInfo, psvLastImageInfo))
						{
							if (psvImageInfo)
							{
								if (psvImageInfo->IsConnected())
								{
									SvOi::IObjectClass* pObject = psvImageInfo->GetInputObjectInfo().PObject;
									if (pObject)
									{
										SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage *>(pObject);
										if (pImage)
										{
											SVString name = pImage->getDisplayedName();
											m_list.insert(std::make_pair(psvImageInfo->GetInputName(), std::make_pair(name, pObject->GetUniqueObjectID())));
										}
									}
									if (m_list.size() < m_maxRequested)
									{
										psvLastImageInfo = psvImageInfo;
										psvImageInfo = nullptr;
									}
								}
								else // get First Connected Image Info
								{
									psvLastImageInfo = psvImageInfo;
									psvImageInfo = nullptr;
								}
							}
						}
					}
					else
					{
						hr = E_INVALIDARG;
					}
				}
				else
				{
					hr = E_POINTER;
				}
				return hr;
			}
			bool empty() const { return false; }
			const SvUl::InputNameGuidPairList& ConnectedObjects() const { return m_list; }

		private:
			SVObjectTypeInfoStruct m_typeInfo;
			SvUl::InputNameGuidPairList m_list;
			GUID m_InstanceID;
			int m_maxRequested;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
