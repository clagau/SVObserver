//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectList
//* .File Name       : $Workfile:   SVTaskObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   19 Dec 2014 04:22:28  $
//******************************************************************************

// @WARNING:  This filename (SVTaskObjectList) does not match the class name (SVTaskObjectListClass).

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObjectList.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVTaskObjectListClass, SVTaskObjectListClassGuid)

#pragma region Constructor
SVTaskObjectListClass::SVTaskObjectListClass(LPCSTR ObjectName)
				      :SVTaskObjectClass(ObjectName), m_LastListUpdateTimestamp( 0 ) 
{
	addDefaultInputObjects();
}

SVTaskObjectListClass::SVTaskObjectListClass(SVObjectClass* POwner, int StringResourceID)
					  :SVTaskObjectClass(POwner, StringResourceID), m_LastListUpdateTimestamp( 0 ) 
{
	addDefaultInputObjects();
}

SVTaskObjectListClass::~SVTaskObjectListClass()
{
	DeleteAll();
}
#pragma endregion

#pragma region public methods
HRESULT SVTaskObjectListClass::GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const
{
	HRESULT l_Status( SVTaskObjectClass::GetOutputList( p_rOutputInfoList ) );

	for( int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); ++ i )
	{
		SVTaskObjectClass* l_pObject( m_TaskObjectVector[i] );

		if( nullptr != l_pObject )
		{
			HRESULT l_Temp = l_pObject->GetOutputList( p_rOutputInfoList );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}
		}
	}

	return l_Status;
}

void SVTaskObjectListClass::AppendInputObjects()
{
	// First Friends...
	SVTaskObjectClass::GetInputObjects(m_InputObjectList);
	
	// Then get your default inputs and outputs ...
	addDefaultInputObjects();
	
	// Append input output objects of all list members...
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); ++ i)
	{
		if (m_TaskObjectVector[i])
		{
			m_TaskObjectVector[i]->GetInputObjects(m_InputObjectList);
		}
	}
}

void SVTaskObjectListClass::RemoveOutputObject( SVOutObjectInfoStruct* p_pOutObject )
{
	if ( SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SVTaskObjectListClass*> (GetParent()) )
	{
		pTaskObjectList->RemoveOutputObject( p_pOutObject );
	}
}

void SVTaskObjectListClass::GetAllInputObjects()
{
	// Try to get all inputs and outputs...
	
	// First clean up current lists...
	m_InputObjectList.clear();
	
	// Call Base Class to inform friends...
	SVTaskObjectClass::GetAllInputObjects();
	
	// Force all list members to rebuild their input output objects lists...
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); ++i)
	{
		if (m_TaskObjectVector[i])
		{
			m_TaskObjectVector[i]->GetAllInputObjects();
		}
	}
	
	// Finally append all objects to my lists...
	AppendInputObjects();
}

void SVTaskObjectListClass::Persist(SvOi::IObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name
	SVTaskObjectClass::Persist(rWriter);

	// Get script of task list members...
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); ++ i)
	{
		if (m_TaskObjectVector[i])
		{
			SVTaskObjectClass* pTaskObject = m_TaskObjectVector[i];
			if (SV_IS_KIND_OF(pTaskObject, SVTaskObjectListClass))
			{
				pTaskObject->Persist(rWriter);
			}
			else
			{
				rWriter.StartElement(pTaskObject->GetObjectName()); // use internal name for node name
				pTaskObject->Persist(rWriter);
				rWriter.EndElement();
			}
		}
	}
	rWriter.EndElement();
}

SVTaskObjectClass* SVTaskObjectListClass::GetObjectAtPoint( const SVPoint<double>& rPoint )
{
	SVTaskObjectClass* pObject = SVTaskObjectClass::GetObjectAtPoint(rPoint);

	// Get Object from our children
	for (int i = 0; nullptr == pObject && i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		pObject = m_TaskObjectVector[i];

		if ( nullptr != pObject )
		{
			pObject = pObject->GetObjectAtPoint( rPoint );
		}
	}

	return pObject;
}

