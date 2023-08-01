//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectList
//* .File Name       : $Workfile:   SVTaskObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   19 Dec 2014 04:22:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObjectList.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "RunStatus.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion


std::vector<std::string> getNamesFromTaskObjectPtrVector(const SvIe::SVTaskObjectPtrVector& rTaskObjectVector)
{
	std::vector<std::string> stringlist;

	std::transform(rTaskObjectVector.cbegin(), rTaskObjectVector.cend(), back_inserter(stringlist),
			[](auto& pObject){return pObject->GetName(); });

	return stringlist;
}


namespace SvIe
{


#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS(SVTaskObjectListClass, SVTaskObjectListClassId)

#pragma region Constructor
SVTaskObjectListClass::SVTaskObjectListClass(LPCSTR ObjectName)
				      :SVTaskObjectClass(ObjectName), m_LastListUpdateTimestamp( 0 ) 
{
}

SVTaskObjectListClass::SVTaskObjectListClass(SVObjectClass* POwner, int StringResourceID)
					  :SVTaskObjectClass(POwner, StringResourceID), m_LastListUpdateTimestamp( 0 ) 
{
}

SVTaskObjectListClass::~SVTaskObjectListClass()
{
	DeleteAll();
}
#pragma endregion

#pragma region public methods
void SVTaskObjectListClass::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
{
	__super::getOutputList(inserter);
	
	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); ++i)
	{
		SVTaskObjectClass* l_pObject(getTaskObject(i));

		if (nullptr != l_pObject)
		{
			l_pObject->getOutputList(inserter);
		}
	}
}

void SVTaskObjectListClass::fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter)
{
	__super::fixInvalidInputs(inserter);

	for (auto pObject : m_TaskObjectVector)
	{
		if (nullptr != pObject)
		{
			pObject->fixInvalidInputs(inserter);
		}
	}
}

void SVTaskObjectListClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed /*= false*/, bool /*firstObject = false*/) const
{
	if (0 != (ObjectAttributesAllowed() & SvPb::viewable))
	{
		nameToType = (SvPb::SVNotSetObjectType == nameToType) ? GetObjectType() : nameToType;
		__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);

		for (const auto* pObject : m_TaskObjectVector)
		{
			if (nullptr != pObject)
			{
				pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			}
		}
	}
}

void SVTaskObjectListClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
{
	if (0 != (ObjectAttributesAllowed() & SvPb::viewable) || addHidden)
	{
		__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);

		for (auto* pObject : m_TaskObjectVector)
		{
			if (nullptr != pObject)
			{
				pObject->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
			}
		}
	}
}

HRESULT SVTaskObjectListClass::ConnectToObject(const std::string& rInputName, uint32_t newID)
{
	HRESULT result = __super::ConnectToObject(rInputName, newID);
	if (S_OK == result)
	{
		return S_OK;
	}

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			result = getTaskObject(i)->ConnectToObject(rInputName, newID);
			if (S_OK == result)
			{
				return S_OK;
			}
		}
	}

	return result;
}

HRESULT SVTaskObjectListClass::connectToObject(const SvPb::ConnectToObjectRequest& rConnectData)
{
	HRESULT result = __super::connectToObject(rConnectData);
	if (S_OK == result)
	{
		return S_OK;
	}

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			result = getTaskObject(i)->connectToObject(rConnectData);
			if (S_OK == result)
			{
				return S_OK;
			}
		}
	}

	return result;
}

void SVTaskObjectListClass::getInputData(const SvPb::GetInputDataRequest& request, SvPb::InspectionCmdResponse& rCmdResponse) const
{
	__super::getInputData(request, rCmdResponse);
	if (rCmdResponse.has_getinputdataresponse())
	{
		return;
	}

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			getTaskObject(i)->getInputData(request, rCmdResponse);
			if (rCmdResponse.has_getinputdataresponse())
			{
				return;
			}
		}
	}
}

