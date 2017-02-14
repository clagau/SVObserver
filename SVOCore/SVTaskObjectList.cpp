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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
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

SVTaskObjectListClass::SVTaskObjectListClass(BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID)
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

	for( int i = 0; i < m_aTaskObjects.GetSize(); ++ i )
	{
		SVTaskObjectClass* l_pObject( m_aTaskObjects.GetAt( i ) );

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
	addDefaultInputObjects(TRUE);
	
	// Append input output objects of all list members...
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->GetInputObjects(m_InputObjectList);
		}
	}
}

void SVTaskObjectListClass::RemoveOutputObject( SVOutObjectInfoStruct* p_pOutObject )
{
	if ( SVTaskObjectListClass* pTaskObjectList = dynamic_cast <SVTaskObjectListClass*> (GetOwner()) )
	{
		pTaskObjectList->RemoveOutputObject( p_pOutObject );
	}
}

void SVTaskObjectListClass::GetAllInputObjects()
{
	// Try to get all inputs and outputs...
	
	// First clean up current lists...
	m_InputObjectList.RemoveAll();
	
	// Call Base Class to inform friends...
	SVTaskObjectClass::GetAllInputObjects();
	
	// Force all list members to rebuild their input output objects lists...
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->GetAllInputObjects();
		}
	}
	
	// Finally append all objects to my lists...
	AppendInputObjects();
}

void SVTaskObjectListClass::Persist(SVObjectWriter& writer)
{
	writer.StartElement(GetObjectName()); // use internal name for node name
	SVTaskObjectClass::Persist(writer);

	// Get script of task list members...
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
			if (SV_IS_KIND_OF(pTaskObject, SVTaskObjectListClass))
			{
				pTaskObject->Persist(writer);
			}
			else
			{
				writer.StartElement(pTaskObject->GetObjectName()); // use internal name for node name
				pTaskObject->Persist(writer);
				writer.EndElement();
			}
		}
	}
	writer.EndElement();
}

SVTaskObjectClass* SVTaskObjectListClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass* l_psvObject = SVTaskObjectClass::GetObjectAtPoint( p_rsvPoint );

	// Get Object from our children
	for (int i = 0; nullptr == l_psvObject && i < m_aTaskObjects.GetSize(); i++)
	{
		l_psvObject = m_aTaskObjects.GetAt(i);

		if ( nullptr != l_psvObject )
		{
			l_psvObject = l_psvObject->GetObjectAtPoint( p_rsvPoint );
		}
	}

	return l_psvObject;
}

HRESULT SVTaskObjectListClass::IsInputImage( SVImageClass* p_psvImage )
{
	HRESULT l_hrOk = SVTaskObjectClass::IsInputImage( p_psvImage );

	if ( nullptr != p_psvImage )
	{
		// Get Object from our children
		for (int i = 0; S_OK != l_hrOk && i < m_aTaskObjects.GetSize(); i++)
		{
			SVTaskObjectClass* l_psvObject = m_aTaskObjects.GetAt(i);

			if ( nullptr != l_psvObject )
			{
				l_hrOk = l_psvObject->IsInputImage( p_psvImage );
			}
		}
	}

	return l_hrOk;
}

