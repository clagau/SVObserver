//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RunStatus
//* .File Name       : $Workfile:   RunStatus.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:21:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces/ITriggerRecordRW.h"
#pragma endregion Includes

namespace SvIe
{
class RunStatus
{
public:
	enum
	{
		SV_INVALID = 0,
	};

private:
	union runstateUnion
	{
		struct statusStruct
		{
			unsigned int passed : 1;	// everything is OK!
			unsigned int failed : 1;	// ran, but failed
			unsigned int warned : 1;	// ran, but warned
			unsigned int valid : 1;	// did anything say it was not valid
			unsigned int run : 1;	// did it run ? (inspection started)
			unsigned int disabled : 1;	// not enabled ?
			unsigned int disabledByCondition : 1;	// disabled because of condition
			unsigned int criticalFailure : 1;	// a critical error occurred and will NAK the inspection
		} status;
		unsigned int state;		// did anything run ?
	} run;

public:
	RunStatus();
	void ResetRunStateAndToolSetTimes();

	void SetPassed();
	void SetFailed();
	void SetWarned();
	void SetInvalid();
	void SetValid();
	void SetInspectionStarted(bool state);
	void SetDisabled();
	void SetDisabledByCondition();
	void SetCriticalFailure();

	bool IsPassed() const;
	bool IsFailed() const;
	bool IsWarned() const;
	bool IsValid() const;

	bool IsDisabled() const;
	bool IsDisabledByCondition() const;
	bool IsCriticalFailure() const;
	bool HasInspectionStarted() const;
	void updateState(const RunStatus& childStatus, bool updateConditionState = true);

	DWORD GetState() const;
	DWORD GetStatusColor() const;

	long m_lTriggerCount {-1};	// TB
	bool m_UpdateCounters {true};

	double m_ToolSetEndTime {0.0};
	double m_ToolSetAvgTime {0.0};

	SvOi::ITriggerRecordRWPtr m_triggerRecord = nullptr;
};

inline void RunStatus::SetPassed()
{
	run.status.passed = ((!run.status.failed && !run.status.warned && run.status.valid) ? true : false);
}

inline void RunStatus::SetFailed()
{
	run.status.failed = true;
	run.status.warned = false;
	run.status.passed = false;
}

inline void RunStatus::SetWarned()
{
	run.status.warned = ((!run.status.failed) ? true : false);
}

inline void RunStatus::SetInvalid()
{
	run.status.valid = false;
}

inline void RunStatus::SetValid()
{
	run.status.valid = true;
}

inline void RunStatus::SetDisabled()
{
	run.status.disabled = true;
}

inline void RunStatus::SetDisabledByCondition()
{
	run.status.disabledByCondition = true;
}

inline void RunStatus::SetCriticalFailure()
{
	run.status.criticalFailure = true;
}

inline void RunStatus::SetInspectionStarted(bool state)
{
	run.status.run = state;
}

inline bool RunStatus::IsPassed() const
{
	return (run.status.passed && !run.status.failed && !run.status.warned && run.status.valid);
}

inline bool RunStatus::IsFailed() const
{
	return (run.status.failed && !run.status.warned && run.status.valid);
}

inline bool RunStatus::IsWarned() const
{
	return (run.status.warned && run.status.valid);
}

inline bool RunStatus::IsValid() const
{
	return run.status.valid;
}

inline bool RunStatus::IsDisabled() const
{
	return run.status.disabled;
}

inline bool RunStatus::IsDisabledByCondition() const
{
	return run.status.disabledByCondition;
}

inline bool RunStatus::IsCriticalFailure() const
{
	return run.status.criticalFailure;
}

inline bool RunStatus::HasInspectionStarted() const
{
	return run.status.run;
}

inline DWORD RunStatus::GetState() const
{
	return run.state;
}

} //namespace SvIe
	