//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManager
//* .File Name       : $Workfile:   SVObjectManagerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   13 Nov 2014 10:01:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectManagerClass.h"
#include "DependencyManager.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVClassRegisterListClass.h"
#include "TextDefinesSvOl.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectManagerClass& SVObjectManagerClass::Instance()
{
	static SVObjectManagerClass l_Object;

	return l_Object;
}

SVObjectManagerClass::SVObjectManagerClass()
	: m_State(ReadWrite)
	, m_ShortPPQIndicator(0)
	, m_ProductIndicator(0)
	, m_PendingImageIndicator(0)
	, m_InspectionIndicator(0)
	, m_LastFrameRate(10)
	, m_FileSequenceNumber(0)
{
	m_TranslationMap[SvDef::FqnInspections] = SvDef::FqnConfiguration;
	m_TranslationMap[SvDef::FqnPPQs] = SvDef::FqnConfiguration;
	m_TranslationMap[SvDef::FqnCameras] = SvDef::FqnConfiguration;
	std::string ReplaceName;
	ReplaceName = SvDef::FqnConfiguration;
	ReplaceName += _T(".");
	ReplaceName += SvDef::FqnRemoteInputs;
	m_TranslationMap[SvDef::FqnRemoteInputs] = ReplaceName;
}

SVObjectManagerClass::~SVObjectManagerClass()
{
	Shutdown();
}

SVObjectManagerClass::SVObjectManagerStateEnum SVObjectManagerClass::GetState() const
{
	return static_cast<SVObjectManagerStateEnum>(m_State);
}

HRESULT SVObjectManagerClass::SetState(SVObjectManagerStateEnum State)
{
	::InterlockedExchange(&m_State, State);
	::Sleep(100);

	if (ReadOnly == m_State)
	{
		::InterlockedIncrement(&m_FileSequenceNumber);
	}

	return S_OK;
}

uint32_t SVObjectManagerClass::GetChildRootObjectID(const std::string& rRootName) const
{
	auto iter = m_RootNameChildren.find(rRootName);
	if (m_RootNameChildren.end() != iter)
	{
		return iter->second;
	}

	//If the root node is not found then return the configuration for backward compatibility
	iter = m_RootNameChildren.find(SvDef::FqnConfiguration);
	if (m_RootNameChildren.end() != iter)
	{
		return iter->second;
	}
	return SvDef::InvalidObjectId;
}

HRESULT SVObjectManagerClass::ConstructRootObject(SvPb::ClassIdEnum classID)
{
	if (m_RootNameChildren.end() != m_RootNameChildren.find(SvDef::FqnRoot) && SvDef::InvalidObjectId != m_RootNameChildren[SvDef::FqnRoot])
	{
		DestroyRootObject();
	}

	SVObjectClass* pRootObject;
	HRESULT Status = ConstructObject(classID, pRootObject);
	if (S_OK == Status)
	{
		if (nullptr != pRootObject)
		{
			m_RootNameChildren[SvDef::FqnRoot] = pRootObject->getObjectId();
		}
	}

	return Status;
}

HRESULT SVObjectManagerClass::DestroyRootObject()
{
	HRESULT Result = S_OK;

	uint32_t RootID = GetChildRootObjectID(SvDef::FqnRoot);
	if (SvDef::InvalidObjectId != RootID)
	{
		SVObjectClass* pObject = GetObject(RootID);

		if (nullptr != pObject)
		{
			delete pObject;
		}
	}

	return Result;
}

void SVObjectManagerClass::setRootChildID(const SVObjectClass* pObject)
{
	
	if (nullptr != pObject)
	{
		m_RootNameChildren[pObject->GetName()] = pObject->getObjectId();
	}
}

void SVObjectManagerClass::TranslateDottedName(std::string& rName) const
{
	const auto& iter = std::find_if(m_TranslationMap.begin(), m_TranslationMap.end(), 
		[&rName](auto& rEntry) {return 0 == rName.find(rEntry.first); });
	if (m_TranslationMap.end() != iter)
	{
		rName.replace(0, iter->first.size(), iter->second.c_str());
	}
}

