//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerList
//* .File Name       : $Workfile:   SVOTriggerList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:14:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary\SVList.h"

#include "SVOTriggerObj.h"
#pragma endregion Includes

namespace SvTi
{
	class SVOTriggerList  
	{
	public:
		typedef SVList< SVOTriggerObjPtr > SVTriggerList;
		typedef SVTriggerList::iterator iterator;
		typedef SVTriggerList::const_iterator const_iterator;

		SVOTriggerList();
		virtual ~SVOTriggerList();

		bool AddTriggerToList(const std::string& sTriggerName, int iDigNumber);
		bool RemoveTriggerFromList(const std::string& sTriggerName);
		bool IsTriggerInList(const std::string& sTiggerName) const;
		int GetTriggerListCount() const;
		SVOTriggerObjPtr GetTriggerObjectByName(const std::string& sTriggerName);
		SVOTriggerObjPtr GetTriggerObjectByPosition(int iPos);

		void ResetContent();

		int GetNextTriggerID() const;

	private:
		SVTriggerList m_TriggerList;
		iterator FindTriggerPosition(const std::string& sTriggerName);
	};
} //namespace SvTi