BOOL SVTaskObjectListClass::CloseObject()
{
	BOOL Result( true );

	// Close our children
	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
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
			for (int i = 0; nullptr == rpObject && i < m_aTaskObjects.GetSize(); i++)
			{
				SVTaskObjectClass* l_pTask = m_aTaskObjects.GetAt(i);
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

const SVClock::SVTimeStamp& SVTaskObjectListClass::GetLastListUpdateTimestamp() const
{
	return m_LastListUpdateTimestamp;
}

void SVTaskObjectListClass::InsertAt(int nIndex, SVTaskObjectClass* PTaskObject, int nCount)
{
	if (! PTaskObject)
	{
		assert( false );
		return;
	}
	
	// Check for Unique names
	const SVString NewName( checkName( PTaskObject->GetName() ) );
	if( NewName != PTaskObject->GetName() )
	{
		PTaskObject->SetName( NewName.c_str() );
	}
	
	PTaskObject->SetObjectOwner(this);
	
	m_aTaskObjects.InsertAt(nIndex, PTaskObject, nCount);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

// Use this if You want to set list entry to nullptr!
// And to replace an entyr with a new pointer after it was deleted!!!
void SVTaskObjectListClass::SetAt(int nIndex, SVTaskObjectClass* PTaskObject)
{
	if (PTaskObject)
	{
		// Check for Unique names
		const SVString NewName( checkName( PTaskObject->GetName() ) );
		if( NewName != PTaskObject->GetName() )
		{
			PTaskObject->SetName( NewName.c_str() );
		}
		
		PTaskObject->SetObjectOwner(this);
	}
	
	m_aTaskObjects.SetAt(nIndex, PTaskObject);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

SVTaskObjectClass* SVTaskObjectListClass::GetAt( int nIndex ) const 
{
	return m_aTaskObjects.GetAt(nIndex);
}

void SVTaskObjectListClass::RemoveAt( int nIndex, int nCount )
{
	m_aTaskObjects.RemoveAt(nIndex, nCount);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

int SVTaskObjectListClass::Add(SVTaskObjectClass* pTaskObject, bool atBegin)
{
	if (! pTaskObject)
	{
		return -1;
	}

	// Check for Unique names 
	const SVString NewName( checkName( pTaskObject->GetName() ) );
	if( NewName != pTaskObject->GetName() )
	{
		pTaskObject->SetName( NewName.c_str() );
	}
	pTaskObject->SetObjectOwner(this);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

	if (!atBegin)
	{
		// call the base class to really add it
		return m_aTaskObjects.Add(pTaskObject);
	}
	m_aTaskObjects.InsertAt(0, pTaskObject);
	return 0;
}

HRESULT SVTaskObjectListClass::RemoveChild( SVTaskObjectClass* pChildObject )
{
	HRESULT Result( S_OK );

	if ( !DestroyChildObject(pChildObject, SVMFSetDefaultInputs) )
	{
		Result = S_FALSE;
	}
	return Result;
}

// Should be overridden and must be called in derived classes...
BOOL SVTaskObjectListClass::SetObjectDepth(int NewObjectDepth)
{
	// Set object depth of members here...

	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
	{
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetObjectDepth(NewObjectDepth);
		}
	}
	return SVTaskObjectClass::SetObjectDepth(NewObjectDepth);
}

// Should be overridden and must be called in derived classes...
BOOL SVTaskObjectListClass::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	// Set object depth of members here...

	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
	{
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
	}
	return SVTaskObjectClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
}

BOOL SVTaskObjectListClass::SetImageDepth(long lDepth)
{
	// Set object depth of members here...

	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
	{
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetImageDepth(lDepth);
		}
	}
	return SVTaskObjectClass::SetImageDepth(lDepth);
}

void SVTaskObjectListClass::SetInvalid()
{
	// Set this object and all own embedded objects to invalid
	SVTaskObjectClass::SetInvalid();

	// Set all children to invalid also
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
	{
		SVTaskObjectClass* pObject = m_aTaskObjects.GetAt(j);
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
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
	{
		SVTaskObjectClass* pObject = m_aTaskObjects.GetAt(j);
		if (pObject)
		{
			pObject->SetDisabled();
		}
	}
}

const SVString SVTaskObjectListClass::checkName( LPCTSTR ToolName ) const
{
	SVString objectName;
	SVString newName( ToolName );

	int ToolIndex( 0 );
	for( int i = 0; i < m_aTaskObjects.GetSize(); i++ )
	{
		SVObjectClass* pObject = m_aTaskObjects.GetAt(i);
		if( nullptr != pObject )
		{
			objectName = pObject->GetName();
			if ( SVString::npos != objectName.find( ToolName ))
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
							SVString numStr = SvUl_SF::Right( objectName, (objectName.size() - 1) - i);
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
		newName = SvUl_SF::Format( _T("%s%d"), ToolName, ToolIndex );
	}

	return newName;
}

HRESULT SVTaskObjectListClass::CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT hrRet = S_OK;

	hrRet = SVTaskObjectClass::CollectOverlays(p_Image,p_MultiLineArray);

	for ( int i = 0; i < m_aTaskObjects.GetSize(); i++ )
	{
		SVTaskObjectClass* pObject = dynamic_cast< SVTaskObjectClass* >( m_aTaskObjects.GetAt(i) );

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
		// if the object is a Child of this
		for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
		{
			SVObjectClass* pObject = m_aTaskObjects.GetAt(i);
			if (nullptr != pObject && pObject == pTaskObject)
			{
				DestroyTaskObject(*pTaskObject, context);
				return true;
			}
		}
	}
	return false;
}

int SVTaskObjectListClass::GetSize() const
{
	return static_cast< int >( m_aTaskObjects.GetSize() );
}

#pragma region virtual method (ITaskObjectListClass)
SvUl::NameGuidList SVTaskObjectListClass::GetTaskObjectList( ) const
{
	SvUl::NameGuidList list;
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		SVTaskObjectClass* pObject = m_aTaskObjects.GetAt(i);
		if (pObject)
		{
			list.push_back(SvUl::NameGuidPair(pObject->GetName(), pObject->GetUniqueObjectID()));
		}
	}
	return list;
}

// .Title       : Delete
// .Description : Calls the list object destructor and removes the pointer from 
//				: the list.
//				: objectId is the taskObjectId
void SVTaskObjectListClass::Delete(GUID& objectID)
{
	SVTaskObjectClass* pTaskObject;

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		pTaskObject = m_aTaskObjects.GetAt(i);

		if (pTaskObject && pTaskObject->GetUniqueObjectID() == objectID)
		{
			m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

			m_aTaskObjects.RemoveAt(i);
			// Delete object not till it is removed from list!!!
			if (pTaskObject)
			{
				delete(pTaskObject);
				return;
			}
		}
	}
	// look in friend list
	if (RemoveFriend(objectID))
	{
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(objectID);
		if (pObject)
		{
			delete(pObject);
			return;
		}
	}
	// unknown owner!!
	assert( false );
}

void SVTaskObjectListClass::InsertAt(int index, SvOi::ITaskObject& rObject, int count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	InsertAt(index, pObject, count);
}

bool SVTaskObjectListClass::DestroyChild(SvOi::ITaskObject& rObject, DWORD context)
{
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	return DestroyChildObject(pTaskObject, context);
}

bool SVTaskObjectListClass::DestroyFriendObject(SvOi::IObjectClass& rObject, DWORD context)
{
	// Get the object's uniqueID
	GUID objectID = rObject.GetUniqueObjectID();

	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(&rObject);

	if (nullptr != pTaskObject)
	{
		// Notify the Owner of our inputs that they are not needed anymore
		pTaskObject->Disconnect();

		// Close the Object
		pTaskObject->CloseObject();

		// Destroy our Friends
		pTaskObject->DestroyFriends();
	}

	bool retVal = (TRUE == RemoveFriend(objectID));
	if (retVal)
	{
		delete(&rObject);
	}

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (GetInspection());
	if( nullptr != pInspection )
	{
		if( SVMFSetDefaultInputs == ( context & SVMFSetDefaultInputs ) )
		{
			pInspection->SetDefaultInputs();
		}

		if( SVMFResetInspection == ( context & SVMFResetInspection ) )
		{
			GetInspection()->resetAllObjects();
		}
	}

	return retVal;
}

SvUl::NameGuidList SVTaskObjectListClass::GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const
{
	SvUl::NameGuidList list = SVTaskObjectClass::GetCreatableObjects(pObjectTypeInfo);

	for (int i = 0; i < m_availableChildren.GetSize(); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren.GetAt(i);
		if (classInfo.m_ObjectTypeInfo.ObjectType == pObjectTypeInfo.ObjectType &&
			(pObjectTypeInfo.SubType == SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.SubType == pObjectTypeInfo.SubType) 
			)
		{
			list.push_back(std::make_pair(classInfo.m_ClassName, classInfo.m_ClassId));
		}
	}
	return list;
}
#pragma endregion virtual method (ITaskObjectListClass)

bool SVTaskObjectListClass::createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool Result = __super::createAllObjects( rCreateStructure );

	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo = GetInspection();

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			Result &= m_aTaskObjects.GetAt(i)->createAllObjects(createStruct);
		}
	}
	return Result;
}

