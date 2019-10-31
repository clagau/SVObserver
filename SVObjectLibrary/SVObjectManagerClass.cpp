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
#include "ObjectInterfaces/ISVImage.h"
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
	, m_ObserverCookie(1)
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

const SVGUID SVObjectManagerClass::GetChildRootObjectID(const std::string& rRootName) const
{
	SVGUID ObjectID;

	if (m_RootNameChildren.end() != m_RootNameChildren.find(rRootName))
	{
		ObjectID = m_RootNameChildren.at(rRootName);
	}
	//If the root node is not found then return the configuration for backward compatibility
	else if (m_RootNameChildren.end() != m_RootNameChildren.find(SvDef::FqnConfiguration))
	{
		ObjectID = m_RootNameChildren.at(SvDef::FqnConfiguration);
	}
	return ObjectID;
}

HRESULT SVObjectManagerClass::ConstructRootObject(const SVGUID& rClassID)
{
	if (m_RootNameChildren.end() != m_RootNameChildren.find(SvDef::FqnRoot) && !m_RootNameChildren[SvDef::FqnRoot].empty())
	{
		DestroyRootObject();
	}

	SVObjectClass* pRootObject;
	HRESULT Status = ConstructObject(rClassID, pRootObject);
	if (S_OK == Status)
	{
		if (nullptr != pRootObject)
		{
			m_RootNameChildren[SvDef::FqnRoot] = pRootObject->GetUniqueObjectID();
		}
	}

	return Status;
}

HRESULT SVObjectManagerClass::DestroyRootObject()
{
	HRESULT Result = S_OK;

	SVGUID RootID = GetChildRootObjectID(SvDef::FqnRoot);
	if (!RootID.empty())
	{
		SVObjectClass* pObject = GetObject(RootID);

		if (nullptr != pObject)
		{
			delete pObject;
		}
	}

	return Result;
}

void SVObjectManagerClass::setRootChildID(const std::string& rRootChild, const SVGUID& rUniqueID)
{
	SVObjectClass* pRootObject(nullptr);
	GetObjectByIdentifier(rUniqueID, pRootObject);
	if (nullptr != pRootObject)
	{
		m_RootNameChildren[rRootChild] = rUniqueID;
	}
}

void SVObjectManagerClass::TranslateDottedName(std::string& rName) const
{
	bool NameReplaced(false);
	SvDef::TranslateMap::const_iterator Iter(m_TranslationMap.begin());
	for (; m_TranslationMap.end() != Iter && !NameReplaced; ++Iter)
	{
		size_t Pos = rName.find(Iter->first);
		//Check only that the start of the dotted name is found
		if (0 == Pos)
		{
			rName.replace(Pos, Iter->first.size(), Iter->second.c_str());
			NameReplaced = true;
		}
	}
}

void SVObjectManagerClass::Shutdown()
{
	//m_State = ReadWrite;
	::InterlockedExchange(&m_State, ReadWrite);
	::Sleep(100);

	std::lock_guard<std::recursive_mutex> lock(m_Mutex);


	if (!(m_CookieEntries.empty()))
	{
		m_CookieEntries.clear();
	}

	if (!(m_UniqueObjectEntries.empty()))
	{
		m_UniqueObjectEntries.clear();
	}
}

HRESULT SVObjectManagerClass::connectDependency(const SVGUID& rSource, const SVGUID& rDestination, SvOl::JoinType Type)
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);

	HRESULT Result = SvOl::DependencyManager::Instance().Connect(rSource, rDestination, Type);

	return Result;
}

HRESULT SVObjectManagerClass::disconnectDependency(const SVGUID& rSource, const SVGUID& rDestination, SvOl::JoinType Type)
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);

	HRESULT Result = SvOl::DependencyManager::Instance().Disconnect(rSource, rDestination, Type);

	return Result;
}

