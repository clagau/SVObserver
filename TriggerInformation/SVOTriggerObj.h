//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerObj
//* .File Name       : $Workfile:   SVOTriggerObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:15:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVTriggerConstants.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes


namespace SvTi
{
	class SVOTriggerObj  
	{
	public:
		SVOTriggerObj(const SVString& sTriggerName, int iDig);
		virtual ~SVOTriggerObj();

		LPCTSTR GetTriggerDisplayName() const;
		int GetTriggerDigNumber() const;

		void SetTimerPeriod(long lPeriod);
		long GetTimerPeriod() const;

		bool IsSoftwareTrigger() const;
		void SetSoftwareTrigger(bool bSoftwareTrigger);

		bool IsAcquisitionTrigger() const;

	private:  //data members
		SVString m_sTriggerDisplayName;
		int m_iDigNumber;

		bool m_bSoftwareTrigger;
		long m_timerPeriod;
	};

	typedef SVSharedPtr< SVOTriggerObj > SVOTriggerObjPtr;

} //namespace SvTi