bool SVTaskObjectListClass::CloseObject()
{
	bool Result( true );

	// Close our children
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		SVTaskObjectClass* pTaskObject = m_TaskObjectVector[i];
		if (pTaskObject)
		{
			Result = pTaskObject->CloseObject() && Result;
		}
	}
	// Close ourself and our friends
	Result = SVTaskObjectClass::CloseObject() && Result;

	return Result;
}

HRESULT SVTaskObjectListClass::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = SVTaskObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[ Index ] == GetName() )
		{
			for (int i = 0; nullptr == rpObject && i < static_cast<int> (m_TaskObjectVector.size()); i++)
			{
				SVTaskObjectClass* l_pTask = m_TaskObjectVector[i];
				if( nullptr != l_pTask )
				{
					l_Status = l_pTask->GetChildObject( rpObject, rNameInfo, Index + 1 );
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}

const SvTl::SVTimeStamp& SVTaskObjectListClass::GetLastListUpdateTimestamp() const
{
	return m_LastListUpdateTimestamp;
}

void SVTaskObjectListClass::InsertAt(int Index, SVTaskObjectClass* pTaskObject, int Count)
{
	if (nullptr == pTaskObject)
	{
		assert( false );
		return;
	}
	
	// Check for Unique names
	const std::string NewName( checkName( pTaskObject->GetName() ) );
	if( NewName != pTaskObject->GetName() )
	{
		pTaskObject->SetName( NewName.c_str() );
	}
	
	pTaskObject->SetObjectOwner(this);

	SVTaskObjectPtrVector::iterator Iter(m_TaskObjectVector.begin());

	if (0 < Index)
	{
		std::advance(Iter, Index);
	}

	m_TaskObjectVector.insert(Iter, Count, pTaskObject);

	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
}
int  SVTaskObjectListClass::InsertAfter(const SVGUID& rGuid, SVTaskObjectClass* pTaskObject)
{
	int Result(-1);
	if (nullptr == pTaskObject)
	{
		assert(false);
		return -1;
	}

	// Check for Unique names ??
	const std::string NewName(checkName(pTaskObject->GetName()));
	if (NewName != pTaskObject->GetName())
	{
		pTaskObject->SetName(NewName.c_str());
	}

	pTaskObject->SetObjectOwner(this);
	if (rGuid.empty())
	{
		m_TaskObjectVector.push_back(pTaskObject);
		Result =  static_cast<int>( m_TaskObjectVector.size()) -1;
	}
	else
	{
		auto it = std::find_if(m_TaskObjectVector.begin(), m_TaskObjectVector.end(),
			[&rGuid](SVTaskObjectClass* pObject)
		{
			if (pObject == nullptr || pObject->GetUniqueObjectID() == rGuid)
				return true;
			else
				return false;
		});

		auto it2 = m_TaskObjectVector.insert(it, pTaskObject);
		Result =  static_cast<int>(std::distance(m_TaskObjectVector.begin(), it2));
	}
	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
	return Result;
}

SVTaskObjectClass* SVTaskObjectListClass::GetAt( int nIndex ) const 
{
	return m_TaskObjectVector[nIndex];
}

void SVTaskObjectListClass::RemoveAt(int Index)
{
	m_TaskObjectVector.erase(m_TaskObjectVector.begin() + Index);

	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
}

int SVTaskObjectListClass::Add(SVTaskObjectClass* pTaskObject, bool atBegin)
{
	if ( nullptr == pTaskObject)
	{
		return -1;
	}

	// Check for Unique names 
	const std::string NewName( checkName( pTaskObject->GetName() ) );
	if( NewName != pTaskObject->GetName() )
	{
		pTaskObject->SetName( NewName.c_str() );
	}
	pTaskObject->SetObjectOwner(this);

	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();

	if (!atBegin)
	{
		m_TaskObjectVector.push_back(pTaskObject);
		return static_cast<int> (m_TaskObjectVector.size() - 1);
	}
	m_TaskObjectVector.insert(m_TaskObjectVector.begin(), 1, pTaskObject);
	return 0;
}

HRESULT SVTaskObjectListClass::RemoveChild( SVTaskObjectClass* pChildObject )
{
	HRESULT Result( S_OK );

	if ( !DestroyChildObject(pChildObject, SvDef::SVMFSetDefaultInputs) )
	{
		Result = S_FALSE;
	}
	return Result;
}

// Should be overridden and must be called in derived classes...
void SVTaskObjectListClass::SetObjectDepth(int NewObjectDepth)
{
	// Set object depth of members here...

	// Set object depth of all task list members...
	for (int j = 0; j < static_cast<int> (m_TaskObjectVector.size()); ++ j)
	{
		if (m_TaskObjectVector[j])
		{
			m_TaskObjectVector[j]->SetObjectDepth(NewObjectDepth);
		}
	}
	SVTaskObjectClass::SetObjectDepth(NewObjectDepth);
}

// Should be overridden and must be called in derived classes...
void SVTaskObjectListClass::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	// Set object depth of members here...

	// Set object depth of all task list members...
	for (int j = 0; j < static_cast<int> (m_TaskObjectVector.size()); ++ j)
	{
		if (m_TaskObjectVector[j])
		{
			m_TaskObjectVector[j]->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	SVTaskObjectClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
}

void SVTaskObjectListClass::SetInvalid()
{
	// Set this object and all own embedded objects to invalid
	SVTaskObjectClass::SetInvalid();

	// Set all children to invalid also
	for (int j = 0; j < static_cast<int> (m_TaskObjectVector.size()); ++ j)
	{
		SVTaskObjectClass* pObject = m_TaskObjectVector[j];
		if (pObject)
		{
			pObject->SetInvalid();
		}
	}
}

// Override this function to implement special object behavior...
void SVTaskObjectListClass::SetDisabled()
{
	// Set this object and all own embedded objects to disabled...
	// Set also all friends to disabled...
	SVTaskObjectClass::SetDisabled();

	// Set all children to disabled also
	for (int j = 0; j < static_cast<int> (m_TaskObjectVector.size()); ++ j)
	{
		SVTaskObjectClass* pObject = m_TaskObjectVector[j];
		if (pObject)
		{
			pObject->SetDisabled();
		}
	}
}
bool SVTaskObjectListClass::IsNameUnique(LPCSTR  pName, LPCTSTR pExclude) const
{
	bool bRetVal = std::none_of(m_TaskObjectVector.begin(), m_TaskObjectVector.end(),
		[pName, pExclude](SVTaskObjectClass* pTaskObj)->bool
	{
		if (nullptr == pTaskObj)
		{
			return false;
		}
		if (nullptr != pExclude)
		{
			if (0 == _stricmp(pExclude, pTaskObj->GetName()))
			{
				return  false;
			}
		}
		return (0 == _stricmp(pName, pTaskObj->GetName()));
	});

	return bRetVal;
}

const std::string SVTaskObjectListClass::checkName( LPCTSTR ToolName ) const
{
	std::string objectName;
	std::string newName( ToolName );

	int ToolIndex( 0 );
	for( int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++ )
	{
		SVObjectClass* pObject = m_TaskObjectVector[i];
		if( nullptr != pObject )
		{
			objectName = pObject->GetName();
			if ( std::string::npos != objectName.find( ToolName ))
			{
				// see if the name ends in a number
				int lastNum;
				bool digit = false;

				for (int i = static_cast<int> (objectName.size()) - 1; i >= 0; i--)
				{
					if (isdigit(objectName[i]))
					{
						digit = true;
					}
					else // found a non digit - stop looking for a digit
					{
						// if any digits were found - convert to a number
						if (digit)
						{	
							// convert to a number
							std::string numStr = SvUl::Right( objectName, (objectName.size() - 1) - i);
							lastNum = atoi(numStr.c_str());
						}
						break;
					}
				}

				if (digit)
				{
					ToolIndex = std::max(ToolIndex, lastNum + 1);
				}
				else
				{
					ToolIndex = std::max(ToolIndex, 1);
				}
			}
		}
	}
	// Set the name
	if( 0 != ToolIndex )
	{
		newName = SvUl::Format( _T("%s%d"), ToolName, ToolIndex );
	}

	return newName;
}

HRESULT SVTaskObjectListClass::CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
{
	HRESULT hrRet = S_OK;

	hrRet = SVTaskObjectClass::CollectOverlays(p_Image,p_MultiLineArray);

	for ( int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++ )
	{
		SVTaskObjectClass* pObject = dynamic_cast< SVTaskObjectClass* >( m_TaskObjectVector[i] );

		if ( nullptr != pObject )
		{
			HRESULT l_Temp = pObject->CollectOverlays(p_Image, p_MultiLineArray);

			if( S_OK == hrRet )
			{
				hrRet = l_Temp;
			}
		}
	}

	return hrRet;
}

bool SVTaskObjectListClass::DestroyChildObject( SVTaskObjectClass* pTaskObject, DWORD context )
{
	// Kill the Object
	if (nullptr != pTaskObject)
	{
		// Get the object's uniqueID
		GUID objectID = pTaskObject->GetUniqueObjectID();
		if (RemoveFromTaskObjectVector(objectID) || RemoveFriend(objectID))
		{
			// Notify the Owner of our inputs that they are not needed anymore
			pTaskObject->Disconnect();

			// Close the Object
			pTaskObject->CloseObject();

			// Destroy our Friends
			pTaskObject->DestroyFriends();

			// delete object.
			delete pTaskObject;
			pTaskObject = nullptr;

			SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
			if (nullptr != pInspection)
			{
				if (SvDef::SVMFSetDefaultInputs == (context & SvDef::SVMFSetDefaultInputs))
				{
					pInspection->SetDefaultInputs();
				}

				if (SvDef::SVMFResetInspection == (context & SvDef::SVMFResetInspection))
				{
					GetInspection()->resetAllObjects();
					pInspection->BuildValueObjectMap();
				}
			}
			return true;
		}
	}
	return false;
}

int SVTaskObjectListClass::GetSize() const
{
	return static_cast< int >( static_cast<int> (m_TaskObjectVector.size()) );
}

#pragma region virtual method (ITaskObjectListClass)
void   SVTaskObjectListClass::GetTaskObjectListInfo(SvPb::TaskObjectListResponse &rResponse) const
{
	
	for (auto pTaskObj : m_TaskObjectVector)
	{
		if (pTaskObj)
		{
			auto pInfo = rResponse.add_taskobjectinfos();
			pInfo->set_displayname(pTaskObj->GetName());
			pInfo->set_isvalid(pTaskObj->IsValid());
			pInfo->set_objectsubtype(pTaskObj->GetObjectSubType());
			pInfo->set_objecttype(pTaskObj->GetObjectType());
			SvPb::SetGuidInProtoBytes(pInfo->mutable_taskobjectid(), pTaskObj->GetUniqueObjectID());
		}
	}
}

void SVTaskObjectListClass::Delete(const SVGUID& rObjectID)
{
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(SVObjectManagerClass::Instance().GetObject(rObjectID));
	if (nullptr != pTaskObject)
	{
		// look in friend list
		if (RemoveFromTaskObjectVector(rObjectID) || RemoveFriend(rObjectID))
		{
			delete(pTaskObject);
			return;
		}
	}
	// unknown owner!!
	assert( false );
}

void SVTaskObjectListClass::InsertAt(int index, SvOi::ITaskObject& rObject, int Count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	InsertAt(index, pObject, Count);
}

void SVTaskObjectListClass::InsertAfter(const SVGUID& rPostObjectId, ITaskObject& rObject)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	InsertAfter(rPostObjectId, pObject);
}

bool SVTaskObjectListClass::DestroyChild(SvOi::ITaskObject& rObject, DWORD context)
{
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	return DestroyChildObject(pTaskObject, context);
}

SvUl::NameGuidList SVTaskObjectListClass::GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const
{
	SvUl::NameGuidList list = SVTaskObjectClass::GetCreatableObjects(pObjectTypeInfo);

	for (int i = 0; i < static_cast<int> (m_availableChildren.size()); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren[i];
		if (classInfo.m_ObjectTypeInfo.ObjectType == pObjectTypeInfo.ObjectType &&
			(pObjectTypeInfo.SubType == SvPb::SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.SubType == pObjectTypeInfo.SubType) 
			)
		{
			list.push_back(std::make_pair(classInfo.m_ClassName, classInfo.m_ClassId));
		}
	}
	return list;
}

void SVTaskObjectListClass::moveTaskObject(const SVGUID& objectToMoveId, const SVGUID& preObjectId)
{
	int currentPos = -1;
	int newPos = -1;
	SVGUID currentGuid = GUID_NULL;
	for (int i = 0; i < m_TaskObjectVector.size(); i++)
	{
		if (nullptr != m_TaskObjectVector[i])
		{
			currentGuid = m_TaskObjectVector[i]->GetUniqueObjectID();
		}
		else
		{
			continue;
		}

		if (currentGuid == objectToMoveId)
		{
			currentPos = i;
		}
		if (currentGuid == preObjectId)
		{
			newPos = i;
		}
	}

	if (0 <= currentPos && m_TaskObjectVector.size() > currentPos)
	{
		auto pObject = m_TaskObjectVector[currentPos];

		if (0 <= newPos && m_TaskObjectVector.size() > newPos)
		{
			//change first object which is later in the list.
			if (currentPos > newPos)
			{
				m_TaskObjectVector.erase(m_TaskObjectVector.begin() + currentPos);
				m_TaskObjectVector.insert(m_TaskObjectVector.begin() + newPos, pObject);
			}
			else
			{
				m_TaskObjectVector.insert(m_TaskObjectVector.begin() + newPos, pObject);
				m_TaskObjectVector.erase(m_TaskObjectVector.begin() + currentPos);
			}
		}
		else
		{
			m_TaskObjectVector.erase(m_TaskObjectVector.begin() + currentPos);
			m_TaskObjectVector.push_back(pObject);
		}
	}
	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
}
#pragma endregion virtual method (ITaskObjectListClass)

bool SVTaskObjectListClass::createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool Result = __super::createAllObjects( rCreateStructure );

	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool	= GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			Result &= m_TaskObjectVector[i]->createAllObjects(createStruct);
		}
	}
	return Result;
}

