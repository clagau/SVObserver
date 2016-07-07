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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVInspectionProcess.h"
#include "SVAnalyzer.h"
#include "SVTool.h"
#pragma endregion

#ifdef _DEBUG
#define new DEBUG_NEW
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

// .Title       : Validate member function of class SVTaskObjectListClass
// .Description : Validates the inputs of this object and its children
//				: Only Override in Special Cases
//				: Note: Normally Override OnValidate
BOOL SVTaskObjectListClass::Validate()
{
	BOOL Result = SVTaskObjectClass::Validate();

	if (Result)
	{
		for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
			if (pTaskObject)
			{
				BOOL l_bTemp = pTaskObject->Validate();
				Result &= l_bTemp;
			}
			else
			{
				Result = FALSE;
			}
		}
	}

	//if Tool-validation is fail but no message in the Message-List, add one
	if (FALSE == Result && 0 == getFirstTaskMessage().getMessage().m_MessageCode)
	{
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_5072_INCONSISTENTDATA, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
	}

	if (Result)
	{
		Result = OnValidateParameter(AllParameters);
	}

	return Result;
}

// .Title       : OnValidate member function of class SVTaskObjectListClass
// .Description : validates the inputs of this object
//				: must be overridden
BOOL SVTaskObjectListClass::OnValidate()
{
	BOOL Result = (0 == getFirstTaskMessage().getMessage().m_MessageCode);

	if ( Result )
	{
		Result = SVTaskObjectClass::OnValidate();
		if( !Result && 0 != getFirstTaskMessage().getMessage().m_MessageCode)
		{
			SvStl::MessageContainer message;
			SVStringArray msgList;
			msgList.push_back(GetName());
			message.setMessage( SVMSG_SVO_5074_BASECLASSONVALIDATEFAILED, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
		}
	}

	if ( Result )
	{
		Result = OnValidateParameter(InspectionSettable);
	}

	if (! Result)
	{
		SetInvalid();
	}

	return Result;
}

BOOL SVTaskObjectListClass::CloseObject()
{
	DWORD_PTR DwResult = 0;
	BOOL retVal = TRUE;

	// Close our children
	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
		if (pTaskObject)
		{
			DwResult = ::SVSendMessage(pTaskObject, SVM_CLOSE_OBJECT, 0, 0);
			retVal = (SVMR_SUCCESS == DwResult) && retVal;
		}
	}
	// Close ourself and our friends
	retVal = SVTaskObjectClass::CloseObject() && retVal;

	return retVal;
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
		ASSERT(0);
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

int SVTaskObjectListClass::Add(SVTaskObjectClass* PTaskObject)
{
	if (! PTaskObject)
	{
		return -1;
	}

	// Check for Unique names 
	const SVString NewName( checkName( PTaskObject->GetName() ) );
	if( NewName != PTaskObject->GetName() )
	{
		PTaskObject->SetName( NewName.c_str() );
	}
	// SEJ Aug 10,1999
	PTaskObject->SetObjectOwner(this);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

	// call the base class to realy add it
	return m_aTaskObjects.Add(PTaskObject);
}

HRESULT SVTaskObjectListClass::RemoveChild( SVTaskObjectClass* pChildObject )
{
	HRESULT hr = S_OK;

	DWORD_PTR uRetCode = SVSendMessage (this, 
		SVM_DESTROY_CHILD_OBJECT,
		reinterpret_cast<DWORD_PTR>(pChildObject),
		SVMFSetDefaultInputs);

	if( SVMR_SUCCESS != uRetCode )
	{
		hr = S_FALSE;
	}
	return hr;
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
	CString name( ToolName );
	CString objectName;
	CString tmp;
	SVString newName( ToolName );

	int num = 0;
	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		SVObjectClass* pObject = m_aTaskObjects.GetAt(i);
		if (pObject)
		{
			objectName = pObject->GetName();
			if (! objectName.Find(name))
			{
				// see if the name ends in a number
				int lastNum;
				bool digit = false;

				for (int i = objectName.GetLength() - 1; i >= 0; i--)
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
							CString numStr = objectName.Right((objectName.GetLength() - 1) - i);
							lastNum = atoi(numStr);
						}
						break;
					}
				}

				if (digit)
				{
					num = std::max(num, lastNum + 1);
				}
				else
				{
					num = std::max(num, 1);
				}
			}
		}
	}
	// Set the name
	if (num)
	{
		tmp.Format("%d", num);
		newName = name + tmp;
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
	ASSERT(0);
}

