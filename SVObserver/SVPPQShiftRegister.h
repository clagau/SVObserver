//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQShiftRegister
//* .File Name       : $Workfile:   SVPPQShiftRegister.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:16  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

struct SVProductInfoStruct;

class SVPPQShiftRegister
{
public:
	SVPPQShiftRegister() = default;
	virtual ~SVPPQShiftRegister() = default;

	size_t size() const;
	void resize( size_t p_Size );
	void clear();

	SVProductInfoStruct* GetProductAt( size_t p_Index ) const;

	HRESULT SetProductAt( size_t p_Index, SVProductInfoStruct* p_pProduct );

	SVProductInfoStruct* IndexRegister();

	HRESULT IsProductAlive( long p_TriggerCount ) const;

	long GetIndexByTriggerCount(long triggerCount) const;
	SVProductInfoStruct* GetProductByTriggerCount(long triggerCount) const;

	long GetIndexByTriggerTimeStamp(double checkTime, int cameraID) const;

	void setTriggerTimeWindow(double preTriggerTimeWindow, double postTriggerTimeWindow) { m_preTriggerTimeWindow = preTriggerTimeWindow; m_postTriggerTimeWindow = postTriggerTimeWindow; }
	void setMaxCameraShutterTime(double shutterTime) { m_maxCameraShutterTime = shutterTime; }

private:
	bool findTimeMatch(double checkTime, double triggerTime) const;

	typedef std::vector<SVProductInfoStruct*> SVProductVector;

	SVProductVector m_Products;

	long m_HeadTriggerCount{ 0L };
	long m_TailTriggerCount{ 0L };
	double m_preTriggerTimeWindow{ 0.0 };	//This is to compensate that the trigger may be recorded after the start frame (in milliseconds)
	double m_postTriggerTimeWindow {0.0};
	double m_maxCameraShutterTime {0.0};
};