void SVTaskObjectListClass::ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	__super::ConnectObject( rCreateStructure );

	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->ConnectObject(createStruct);
		}
	}
}

void SVTaskObjectListClass::GetInputInterface(SVInputInfoListClass& rInputList, bool bAlsoFriends) const
{
	SVTaskObjectClass::GetInputInterface(rInputList, bAlsoFriends);

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->GetInputInterface(rInputList, bAlsoFriends);
		}
	}
}

SvOi::IObjectClass* SVTaskObjectListClass::getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
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

	for (int i = 0; nullptr == retObject && i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			retObject = m_aTaskObjects.GetAt(i)->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
	}
	return retObject;
}

void SVTaskObjectListClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVTaskObjectClass::OnObjectRenamed(rRenamedObject, rOldName);

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

bool SVTaskObjectListClass::ConnectAllInputs()
{
	bool Result = SVTaskObjectClass::ConnectAllInputs();

	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			Result &= m_aTaskObjects.GetAt(i)->ConnectAllInputs();
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
		for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
		{
			if (m_aTaskObjects.GetAt(i) && m_aTaskObjects.GetAt(i)->GetUniqueObjectID() == rNewGuid)
			{
				// Delete this list member... the destructor will send SVM_OBJECT_CLOSED!
				SVObjectClass* pDuplicateObject = m_aTaskObjects.GetAt(i);
				delete pDuplicateObject;

				m_aTaskObjects.SetAt(i, nullptr);

				// Replace list member...
				m_aTaskObjects.SetAt(i, pTaskObject);

				m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

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
			SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.UniqueObjectID);
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
	
	if (Index >= m_aTaskObjects.GetSize())
	{
		assert( false );
		return;
	}
	
	if (Index + Count > m_aTaskObjects.GetSize())
	{
		assert( false );
		Count = m_aTaskObjects.GetSize() - Index; 
	}
	
	SVTaskObjectClass* pTaskObject( nullptr );
	for (int i = Index + Count - 1; i >= Index; -- i)
	{
		pTaskObject = m_aTaskObjects.GetAt(i);
		if (pTaskObject)
		{
			DestroyChildObject( pTaskObject );
		}
	}
}

// Calls DeleteAt( 0, GetSize() )...
void SVTaskObjectListClass::DeleteAll()
{
	DeleteAt(0, m_aTaskObjects.GetSize());

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

HRESULT SVTaskObjectListClass::onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT hrRet = SVTaskObjectClass::onCollectOverlays(p_Image, p_MultiLineArray);

	return hrRet;
}

SVObjectClass* SVTaskObjectListClass::UpdateObject( const GUID &p_oFriendGuid, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner )
{
	SVObjectClass* l_psvObject = nullptr;

	int l_iSize = m_aTaskObjects.GetSize();

	// find the friend in our taskObject List
	for (int i = 0; nullptr == l_psvObject && i < l_iSize; i++)
	{
		l_psvObject = m_aTaskObjects.GetAt(i);

		if ( nullptr != l_psvObject && p_oFriendGuid == l_psvObject->GetUniqueObjectID() )
		{
			m_aTaskObjects.RemoveAt(i);

			m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
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

BOOL SVTaskObjectListClass::getAvailableObjects(SVClassInfoStructListClass* pList, const SVObjectTypeInfoStruct* pObjectTypeInfo) const
{
	BOOL rc = FALSE;

	for (int i = 0; i < m_availableChildren.GetSize(); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren.GetAt(i);
		if (classInfo.m_ObjectTypeInfo.ObjectType == pObjectTypeInfo->ObjectType &&
			(pObjectTypeInfo->SubType == SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.SubType == pObjectTypeInfo->SubType) 
			)
		{
			pList->Add(classInfo);
			rc = TRUE;
		}
	}
	return rc;
}

bool SVTaskObjectListClass::resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages/*=nullptr */ )
{
	bool Result = SVTaskObjectClass::resetAllOutputListObjects( pErrorMessages );

	// Try to send message to outputObjectList members
	for( int i = 0; i < m_aTaskObjects.GetSize(); ++ i )
	{
		SVTaskObjectClass* l_pObject( m_aTaskObjects[ i ] );
		if( nullptr != l_pObject )
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error-messages to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			l_pObject->resetAllOutputListObjects();
		}
	}
	return Result;
}

BOOL SVTaskObjectListClass::Run(SVRunStatusClass& RRunStatus)
{
	clearRunErrorMessages();
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ChildRunStatus.Images = RRunStatus.Images;
	ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	// Run yourself...
	BOOL bRetVal = onRun(RRunStatus);

	if (!RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition())
	{
		// Run your children...
		for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
			if ( nullptr != pTaskObject )
			{
				ChildRunStatus.ResetRunStateAndToolSetTimes();

				BOOL l_bTemp = pTaskObject->Run(ChildRunStatus);
				bRetVal &= l_bTemp;

				// Update our Run Status
				if ( ChildRunStatus.IsDisabled() ) { RRunStatus.SetDisabled(); }

				if ( ChildRunStatus.IsDisabledByCondition() ) { RRunStatus.SetDisabledByCondition(); }

				if ( ChildRunStatus.IsWarned() ) { RRunStatus.SetWarned(); }

				if ( ChildRunStatus.IsFailed() ) { RRunStatus.SetFailed(); }

				if ( ChildRunStatus.IsPassed() ) { RRunStatus.SetPassed(); }

				if ( ChildRunStatus.IsCriticalFailure() ) { RRunStatus.SetCriticalFailure(); }
			}
		}
	}

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

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

	for( l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
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
	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			Result = (m_aTaskObjects.GetAt(i)->resetAllObjects(&taskErrorMessages) && Result);
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

	m_isObjectValid.SetValue(1, Result);

	return Result;
}

void SVTaskObjectListClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;

	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	rChildObject.ConnectObject(createStruct);
}
#pragma endregion protected methods

#pragma region Private Methods
void SVTaskObjectListClass::DestroyTaskObject(SVTaskObjectClass& rTaskObject, DWORD context)
{
	// Notify the Owner of our inputs that they are not needed anymore
	rTaskObject.Disconnect();

	// Close the Object
	rTaskObject.CloseObject();

	// Get the object's uniqueID
	GUID objectID = rTaskObject.GetUniqueObjectID();

	// Destroy our Friends
	rTaskObject.DestroyFriends();

	// Remove it from the SVTaskObjectList ( Destruct it )
	Delete(objectID);

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(GetInspection());
	if( nullptr != pInspection )
	{
		if( SVMFSetDefaultInputs == ( context & SVMFSetDefaultInputs ) )
		{
			pInspection->SetDefaultInputs();
		}

		if( SVMFResetInspection == ( context & SVMFResetInspection ) )
		{
			GetInspection()->resetAllObjects();
		}
	}
}

SvOi::IObjectClass* SVTaskObjectListClass::getFirstObjectWithRequestor( const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor ) const
{
	SvOi::IObjectClass* retObject = nullptr;
	if (pRequestor == this || pRequestor == GetOwner())
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
			pObject = pOutputInfo->PObject;

			if (nullptr == pObject)
			{
				pObject = SVObjectManagerClass::Instance().GetObject(pOutputInfo->UniqueObjectID);
			}

			if (nullptr != pObject && pObject->GetOwner() != pRequestor && pObject != pRequestor)
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
	for (int i = 0; nullptr == retObject && i < m_aTaskObjects.GetSize(); i++)
	{
		pObject = m_aTaskObjects.GetAt(i);
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
#pragma endregion Private Methods