void SVTaskObjectListClass::InsertAt(int index, SvOi::ITaskObject& rObject, int count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	InsertAt(index, pObject, count);
}

DWORD_PTR SVTaskObjectListClass::DestroyChild(SvOi::ITaskObject& rObject, DWORD context)
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

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		if( SVMFSetDefaultInputs == ( context & SVMFSetDefaultInputs ) )
		{
			pInspection->SetDefaultInputs();
		}

		if( SVMFResetInspection == ( context & SVMFResetInspection ) )
		{
			::SVSendMessage( pInspection, SVM_RESET_ALL_OBJECTS, 0, 0 );
		}
	}

	return retVal;
}

SvUl::NameGuidList SVTaskObjectListClass::GetCreatableObjects(const SVObjectTypeInfoStruct& pObjectTypeInfo) const
{
	SvUl::NameGuidList list = SVTaskObjectClass::GetCreatableObjects(pObjectTypeInfo);

	for (int i = 0; i < availableChildren.GetSize(); i++)
	{
		SVClassInfoStruct classInfo = availableChildren.GetAt(i);
		if (classInfo.ObjectTypeInfo.ObjectType == pObjectTypeInfo.ObjectType &&
			(pObjectTypeInfo.SubType == SVNotSetSubObjectType ||
			classInfo.ObjectTypeInfo.SubType == pObjectTypeInfo.SubType) 
			)
		{
			list.push_back(std::make_pair(SVString(classInfo.ClassName), classInfo.ClassId));
		}
	}
	return list;
}
#pragma endregion virtual method (ITaskObjectListClass)

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
		ASSERT(0);
		Index = 0;
	}
	
	if (Index >= m_aTaskObjects.GetSize())
	{
		ASSERT(0);	
		return;
	}
	
	if (Index + Count > m_aTaskObjects.GetSize())
	{
		ASSERT(0);	
		Count = m_aTaskObjects.GetSize() - Index; 
	}
	
	SVTaskObjectClass* pTaskObject;
	for (int i = Index + Count - 1; i >= Index; -- i)
	{
		pTaskObject = m_aTaskObjects.GetAt(i);
		if (pTaskObject)
		{
			::SVSendMessage(this, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pTaskObject), 0);
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

	for (int i = 0; i < availableChildren.GetSize(); i++)
	{
		SVClassInfoStruct classInfo = availableChildren.GetAt(i);
		if (classInfo.ObjectTypeInfo.ObjectType == pObjectTypeInfo->ObjectType &&
			(pObjectTypeInfo->SubType == SVNotSetSubObjectType ||
			classInfo.ObjectTypeInfo.SubType == pObjectTypeInfo->SubType) 
			)
		{
			pList->Add(classInfo);
			rc = TRUE;
		}
	}
	return rc;
}

BOOL SVTaskObjectListClass::Run(SVRunStatusClass& RRunStatus)
{
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
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return bRetVal;
}

SVTaskObjectListClass::SVObjectPtrDeque SVTaskObjectListClass::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVTaskObjectClass::GetPreProcessObjects();

	return l_Objects;
}

SVTaskObjectListClass::SVObjectPtrDeque SVTaskObjectListClass::GetPostProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVTaskObjectClass::GetPostProcessObjects();

	SVTaskObjectPtrVector::const_iterator l_Iter;

	for( l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
	{
		SVTaskObjectClass* l_pTask = *l_Iter;

		if( nullptr != l_pTask )
		{
			l_Objects.push_back( l_pTask );
		}
	}

	return l_Objects;
}

BOOL SVTaskObjectListClass::onRun(SVRunStatusClass& RRunStatus)
{
	return SVTaskObjectClass::onRun(RRunStatus);
}

