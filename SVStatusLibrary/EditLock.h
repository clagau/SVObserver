/// \copyright 2023 by Körber Pharma Inspection GmbH. All Rights Reserved.
/// \file EditLock.h
/// Contains functions for manging EditLocks from the SVObserver side

#pragma once

#include "SVSharedMemoryLibrary/SharedMemoryLock.h"

namespace SvStl
{

namespace EditLock
{
bool isAcquiredBySVIM();

bool setEditLock(bool toBeLocked);
bool acquire();
void release();
}

}