void SVTaskObjectListClass::Persist(SvOi::IObjectWriter& rWriter, bool closeObject/* = true*/) const
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name
	SVTaskObjectClass::Persist(rWriter);

	// Get script of task list members...
	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); ++ i)
	{
		if (getTaskObject(i))
		{
			SVTaskObjectClass* pTaskObject = getTaskObject(i);
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
	if (closeObject)
	{
		rWriter.EndElement();
	}
}

bool SVTaskObjectListClass::CloseObject()
{
	bool Result( true );

	// Close our children
	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		SVTaskObjectClass* pTaskObject = getTaskObject(i);
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
			for (int i = 0; nullptr == rpObject && i < static_cast<int> (numberOfTaskObjects()); i++)
			{
				SVTaskObjectClass* l_pTask = getTaskObject(i);
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

const double& SVTaskObjectListClass::GetLastListUpdateTimestamp() const
{
	return m_LastListUpdateTimestamp;
}

int  SVTaskObjectListClass::InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject)
{
	int Result(-1);
	if (nullptr == pTaskObject)
	{
		Log_Assert(false);
		return -1;
	}

	auto NewName = makeNameUnique(pTaskObject->GetName(), {}, false);
	if (NewName != pTaskObject->GetName())
	{
		pTaskObject->SetName(NewName.c_str());
	}

	pTaskObject->SetObjectOwner(this);
	if (SvDef::InvalidObjectId == objectBeforeID)
	{
		m_TaskObjectVector.push_back(pTaskObject);
		Result =  static_cast<int>( numberOfTaskObjects()) -1;
	}
	else
	{
		auto it = std::find_if(m_TaskObjectVector.begin(), m_TaskObjectVector.end(),
			[&objectBeforeID](SVTaskObjectClass* pObject)
		{
			return (pObject == nullptr || pObject->getObjectId() == objectBeforeID);
		});

		auto it2 = m_TaskObjectVector.insert(it, pTaskObject);
		Result =  static_cast<int>(std::distance(m_TaskObjectVector.begin(), it2));
	}
	m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
	return Result;
}

void SVTaskObjectListClass::RemoveAt(int Index)
{
	m_TaskObjectVector.erase(m_TaskObjectVector.begin() + Index);

	m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
}

int SVTaskObjectListClass::Add(SVTaskObjectClass* pTaskObject, bool atBegin, bool useExplorerStyle)
{
	if (nullptr == pTaskObject)
	{
		return -1;
	}

	// Check for unique names 
	auto NewName = makeNameUnique(pTaskObject->GetName(), {},  useExplorerStyle);
	if( NewName != pTaskObject->GetName() )
	{
		pTaskObject->SetName( NewName.c_str() );
	}

	pTaskObject->SetObjectOwner(this);

	m_LastListUpdateTimestamp = SvUl::GetTimeStamp();

	if (!atBegin)
	{
		m_TaskObjectVector.push_back(pTaskObject);
		return static_cast<int> (numberOfTaskObjects() - 1);
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

// Override this function to implement special object behavior...
void SVTaskObjectListClass::SetDisabled()
{
	// Set this object and all own embedded objects to disabled...
	// Set also all friends to disabled...
	SVTaskObjectClass::SetDisabled();

	// Set all children to disabled also
	for (int j = 0; j < static_cast<int> (numberOfTaskObjects()); ++ j)
	{
		SVTaskObjectClass* pObject = getTaskObject(j);
		if (pObject)
		{
			pObject->SetDisabled();
		}
	}
}

bool SVTaskObjectListClass::IsNameUnique(const std::string& rName, const std::string& rExclude) const
{
	return false == SvUl::isStringInList(rName, getNamesFromTaskObjectPtrVector(m_TaskObjectVector), rExclude);
}


std::string SVTaskObjectListClass::makeNameUnique(const std::string& rOriginalName, const std::vector<std::string>& rAdditionalNames, bool useExplorerStyle) const
{
	auto taskObjectNames = getNamesFromTaskObjectPtrVector(m_TaskObjectVector);

	for (const auto& rName : rAdditionalNames)
	{
		taskObjectNames.push_back(rName);
	}

	return SvUl::makeNameUnique(rOriginalName, taskObjectNames, useExplorerStyle);
}


void SVTaskObjectListClass::setEditModeFreezeFlag(bool flag)
{
	__super::setEditModeFreezeFlag(flag);
	for (auto* pTasks : m_TaskObjectVector)
	{
		if (nullptr != pTasks)
		{
			pTasks->setEditModeFreezeFlag(flag);
		}
	}
}

void SVTaskObjectListClass::copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr)
{
	__super::copiedSavedImage(pTr);
	for (auto* pTasks : m_TaskObjectVector)
	{
		if (nullptr != pTasks)
		{
			pTasks->copiedSavedImage(pTr);
		}
	}
}

void SVTaskObjectListClass::goingOffline()
{
	__super::goingOffline();
	for (auto* pTasks : m_TaskObjectVector)
	{
		if (nullptr != pTasks)
		{
			pTasks->goingOffline();
		}
	}
}

void SVTaskObjectListClass::collectOverlays(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const
{
	__super::collectOverlays(rMultiLineArray, rImageExtents);

	for (auto* pObject : m_TaskObjectVector)
	{
		if (nullptr != pObject)
		{
			pObject->collectOverlays(rMultiLineArray, rImageExtents);
		}
	}
}

void SVTaskObjectListClass::collectOverlays(SvPb::Overlay& rOverlay) const
{
	__super::collectOverlays(rOverlay);

	for (auto* pObject : m_TaskObjectVector)
	{
		if (nullptr != pObject)
		{
			pObject->collectOverlays(rOverlay);
		}
	}
}

bool SVTaskObjectListClass::DestroyChildObject( SVTaskObjectClass* pTaskObject, DWORD context )
{
	// Kill the Object
	if (nullptr != pTaskObject)
	{
		// Get the object's uniqueID
		uint32_t objectID = pTaskObject->getObjectId();
		if (RemoveFromTaskObjectVector(objectID) || RemoveFriend(objectID))
		{
			// Notify the Owner of our inputs that they are not needed anymore
			pTaskObject->disconnectAllInputs();

			// Close the Object
			pTaskObject->CloseObject();

			// Destroy our Friends
			pTaskObject->DestroyFriends();

			// delete object.
			delete pTaskObject;

			SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
			if (nullptr != pInspection)
			{
				pInspection->setResultListUpdateFlag();
				if (SvDef::SVMFSetDefaultInputs == (context & SvDef::SVMFSetDefaultInputs))
				{
					pInspection->SetDefaultInputs();
				}

				if (SvDef::SVMFResetInspection == (context & SvDef::SVMFResetInspection))
				{
					GetInspection()->resetAllObjects();
				}
			}
			return true;
		}
	}
	return false;
}

int SVTaskObjectListClass::GetSize() const
{
	return static_cast< int >( static_cast<int> (numberOfTaskObjects()) );
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
			pInfo->set_isvalid(pTaskObj->isErrorMessageEmpty());
			pInfo->set_objectsubtype(pTaskObj->GetObjectSubType());
			pInfo->set_objecttype(pTaskObj->GetObjectType());
			pInfo->set_taskobjectid(pTaskObj->getObjectId());
			pInfo->set_canhaschildren(pTaskObj->canHasChildren());
		}
	}
}

void SVTaskObjectListClass::getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse& rRequest) const
{
	for (auto* pTaskObj : m_TaskObjectVector)
	{
		pTaskObj->getToolsWithReplaceableSourceImage(rRequest);
	}
}

int SVTaskObjectListClass::getToolDepth(bool goUpwards) const
{
	int depth = 0;
	if (goUpwards)
	{
		const SVObjectClass* pObject = this;
		while (nullptr != pObject && SvPb::SVToolSetObjectType != pObject->GetObjectType())
		{
			if (SvPb::SVToolObjectType == pObject->GetObjectType())
			{
				++depth;
			}
			pObject = pObject->GetParent();
		}
	}
	else
	{
		if (SvPb::SVToolObjectType == GetObjectType())
		{
			for (auto* pObject : m_TaskObjectVector)
			{
				if (auto* pTaskList = dynamic_cast<SVTaskObjectListClass*>(pObject); nullptr != pTaskList)
				{
					depth = std::max(depth, pTaskList->getToolDepth(false));
				}
			}
		}
	}
	return depth;
}

void SVTaskObjectListClass::Delete(IObjectClass* pTaskObject)
{
	if (nullptr == pTaskObject)
	{
		Log_Assert(false);
			return;
		}
		
	auto objectID = pTaskObject->getObjectId();
	bool isOk = RemoveFromTaskObjectVector(objectID) || RemoveFriend(objectID);
	Log_Assert(isOk);
	delete(pTaskObject);
}

void SVTaskObjectListClass::InsertBefore(uint32_t objectBeforeID, ITaskObject& rObject)
{
	auto* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	InsertBefore(objectBeforeID, pObject);
}

void SVTaskObjectListClass::InsertAt(int pos, ITaskObject& rObject)
{
	if (0 <= pos && numberOfTaskObjects() > pos)
	{
		InsertBefore(getTaskObject(pos)->getObjectId(), rObject);
	}
	else
	{
		InsertBefore(SvDef::InvalidObjectId, rObject);
	}
}

bool SVTaskObjectListClass::DestroyChild(SvOi::ITaskObject& rObject, DWORD context)
{
	auto* pTaskObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	return DestroyChildObject(pTaskObject, context);
}

SvUl::NameClassIdList SVTaskObjectListClass::GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& pObjectTypeInfo) const
{
	SvUl::NameClassIdList list = SVTaskObjectClass::GetCreatableObjects(pObjectTypeInfo);

	for (int i = 0; i < static_cast<int> (m_availableChildren.size()); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren[i];
		if (classInfo.m_ObjectTypeInfo.m_ObjectType == pObjectTypeInfo.m_ObjectType &&
			(pObjectTypeInfo.m_SubType == SvPb::SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.m_SubType == pObjectTypeInfo.m_SubType) 
			)
		{
			list.push_back(std::make_pair(classInfo.m_ClassName, classInfo.m_ClassId));
		}
	}
	return list;
}

