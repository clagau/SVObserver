//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectList
//* .File Name       : $Workfile:   SVTaskObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Feb 2014 12:18:36  $
//******************************************************************************

// @WARNING:  This filename (SVTaskObjectList) does not match the class name (SVTaskObjectListClass).

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObjectList.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVTimerLibrary/SVClock.h"
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

HRESULT SVTaskObjectListClass::GetOutputList( SVOutputInfoListClass& p_rOutputInfoList )
{
	HRESULT l_Status( SVTaskObjectClass::GetOutputList( p_rOutputInfoList ) );

	for( int i = 0; i < m_aTaskObjects.GetSize(); ++ i )
	{
		SVTaskObjectClass* l_pObject( m_aTaskObjects.GetAt( i ) );

		if( l_pObject != NULL )
		{
			HRESULT l_Temp = l_pObject->GetOutputList( p_rOutputInfoList );

			if( l_Status == S_OK )
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

#ifdef USE_OBJECT_SCRIPT
void SVTaskObjectListClass::GetObjectScript(CString& RStrScript, CString& RStrAliasTable, int Indent)
{
	CString script;
	
	// preallocate 512K
	script.GetBuffer(512*1024);  
	script.ReleaseBuffer(); 
	
	SVTaskObjectClass::GetObjectScript(script, RStrAliasTable, Indent);
	
	// Generate indent...
	CString strIndent = _T("\n");
	if (Indent)
	{
		CString tabsStrIndent(_T('\t'), Indent);
		strIndent += tabsStrIndent;
	}
	
	// Overwrite task object termination termination...
	int last = script.ReverseFind(TCHAR('}'));
	if (last >= 0)
	{
		script = script.Left(last);
	}

	// Set up further object definitions...

	// Get script of task list members...
	for (int i = 0; i < m_aTaskObjects.GetSize(); ++ i)
	{
		if (m_aTaskObjects.GetAt(i))
		{
			m_aTaskObjects.GetAt(i)->GetObjectScript(script, RStrAliasTable, Indent + 1);
		}
	}

	script.FreeExtra();

	RStrScript += script;
	
	// Add termination...
	RStrScript += strIndent + _T("};");
}
#endif

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

int SVTaskObjectListClass::GetSize() const
{
	return static_cast< int >( m_aTaskObjects.GetSize() );
}

// @WARNING:  checkName method 	doesn't only check the name, but it also changes the name if neccessary.
void SVTaskObjectListClass::checkName(SVTaskObjectClass* PTaskObject)
{
	CString name = PTaskObject->GetName();
	CString objectName;
	CString tmp;
	CString newName;
	
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
		
		PTaskObject->SetName(newName);
	}
}

SVTaskObjectClass *SVTaskObjectListClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass *l_psvObject = SVTaskObjectClass::GetObjectAtPoint( p_rsvPoint );

	// Get Object from our children
	for (int i = 0; l_psvObject == NULL && i < m_aTaskObjects.GetSize(); i++)
	{
		l_psvObject = m_aTaskObjects.GetAt(i);

		if ( l_psvObject != NULL )
		{
			l_psvObject = l_psvObject->GetObjectAtPoint( p_rsvPoint );
		}
	}

	return l_psvObject;
}

HRESULT SVTaskObjectListClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = SVTaskObjectClass::IsInputImage( p_psvImage );

	if ( p_psvImage != NULL )
	{
		// Get Object from our children
		for (int i = 0; l_hrOk != S_OK && i < m_aTaskObjects.GetSize(); i++)
		{
			SVTaskObjectClass *l_psvObject = m_aTaskObjects.GetAt(i);

			if ( l_psvObject != NULL )
			{
				l_hrOk = l_psvObject->IsInputImage( p_psvImage );
			}
		}
	}

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate member function of class SVTaskObjectListClass
// -----------------------------------------------------------------------------
// .Description : Validates the inputs of this object and its children
//				: Only Override in Special Cases
//				: Note: Normally Override OnValidate
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :05.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectListClass::Validate()
{
	BOOL retVal = SVTaskObjectClass::Validate();
	
	if (retVal)
	{
		for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
			if (pTaskObject)
			{
				BOOL l_bTemp = pTaskObject->Validate();

				ASSERT( l_bTemp );

				retVal &= l_bTemp;
			}
			else
			{
				retVal = FALSE;
			}
		}
	}
	
	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate member function of class SVTaskObjectListClass
// -----------------------------------------------------------------------------
// .Description : validates the inputs of this object
//				: must be overridden
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :25.08.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVTaskObjectListClass::OnValidate()
{
	BOOL retVal = SVTaskObjectClass::OnValidate();
	
	if (! retVal)
	{
		SetInvalid();
	}
	
	return retVal;
}

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

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
	// MZA: It doesn't check only the name, it also changes the name if neccessary.
	checkName(PTaskObject);
	
	// SEJ Aug 10,1999
	PTaskObject->SetObjectOwner(this);
	
	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

	// call the base class to realy add it
	return m_aTaskObjects.Add(PTaskObject);
}