void SVObjectManagerClass::Shutdown()
{
	//m_State = ReadWrite;
	::InterlockedExchange(&m_State, ReadWrite);
	::Sleep(100);

	std::lock_guard<std::recursive_mutex> lock(m_Mutex);

	if (!(m_UniqueObjectEntries.empty()))
	{
		m_UniqueObjectEntries.clear();
	}
}

HRESULT SVObjectManagerClass::connectDependency(uint32_t source, uint32_t destination, SvOl::JoinType Type)
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);

	HRESULT Result = SvOl::DependencyManager::Instance().Connect(source, destination, Type);

	return Result;
}

HRESULT SVObjectManagerClass::disconnectDependency(uint32_t source, uint32_t destination, SvOl::JoinType Type)
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);

	HRESULT Result = SvOl::DependencyManager::Instance().Disconnect(source, destination, Type);

	return Result;
}

HRESULT SVObjectManagerClass::ConstructObject(SvPb::ClassIdEnum classID, uint32_t& rObjectID)
{
	SVObjectClass* l_pObject = nullptr;

	HRESULT Result = ConstructObject(classID, l_pObject);

	if (nullptr != l_pObject)
	{
		rObjectID = l_pObject->getObjectId();
	}
	else
	{
		rObjectID = SvDef::InvalidObjectId;
		if (S_OK == Result)
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::ConstructObject(SvPb::ClassIdEnum classID, SVObjectClass*& rpObject)
{
	HRESULT Result = S_OK;

	if (SvPb::NoObjectClassId != classID)
	{
		rpObject = SVClassRegisterListClass::Instance().ConstructNewObject(classID);
	}
	else
	{
		rpObject = nullptr;
	}

	if (nullptr == rpObject)
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObjectByIdentifier(uint32_t objectID, SVObjectClass*& rpObject) const
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	bool Status = (SvDef::InvalidObjectId != objectID);

	if (Status && ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
	}

	if (Status)
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(objectID);

		if (nullptr != pUniqueObject)
		{
			rpObject = pUniqueObject->m_pObject;
		}
		else
		{
			rpObject = nullptr;
		}
	}
	else
	{
		rpObject = nullptr;
	}

	if (S_OK == Result && nullptr == rpObject)
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName(const std::string& rFullName, uint32_t& rObjectID) const
{
	SVObjectReference ObjectRef;
	HRESULT Status = GetObjectByDottedName(rFullName, ObjectRef);

	if (nullptr != ObjectRef.getObject())
	{
		rObjectID = ObjectRef.getObject()->getObjectId();
	}
	else
	{
		if (S_OK == Status)
		{
			Status = E_FAIL;
		}

		rObjectID = SvDef::InvalidObjectId;
	}

	return Status;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName(const std::string& rFullName, SVObjectClass*& rpObject) const
{
	SVObjectReference ObjectRef;
	HRESULT Result = GetObjectByDottedName(rFullName, ObjectRef);

	if (nullptr != ObjectRef.getObject())
	{
		rpObject = ObjectRef.getObject();
	}
	else
	{
		rpObject = nullptr;
	}

	if (S_OK == Result && nullptr == rpObject)
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObjectByDottedName(const std::string& rFullName, SVObjectReference& rObjectRef) const
{
	HRESULT Result = S_OK;

	if (0 == rFullName.find(SvOl::ToolSetName))
	{
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_96_DOTTED_NAME_NOT_UNIQUE, rFullName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25049_DottedName);
		assert(false);
		Result = E_FAIL;
		rObjectRef = SVObjectReference();
		return Result;
	}

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);
	bool Status = !(rFullName.empty());

	if (Status && ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
	}

	if (Status)
	{
		std::string Name = rFullName;
		TranslateDottedName(Name);
		SVObjectClass* pChildRootObject(nullptr);
		SVObjectNameInfo NameInfo;

		NameInfo.ParseObjectName(Name);

		uint32_t ChildRootID = GetChildRootObjectID(NameInfo.m_NameArray[0]);
		uint32_t ConfigID = GetChildRootObjectID(SvDef::FqnConfiguration);
		Result = GetObjectByIdentifier(ChildRootID, pChildRootObject);

		if (nullptr != pChildRootObject)
		{
			//Check if it is configuration object as it needs to be stripped of the FqnConfiguration name
			if (ChildRootID == ConfigID)
			{
				if (0 == Name.find(pChildRootObject->GetName()))
				{
					NameInfo.RemoveTopName();
				}
			}
			SVObjectClass* pObject(nullptr);
			//Only child root object
			if (1 == NameInfo.m_NameArray.size() && m_RootNameChildren.end() != m_RootNameChildren.find(NameInfo.m_NameArray[0]))
			{
				pObject = pChildRootObject;
			}
			else
			{
				SVObjectClass* pParent(nullptr);
				if (ChildRootID == ConfigID)
				{
					pParent = pChildRootObject;
				}
				else
				{
					uint32_t RootObjetctID = GetChildRootObjectID(SvDef::FqnRoot);
					Result = GetObjectByIdentifier(RootObjetctID, pParent);
				}
				if (nullptr != pParent)
				{
					Result = pParent->GetChildObject(pObject, NameInfo);
				}
			}

			if (nullptr != pObject)
			{
				rObjectRef = SVObjectReference(pObject, NameInfo);
			}
			else
			{
				rObjectRef = SVObjectReference();
			}
		}
		else
		{
			rObjectRef = SVObjectReference();
		}
	}
	else
	{
		rObjectRef = SVObjectReference();
	}

	if (S_OK == Result && nullptr == rObjectRef.getObject())
	{
		Result = E_FAIL;
	}

	return Result;
}

bool SVObjectManagerClass::CreateObjectID(SVObjectClass* pObject)
{
	bool Result = (ReadWrite == m_State);

	if (Result)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (Autolock.owns_lock() == false)
		{
			Autolock.lock();
		}
		Result = Result && nullptr != pObject;

		if (Result)
		{
			SVUniqueObjectEntryStructPtr pUniqueObject {new SVUniqueObjectEntryStruct};

			Result = (nullptr != pUniqueObject);

			if (Result)
			{
				pUniqueObject->m_ObjectID = getNextObjectId();

				pUniqueObject->m_pObject = pObject;
				pObject->m_outObjectInfo.GetObjectReference().setObjectId(pUniqueObject->m_ObjectID);
				m_UniqueObjectEntries[pUniqueObject->m_ObjectID] = pUniqueObject;

				SvOl::DependencyManager::Instance().Add(pUniqueObject->m_ObjectID);
				uint32_t OwnerId = pObject->GetParentID();
				if (SvDef::InvalidObjectId != OwnerId && OwnerId != pUniqueObject->m_ObjectID)
				{
					SvOl::DependencyManager::Instance().Connect(OwnerId, pUniqueObject->m_ObjectID, SvOl::JoinType::Owner);
				}
			}
		}
	}

	return Result;
}

uint32_t SVObjectManagerClass::getNextObjectId() 
{
	auto iter = m_deletedObjectIdSet.begin();
	if (m_deletedObjectIdSet.end() != iter)
	{
		auto retValue = *iter;
		m_deletedObjectIdSet.erase(iter);
		return retValue;
	}
	return m_nextObjectId++; 
}

void SVObjectManagerClass::fitNextObjectId(uint32_t usedObjectId)
{
	if (m_nextObjectId <= usedObjectId)
	{
		m_nextObjectId = usedObjectId;
	}
}

void SVObjectManagerClass::reduceNextObjectId(uint32_t removedObjectId)
{
	if (removedObjectId > m_firstObjectId)
	{
		if (m_nextObjectId - 1 == removedObjectId)
		{
			m_nextObjectId--;
			auto iter = m_deletedObjectIdSet.find(m_nextObjectId - 1);
			while (m_deletedObjectIdSet.end() != iter)
			{
				m_deletedObjectIdSet.erase(iter);
				m_nextObjectId--;
				iter = m_deletedObjectIdSet.find(m_nextObjectId - 1);
			}
		}
		else
		{
			m_deletedObjectIdSet.emplace(removedObjectId);
		}
	}
}

void SVObjectManagerClass::resetNextObjectId()
{
	m_deletedObjectIdSet.clear();
	auto maxIter = std::max_element(m_UniqueObjectEntries.begin(), m_UniqueObjectEntries.end());
	if (m_UniqueObjectEntries.end() != maxIter && m_firstObjectId < maxIter->first - 1)
	{
		m_nextObjectId = maxIter->first + 1;
		for (uint32_t i = maxIter->first - 1; i >= m_firstObjectId; --i)
		{
			auto findIter = m_UniqueObjectEntries.find(i);
			if (m_UniqueObjectEntries.end() == findIter)
			{
				m_deletedObjectIdSet.emplace(i);
			}
		}
	}
	else
	{
		m_nextObjectId = m_firstObjectId;
	}
	resetExchangeObjectIdMap();
}

bool SVObjectManagerClass::OpenUniqueObjectID(SVObjectClass* pObject)
{
	bool Result = (ReadWrite == m_State);

	if (Result)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);
		if (Autolock.owns_lock() == false)
		{
			Autolock.lock();
		}
		Result = Result && nullptr != pObject;

		if (Result)
		{
			SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(pObject->m_outObjectInfo.getObjectId());
			assert(nullptr == pUniqueObject);
			Result = (nullptr == pUniqueObject);

			if (Result)
			{
				pUniqueObject = SVUniqueObjectEntryStructPtr {new SVUniqueObjectEntryStruct};

				Result = (nullptr != pUniqueObject);

				if (Result)
				{
					pUniqueObject->m_pObject = pObject;
					pUniqueObject->m_ObjectID = pObject->m_outObjectInfo.getObjectId();
					m_UniqueObjectEntries[pUniqueObject->m_ObjectID] = pUniqueObject;

					SvOl::DependencyManager::Instance().Add(pUniqueObject->m_ObjectID);
					uint32_t OwnerId = pObject->GetParentID();
					if (SvDef::InvalidObjectId != OwnerId && OwnerId != pUniqueObject->m_ObjectID)
					{
						SvOl::DependencyManager::Instance().Connect(OwnerId, pUniqueObject->m_ObjectID, SvOl::JoinType::Owner);
					}
				}
			}
		}
	}

	return Result;
}