void SVTaskObjectListClass::moveTaskObject(uint32_t objectToMoveId, uint32_t preObjectId)
{
	int currentPos = -1;
	int newPos = -1;
	uint32_t currentId = SvDef::InvalidObjectId;
	for (int i = 0; i < numberOfTaskObjects(); i++)
	{
		if (nullptr != getTaskObject(i))
		{
			currentId = getTaskObject(i)->getObjectId();
		}
		else
		{
			continue;
		}

		if (currentId == objectToMoveId)
		{
			currentPos = i;
		}
		if (currentId == preObjectId)
		{
			newPos = i;
		}
	}

	if (0 <= currentPos && numberOfTaskObjects() > currentPos)
	{
		auto pObject = getTaskObject(currentPos);

		if (0 <= newPos && numberOfTaskObjects() > newPos)
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
	m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
}
#pragma endregion virtual method (ITaskObjectListClass)

bool SVTaskObjectListClass::createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool Result = __super::createAllObjects( rCreateStructure );

	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool	= GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			Result &= getTaskObject(i)->createAllObjects(createStruct);
		}
	}
	return Result;
}

void SVTaskObjectListClass::refreshAllObjects(const SVObjectLevelCreateStruct& rCreateStructure)
{
	__super::refreshAllObjects(rCreateStructure);

	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			getTaskObject(i)->refreshAllObjects(createStruct);
		}
	}
}