HRESULT SVTaskObjectListClass::RemoveChild( SVTaskObjectClass* pChildObject )
{
	HRESULT hr = S_OK;

	LONG_PTR uRetCode = SVSendMessage (this, 
	               SVM_DESTROY_CHILD_OBJECT,
	               reinterpret_cast<LONG_PTR>(pChildObject),
	               SVMFSetDefaultInputs);

	if( uRetCode != SVMR_SUCCESS )
	{
		hr = S_FALSE;
	}
	return hr;
}

void SVTaskObjectListClass::InsertAt(int nIndex, SVTaskObjectClass* PTaskObject, int nCount)
{
	if (! PTaskObject)
	{
		ASSERT(0);
		return;
	}
	
	// Check for Unique names
	checkName(PTaskObject);
	
	// SEJ Aug 10,1999
	PTaskObject->SetObjectOwner(this);
	
	m_aTaskObjects.InsertAt(nIndex, PTaskObject, nCount);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

// Use this if You want to set list entry to NULL!
// And to replace an entyr with a new pointer after it was deleted!!!
void SVTaskObjectListClass::SetAt(int nIndex, SVTaskObjectClass* PTaskObject)
{
	if (PTaskObject)
	{
		// Check for Unique names
		checkName(PTaskObject);
		
		// SEJ Aug 10,1999
		PTaskObject->SetObjectOwner(this);
	}
	
	m_aTaskObjects.SetAt(nIndex, PTaskObject);

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
}

// Should be overridden and must be called in derived classes...
BOOL SVTaskObjectListClass::SetObjectDepth(int NewObjectDepth)
{
	// Set object depth of members here...
	
	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetObjectDepth(NewObjectDepth);
		}
		
		return SVTaskObjectClass::SetObjectDepth(NewObjectDepth);
}

// Should be overridden and must be called in derived classes...
BOOL SVTaskObjectListClass::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	// Set object depth of members here...
	
	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
		}
		
		return SVTaskObjectClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);
}