void SVTaskObjectListClass::ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	__super::ConnectObject( rCreateStructure );

	SVObjectLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			m_TaskObjectVector[i]->ConnectObject(createStruct);
		}
	}
}

void SVTaskObjectListClass::GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const
{
	SVTaskObjectClass::GetInputInterface(rInputList, bAlsoFriends);

	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			m_TaskObjectVector[i]->GetInputInterface(rInputList, bAlsoFriends);
		}
	}
}

SvOi::IObjectClass* SVTaskObjectListClass::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* retObject = SVTaskObjectClass::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	if (nullptr != retObject)
	{
		return retObject;
	}

	if (nullptr != pRequestor)
	{
		return getFirstObjectWithRequestor(rObjectTypeInfo, useFriends, pRequestor);
	}

	for (int i = 0; nullptr == retObject && i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			retObject = m_TaskObjectVector[i]->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
	}
	return retObject;
}

void SVTaskObjectListClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	SVTaskObjectClass::OnObjectRenamed(rRenamedObject, rOldName);

	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			m_TaskObjectVector[i]->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

bool SVTaskObjectListClass::ConnectAllInputs()
{
	bool Result = SVTaskObjectClass::ConnectAllInputs();

	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			Result &= m_TaskObjectVector[i]->ConnectAllInputs();
		}
	}
	return Result;
}

