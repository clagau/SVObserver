//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSet
//* .File Name       : $Workfile:   SVToolSet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 04:24:36  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IToolSet.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "InspectionEngine/SVMainImageClass.h"
#include "Tools/SVTool.h"
#include "SVResultList.h"
#include "InspectionEngine/SVTaskObjectList.h"
#pragma region Includes

namespace SvOp
{
class SVConditionalClass;
}

enum ToolSetTimes
{
	TriggerDelta,				///The time interval between two triggers
	TriggerToStart,				///The time interval between the trigger that started the product and the start of the inspection in microseconds
	TriggerToCompletion,		///The time interval between the trigger that started the product and inspection completion in microseconds
	TriggerToAcquisitionStart,	///The time interval between the trigger that started the product and the start of tool set image acquisition in microseconds
	AcquisitionTime,			///The time interval between the tool set image acquisition start and end in microseconds
	MaxCount
};

// This class encapsules basic properties to handle and execute an Image Processing Tool Set

class SVToolSetClass : public SvIe::SVTaskObjectListClass, public SvOi::IToolSet
{
	SV_DECLARE_CLASS( SVToolSetClass );

	friend class SVInspectionProcess; // for access to Run()

#pragma region Public Methods
public:
	SVToolSetClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOLSET );

	virtual ~SVToolSetClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void Destroy();

	bool IsEnabled() const;

	bool WasEnabled() const;

	HRESULT ResetCounts();
	HRESULT ClearResetCounts();

	//! Update the tool position
	void updateToolPosition();

	void SetDefaultInputs();
	
	SVResultListClass* GetResultList();

	SvIe::SVImageClass* getCurrentImage();////@TODO[Arvid][7.50][08.05.2017] all occurrences of this function should be replaced by getCurrentImageInterface() when possible

	SvOi::ISVImage* getCurrentImageInterface() override;

	SvVol::SVEnumerateValueObjectClass* GetDrawFlagObject();

	bool getEnableAuxiliaryExtents() { BOOL Enabled{false}; m_EnableAuxiliaryExtents.GetValue(Enabled); return Enabled ? true : false; }
	void setEnableAuxiliaryExtents(bool Enabled) { m_EnableAuxiliaryExtents.SetValue(BOOL(Enabled));}

	template<typename SvGuidInserter>
	void GetToolIds(SvGuidInserter Inserter) const
	{
		for (SVTaskObjectPtrVector::const_iterator Iter = m_TaskObjectVector.begin(); Iter != m_TaskObjectVector.end(); ++Iter)
		{
			SVTaskObjectClass* pTask = (*Iter);

			if (nullptr != pTask)
			{
				Inserter = pTask->GetUniqueObjectID();
			}
		}
	};

	HRESULT getResetCounts( bool& rResetCounts ) const override;

	void setTime(double time, ToolSetTimes timeType) { m_Times[timeType].SetValue(static_cast<__int64> (time)); }
	void setPpqPosition(long ppqPosition) { m_PPQIndexAtCompletion.SetValue(ppqPosition); }

	virtual void goingOffline() override;
#pragma region virtual method (IToolSet)
	virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const override;
	/// Return the first band 0 image from the first color tool. If there are no color Tool it return nullptr.
	/// \returns SvOi::IObjectClass*
	virtual SvOi::IObjectClass* getBand0Image() const override;
#pragma endregion virtual method (IToolSet)
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	bool getConditionalResult(bool bRunMode = false) const;

	virtual bool Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual HRESULT onCollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray ) override;

	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject(SvIe::SVTaskObjectClass& rChildObject ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void init();

	void UpdateRunStatus(SVRunStatusClass& rRunStatus, const SVRunStatusClass& rToolRunStatus) const;

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	void setPostRunStatus(SvTl::SVTimeStamp l_Timer, SVRunStatusClass &rRunStatus);
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvVol::SVBoolValueObjectClass m_RegressionTestMode;

	long m_SetNumber;

	SvTl::SVTimeStamp m_StartTime;
	SvTl::SVTimeStamp m_EndTime;
	SvTl::SVTimeStamp m_AverageTime;
	SvTl::SVTimeStamp m_ProcessTime;

	SvTl::SVTimeStamp m_TimeStampStart;
	SvTl::SVTimeStamp m_TimeStampEnd;

	SvVol::SVBoolValueObjectClass  m_isObjectValid;	//	Embedded
	SvVol::SVBoolValueObjectClass m_Enabled;
	SvVol::SVBoolValueObjectClass m_ResetCounts;
	SvVol::SVLongValueObjectClass m_TriggerCount;

	SVResultListClass m_ResultList;

	// Passed, if TRUE ( Reset Value: FALSE )
	SvVol::SVBoolValueObjectClass m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass m_Failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SvVol::SVBoolValueObjectClass m_ExplicitFailed;

	SvVol::SVBoolValueObjectClass m_EnableAuxiliaryExtents;

	SvVol::SVLongValueObjectClass m_PassedCount;
	SvVol::SVLongValueObjectClass m_FailedCount;
	SvVol::SVLongValueObjectClass m_WarnedCount;

	SvVol::SVLongValueObjectClass m_EnabledCount;
	SvVol::SVLongValueObjectClass m_ProcessedCount;

	// Conditional input
	SvOl::SVInObjectInfoStruct m_inputConditionBoolObjectInfo;

	// Conditional tool set drawing flag.
	SvVol::SVEnumerateValueObjectClass m_DrawFlag;
	SvVol::SVTimerValueObjectClass m_ToolTime;

	bool m_bResetMinMaxToolsetTime;

	SvVol::SVTimerValueObjectClass m_MinToolsetTime;
	SvVol::SVTimerValueObjectClass m_MaxToolsetTime;

private:
	// Embedded Object:
	SvIe::SVMainImageClass m_MainImageObject;	// Main toolset image
	SvVol::SVLongValueObjectClass m_PPQIndexAtCompletion; // the PPQ position at which the product was located when at completion
	SvVol::SVTimerValueObjectClass m_Times[ToolSetTimes::MaxCount]; ///The times relevant to the tool set see the enums for detailed description
	SvVol::SVDoubleValueObjectClass m_Width;	//! The toolset image width			
	SvVol::SVDoubleValueObjectClass m_Height;	//! The toolset image height
#pragma endregion Member Variables
};