DWORD_PTR SVTaskObjectListClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	//
	// NOTE: 
	//	General rule for routing:
	//		Opening Direction ==> 1. Friends, 2. You ( and Your embeddeds ), 3. Children
	//		Closing Direction ==> 1. Children, 2. You ( and Your embeddeds ), 3. Friends
	//

	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Check if friend should process this message first....
	if (SVM_NOTIFY_FRIENDS == (DwMessageID & SVM_NOTIFY_FRIENDS))
	{
		//
		// NOTE: Beware to route Create Object Messages here !!!
		//

		// Let Base Class process Friend Notifications.
		// Make sure that only friends get this message.
		DwResult = SVTaskObjectClass::processMessage(DwMessageID | SVM_NOTIFY_ONLY_FRIENDS, DwMessageValue, DwMessageContext);

		// Check if processed and Notify First Responding...
		if (SVMR_NOT_PROCESSED != DwResult && SVM_NOTIFY_FIRST_RESPONDING == (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING))
		{
			return DwResult;
		}

		// Check if Notify Only Friends...
		if (SVM_NOTIFY_ONLY_FRIENDS == (DwMessageID & SVM_NOTIFY_ONLY_FRIENDS))
		{
			return DwResult;
		}
	}

	SVAnalyzerLevelCreateStruct createStruct;

	// Try to process message by yourself...
	// ( if necessary process here the incoming messages )
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
	case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo = GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

	case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( S_OK != ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

	case SVMSGID_GETFIRST_OBJECT:
		{
			// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo!
			//( SVObjectTypeInfoStruct->objectType => SVObjectTypeEnum )
			SVObjectTypeInfoStruct* pObjectTypeInfo = reinterpret_cast<SVObjectTypeInfoStruct*>(DwMessageContext);

			// ...use second message parameter ( DwMessageValue ) to specify the requesting object
			// Note: if second parameter is specified - stop when requesting object encountered.
			// Optional - to get an input
			SVObjectClass* pRequestor = reinterpret_cast<SVObjectClass*>(DwMessageValue);

			if (pRequestor)
			{
				if (pRequestor == this || pRequestor == GetOwner())
				{
					return SVMR_NOT_PROCESSED;
				}
				SVObjectClass* pObject = nullptr;

				// look at outputs first
				SVOutputInfoListClass l_OutputInfoList;

				GetOutputList( l_OutputInfoList );

				for (int oIndex = 0; !DwResult && oIndex < l_OutputInfoList.GetSize(); oIndex++)
				{
					SVOutObjectInfoStruct* pOutputInfo = l_OutputInfoList.GetAt(oIndex);

					if (pOutputInfo)
					{
						pObject = pOutputInfo->PObject;

						if (!pObject)
						{
							pObject = SVObjectManagerClass::Instance().GetObject(pOutputInfo->UniqueObjectID);
						}

						if (pObject && pObject->GetOwner() != pRequestor && pObject != pRequestor)
						{
							// Don't send to requestor owned outputs
							DwResult = ::SVSendMessage(pObject, DwMessageID, DwMessageValue, DwMessageContext);
						}
						else
						{
							break; // stop looking at outputs
						}
					}
				}
				// look at children
				// Try to send message to list members, if not already processed...
				for (int i = 0; !DwResult && i < m_aTaskObjects.GetSize(); i++)
				{
					pObject = m_aTaskObjects.GetAt(i);
					if (pObject && pObject != pRequestor)
					{
						DwResult = ::SVSendMessage(pObject, DwMessageID, DwMessageValue, DwMessageContext);
					}
					else
					{
						break; // encountered requestor Object
					}
				}
				return DwResult;
			}

			DwResult = SVObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
			if (SVMR_NOT_PROCESSED != DwResult)
			{
				return DwResult;
			}
			break;
		}

	case SVMSGID_GETAVAILABLE_OBJECTS: // Only SVTaskObjectListClasses have available objects (currently)
		{
			SVClassInfoStructListClass* pList = reinterpret_cast<SVClassInfoStructListClass*>(DwMessageValue);
			SVObjectTypeInfoStruct* pObjectTypeInfo = reinterpret_cast<SVObjectTypeInfoStruct*>(DwMessageContext);

			if (getAvailableObjects(pList, pObjectTypeInfo))
			{
				return SVMR_SUCCESS;
			}

			return SVMR_NOT_PROCESSED;

			break;
		}

	case SVMSGID_REPLACE_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
			// ...use third message parameter ( DwMessageContext ) as SVObjectClass*
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS
			const GUID taskObjectID = * ((GUID*) DwMessageValue);
			SVTaskObjectClass* pTaskObject = reinterpret_cast<SVTaskObjectClass*>(DwMessageContext);
			if (nullptr != pTaskObject)
			{
				// NOTE:	Only dynamically generated objects could be replaced, 
				//			embedded objects must be overwritten using SVM_OVERWRITE_OBJECT!!!

				// - Remove All Dynamic Children (they will be constructed anew)
				if (SVTaskObjectListClass* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pTaskObject))
				{
					// Kill the Friends
					pTaskObjectList->DestroyFriends();

					// Kill all the Dynamic Children
					pTaskObjectList->DeleteAll();
				}
				else
				{
					// Kill the Friends
					// Both SVTaskObjectListClass and SVTaskObjectClass can have Friends
					pTaskObject->DestroyFriends();
				}
				// Check task list members...
				for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
				{
					if (m_aTaskObjects.GetAt(i) && m_aTaskObjects.GetAt(i)->GetUniqueObjectID() == taskObjectID)
					{
						// Delete this list member... the destructors will send SVM_OBJECT_CLOSED!
						SVObjectClass* l_pObject = m_aTaskObjects.GetAt(i);
						delete l_pObject;

						m_aTaskObjects.SetAt(i, nullptr);

						// Replace list member...
						m_aTaskObjects.SetAt(i, pTaskObject);

						m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

						// Set unique object ID...
						if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, taskObjectID))
						{
							::SVSendMessage( this, SVM_CONNECT_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pTaskObject), 0 );

							return SVMR_SUCCESS;
						}

						return SVMR_NO_SUCCESS;
					}
				}

				// Special code for Duplicates!!
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(taskObjectID);
				if (pObject)
				{
					// Get the Owner
					SVObjectInfoStruct ownerInfo = pObject->GetOwnerInfo();
					SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.UniqueObjectID);
					if (pOwner)
					{
						// Ask the owner to kill the imposter!
						if (SVMR_NO_SUCCESS == ::SVSendMessage(pOwner, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pObject), 0))
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

				if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, taskObjectID))
				{
					::SVSendMessage( this, SVM_CONNECT_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pTaskObject), 0 );

					return SVMR_SUCCESS;
				}
			}

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_OVERWRITE_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
			// ...use third message parameter ( DwMessageContext ) as embeddedID ( GUID* ) 
			// ...returns pointer to embedded SVObjectClass
			//			const GUID taskObjectID = *( ( GUID* ) DwMessageValue );
			//			const GUID embeddedID	= *( ( GUID* ) DwMessageContext );
			// NOTE:	Only for embedded objects !!! 
			//			Dynamically generated objects must be replaced,
			//			using SVM_REPLACE_OBJECT!!!

			// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
			// ... and if object could not be identified then call base class processMessage member function!!!

			return SVTaskObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
		}

	case SVMSGID_DESTROY_CHILD_OBJECT:
		{
			SVTaskObjectClass* pTaskObject = reinterpret_cast<SVTaskObjectClass*>(DwMessageValue);
			return DestroyChildObject(pTaskObject, static_cast<DWORD>(DwMessageContext));
		}

	case SVMSGID_CLOSE_OBJECT:
		{
			if ( CloseObject() ) { return SVMR_SUCCESS; }

			return SVMR_NO_SUCCESS;
		}

	case SVMSGID_SET_OBJECT_VALUE:
		{
			return SVTaskObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);
		}

	default:
		{
			// Do nothing.
			break;
		}
	} // switch

	if (SVM_NOTIFY_ONLY_THIS != (DwMessageID & SVM_NOTIFY_ONLY_THIS))
	{
		// Try to send message to list members, if not already processed...
		if (SVM_NOTIFY_FIRST_RESPONDING == (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING))
		{
			// Send to base class - Since SVTaskObjectListClass is also a SVTaskObjectClass
			// But don't notify friends again...
			DWORD dwMaskID = DwMessageID & ~(SVM_NOTIFY_ONLY_FRIENDS | SVM_NOTIFY_FRIENDS);
			DwResult = SVTaskObjectClass::processMessage(dwMaskID, DwMessageValue, DwMessageContext);

			// Stop queuing, if DwResult is not nullptr...
			// Try to send message to list members, if not already processed...
			for (int i = 0; ! DwResult && i < m_aTaskObjects.GetSize(); i++)
			{
				if (m_aTaskObjects.GetAt(i))
				{
					DwResult = ::SVSendMessage(m_aTaskObjects.GetAt(i), DwMessageID, DwMessageValue, DwMessageContext);
				}
			}
		}
		else
		{
			// Send to base class - Since SVTaskObjectListClass is also a SVTaskObjectClass
			// But don't notify friends again...
			DWORD dwMaskID = DwMessageID & ~(SVM_NOTIFY_ONLY_FRIENDS | SVM_NOTIFY_FRIENDS);
			DwResult = SVTaskObjectClass::processMessage(dwMaskID, DwMessageValue, DwMessageContext) | DwResult;

			// Notify all...
			for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
			{
				if (m_aTaskObjects.GetAt(i))
				{
					DwResult = ::SVSendMessage(m_aTaskObjects.GetAt(i), DwMessageID, DwMessageValue, DwMessageContext) | DwResult;
				}
			}
		}
	}
	else
	{
		// Route to Base Class if not yet processed.
		if (SVMR_NOT_PROCESSED == DwResult)
		{
			// But don't notify friends again...
			DWORD dwMaskID = DwMessageID & ~(SVM_NOTIFY_ONLY_FRIENDS | SVM_NOTIFY_FRIENDS);
			return SVTaskObjectClass::processMessage(dwMaskID, DwMessageValue, DwMessageContext);
		}
	}

	return DwResult;
}

