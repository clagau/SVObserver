//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPublishList.h
//* .File Name       : $Workfile:   SVPublishList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:38:36  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#pragma endregion Includes

class SVTaskObjectClass;
class SVInspectionProcess;

class SVPublishListClass : public SVOutputInfoListClass
{
public:
	SVPublishListClass();

	virtual ~SVPublishListClass();

public:
	void Destroy();

	void Refresh(SVTaskObjectClass* pRootObject);
	void Release(SVTaskObjectClass* pRootObject);
	bool RemovePublishedEntry( const SVGUID& RGuid );

	SVInspectionProcess* m_pInspection;

protected:
	// To handle public results ( outputs which are send to PPQ )
	// This list doesn't own the entries! They are owned by
	// the output owners !
	HANDLE								hProtectionMutex;
	DWORD								dwWaitTime;
};