void SVTaskObjectListClass::ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	__super::ConnectObject( rCreateStructure );

	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = GetAnalyzer();

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			getTaskObject(i)->ConnectObject(createStruct);
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

	for (int i = 0; nullptr == retObject && i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			retObject = getTaskObject(i)->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
	}
	return retObject;
}

void SVTaskObjectListClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	SVTaskObjectClass::OnObjectRenamed(rRenamedObject, rOldName);

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			getTaskObject(i)->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

bool SVTaskObjectListClass::connectAllInputs()
{
	bool Result = SVTaskObjectClass::connectAllInputs();

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			Result &= getTaskObject(i)->connectAllInputs();
		}
	}
	return Result;
}

void SVTaskObjectListClass::disconnectAllInputs()
{
	__super::disconnectAllInputs();

	for (auto* pTask : m_TaskObjectVector)
	{
		if (nullptr != pTask)
		{
			pTask->disconnectAllInputs();
		}
	}
}

void SVTaskObjectListClass::getInputs(std::back_insert_iterator<std::vector<SvOl::InputObject*>> inserter) const
{
	SVTaskObjectClass::getInputs(inserter);

	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			getTaskObject(i)->getInputs(inserter);
		}
	}
}


bool SVTaskObjectListClass::replaceObject(SVObjectClass* pObject, uint32_t newId)
{
	auto* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject);
	if (nullptr != pTaskObject)
	{
		// NOTE:	Only dynamically generated objects could be replaced, 
		//			embedded objects must be overwritten using OverwriteEmdeddedObject!!!

		// Kill the Friends
		pTaskObject->DestroyFriends();

		// - Remove All Dynamic Children (they will be constructed anew)
		if (auto* pTaskObjectList = dynamic_cast<SVTaskObjectListClass*>(pTaskObject))
		{
			// Kill all the Dynamic Children
			pTaskObjectList->DeleteAll();
		}

		// Check task list members...
		for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
		{
			if (getTaskObject(i) && getTaskObject(i)->getObjectId() == newId)
			{
				// Delete this list member... the destructor will send SVM_OBJECT_CLOSED!
				SVObjectClass* pDuplicateObject = getTaskObject(i);
				delete pDuplicateObject;

				m_TaskObjectVector[i] = pTaskObject;

				m_LastListUpdateTimestamp = SvUl::GetTimeStamp();

				// Set unique object ID...
				if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, newId))
				{
					connectChildObject(*pTaskObject);
					return true;
				}

				return false;
			}
		}

		// Special code for Duplicates!!
		SVObjectClass* pDuplicatedObject = SVObjectManagerClass::Instance().GetObject(newId);
		if (pDuplicatedObject)
		{
			// Get the Owner
			SVObjectClass* pOwner = pDuplicatedObject->GetParent();
			if (pOwner)
			{
				auto* pTaskListOwner = dynamic_cast<SVTaskObjectListClass*>(pOwner);
				auto* pTaskList = dynamic_cast<SVTaskObjectListClass*>(pDuplicatedObject);
				if (nullptr == pTaskListOwner || !pTaskListOwner->DestroyChildObject(pTaskList))
				{
					// must be a Friend
					pOwner->DestroyFriends();
				}
			}
		}

		// If this object does not already exist: add it to the task list...
		Add(pTaskObject, false, true);
		m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
		//@Todo[nec] should be propagated to the owner?  

		// Special code for objects that allocate friends on SetOwner()
		pTaskObject->DestroyFriends();

		if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pTaskObject, newId))
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
		Log_Assert( false );
		Index = 0;
	}
	
	if (Index >= static_cast<int> (numberOfTaskObjects()))
	{
		Log_Assert( false );
		return;
	}
	
	if (Index + Count > static_cast<int> (numberOfTaskObjects()))
	{
		Log_Assert( false );
		Count = static_cast<int> (numberOfTaskObjects()) - Index; 
	}
	
	for (int i = Index + Count - 1; i >= Index; -- i)
	{
		SVTaskObjectClass* pTaskObject = getTaskObject(i);
		if (pTaskObject)
		{
			DestroyChildObject( pTaskObject );
		}
	}
}