bool SVTaskObjectListClass::replaceObject(SVObjectClass* pObject, const GUID& rNewGuid)
{
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject);
	if (nullptr != pTaskObject)
	{
		// NOTE:	Only dynamically generated objects could be replaced, 
		//			embedded objects must be overwritten using OverwriteEmdeddedObject!!!

		// Kill the Friends
		pTaskObject->DestroyFriends();

		// - Remove All Dynamic Children (they will be constructed anew)
		if (SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pTaskObject))
		{
			// Kill all the Dynamic Children
			pTaskObjectList->DeleteAll();
		}

		// Check task list members...
		for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
		{
			if (m_TaskObjectVector[i] && m_TaskObjectVector[i]->GetUniqueObjectID() == rNewGuid)
			{
				// Delete this list member... the destructor will send SVM_OBJECT_CLOSED!
				SVObjectClass* pDuplicateObject = m_TaskObjectVector[i];
				delete pDuplicateObject;

				m_TaskObjectVector[i] = pTaskObject;

				m_LastListUpdateTimestamp = SvTl::GetTimeStamp();

				// Set unique object ID...
				if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, rNewGuid))
				{
					connectChildObject(*pTaskObject);
					return true;
				}

				return false;
			}
		}

		// Special code for Duplicates!!
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(rNewGuid);
		if (pObject)
		{
			// Get the Owner
			SVObjectInfoStruct ownerInfo = pObject->GetOwnerInfo();
			SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.getUniqueObjectID());
			if (pOwner)
			{
				SVTaskObjectListClass* pTaskListOwner = dynamic_cast<SVTaskObjectListClass*>(pOwner);
				SVTaskObjectListClass* pTaskList = dynamic_cast<SVTaskObjectListClass*>(pObject);
				if (nullptr == pTaskListOwner || !pTaskList->DestroyChildObject(pTaskList))
				{
					// must be a Friend
					pOwner->DestroyFriends();
				}
			}
		}

		// If this object not already exists, add it to the task list...
		Add(pTaskObject);

		// Special code for Objects that allocate Friends on SetOwner()
		pTaskObject->DestroyFriends();

		if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, rNewGuid))
		{
			connectChildObject(*pTaskObject);
			return true;
		}
	}

	return false;
}
#pragma endregion public methods