DWORD_PTR SVTaskObjectListClass::OutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVTaskObjectClass::OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );

	// Try to send message to outputObjectList members, if not already processed...
	if( SVM_NOTIFY_FIRST_RESPONDING == (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) )
	{
		if( ! DwResult )
		{
			DwResult = ChildrenOutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
		}
	}
	else
	{
		DwResult = ChildrenOutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}

	return DwResult;
}

DWORD_PTR SVTaskObjectListClass::ChildrenOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to send message to outputObjectList members, if not already processed...
	if( SVM_NOTIFY_FIRST_RESPONDING == (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) )
	{
		for( int i = 0; ! DwResult && i < m_aTaskObjects.GetSize(); ++ i )
		{
			SVTaskObjectClass* l_pObject( m_aTaskObjects[ i ] );

			if( nullptr != l_pObject )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED;
			}
		}
	}
	else
	{
		for( int i = 0; i < m_aTaskObjects.GetSize(); ++ i )
		{
			SVTaskObjectClass* l_pObject( m_aTaskObjects[ i ] );

			if( nullptr != l_pObject )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext) | DwResult;
			}
			else
			{
				DwResult = SVMR_NOT_PROCESSED | DwResult;
			}
		}
	}

	return DwResult;
}

bool SVTaskObjectListClass::OnValidateParameter (ValidationLevelEnum validationLevel)
{
	bool Result(true);

	Result = ValidateInspectionSettableParameters();

	if (Result && (RemotelyAndInspectionSettable == validationLevel || AllParameters == validationLevel))
	{
		Result = ValidateRemotelySettableParameters ();
	}

	if (Result && AllParameters == validationLevel)
	{
		Result = ValidateOfflineParameters ();
	}

	return Result;
}