bool SVObjectManagerClass::CloseUniqueObjectID(SVObjectClass* pObject)
{
	bool Result = (ReadWrite == m_State);

	if (Result)
	{
		std::lock_guard<std::recursive_mutex>  AutoLock(m_Mutex);
		Result = Result && nullptr != pObject;

		if (Result)
		{
			uint32_t ObjectID(pObject->getObjectId());
			DetachSubjectsAndObservers(ObjectID);

			SvOl::DependencyManager::Instance().Remove(ObjectID);

			SVUniqueObjectEntryMap::iterator l_Iter(m_UniqueObjectEntries.find(ObjectID));

			if (l_Iter != m_UniqueObjectEntries.end())
			{
				m_UniqueObjectEntries.erase(l_Iter);
			}
		}
	}

	return Result;
}

bool SVObjectManagerClass::ChangeUniqueObjectID(SVObjectClass* pObject, uint32_t newId)
{
	if (SvDef::InvalidObjectId != newId && CloseUniqueObjectID(pObject))
	{
		pObject->m_outObjectInfo.GetObjectReference().setObjectId(newId);
		bool bRetVal = OpenUniqueObjectID(pObject);

		// Change ObjectID setting in private input interface...
		pObject->ResetPrivateInputInterface();

		return bRetVal;
	}
	return false;
}