#pragma region protected methods
// .Title       : DeleteAt
// -----------------------------------------------------------------------------
// .Description : Calls the list object destructor and removes the pointer from 
//				: the list.
//				: Index is the zero based list index, Count is the number of 
//				: items to kill.
//				:
//				: If Index or Count is out of range, you will get an ASSERT 
//				: message, and Index and Count will be set to their max 
//				: possible values!!!
void SVTaskObjectListClass::DeleteAt(int Index, int Count /*= 1*/)
{
	if (Count < 1)
	{
		return;
	}
	
	if (Index < 0)
	{
		assert( false );
		Index = 0;
	}
	
	if (Index >= static_cast<int> (m_TaskObjectVector.size()))
	{
		assert( false );
		return;
	}
	
	if (Index + Count > static_cast<int> (m_TaskObjectVector.size()))
	{
		assert( false );
		Count = static_cast<int> (m_TaskObjectVector.size()) - Index; 
	}
	
	SVTaskObjectClass* pTaskObject( nullptr );
	for (int i = Index + Count - 1; i >= Index; -- i)
	{
		pTaskObject = m_TaskObjectVector[i];
		if (pTaskObject)
		{
			DestroyChildObject( pTaskObject );
		}
	}
}

// Calls DeleteAt( 0, GetSize() )...
void SVTaskObjectListClass::DeleteAll()
{
	DeleteAt(0, static_cast<int> (m_TaskObjectVector.size()));

	m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
}

