// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalArrayClass
// * .File Name       : $Workfile:   SVIODigitalArrayClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:36  $
// ******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVIODigitalStruct.h"

class SVIODigitalArrayClass : public SVVector< SVIODigitalStruct >  
{
public:
	SVIODigitalArrayClass();
	virtual ~SVIODigitalArrayClass();
};