SVObjectClass* SVObjectManagerClass::GetObject(uint32_t objectId) const
{
	SVObjectClass* pObject = nullptr;

	if(SvDef::InvalidObjectId == objectId)
	{
		return pObject;
	}

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(objectId);
	if (nullptr != pUniqueObject)
	{
		pObject = pUniqueObject->m_pObject;
	}

	return pObject;
}

SVObjectClass* SVObjectManagerClass::GetObjectCompleteName(LPCTSTR Name)
{
	SVObjectClass* pObject = nullptr;
	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	bool Status = (nullptr != Name);

	if (Status && ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
	}

	if (Status)
	{
		SVObjectClass* pConfig;
		GetRootChildObject(pConfig, SvDef::FqnConfiguration);

		if (nullptr != pConfig)
		{
			SVObjectNameInfo NameInfo;

			NameInfo.ParseObjectName(Name);

			/*Status = S_OK ==*/ pConfig->GetChildObject(pObject, NameInfo);
		}
	}

	return pObject;
}

SVObjectReference SVObjectManagerClass::GetObjectReference(LPCTSTR Name)
{
	SVObjectReference Result;

	SVObjectNameInfo NameInfo;

	if (S_OK == NameInfo.ParseObjectName(Name))
	{
		SVObjectClass* pObject = GetObjectCompleteName(NameInfo.GetObjectName().c_str());
		if (nullptr != pObject)
		{
			Result = SVObjectReference(pObject, NameInfo);
		}
	}

	return Result;
}