HRESULT SVTaskObjectListClass::onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
{
	HRESULT hrRet = SVTaskObjectClass::onCollectOverlays(p_Image, p_MultiLineArray);

	return hrRet;
}

SVObjectClass* SVTaskObjectListClass::UpdateObject( const GUID &p_oFriendGuid, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner )
{
	SVObjectClass* l_psvObject = nullptr;

	int l_iSize = static_cast<int> (m_TaskObjectVector.size());

	// find the friend in our taskObject List
	for (int i = 0; nullptr == l_psvObject && i < l_iSize; i++)
	{
		l_psvObject = m_TaskObjectVector[i];

		if ( nullptr != l_psvObject && p_oFriendGuid == l_psvObject->GetUniqueObjectID() )
		{
			m_TaskObjectVector.erase(m_TaskObjectVector.begin() + i);

			m_LastListUpdateTimestamp = SvTl::GetTimeStamp();
		}
		else
		{
			l_psvObject = nullptr;
		}
	}

	if ( nullptr != p_psvObject )
	{
		p_psvObject->SetObjectOwner( p_psvNewOwner );
	}

	return l_psvObject;
}

bool SVTaskObjectListClass::getAvailableObjects(SVClassInfoStructVector* pList, const SvDef::SVObjectTypeInfoStruct* pObjectTypeInfo) const
{
	bool rc = false;

	for (int i = 0; i < static_cast<int> (m_availableChildren.size()); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren[i];
		if (classInfo.m_ObjectTypeInfo.ObjectType == pObjectTypeInfo->ObjectType &&
			(pObjectTypeInfo->SubType == SvPb::SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.SubType == pObjectTypeInfo->SubType) 
			)
		{
			pList->push_back(classInfo);
			rc = true;
		}
	}
	return rc;
}