HRESULT SVObjectManagerClass::ConstructObject(const SVGUID& rClassID, GUID& rObjectID)
{
	SVObjectClass* l_pObject = nullptr;

	HRESULT Result = ConstructObject(rClassID, l_pObject);

	if (nullptr != l_pObject)
	{
		rObjectID = l_pObject->GetUniqueObjectID();
	}
	else
	{
		rObjectID = GUID_NULL;

		if (S_OK == Result)
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::ConstructObject(const SVGUID& rClassID, SVObjectClass*& rpObject)
{
	HRESULT Result = S_OK;

	rpObject = SVClassRegisterListClass::Instance().ConstructNewObject(rClassID);

	if (nullptr == rpObject)
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObjectByIdentifier(const SVGUID& rObjectID, SVObjectClass*& rpObject) const
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	bool Status = !(rObjectID.empty());

	if (Status && ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (Status)
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rObjectID);

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

HRESULT SVObjectManagerClass::GetObjectByDottedName(const std::string& rFullName, GUID& rObjectID) const
{
	SVObjectReference ObjectRef;
	HRESULT Status = GetObjectByDottedName(rFullName, ObjectRef);

	if (nullptr != ObjectRef.getObject())
	{
		rObjectID = ObjectRef.getObject()->GetUniqueObjectID();
	}
	else
	{
		if (S_OK == Status)
		{
			Status = E_FAIL;
		}

		rObjectID = GUID_NULL;
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

	if (Status && ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (Status)
	{
		std::string Name = rFullName;
		Instance().TranslateDottedName(Name);
		SVObjectClass* pChildRootObject(nullptr);
		SVObjectNameInfo NameInfo;

		NameInfo.ParseObjectName(Name);

		SVGUID ChildRootID = GetChildRootObjectID(NameInfo.m_NameArray[0]);
		SVGUID ConfigID = GetChildRootObjectID(SvDef::FqnConfiguration);
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
					SVGUID RootObjetctID = GetChildRootObjectID(SvDef::FqnRoot);
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

bool SVObjectManagerClass::CreateUniqueObjectID(SVObjectClass* pObject)
{
	bool Result = (ReadWrite == m_State);

	if (Result)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		Autolock.lock();
		Result = Result && nullptr != pObject;

		if (Result)
		{
			SVUniqueObjectEntryStructPtr pUniqueObject {new SVUniqueObjectEntryStruct};

			Result = (nullptr != pUniqueObject);

			if (Result)
			{
				Result = (S_OK == CoCreateGuid(&(pUniqueObject->m_ObjectUID.ToGUID())));

				if (Result)
				{
					pUniqueObject->m_pObject = pObject;
					pObject->m_outObjectInfo.GetObjectReference().setGuid(pUniqueObject->m_ObjectUID);
					m_UniqueObjectEntries[pUniqueObject->m_ObjectUID] = pUniqueObject;

					SvOl::DependencyManager::Instance().Add(pUniqueObject->m_ObjectUID);
					SVGUID OwnerGuid = pObject->GetParentID();
					if (GUID_NULL != OwnerGuid && OwnerGuid != pUniqueObject->m_ObjectUID)
					{
						SvOl::DependencyManager::Instance().Connect(OwnerGuid, pUniqueObject->m_ObjectUID, SvOl::JoinType::Owner);
					}
				}
			}
		}
	}

	return Result;
}

bool SVObjectManagerClass::OpenUniqueObjectID(SVObjectClass* pObject)
{
	bool Result = (ReadWrite == m_State);

	if (Result)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);
		Autolock.lock();
		Result = Result && nullptr != pObject;

		if (Result)
		{
			SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(pObject->m_outObjectInfo.getUniqueObjectID());

			Result = (nullptr == pUniqueObject);

			if (Result)
			{
				pUniqueObject = SVUniqueObjectEntryStructPtr {new SVUniqueObjectEntryStruct};

				Result = (nullptr != pUniqueObject);

				if (Result)
				{
					pUniqueObject->m_pObject = pObject;
					pUniqueObject->m_ObjectUID = pObject->m_outObjectInfo.getUniqueObjectID();
					m_UniqueObjectEntries[pUniqueObject->m_ObjectUID] = pUniqueObject;

					SvOl::DependencyManager::Instance().Add(pUniqueObject->m_ObjectUID);
					SVGUID OwnerGuid = pObject->GetParentID();
					if (GUID_NULL != OwnerGuid && OwnerGuid != pUniqueObject->m_ObjectUID)
					{
						SvOl::DependencyManager::Instance().Connect(OwnerGuid, pUniqueObject->m_ObjectUID, SvOl::JoinType::Owner);
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
			SVGUID ObjectID(pObject->GetUniqueObjectID());
			DetachObservers(ObjectID);
			DetachSubjects(ObjectID);

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

bool SVObjectManagerClass::ChangeUniqueObjectID(SVObjectClass* pObject, const SVGUID& rNewGuid)
{
	if (GUID_NULL != rNewGuid && CloseUniqueObjectID(pObject))
	{
		pObject->m_outObjectInfo.GetObjectReference().setGuid(rNewGuid);
		bool bRetVal = OpenUniqueObjectID(pObject);

		// Change ObjectID setting in private input interface...
		pObject->ResetPrivateInputInterface();

		return bRetVal;
	}
	return false;
}

SVObjectClass* SVObjectManagerClass::GetObject(const SVGUID& rGuid) const
{
	SVObjectClass* pObject = nullptr;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	bool Result = (GUID_NULL != rGuid);


	if (Result && ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (Result)
	{
		SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rGuid);

		if (nullptr != pUniqueObject)
		{
			pObject = pUniqueObject->m_pObject;
		}
	}

	return pObject;
}

SVObjectClass* SVObjectManagerClass::GetObjectCompleteName(LPCTSTR Name)
{
	SVObjectClass* pObject = nullptr;
	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	bool Status = (nullptr != Name);

	if (Status && ReadWrite == m_State)
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

SVGUID SVObjectManagerClass::GetObjectIdFromCompleteName(LPCTSTR Name)
{
	SVGUID Result;

	SVObjectClass* pObject = GetObjectCompleteName(Name);

	if (nullptr != pObject)
	{
		Result = pObject->GetUniqueObjectID();
	}

	return Result;
}
void SVObjectManagerClass::getObjectsOfType(SVObjectPtrVectorInserter Inserter, SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum ObjectSubType) const
{
	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
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


std::string SVObjectManagerClass::GetCompleteObjectName(const SVGUID& rGuid)
{
	std::string Result;

	SVObjectClass* pObject = GetObject(rGuid);

	if (nullptr != pObject)
	{
		Result = pObject->GetCompleteName();
	}

	return Result;
}

HRESULT SVObjectManagerClass::InsertObserver(SVObserverNotificationFunctorPtr pFunctor, long& rCookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		rCookie = ::InterlockedExchangeAdd(&m_ObserverCookie, 1);

		SVCookieEntryStructPtr pCookieEntry(new SVCookieEntryStruct(rCookie, pFunctor));
		m_CookieEntries[rCookie] = pCookieEntry;
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::EraseObserver(long Cookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		DetachSubjects(Cookie);

		m_CookieEntries.erase(Cookie);
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObserverSubject(const std::string& rSubjectDataName, const SVGUID& rObserverID, GUID& rSubjectID) const
{
	HRESULT Result = S_OK;

	rSubjectID = GetSubjectID(rSubjectDataName, getUniqueObjectEntry(rObserverID));

	if (SVGUID(rSubjectID).empty())
	{
		Result = S_FALSE;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObserverSubject(const std::string& rSubjectDataName, long Cookie, GUID& rSubjectID) const
{
	HRESULT Result = S_OK;

	rSubjectID = GetSubjectID(rSubjectDataName, GetCookieEntry(Cookie));

	if (SVGUID(rSubjectID).empty())
	{
		Result = S_FALSE;
	}

	return Result;
}

HRESULT SVObjectManagerClass::GetObserverIds(const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVGuidSet& rObserverIds)
{
	rObserverIds.clear();

	SVSubjectEnabledObserverMap Observers;

	HRESULT Result = GetObservers(rSubjectDataName, rSubjectID, Observers);

	if (S_OK == Result)
	{
		SVSubjectEnabledObserverMap::iterator Iter;

		for (Iter = Observers.begin(); Observers.end() != Iter; ++Iter)
		{
			if (Iter->second == 1)
			{
				rObserverIds.insert(Iter->first);
			}
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::AttachObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry(rObserverID);

		if (nullptr != pSubjectObject && nullptr != pObserverObject)
		{
			SVGUID SubjectID = GetSubjectID(rSubjectDataName, pObserverObject);

			if (!SubjectID.empty())
			{
				DetachObserver(rSubjectDataName, SubjectID, rObserverID);
			}

			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectObservers[rObserverID] = 1;
			pObserverObject->m_SubjectIDs[rSubjectDataName] = rSubjectID;
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

HRESULT SVObjectManagerClass::AttachObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry(p_Cookie);

		if (nullptr != pSubjectObject && nullptr != pObserverObject)
		{
			SVGUID l_SubjectID = GetSubjectID(rSubjectDataName, pObserverObject);

			if (!l_SubjectID.empty() && l_SubjectID == rSubjectID)
			{
				DetachObserver(rSubjectDataName, l_SubjectID, p_Cookie);
			}

			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectCookies[p_Cookie] = 1;
			pObserverObject->m_SubjectIDs[rSubjectDataName] = rSubjectID;
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

HRESULT SVObjectManagerClass::EnableObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (S_OK == Result)
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectObservers[rObserverID] = 1;
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::EnableObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (S_OK == Result)
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectCookies[Cookie] = 1;
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DisableObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (S_OK == Result)
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectObservers[rObserverID] = 0;
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DisableObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}

	if (S_OK == Result)
	{
		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectCookies[Cookie] = 0;
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		SVUniqueObjectEntryStructPtr pObserverObject = getUniqueObjectEntry(rObserverID);

		if (nullptr != pObserverObject)
		{
			pObserverObject->m_SubjectIDs.erase(rSubjectDataName);
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectObservers.erase(rObserverID);
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObserver(const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie)
{
	HRESULT Result = S_OK;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

	if (ReadWrite == m_State)
	{
		Autolock.lock();
		SVCookieEntryStructPtr pObserverObject = GetCookieEntry(Cookie);

		if (nullptr != pObserverObject)
		{
			pObserverObject->m_SubjectIDs.erase(rSubjectDataName);
		}

		SVUniqueObjectEntryStructPtr pSubjectObject = getUniqueObjectEntry(rSubjectID);

		if (nullptr != pSubjectObject)
		{
			pSubjectObject->m_DataNameSubjectObservers[rSubjectDataName].m_SubjectCookies.erase(Cookie);
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObservers(const std::string& rSubjectDataName, const SVGUID& rSubjectID)
{
	SVSubjectEnabledObserverMap Observers;
	SVSubjectEnabledCookieMap CookieObservers;

	HRESULT Result = GetObservers(rSubjectDataName, rSubjectID, Observers, CookieObservers);

	if (S_OK == Result)
	{
		SVSubjectEnabledObserverMap::iterator Iter = Observers.begin();

		while (Iter != Observers.end())
		{
			HRESULT l_Temp = DetachObserver(rSubjectDataName, rSubjectID, Iter->first);

			if (S_OK == Result)
			{
				Result = l_Temp;
			}

			++Iter;
		}

		SVSubjectEnabledCookieMap::iterator CookieIter = CookieObservers.begin();

		while (CookieIter != CookieObservers.end())
		{
			HRESULT l_Temp = DetachObserver(rSubjectDataName, rSubjectID, CookieIter->first);

			if (S_OK == Result)
			{
				Result = l_Temp;
			}

			++CookieIter;
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachSubjectsAndObservers(const SVGUID& rObjectID)
{
	HRESULT Result = DetachObservers(rObjectID);
	HRESULT SubjectStatus = DetachSubjects(rObjectID);

	if (S_OK == Result)
	{
		Result = SubjectStatus;
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachSubjects(long Cookie)
{
	SVSubjectDataNameDeque SubjectDataNames;
	HRESULT Result = GetObserverDataNames(Cookie, SubjectDataNames);

	if (S_OK == Result)
	{
		SVSubjectDataNameDeque::iterator Iter;

		for (Iter = SubjectDataNames.begin(); SubjectDataNames.end() != Iter; ++Iter)
		{
			SVGUID SubjectId;

			GetObserverSubject(*Iter, Cookie, SubjectId);

			DetachObserver(*Iter, SubjectId, Cookie);
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachSubjects(const SVGUID& rObserverID)
{
	SVSubjectDataNameDeque SubjectDataNames;

	HRESULT Result = GetObserverDataNames(rObserverID, SubjectDataNames);

	if (S_OK == Result)
	{
		SVSubjectDataNameDeque::iterator Iter;

		for (Iter = SubjectDataNames.begin(); SubjectDataNames.end() != Iter; ++Iter)
		{
			SVGUID SubjectId;

			GetObserverSubject(*Iter, rObserverID, SubjectId);

			DetachObserver(*Iter, SubjectId, rObserverID);
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DetachObservers(const SVGUID& rSubjectID)
{
	SVSubjectDataNameDeque SubjectDataNames;

	HRESULT Result = GetSubjectDataNames(rSubjectID, SubjectDataNames);

	if (S_OK == Result)
	{
		SVSubjectDataNameDeque::iterator Iter;

		for (Iter = SubjectDataNames.begin(); SubjectDataNames.end() != Iter; ++Iter)
		{
			HRESULT Temp = DetachObservers(*Iter, rSubjectID);

			if (S_OK == Result)
			{
				Result = Temp;
			}
		}
	}

	return Result;
}

HRESULT SVObjectManagerClass::DisconnectObjects(const SVGUID& rObjectID, const SVGUID& rRemoteID)
{
	HRESULT Result = S_OK;

	SVObjectClass* pObject = GetObject(rObjectID);

	if (nullptr != pObject)
	{
		Result = pObject->RemoveObjectConnection(rRemoteID);
	}

	pObject = GetObject(rRemoteID);

	if (nullptr != pObject)
	{
		HRESULT Temp = pObject->RemoveObjectConnection(rObjectID);

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
	SVGuidSet GuidObjectList;

	SVObjectClass* pStartObject(nullptr);
	GetObjectByDottedName(rPath, pStartObject);

	if (nullptr != pStartObject)
	{
		std::string InternalPath = pStartObject->GetCompleteName();
		if ((pStartObject->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter)
		{
			GuidObjectList.insert(pStartObject->GetUniqueObjectID());
		}

		SVUniqueObjectEntryStructPtr pUniqueObjectEntry;

		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State)
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
							GuidObjectList.insert(pUniqueObjectEntry->m_pObject->GetUniqueObjectID());
						}
					}
				}
			}
			++Iter;
		}

	}

	SVGuidSet::iterator Iter(GuidObjectList.begin());
	while (Iter != GuidObjectList.end())
	{
		SVObjectReference ObjectRef(GetObject(*Iter));
		rObjectList.push_back(ObjectRef);
		++Iter;
	}

	return Result;
}

SVObjectManagerClass::SVCookieEntryStructPtr SVObjectManagerClass::GetCookieEntry(long Cookie) const
{
	SVCookieEntryStructPtr pResult;

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}


	SVCookieEntryMap::const_iterator Iter(m_CookieEntries.find(Cookie));

	if (Iter != m_CookieEntries.end())
	{
		pResult = Iter->second;
	}


	return pResult;
}

SVGUID SVObjectManagerClass::GetSubjectID(const std::string& rSubjectDataName, SVUniqueObjectEntryStructPtr pObjectEntry) const
{
	SVGUID Result;

	if (nullptr != pObjectEntry)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



		if (ReadWrite == m_State)
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

SVGUID SVObjectManagerClass::GetSubjectID(const std::string& rSubjectDataName, SVCookieEntryStructPtr pCookieEntry) const
{
	SVGUID Result;

	if (nullptr != pCookieEntry)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);


		if (ReadWrite == m_State)
		{
			Autolock.lock();
		}


		SVSubjectDataNameSubjectIDMap::const_iterator Iter = pCookieEntry->m_SubjectIDs.find(rSubjectDataName);

		if (Iter != pCookieEntry->m_SubjectIDs.end())
		{
			Result = Iter->second;
		}

	}

	return Result;
}

SVObjectManagerClass::SVUniqueObjectEntryStructPtr SVObjectManagerClass::getUniqueObjectEntry(const SVGUID& rGuid) const
{
	SVUniqueObjectEntryStructPtr pUniqueObjectEntry(nullptr);

	std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



	if (ReadWrite == m_State)
	{
		Autolock.lock();
	}


	SVUniqueObjectEntryMap::const_iterator Iter(m_UniqueObjectEntries.find(rGuid));

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


		if (ReadWrite == m_State)
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

HRESULT SVObjectManagerClass::GetSubjectDataNames(const SVGUID& rSubjectID, SVSubjectDataNameDeque& rSubjectDataNames) const
{
	HRESULT Result = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rSubjectID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



		if (ReadWrite == m_State)
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

HRESULT SVObjectManagerClass::GetObserverDataNames(long Cookie, SVSubjectDataNameDeque& rSubjectDataNames) const
{
	HRESULT Result = S_OK;

	rSubjectDataNames.clear();

	SVCookieEntryStructPtr pCookie = GetCookieEntry(Cookie);

	if (nullptr != pCookie)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);


		if (ReadWrite == m_State)
		{
			Autolock.lock();
		}


		SVSubjectDataNameSubjectIDMap::const_iterator Iter;

		for (Iter = pCookie->m_SubjectIDs.begin(); pCookie->m_SubjectIDs.end() != Iter; ++Iter)
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

HRESULT SVObjectManagerClass::GetObserverDataNames(const SVGUID& rObserverID, SVSubjectDataNameDeque& rSubjectDataNames) const
{
	HRESULT Result = S_OK;

	rSubjectDataNames.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rObserverID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State)
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

HRESULT SVObjectManagerClass::GetObservers(const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers)
{
	HRESULT Result = S_OK;

	rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rSubjectID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);

		if (ReadWrite == m_State)
		{
			Autolock.lock();
		}

		SVSubjectDataNameObserverMap::iterator Iter = pUniqueObject->m_DataNameSubjectObservers.find(rSubjectDataName);

		if (Iter != pUniqueObject->m_DataNameSubjectObservers.end())
		{
			rObservers = Iter->second.m_SubjectObservers;
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

HRESULT SVObjectManagerClass::GetObservers(const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies)
{
	HRESULT l_Status = S_OK;

	rObservers.clear();

	SVUniqueObjectEntryStructPtr pUniqueObject = getUniqueObjectEntry(rSubjectID);

	if (nullptr != pUniqueObject)
	{
		std::unique_lock<std::recursive_mutex> Autolock(m_Mutex, std::defer_lock);



		if (ReadWrite == m_State)
		{
			Autolock.lock();
		}


		SVSubjectDataNameObserverMap::iterator Iter = pUniqueObject->m_DataNameSubjectObservers.find(rSubjectDataName);

		if (Iter != pUniqueObject->m_DataNameSubjectObservers.end())
		{
			rObservers = Iter->second.m_SubjectObservers;
			rObserverCookies = Iter->second.m_SubjectCookies;
		}
		else
		{
			l_Status = E_FAIL;
		}

	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVObjectManagerClass::RegisterSubObject(const SVGUID& rSubObjectID)
{
	HRESULT hr = S_FALSE;

	// get Tool and InspectionProcess
	SVObjectClass* pObject = GetObject(rSubObjectID);
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

HRESULT SVObjectManagerClass::UnregisterSubObject(const SVGUID& rSubObjectID)
{
	HRESULT hr = S_FALSE;

	// get Tool and InspectionProcess
	SVObjectClass* pObject = GetObject(rSubObjectID);
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

SvOi::IObjectClass* SVObjectManagerClass::getFirstObject(const SVGUID& rSourceId, const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const
{
	SvOi::IObjectClass* pRetObject = nullptr;
	SvOi::IObjectClass* pSource = SVObjectManagerClass::Instance().GetObject(rSourceId);
	if (nullptr != pSource)
	{
		pRetObject = pSource->getFirstObject(rObjectTypeInfo);
	}
	return pRetObject;
}

bool SVObjectManagerClass::ConnectObjectInput(const SVGUID& rSourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = false;
	SVObjectClass* pSource = SVObjectManagerClass::Instance().GetObject(rSourceId);
	if (nullptr != pSource)
	{
		Result = pSource->ConnectObjectInput(pObjectInInfo);
	}
	return Result;
}

bool SVObjectManagerClass::DisconnectObjectInput(const SVGUID& rSourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = false;
	SVObjectClass* pSource = SVObjectManagerClass::Instance().GetObject(rSourceId);
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

SvOi::IObjectClass* SvOi::ConstructObject(const SVGUID& rClassID)
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().ConstructObject(rClassID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::getObject(const SVGUID& rObjectID)
{
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(rObjectID, pObject);
	return pObject;
}

SvOi::IObjectClass* SvOi::FindObject(const SVGUID& rParentID, const SvDef::SVObjectTypeInfoStruct& rInfo)
{
	return SVObjectManagerClass::Instance().getFirstObject(rParentID, rInfo);
}

SvOi::ISVImage* SvOi::FindImageObject(const SVGUID& rParentID, const SvDef::SVObjectTypeInfoStruct& rInfo)
{
	SvOi::IObjectClass* pObject = SvOi::FindObject(rParentID, rInfo);

	return dynamic_cast<SvOi::ISVImage*> (pObject);
}
#pragma endregion IObjectManager-function