uint32_t SVObjectManagerClass::GetObjectIdFromCompleteName(LPCTSTR Name)
{
	uint32_t Result = SvDef::InvalidObjectId;

	SVObjectClass* pObject = GetObjectCompleteName(Name);

	if (nullptr != pObject)
	{
		Result = pObject->getObjectId();
	}

	return Result;
}
void SVObjectManagerClass::getObjectsOfType(SVObjectPtrVectorInserter Inserter, SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum ObjectSubType) const
{
	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
	}

	SVUniqueObjectEntryMap::const_iterator Iter(m_UniqueObjectEntries.begin());

	for (; m_UniqueObjectEntries.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = Iter->second->m_pObject;

		if (nullptr != pObject)
		{
			//Check only 
			if (SvPb::SVNotSetSubObjectType != ObjectSubType)
			{
				if (pObject->GetObjectType() == ObjectType && pObject->GetObjectSubType() == ObjectSubType)
				{
					Inserter = pObject;
				}
			}
			else
			{
				if (pObject->GetObjectType() == ObjectType)
				{
					Inserter = pObject;
				}
			}
		}
	}

}


std::string SVObjectManagerClass::GetCompleteObjectName(uint32_t objectId)
{
	std::string Result;

	SVObjectClass* pObject = GetObject(objectId);

	if (nullptr != pObject)
	{
		Result = pObject->GetCompleteName();
	}

	return Result;
}

uint32_t SVObjectManagerClass::getObserverSubject(const std::string& rSubjectDataName, uint32_t observerID) const
{
	return GetSubjectID(rSubjectDataName, getUniqueObjectEntry(observerID));
}

