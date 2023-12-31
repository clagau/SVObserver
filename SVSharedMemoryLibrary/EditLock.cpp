/// \copyright 2023 by K�rber Pharma Inspection GmbH. All Rights Reserved.
/// \file EditLock.cpp
/// Contains functions for managing EditLocks from the SVObserver side

#include "stdafx.h"
#include "EditLock.h"
#include "SVSharedMemoryLibrary/SharedMemoryLock.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVMessage/SVMessage.h"

namespace SvSml
{

	TemporaryState_Editing::TemporaryState_Editing()
	{
		m_editLockAcquired = EditLock::acquire();

		if (m_editLockAcquired)
		{
			SvimState::AddState_Editing(); // Use only this function to set SV_STATE_EDITING!
		}
	}
	TemporaryState_Editing::~TemporaryState_Editing()
	{
		if (m_editLockAcquired)
		{
			if (SvimState::RemoveState_Editing()) // Use only this function to remove SV_STATE_EDITING!
			{
				EditLock::release();
			}
		}

	}

namespace
{

auto g_svimUsername = _T("SVIMUser");
auto g_svimHostname = _T("<SVIM>");
void onEditLockStateChange(LockState state); //callback function

LockEntity g_currentLockOwner;

LockEntity SvimUser()
{
	LockEntity entity;
	entity.type = EntityType::SVObserver;
	strncpy(entity.username, g_svimUsername, maxHostSize);
	strncpy(entity.host, g_svimHostname, maxHostSize);

	return entity;
}


LockEntity NoOne()
{
	LockEntity entity;
	entity.type = EntityType::Empty;
	strncpy(entity.username, _T(""), maxHostSize);
	strncpy(entity.host, _T(""), maxHostSize);

	return entity;
}


static SVObserverSharedMemoryLock& getEditLockInstance()
{
	static SVObserverSharedMemoryLock s_theEditLock(onEditLockStateChange); ///< the one and only EditLock
	return s_theEditLock;
}


const TCHAR* getTypeName(EntityType type)
{
	switch (type)
	{
		case EntityType::Empty:
			return "no one";
		case EntityType::SVObserver:
			return "SVO";
		case EntityType::SVOGateway:
			return "Gateway";
	}
	return "[invalid entity]";
}


std::string LockEntityInfo(LockEntity le)
{
	std::stringstream lockEntityInfoStream;
	if (le.type != EntityType::SVOGateway)
	{
		lockEntityInfoStream << getTypeName(le.type);
	}
	if (le.type == EntityType::SVOGateway)
	{
		lockEntityInfoStream << le.username << " on " << le.host << "";
	}

	return lockEntityInfoStream.str();
}

void onEditLockStateChange(LockState state)
{
	g_currentLockOwner = state.owner;
#if defined (TRACE_THEM_ALL) || defined (TRACE_EDIT_LOCK)
	std::stringstream stateinfo;

	stateinfo << "\t=> Owned by " << LockEntityInfo(state.owner);

	if (state.requester.type != EntityType::Empty)
	{
		stateinfo << ", requested by " << LockEntityInfo(state.requester);

	}
	stateinfo << ".\n";

	OutputDebugString(stateinfo.str().c_str());

#endif	
	if (state.requester.type == EntityType::SVOGateway)
	{
		SvDef::StringVector msgList;
		msgList.push_back("Edit Lock requested by " + LockEntityInfo(state.requester) + ". Please release Edit Lock!");
		SvStl::MessageManager msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
}

}


namespace EditLock
{
bool isAcquiredBySVIM()
{

#if defined (TRACE_THEM_ALL) || defined (TRACE_EDIT_LOCK)
	::OutputDebugString(g_currentLockOwner.type == EntityType::SVObserver ? "\t\tLocked\n" : "\t\tUnlocked\n");
#endif
	return g_currentLockOwner.type == EntityType::SVObserver;
}

bool setEditLock(bool toBeLocked)
{
	if (isAcquiredBySVIM() == toBeLocked) //nothing to do
	{
		return isAcquiredBySVIM();
	}

	if (toBeLocked)
	{
		auto acquired = getEditLockInstance().Acquire(g_svimUsername, g_svimHostname);

		if (false == acquired)
		{
			getEditLockInstance().RequestTakeover(g_svimUsername, g_svimHostname);

			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_NoEditLock, {LockEntityInfo(g_currentLockOwner)}, SvStl::SourceFileParams(StdMessageParams));
		}
		else
		{
			g_currentLockOwner = SvimUser();

#if defined (TRACE_THEM_ALL) || defined (TRACE_EDIT_LOCK)
			OutputDebugString("\t[Acquired]\n");
#endif	
		}
	}
	else
	{
		getEditLockInstance().Release();

		g_currentLockOwner = NoOne();

#if defined (TRACE_THEM_ALL) || defined (TRACE_EDIT_LOCK)
		OutputDebugString("\t[Released]\n");
#endif	
	}

	return isAcquiredBySVIM();
}


bool acquire()
{
	if (SvimState::CheckState(SV_STATE_REMOTE_CMD)) //we are currently executing a remote command so we don't need (or want) to acquire the edit lock
	{
		return true;
	}

	return setEditLock(true);
}


void release()
{
	setEditLock(false);
}
}
}// namespace SvStl
