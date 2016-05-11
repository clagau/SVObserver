//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ManagedIndexArrayList.h
//* .File Name       : $Workfile:   ManagedIndexArrayList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:42  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"

class SVManagedIndexArray;

class SVManagedIndexArrayList :
	public SVVector< SVManagedIndexArray* >
{
public:
	SVManagedIndexArrayList();

	virtual ~SVManagedIndexArrayList();
};

