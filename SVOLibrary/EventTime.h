//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file EventTime.h
/// All Rights Reserved
//*****************************************************************************
/// This is a class which contains start end time and state of the event
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

class EventTime
{
#pragma region Constructor
public:
    EventTime();
	EventTime(const EventTime &rRhs);
	virtual ~EventTime() {}

	const EventTime& operator=(const EventTime& rRhs);
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void reset();

	bool isValid() const { return m_bIsValid; };
	void setIsValid(bool bIsValid) { m_bIsValid = bIsValid; }

	bool isComplete() const { return m_bIsComplete; };
	void setIsComplete(bool bIsComplete) { m_bIsComplete = bIsComplete; }

	const double& getStartTime() const { return m_StartTime; }
	void setStartTime(const double& rTime) { m_StartTime = rTime; }

	const double& getEndTime() const { return m_EndTime; }
	void setEndTime(const double& rTime) { m_EndTime = rTime; }
#pragma endregion Public Methods

#pragma region Member Variables
private:
	bool m_bIsValid;
	bool m_bIsComplete;

	//This attribute holds the start time stamp
	double m_StartTime;
	//This attribute holds the end time stamp
	double m_EndTime;
#pragma endregion Member Variables
};