HRESULT SVObjectManagerClass::AttachObserver(const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(subjectID);
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry(observerID);

		if (nullptr != pSubjectObject && nullptr != pObserverObject)
		{
			uint32_t oldSubjectId = GetSubjectID(rSubjectDataName, pObserverObject);

			if (SvDef::InvalidObjectId != oldSubjectId)
			{
				DetachObserver(rSubjectDataName, oldSubjectId, observerID);
			}

			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName][observerID] = 1;
			pObserverObject->m_SubjectIDs[rSubjectDataName] = subjectID;
		}
		else
		{
			Result = E_FAIL;
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObserver(const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry(observerID);

		if (nullptr != pObserverObject)
		{
			pObserverObject->m_SubjectIDs.erase(rSubjectDataName);
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(subjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].erase(observerID);
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObservers(const std::string& rSubjectDataName, uint32_t subjectID)
{
	SVSubjectEnabledObserverMap Observers;

	HRESULT Result = GetObservers(rSubjectDataName, subjectID, Observers);

	if (S_OK == Result)
	{
		SVSubjectEnabledObserverMap::iterator Iter = Observers.begin();

		while (Iter != Observers.end())
		{
			HRESULT l_Temp = DetachObserver(rSubjectDataName, subjectID, Iter->first);

			if (S_OK == Result)
			{
				Result = l_Temp;
			}

			++Iter;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachSubjectsAndObservers(uint32_t objectID)
{
	HRESULT Result = DetachObservers(objectID);
	HRESULT SubjectStatus = DetachSubjects(objectID);

	if (S_OK == Result)
	{
		Result = SubjectStatus;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachSubjects(uint32_t observerID)
{
	SVSubjectDataNameDeque SubjectDataNames;

	HRESULT Result = GetObserverDataNames(observerID, SubjectDataNames);

	if (S_OK == Result)
	{
		SVSubjectDataNameDeque::iterator Iter;

		for (Iter = SubjectDataNames.begin(); SubjectDataNames.end() != Iter; ++Iter)
		{
			uint32_t subjectId = getObserverSubject(*Iter, observerID);
			DetachObserver(*Iter, subjectId, observerID);
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObservers(uint32_t subjectID)
{
	SVSubjectDataNameDeque SubjectDataNames;

	HRESULT Result = GetSubjectDataNames(subjectID, SubjectDataNames);

	if (S_OK == Result)
	{
		if (m_addToDeletedList)
		{
			reduceNextObjectId(subjectID);
		}
		SVSubjectDataNameDeque::iterator Iter;

		for (Iter = SubjectDataNames.begin(); SubjectDataNames.end() != Iter; ++Iter)
		{
			HRESULT Temp = DetachObservers(*Iter, subjectID);

			if (S_OK == Result)
			{
				Result = Temp;
			}
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DisconnectObjects(uint32_t objectID, uint32_t remoteID)
{
	HRESULT Result = S_OK;

	SVObjectClass* pObject = GetObject(objectID);

	if (nullptr != pObject)
	{
		Result = pObject->RemoveObjectConnection(remoteID);
	}

	pObject = GetObject(remoteID);

	if (nullptr != pObject)
	{
		HRESULT Temp = pObject->RemoveObjectConnection(objectID);

		if (S_OK == Result)
		{
			Result = Temp;
		}
	}

	return Result;
}

long SVObjectManagerClass::GetShortPPQIndicator() const
{
	return m_ShortPPQIndicator;
}


void SVObjectManagerClass::ClearAllIndicator()
{
	ClearShortPPQIndicator();
	ClearProductIndicator();
	ClearPendingImageIndicator();
	ClearInspectionIndicator();

}


void SVObjectManagerClass::ClearShortPPQIndicator()
{
	::InterlockedExchange(&m_ShortPPQIndicator, 0);
}

void SVObjectManagerClass::IncrementShortPPQIndicator()
{
	::InterlockedIncrement(&m_ShortPPQIndicator);
}

void SVObjectManagerClass::DecrementShortPPQIndicator()
{
	::InterlockedDecrement(&m_ShortPPQIndicator);
}

long SVObjectManagerClass::GetProductIndicator() const
{
	return m_ProductIndicator;
}

void SVObjectManagerClass::ClearProductIndicator()
{
	::InterlockedExchange(&m_ProductIndicator, 0);
}

void SVObjectManagerClass::IncrementProductIndicator()
{
	::InterlockedIncrement(&m_ProductIndicator);
}

void SVObjectManagerClass::DecrementProductIndicator()
{
	::InterlockedDecrement(&m_ProductIndicator);
}

void SVObjectManagerClass::AdjustProductIndicator(long Amount)
{
	::InterlockedExchangeAdd(&m_ProductIndicator, Amount);
}

long SVObjectManagerClass::GetPendingImageIndicator() const
{
	return m_PendingImageIndicator;
}

void SVObjectManagerClass::ClearPendingImageIndicator()
{
	::InterlockedExchange(&m_PendingImageIndicator, 0);
}

void SVObjectManagerClass::IncrementPendingImageIndicator()
{
	::InterlockedIncrement(&m_PendingImageIndicator);
}

void SVObjectManagerClass::DecrementPendingImageIndicator()
{
	::InterlockedDecrement(&m_PendingImageIndicator);
}

void SVObjectManagerClass::AdjustPendingImageIndicator(long Amount)
{
	::InterlockedExchangeAdd(&m_PendingImageIndicator, Amount);
}

long SVObjectManagerClass::GetInspectionIndicator() const
{
	return m_InspectionIndicator;
}

void SVObjectManagerClass::ClearInspectionIndicator()
{
	::InterlockedExchange(&m_InspectionIndicator, 0);
}

void SVObjectManagerClass::IncrementInspectionIndicator()
{
	::InterlockedIncrement(&m_InspectionIndicator);
}

void SVObjectManagerClass::DecrementInspectionIndicator()
{
	::InterlockedDecrement(&m_InspectionIndicator);
}

void SVObjectManagerClass::AdjustInspectionIndicator(long Amount)
{
	::InterlockedExchangeAdd(&m_InspectionIndicator, Amount);
}

long SVObjectManagerClass::GetNextFrameRate(long LastFrameRate)
{
	long FrameRate = m_LastFrameRate;

	::InterlockedExchange(&m_LastFrameRate, LastFrameRate);

	return FrameRate;
}

long SVObjectManagerClass::GetFileSequenceNumber() const
{
	return m_FileSequenceNumber;
}

HRESULT SVObjectManagerClass::getTreeList(const std::string& rPath, SVObjectReferenceVector& rObjectList, UINT AttributesAllowedFilter) const
{
	HRESULT Result = S_OK;
	std::set<uint32_t> objectIdSet;

	SVObjectClass* pStartObject(nullptr);
	GetObjectByDottedName(rPath, pStartObject);

	if (nullptr != pStartObject)
	{
		std::string InternalPath = pStartObject->GetCompleteName();
		if ((pStartObject->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter)
		{
			objectIdSet.insert(pStartObject->getObjectId());
		}

		SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}


		SVUniqueObjectEntryMap::const_iterator Iter(m_UniqueObjectEntries.begin());

		while (Iter != m_UniqueObjectEntries.end())
		{
			pUniqueObjectEntry = Iter->second;

			if (nullptr != pUniqueObjectEntry && nullptr != pUniqueObjectEntry->m_pObject)
			{
				if ((pUniqueObjectEntry->m_pObject->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter)
				{
					//Check if owner is in list
					std::string ObjectPath = pUniqueObjectEntry->m_pObject->GetCompleteName();
					std::string::size_type Pos = ObjectPath.find(InternalPath.c_str());
					if (std::string::npos != Pos)
					{
						if (ObjectPath.size() == Pos + InternalPath.size() || ObjectPath[Pos + InternalPath.size()] == '.')
						{
							objectIdSet.insert(pUniqueObjectEntry->m_pObject->getObjectId());
						}
					}
				}
			}
			++Iter;
		}

	}

	auto Iter(objectIdSet.begin());
	while (Iter != objectIdSet.end())
	{
		SVObjectReference ObjectRef(GetObject(*Iter));
		rObjectList.push_back(ObjectRef);
		++Iter;
	}

	return Result;
}

uint32_t SVObjectManagerClass::GetSubjectID(const std::string& rSubjectDataName, SVUniqueObjectEntryStructPtr pObjectEntry) const
{
	uint32_t Result = SvDef::InvalidObjectId;

	if (nullptr != pObjectEntry)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}

		SVSubjectDataNameSubjectIDMap::const_iterator l_Iter = pObjectEntry->m_SubjectIDs.find(rSubjectDataName);

		if (l_Iter != pObjectEntry->m_SubjectIDs.end())
		{
			Result = l_Iter->second;
		}

	}

	return Result;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntry(uint32_t objectId) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry(nullptr);

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



	if (ReadWrite == m_State && (Autolock.owns_lock() == false))
	{
		Autolock.lock();
	}


	SVUniqueObjectEntryMap::const_iterator Iter(m_UniqueObjectEntries.find(objectId));

	if (Iter != m_UniqueObjectEntries.end())
	{
		pUniqueObjectEntry = Iter->second;
	}


	return pUniqueObjectEntry;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntry(const std::string& rName) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry(nullptr);

	if (!rName.empty())
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);


		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}


		SVUniqueObjectEntryMap::const_iterator Iter(m_UniqueObjectEntries.begin());

		while (nullptr == pUniqueObjectEntry && Iter != m_UniqueObjectEntries.end())
		{
			pUniqueObjectEntry = Iter->second;

			if (nullptr != pUniqueObjectEntry)
			{
				if (nullptr == pUniqueObjectEntry->m_pObject || pUniqueObjectEntry->m_pObject->GetName() != rName)
				{
					pUniqueObjectEntry.reset();
				}
			}

			if (nullptr == pUniqueObjectEntry)
			{
				++Iter;
			}
		}

	}

	return pUniqueObjectEntry;
}

HRESULT SVObjectManagerClass::GetSubjectDataNames(uint32_t subjectID, SVSubjectDataNameDeque& rSubjectDataNames) const
{
	HRESULT Result = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(subjectID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}

		SVSubjectDataNameObserverMap::const_iterator Iter;

		for (Iter = pUniqueObject->m_DataNameSubjectObservers.begin(); pUniqueObject->m_DataNameSubjectObservers.end() != Iter; ++Iter)
		{
			rSubjectDataNames.push_back(Iter->first);
		}

	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObserverDataNames(uint32_t observerID, SVSubjectDataNameDeque& rSubjectDataNames) const
{
	HRESULT Result = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(observerID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}

		SVSubjectDataNameSubjectIDMap::const_iterator Iter;

		for (Iter = pUniqueObject->m_SubjectIDs.begin(); pUniqueObject->m_SubjectIDs.end() != Iter; ++Iter)
		{
			rSubjectDataNames.push_back(Iter->first);
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObservers(const std::string& rSubjectDataName, uint32_t subjectID, SVSubjectEnabledObserverMap& rObservers)
{
	HRESULT Result = S_OK;

	rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(subjectID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State && (Autolock.owns_lock() == false))
		{
			Autolock.lock();
		}

		SVSubjectDataNameObserverMap::iterator Iter = pUniqueObject->m_DataNameSubjectObservers.find(rSubjectDataName);

		if (Iter != pUniqueObject->m_DataNameSubjectObservers.end())
		{
			rObservers = Iter->second;
		}
		else
		{
			Result = E_FAIL;
		}

	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::RegisterSubObject(uint32_t subObjectID)
{
	HRESULT hr = S_FALSE;

	// get Tool and InspectionProcess
	SVObjectClass* pObject = GetObject(subObjectID);
	if (pObject)
	{
		SVObjectClass* pTool = pObject->GetAncestor(SvPb::SVToolObjectType);
		if (pTool)
		{
			pTool->RegisterSubObject(pObject);
			hr = S_OK;
		}
		SVObjectClass* pInspection = pObject->GetAncestor(SvPb::SVInspectionObjectType);
		if (pInspection)
		{
			pInspection->RegisterSubObject(pObject);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVObjectManagerClass::UnregisterSubObject(uint32_t subObjectID)
{
	HRESULT hr = S_FALSE;

	// get Tool and InspectionProcess
	SVObjectClass* pObject = GetObject(subObjectID);
	if (pObject)
	{
		SVObjectClass* pTool = pObject->GetAncestor(SvPb::SVToolObjectType);
		if (pTool)
		{
			pTool->UnregisterSubObject(pObject);
			hr = S_OK;
		}
		SVObjectClass* pInspection = pObject->GetAncestor(SvPb::SVInspectionObjectType);
		if (pInspection)
		{
			pInspection->UnregisterSubObject(pObject);
			hr = S_OK;
		}
	}
	return hr;
}

SvOi::IObjectClass* SVObjectManagerClass::getFirstObject(uint32_t sourceId, const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const
{
	SvOi::IObjectClass* pRetObject = nullptr;
	SvOi::IObjectClass* pSource = GetObject(sourceId);
	if (nullptr != pSource)
	{
		pRetObject = pSource->getFirstObject(rObjectTypeInfo);
	}
	return pRetObject;
}

bool SVObjectManagerClass::ConnectObjectInput(uint32_t sourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = false;
	SVObjectClass* pSource =GetObject(sourceId);
	if (nullptr != pSource)
	{
		Result = pSource->ConnectObjectInput(pObjectInInfo);
	}
	return Result;
}

bool SVObjectManagerClass::DisconnectObjectInput(uint32_t sourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = false;
	SVObjectClass* pSource = GetObject(sourceId);
	if (nullptr != pSource)
	{
		Result = pSource->DisconnectObjectInput(pObjectInInfo);
	}
	return Result;
}


#pragma region IObjectManager-function
SvOi::IObjectClass* SvOi::getObjectByDottedName(const std::string& rFullName)
{
	SVObjectClass* pObject(nullptr);
	//To have the function available without to know the class EvironmentObject
	SVObjectManagerClass::Instance().GetObjectByDottedName(rFullName, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::ConstructObject(SvPb::ClassIdEnum classID)
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().ConstructObject(classID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::getObject(uint32_t objectID)
{
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(objectID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::FindObject(uint32_t parentID, const SvDef::SVObjectTypeInfoStruct& rInfo)
{
	return SVObjectManagerClass::Instance().getFirstObject(parentID, rInfo);
}
#pragma endregion IObjectManager-function