BOOL SVTaskObjectListClass::SetImageDepth(long lDepth)
{
	// Set object depth of members here...
	
	// Set object depth of all task list members...
	for (int j = 0; j < m_aTaskObjects.GetSize(); ++ j)
		if (m_aTaskObjects.GetAt(j))
		{
			m_aTaskObjects.GetAt(j)->SetImageDepth(lDepth);
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

////////////////////////////////////////////////////////////////////////////////
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
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int				Index
//	:int				Count
// .Return Value
//	:None 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :17.02.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVTaskObjectListClass::DeleteAt(int Index, int Count /*= 1*/)
{
	// SV_FORMAT_MESSAGE_5( "%s - > SVTaskObjectListClass::DeleteAt( int Index, int Count /*= 1*/ )\nSize: %d\nIndex: %d\nCount: %d", GetName(), GetSize(), Index, Count, NULL );
	
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
		// RemoveAt( i );
		// Delete object not till it is removed from list!!!
		if (pTaskObject)
		{
			// delete( pTaskObject );
			::SVSendMessage(this, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<LONG_PTR>(pTaskObject), NULL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Delete
// -----------------------------------------------------------------------------
// .Description : Calls the list object destructor and removes the pointer from 
//				: the list.
//				: objectId is the taskObjectId
//
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int				Index
//	:int				Count
// .Return Value
//	:None 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :14.07.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
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

// Calls DeleteAt( 0, GetSize() )...
void SVTaskObjectListClass::DeleteAll()
{
	DeleteAt(0, m_aTaskObjects.GetSize());

	m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
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
			if ( pTaskObject != NULL )
			{
				ChildRunStatus.ClearAll();
				
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

		if( l_pTask != NULL )
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

SVObjectClass *SVTaskObjectListClass::UpdateObject( const GUID &p_oFriendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner )
{
	SVObjectClass *l_psvObject = NULL;
	
	int l_iSize = m_aTaskObjects.GetSize();

	// find the friend in our taskObject List
	for (int i = 0; l_psvObject == NULL && i < l_iSize; i++)
	{
		l_psvObject = m_aTaskObjects.GetAt(i);

		if ( l_psvObject != NULL && p_oFriendGuid == l_psvObject->GetUniqueObjectID() )
		{
			m_aTaskObjects.RemoveAt(i);

			m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
		}
		else
		{
			l_psvObject = NULL;
		}
	}

	if ( p_psvObject != NULL )
	{
		p_psvObject->SetObjectOwner( p_psvNewOwner );
	}

	return l_psvObject;
}

BOOL SVTaskObjectListClass::CloseObject()
{
	LONG_PTR DwResult;
	BOOL retVal = TRUE;
	
	// Close our children
	for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
	{
		SVTaskObjectClass* pTaskObject = m_aTaskObjects.GetAt(i);
		if (pTaskObject)
		{
			DwResult = ::SVSendMessage(pTaskObject, SVM_CLOSE_OBJECT, NULL, NULL);
			retVal = (DwResult == SVMR_SUCCESS) && retVal;
		}
	}
	// Close ourself and our friends
	retVal = SVTaskObjectClass::CloseObject() && retVal;
	
	return retVal;
}

HRESULT SVTaskObjectListClass::GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index ) const
{
	HRESULT l_Status = SVTaskObjectClass::GetChildObject( p_rpObject, p_rNameInfo, p_Index );

	if( l_Status != S_OK )
	{
		if( 0 < p_rNameInfo.m_NameArray.size() && p_rNameInfo.m_NameArray[ p_Index ] == GetName() )
		{
			for (int i = 0; p_rpObject == NULL && i < m_aTaskObjects.GetSize(); i++)
			{
				SVTaskObjectClass* l_pTask = m_aTaskObjects.GetAt(i);
				if( l_pTask != NULL )
				{
					l_Status = l_pTask->GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
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
	
LONG_PTR SVTaskObjectListClass::processMessage(DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext)
{
	//
	// NOTE: 
	//	General rule for routing:
	//		Opening Direction ==> 1. Friends, 2. You ( and Your embeddeds ), 3. Children
	//		Closing Direction ==> 1. Children, 2. You ( and Your embeddeds ), 3. Friends
	//

	LONG_PTR DwResult = SVMR_NOT_PROCESSED;

	// Check if friend should process this message first....
	if ((DwMessageID & SVM_NOTIFY_FRIENDS) == SVM_NOTIFY_FRIENDS)
	{
		//
		// NOTE: Beware to route Create Object Messages here !!!
		//

		// Let Base Class process Friend Notifications.
		// Make sure that only friends get this message.
		DwResult = SVTaskObjectClass::processMessage(DwMessageID | SVM_NOTIFY_ONLY_FRIENDS, DwMessageValue, DwMessageContext);

		// Check if processed and Notify First Responding...
		if (DwResult != SVMR_NOT_PROCESSED && (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING)
		{
			return DwResult;
		}

		// Check if Notify Only Friends...
		if ((DwMessageID & SVM_NOTIFY_ONLY_FRIENDS) == SVM_NOTIFY_ONLY_FRIENDS)
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
			if( !IsCreated() && !CreateObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) )
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

			DwMessageValue = (DWORD)&createStruct;

			break;
		}

	case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( ConnectObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) != S_OK )
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

			DwMessageValue = (DWORD)&createStruct;

			break;
		}

	case SVMSGID_GET_IMAGE_BAND0_OBJECT:
		{
			// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo!
			//( SVObjectTypeInfoStruct->objectType => SVObjectTypeEnum )
			SVObjectTypeInfoStruct* pObjectTypeInfo = (SVObjectTypeInfoStruct*)DwMessageContext;
				
			// ...use second message parameter ( DwMessageValue ) to specify the requesting object
			// Note: if second parameter is specified - stop when requesting object encountered.
			// Optional - to get an input
			SVObjectClass* pRequestor = (SVObjectClass*)DwMessageValue;

			if (pRequestor)
			{
				if (pRequestor == this || pRequestor == GetOwner())
				{
					return NULL;
				}
				SVObjectClass* pObject = NULL;
					
				SVOutputInfoListClass l_OutputInfoList;

				GetOutputList( l_OutputInfoList );

				// look at outputs first
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
							DwResult = ::SVSendMessage(pObject, SVM_GETFIRST_OBJECT, DwMessageValue, DwMessageContext);
							if (DwResult)
							{
								SVObjectClass* pObject1;
								pObject1 = (SVObjectClass*)DwResult;

								if( ( ::strcmp( pObject1->GetObjectName(), _T("Band 0 Image") ) == 0 ) && ( ::strcmp( pObject1->GetOwner()->GetObjectName(), _T("Color Tool") ) == 0 ) )
								{
									break;
								}
								else
								{
									DwResult = 0;
								}
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
				for (int i = 0; !DwResult && i < m_aTaskObjects.GetSize(); i++)
				{
					pObject = m_aTaskObjects.GetAt(i);
					if (pObject && pObject != pRequestor)
					{
						DwResult = ::SVSendMessage(pObject, SVM_GETFIRST_OBJECT, DwMessageValue, DwMessageContext);
						if (DwResult)
						{
							SVObjectClass* pObject1;
							pObject1 = (SVObjectClass*)DwResult;

							if( ( ::strcmp( pObject1->GetObjectName(), _T("Band 0 Image") ) == 0 ) && ( ::strcmp( pObject1->GetOwner()->GetObjectName(), _T("Color Tool") ) == 0 ) )
							{
								break;
							}
							else
							{
								DwResult = 0;
							}
						}
					}
					else
					{
						break; // encountered requestor Object
					}
				}
				return DwResult;
			}

			DwResult = SVObjectClass::processMessage(SVM_GETFIRST_OBJECT, DwMessageValue, DwMessageContext);
			if (DwResult)
			{
				SVObjectClass* pObject1;
				pObject1 = (SVObjectClass*)DwResult;

				if( ( ::strcmp( pObject1->GetObjectName(), _T("Band 0 Image") ) == 0 ) && ( ::strcmp( pObject1->GetOwner()->GetObjectName(), _T("Color Tool") ) == 0 ) )
				{
					break;
				}
				else
				{
					DwResult = 0;
				}
			}
			if (DwResult != SVMR_NOT_PROCESSED)
			{
				return DwResult;
			}

			break;
		}

	case SVMSGID_GETFIRST_OBJECT:
		{
			// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo!
			//( SVObjectTypeInfoStruct->objectType => SVObjectTypeEnum )
			SVObjectTypeInfoStruct* pObjectTypeInfo = (SVObjectTypeInfoStruct*)DwMessageContext;

			// ...use second message parameter ( DwMessageValue ) to specify the requesting object
			// Note: if second parameter is specified - stop when requesting object encountered.
			// Optional - to get an input
			SVObjectClass* pRequestor = (SVObjectClass*)DwMessageValue;

			if (pRequestor)
			{
				if (pRequestor == this || pRequestor == GetOwner())
				{
					return NULL;
				}
				SVObjectClass* pObject = NULL;

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
			if (DwResult != SVMR_NOT_PROCESSED)
			{
				return DwResult;
			}

			break;
		}

	case SVMSGID_GETAVAILABLE_OBJECTS: // Only SVTaskObjectListClasses have available objects (currently)
		{
			SVClassInfoStructListClass* pList = (SVClassInfoStructListClass *)DwMessageValue;
			SVObjectTypeInfoStruct* pObjectTypeInfo = (SVObjectTypeInfoStruct *)DwMessageContext;
					
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
			SVTaskObjectClass* pTaskObject = (SVTaskObjectClass*) DwMessageContext;
			if (pTaskObject && SV_IS_KIND_OF(pTaskObject, SVTaskObjectClass))
			{
				// NOTE:	Only dynamically generated objects could be replaced, 
				//			embedded objects must be overwritten using SVM_OVERWRITE_OBJECT!!!

				// SEJ (July 15 1999) - Remove All Dynamic Children (they will be constructed anew)
				if (SV_IS_KIND_OF(pTaskObject, SVTaskObjectListClass))
				{
					SVTaskObjectListClass* pTaskObjectList = (SVTaskObjectListClass*) DwMessageContext;

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

						// delete( m_aTaskObjects.GetAt( i ) );
						m_aTaskObjects.SetAt(i, NULL);

						// Replace list member...
						m_aTaskObjects.SetAt(i, pTaskObject);

						m_LastListUpdateTimestamp = SVClock::GetTimeStamp();

						// Set unique object ID...
						if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, taskObjectID))
						{
							::SVSendMessage( this, SVM_CONNECT_CHILD_OBJECT, reinterpret_cast<LONG_PTR>(pTaskObject), NULL );
								
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
						if (::SVSendMessage(pOwner, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<LONG_PTR>(pObject), NULL) == SVMR_NO_SUCCESS)
						{
							// must be a Friend
							pOwner->DestroyFriends();
						}
					}
				}

				// If this object not already exists, add it to the task list...
				Add(pTaskObject);

				// SEJ - Sep 28,1999
				// Special code for Objects that allocate Friends on SetOwner()
				pTaskObject->DestroyFriends();

				if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, taskObjectID))
				{
					::SVSendMessage( this, SVM_CONNECT_CHILD_OBJECT, reinterpret_cast<LONG_PTR>(pTaskObject), NULL );

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

	case SVMSGID_DESTROY_CHILD_OBJECT:		// SEJ Aug 6,1999 New Message
		{
			SVTaskObjectClass* pTaskObject = (SVTaskObjectClass*) DwMessageValue;

			// Kill the Object
			if (pTaskObject)
			{
				// if the object is a Child of this
				for (int i = 0; i < m_aTaskObjects.GetSize(); i++)
				{
					SVObjectClass* pObject = m_aTaskObjects.GetAt(i);
					if (pObject && pObject == pTaskObject)
					{
						// Notify the Owner of our inputs that they are not needed anymore
						pTaskObject->Disconnect();

						// Close the Object
						pTaskObject->CloseObject();

						// Get the object's uniqueID
						GUID objectID = pTaskObject->GetUniqueObjectID();

						// Destroy our Friends
						pTaskObject->DestroyFriends();

						// Remove it from the SVTaskObjectList ( Destruct it )
						Delete(objectID);

						if( GetInspection() != NULL )
						{
							if( ( DwMessageContext & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
							{
								GetInspection()->SetDefaultInputs();
							}

							if( ( DwMessageContext & SVMFResetInspection ) == SVMFResetInspection )
							{
								::SVSendMessage( GetInspection(), SVM_RESET_ALL_OBJECTS, NULL, NULL );
							}
						}

						return SVMR_SUCCESS;
					}
				}
			}

			return SVMR_NO_SUCCESS;
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

	if ((DwMessageID & SVM_NOTIFY_ONLY_THIS) != SVM_NOTIFY_ONLY_THIS)
	{
		// Try to send message to list members, if not already processed...
		if ((DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING)
		{
			// Send to base class - Since SVTaskObjectListClass is also a SVTaskObjectClass - SEJ July 20,1999
			// But don't notify friends again...
			DWORD dwMaskID = DwMessageID & ~(SVM_NOTIFY_ONLY_FRIENDS | SVM_NOTIFY_FRIENDS);
			DwResult = SVTaskObjectClass::processMessage(dwMaskID, DwMessageValue, DwMessageContext);

			// Stop queueing, if DwResult is not NULL...
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
			// Send to base class - Since SVTaskObjectListClass is also a SVTaskObjectClass - SEJ Aug 11,1999
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
		if (DwResult == SVMR_NOT_PROCESSED)
		{
			// But don't notify friends again...
			DWORD dwMaskID = DwMessageID & ~(SVM_NOTIFY_ONLY_FRIENDS | SVM_NOTIFY_FRIENDS);
			return SVTaskObjectClass::processMessage(dwMaskID, DwMessageValue, DwMessageContext);
		}
	}
	
	return DwResult;
}

LONG_PTR SVTaskObjectListClass::OutputListProcessMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = SVTaskObjectClass::OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );

	// Try to send message to outputObjectList members, if not already processed...
	if( (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING )
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

LONG_PTR SVTaskObjectListClass::ChildrenOutputListProcessMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = SVMR_NOT_PROCESSED;

	// Try to send message to outputObjectList members, if not already processed...
	if( (DwMessageID & SVM_NOTIFY_FIRST_RESPONDING) == SVM_NOTIFY_FIRST_RESPONDING )
	{
		for( int i = 0; ! DwResult && i < m_aTaskObjects.GetSize(); ++ i )
		{
			SVTaskObjectClass* l_pObject( m_aTaskObjects[ i ] );

			if( l_pObject != NULL )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext );
			}
			else
			{
				DwResult = NULL;
			}
		}
	}
	else
	{
		for( int i = 0; i < m_aTaskObjects.GetSize(); ++ i )
		{
			SVTaskObjectClass* l_pObject( m_aTaskObjects[ i ] );

			if( l_pObject != NULL )
			{
				DwResult = l_pObject->OutputListProcessMessage( DwMessageID, DwMessageValue, DwMessageContext) | DwResult;
			}
			else
			{
				DwResult = NULL | DwResult;
			}
		}
	}

	return DwResult;
}

BOOL SVTaskObjectListClass::getAvailableObjects(SVClassInfoStructListClass* pList, SVObjectTypeInfoStruct* pObjectTypeInfo)
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

HRESULT SVTaskObjectListClass::CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT hrRet = S_OK;

	hrRet = SVTaskObjectClass::CollectOverlays(p_Image,p_MultiLineArray);

	for ( int i = 0; i < m_aTaskObjects.GetSize(); i++ )
	{
		SVTaskObjectClass *pObject = dynamic_cast< SVTaskObjectClass* >( m_aTaskObjects.GetAt(i) );

		if ( pObject != NULL )
		{
			HRESULT l_Temp = pObject->CollectOverlays(p_Image, p_MultiLineArray);

			if( hrRet == S_OK )
			{
				hrRet = l_Temp;
			}
		}
	}

	return hrRet;
}

HRESULT SVTaskObjectListClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT hrRet = S_FALSE;

	hrRet = SVTaskObjectClass::onCollectOverlays(p_Image,p_MultiLineArray);

	return hrRet;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTaskObjectList.cpp_v  $
 * 
 *    Rev 1.4   01 Feb 2014 12:18:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Jan 2014 16:52:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * In the Add method, created a comment to explain what checkName does.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:43:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   29 Apr 2013 16:19:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   29 Apr 2013 14:33:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:34:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   25 Mar 2013 12:58:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the copying of the errorlist from the ChildRunStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   27 Feb 2013 11:27:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Run method - copied the ChildRunStatus errorlist to the main RunStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   10 Jan 2013 15:15:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   04 Sep 2012 15:20:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23.1.0   08 Jan 2013 12:30:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   01 Aug 2012 13:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Jul 2012 14:27:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   16 Jul 2012 13:03:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 Jul 2012 17:44:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   13 Apr 2011 15:35:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   11 Apr 2011 18:55:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated child run status objects with update counters flag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Mar 2011 15:29:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   08 Dec 2010 13:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Jun 2010 14:08:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 Jun 2010 15:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Jun 2010 11:10:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   05 Mar 2010 11:01:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element to run status for critical errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Dec 2009 13:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Mar 2006 13:57:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  547
 * SCR Title:  Fix Image overlay data
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed overlays issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Nov 2005 07:33:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   03 Oct 2005 15:14:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   correctly handle removal of objects from outputObjectList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Aug 2005 08:22:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 May 2005 14:09:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified CollectOverlays(.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Mar 2005 12:26:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved loop variable to make debugging easier
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Mar 2005 12:03:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved CloseObject to after Disconnect to remove undefined objects from dependents list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2005 06:55:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes Made to support new Extents.
 * Added GetObjectAtPoint(
 * Added GetImageExtent(
 * Added SetImageExtent(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2005 15:21:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jul 2004 12:03:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implement by containment, not inheritance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2003 16:36:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/