// Calls DeleteAt( 0, GetSize() )...
void SVTaskObjectListClass::DeleteAll()
{
	DeleteAt(0, static_cast<int> (numberOfTaskObjects()));

	m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
}

SVTaskObjectClass* SVTaskObjectListClass::UpdateObject(uint32_t friendId, SVObjectClass* p_psvObject, SVObjectClass* p_psvNewOwner )
{
	SVTaskObjectClass* pObject = nullptr;

	int l_iSize = static_cast<int> (numberOfTaskObjects());

	// find the friend in our taskObject List
	for (int i = 0; nullptr == pObject && i < l_iSize; i++)
	{
		pObject = getTaskObject(i);

		if ( nullptr != pObject && friendId == pObject->getObjectId() )
		{
			m_TaskObjectVector.erase(m_TaskObjectVector.begin() + i);

			m_LastListUpdateTimestamp = SvUl::GetTimeStamp();
		}
		else
		{
			pObject = nullptr;
		}
	}

	if ( nullptr != p_psvObject )
	{
		p_psvObject->SetObjectOwner( p_psvNewOwner );
	}

	return pObject;
}

bool SVTaskObjectListClass::getAvailableObjects(SVClassInfoStructVector* pList, const SvDef::SVObjectTypeInfoStruct* pObjectTypeInfo) const
{
	bool rc = false;

	for (int i = 0; i < static_cast<int> (m_availableChildren.size()); i++)
	{
		SVClassInfoStruct classInfo = m_availableChildren[i];
		if (classInfo.m_ObjectTypeInfo.m_ObjectType == pObjectTypeInfo->m_ObjectType &&
			(pObjectTypeInfo->m_SubType == SvPb::SVNotSetSubObjectType ||
			classInfo.m_ObjectTypeInfo.m_SubType == pObjectTypeInfo->m_SubType) 
			)
		{
			pList->push_back(classInfo);
			rc = true;
		}
	}
	return rc;
}