bool SVTaskObjectListClass::ValidateInspectionSettableParameters ()
{
	bool Result = true;
	// Possibly should include ROI extents (top, bottom, height, width), but 
	// I think Extents are checked elsewhere.
	return Result;
}

bool SVTaskObjectListClass::ValidateRemotelySettableParameters ()
{
	bool Result = true;

	return Result;
}

bool SVTaskObjectListClass::ValidateOfflineParameters ()
{
	bool Result = true;

	return Result;
}
#pragma endregion protected methods

#pragma region Private Methods
void SVTaskObjectListClass::cleanUpEmptyEntries()
{
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		if (! m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.RemoveAt(i--);
		}
	}
	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

DWORD_PTR SVTaskObjectListClass::DestroyChildObject(SVTaskObjectClass* pTaskObject, DWORD context)
{
	//This code was located before in processMessage case SVMSGID_DESTROY_CHILD_OBJECT and is moved to this method.

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
				return SVMR_SUCCESS;
			}
		}
	}
	return SVMR_NO_SUCCESS;
}

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

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		if( SVMFSetDefaultInputs == ( context & SVMFSetDefaultInputs ) )
		{
			pInspection->SetDefaultInputs();
		}

		if( SVMFResetInspection == ( context & SVMFResetInspection ) )
		{
			::SVSendMessage( pInspection, SVM_RESET_ALL_OBJECTS, 0, 0 );
		}
	}
}
#pragma endregion Private Methods