bool SVTaskObjectListClass::resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = SVTaskObjectClass::resetAllOutputListObjects( pErrorMessages );

	// Try to send message to outputObjectList members
	for( int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); ++ i )
	{
		SVTaskObjectClass* l_pObject( m_TaskObjectVector[ i ] );
		if( nullptr != l_pObject )
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error-messages to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			l_pObject->resetAllOutputListObjects();
		}
	}
	return Result;
}

bool SVTaskObjectListClass::isInputImage(const SVGUID& rImageGuid) const
{
	bool Result = SVTaskObjectClass::isInputImage(rImageGuid);

	if (GUID_NULL != rImageGuid)
	{
		// Get Object from our children
		for (int i = 0; !Result && i < static_cast<int> (m_TaskObjectVector.size()); i++)
		{
			if (nullptr != m_TaskObjectVector[i])
			{
				Result = m_TaskObjectVector[i]->isInputImage(rImageGuid);
			}
		}
	}

	return Result;
}

bool SVTaskObjectListClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	clearRunErrorMessages();
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
	ChildRunStatus.m_triggerRecord = rRunStatus.m_triggerRecord;
	ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	if (!rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition())
	{
		// Run your children...
		for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
		{
			SVTaskObjectClass* pTaskObject = m_TaskObjectVector[i];
			if ( nullptr != pTaskObject )
			{
				ChildRunStatus.ResetRunStateAndToolSetTimes();

				bRetVal = pTaskObject->Run(ChildRunStatus, &m_RunErrorMessages) & bRetVal;

				// Update our Run Status
				if ( ChildRunStatus.IsDisabled() ) { rRunStatus.SetDisabled(); }

				if ( ChildRunStatus.IsDisabledByCondition() ) { rRunStatus.SetDisabledByCondition(); }

				if ( ChildRunStatus.IsWarned() ) { rRunStatus.SetWarned(); }

				if ( ChildRunStatus.IsFailed() ) { rRunStatus.SetFailed(); }

				if ( ChildRunStatus.IsPassed() ) { rRunStatus.SetPassed(); }

				if ( ChildRunStatus.IsCriticalFailure() ) { rRunStatus.SetCriticalFailure(); }
			}
		}
	}

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

SVTaskObjectListClass::SVObjectPtrDeque SVTaskObjectListClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque Objects = __super::GetPreProcessObjects();

	return Objects;
}

SVTaskObjectListClass::SVObjectPtrDeque SVTaskObjectListClass::GetPostProcessObjects() const
{
	SVObjectPtrDeque Objects = __super::GetPostProcessObjects();

	SVTaskObjectPtrVector::const_iterator l_Iter;

	for( l_Iter = m_TaskObjectVector.begin(); l_Iter != m_TaskObjectVector.end(); ++l_Iter )
	{
		SVTaskObjectClass* l_pTask = *l_Iter;

		if( nullptr != l_pTask )
		{
			Objects.push_back( l_pTask );
		}
	}

	return Objects;
}

