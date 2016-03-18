//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageListClass
//* .File Name       : $Workfile:   SVImageListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:53:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#pragma endregion Includes

class SVImageClass;

class SVImageListClass : public SVVector< SVImageClass* >
{
public:
	SVImageListClass();

	virtual ~SVImageListClass();

	SVImageClass * GetAtByTool(int nIndex);             // 25 Jun 1999 - frb.

};