bool SVTaskObjectListClass::isInputImage(uint32_t imageId) const
{
	bool Result = SVTaskObjectClass::isInputImage(imageId);

	if (SvDef::InvalidObjectId != imageId)
	{
		// Get Object from our children
		for (int i = 0; !Result && i < static_cast<int> (numberOfTaskObjects()); i++)
		{
			if (nullptr != getTaskObject(i))
			{
				Result = getTaskObject(i)->isInputImage(imageId);
			}
		}
	}

	return Result;
}

bool SVTaskObjectListClass::Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	clearRunErrorMessages();
	bool childUpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	if (!rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition())
	{
		RunStatus ChildRunStatus;
		ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
		ChildRunStatus.m_UpdateCounters = childUpdateCounters;
		// Run your children...
		for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
		{
			SVTaskObjectClass* pTaskObject = getTaskObject(i);
			if ( nullptr != pTaskObject )
			{
				ChildRunStatus.ResetRunStateAndToolSetTimes();

				bRetVal = pTaskObject->Run(ChildRunStatus, &m_RunErrorMessages) & bRetVal;
				rRunStatus.updateState(ChildRunStatus);
			}
		}
		rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);
	}

	setStatus(rRunStatus);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

bool SVTaskObjectListClass::resetAllObjects(SvStl::MessageContainerVector* pErrorMessages/*=nullptr */, int  /*nResetDepth = 0*/)
{
	bool Result = __super::resetAllObjects(pErrorMessages);

	SvStl::MessageContainerVector taskErrorMessages;
	// Notify all...
	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		if (getTaskObject(i))
		{
			Result = (getTaskObject(i)->resetAllObjects(&taskErrorMessages) && Result);
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

	return Result;
}

void SVTaskObjectListClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct(*this);
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

	// look at outputs first
	std::vector<SvOi::IObjectClass*> outputList;
	getOutputList(std::back_inserter(outputList));
	for (auto* pIObject : outputList)
	{
		auto* pObject = dynamic_cast<SVObjectClass*> (pIObject);
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
	// look at children
	// Try to send message to list members, if not already processed...
	for (int i = 0; nullptr == retObject && i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		SVObjectClass* pObject = getTaskObject(i);
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
bool SVTaskObjectListClass::RemoveFriend(uint32_t friendID)
{
	// Check ID...
	if (SvDef::InvalidObjectId != friendID)
	{
		// Check if friend is applied...
		if (m_friendList.size())
		{
			for (int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; --i)
			{
				if (nullptr != m_friendList[i] && m_friendList[i]->getObjectId() == friendID)
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

bool SVTaskObjectListClass::RemoveFromTaskObjectVector(uint32_t objectID)
{
	for (int i = 0; i < static_cast<int> (numberOfTaskObjects()); i++)
	{
		SVTaskObjectClass* pTaskObject = getTaskObject(i);

		if (pTaskObject && pTaskObject->getObjectId() == objectID)
		{
			m_LastListUpdateTimestamp = SvUl::GetTimeStamp();

			m_TaskObjectVector.erase(m_TaskObjectVector.begin() + i);
			// Delete object not till it is removed from list!!!
			return true;
		}
	}
	return false;
}

#pragma endregion Private Methods
} //namespace SvIe