bool SVTaskObjectListClass::resetAllObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = __super::resetAllObjects(pErrorMessages);

	SvStl::MessageContainerVector taskErrorMessages;
	// Notify all...
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		if (m_TaskObjectVector[i])
		{
			Result = (m_TaskObjectVector[i]->resetAllObjects(&taskErrorMessages) && Result);
		}
	}

	//set error return from the tasks to the member error container and to the return container if necessary. 
	if (!taskErrorMessages.empty())
	{
		m_ResetErrorMessages.insert(m_ResetErrorMessages.end(), taskErrorMessages.begin(), taskErrorMessages.end());
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->insert(pErrorMessages->end(), taskErrorMessages.begin(), taskErrorMessages.end());
		}
	}

	m_isObjectValid.SetValue(BOOL(Result));

	return Result;
}

void SVTaskObjectListClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct;

	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool	= GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	rChildObject.ConnectObject(createStruct);
}
#pragma endregion protected methods

#pragma region Private Methods
SvOi::IObjectClass* SVTaskObjectListClass::getFirstObjectWithRequestor( const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor ) const
{
	SvOi::IObjectClass* retObject = nullptr;
	if (pRequestor == this || pRequestor == GetParent())
	{
		return nullptr;
	}
	SVObjectClass* pObject = nullptr;

	// look at outputs first
	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList( l_OutputInfoList );

	for (int oIndex = 0; oIndex < l_OutputInfoList.GetSize(); oIndex++)
	{
		SVOutObjectInfoStruct* pOutputInfo = l_OutputInfoList.GetAt(oIndex);

		if (pOutputInfo)
		{
			pObject = pOutputInfo->getObject();

			if (nullptr == pObject)
			{
				pObject = SVObjectManagerClass::Instance().GetObject(pOutputInfo->getUniqueObjectID());
			}

			if (nullptr != pObject && pObject->GetParent() != pRequestor && pObject != pRequestor)
			{
				// Don't send to requester owned outputs
				retObject = pObject->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
				if (nullptr != retObject)
				{
					return retObject;
				}
			}
			else
			{
				break; // stop looking at outputs
			}
		}
	}
	// look at children
	// Try to send message to list members, if not already processed...
	for (int i = 0; nullptr == retObject && i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		pObject = m_TaskObjectVector[i];
		if (nullptr != pObject && pObject != pRequestor)
		{
			retObject = pObject->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
		else
		{
			break; // encountered requester Object
		}
	}
	return retObject;
}

/*
This method is used to remove an object from the friends list via the object's unique object identifier.
*/
bool SVTaskObjectListClass::RemoveFriend(const GUID& rFriendGUID)
{
	// Check GUID...
	if (GUID_NULL != rFriendGUID)
	{
		// Check if friend is applied...
		if (m_friendList.size())
		{
			for (int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; --i)
			{
				if (m_friendList[i].getUniqueObjectID() == rFriendGUID)
				{
					// Remove Friend...
					m_friendList.RemoveAt(i);

					// Success...
					return true;
				}
			}
		}
	}
	return false;
}

bool SVTaskObjectListClass::RemoveFromTaskObjectVector(const SVGUID& rObjectID)
{
	for (int i = 0; i < static_cast<int> (m_TaskObjectVector.size()); i++)
	{
		SVTaskObjectClass* pTaskObject = m_TaskObjectVector[i];

		if (pTaskObject && pTaskObject->GetUniqueObjectID() == rObjectID)
		{
			m_LastListUpdateTimestamp = SvTl::GetTimeStamp();

			m_TaskObjectVector.erase(m_TaskObjectVector.begin() + i);
			// Delete object not till it is removed from list!!!
			return true;
		}
	}
	return false;
}

#pragma endregion Private Methods