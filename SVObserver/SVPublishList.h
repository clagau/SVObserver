//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

namespace SvIe
{
class SVTaskObjectClass;
}
class SVInspectionProcess;

class SVPublishList
{
public:
	void Refresh(SvIe::SVTaskObjectClass* pRootObject);
	void Release(SvIe::SVTaskObjectClass* pRootObject);
	bool RemovePublishedEntry(uint32_t id);
	void setInspection(SVInspectionProcess* pInspection) { m_pInspection = pInspection; };

protected:
	SVInspectionProcess* m_pInspection{ nullptr };
	DWORD								dwWaitTime{ 2000 };
	std::vector<uint32_t>				m_objectIdList;
};


