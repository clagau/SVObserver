/// \copyright 2023 by Körber Pharma Inspection GmbH. All Rights Reserved.
/// \file EditLock.h
/// Contains functions for managing EditLocks from the SVObserver side

#pragma once

namespace SvSml
{

namespace EditLock
{
bool isAcquiredBySVIM();

bool setEditLock(bool toBeLocked);
bool acquire();
void release();
}

class TemporaryState_Editing // RAII class analogous to SvimState::TemporaryState to be used _exclusively_ for SV_STATE_EDITING
{
public:
	TemporaryState_Editing();
	virtual ~TemporaryState_Editing();
	bool stateWasEntered() const
	{
		return m_editLockAcquired;
	}

private:
	bool m_editLockAcquired = false;
};

}