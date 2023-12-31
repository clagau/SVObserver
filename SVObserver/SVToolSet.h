//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma region Includes

enum ToolSetTimes
{
	TriggerDelta,				///The time interval between the trigger that started the product and the previous trigger
	TriggerToStart,				///The time interval between the trigger that started the product and the start of the inspection in microseconds
	TriggerToCompletion,		///The time interval between the trigger that started the product and inspection completion in microseconds
	TriggerToAcquisitionStart,	///The time interval between the trigger that started the product and the start of tool set image acquisition in microseconds
	AcquisitionTime,			///The time interval between the tool set image acquisition start and end in microseconds
	TriggerTimeStamp,			///The time stamp in microseconds for the trigger that started the products

	MaxCount
};

// This class encapsulates basic properties to handle and execute an Image Processing Tool Set

class SVToolSet : public SvIe::SVTaskObjectListClass, public SvOi::IToolSet
{
	SV_DECLARE_CLASS

	friend class SVInspectionProcess; // for access to Run()

#pragma region Public Methods
public:
	SVToolSet( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOLSET );

	virtual ~SVToolSet();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr, int nResetDepth = 0) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void resetCounterDirectly();

	void Destroy();

	bool IsEnabled() const;

	bool WasEnabled() const;


	//! Update the tool position
	void updateToolPosition();

	void SetDefaultInputs();
	
	SVResultList* GetResultList();

	SvIe::SVImageClass* getCurrentImage();////@TODO[Arvid][7.50][08.05.2017] all occurrences of this function should be replaced by getCurrentImageInterface() when possible

	SvOi::ISVImage* getCurrentImageInterface() override;

	SvVol::SVEnumerateValueObjectClass* GetDrawFlagObject();

	bool getEnableAuxiliaryExtents() { BOOL Enabled{false}; m_EnableAuxiliaryExtents.GetValue(Enabled); return Enabled ? true : false; }
	void setEnableAuxiliaryExtents(bool Enabled) { m_EnableAuxiliaryExtents.SetValue(BOOL(Enabled));}

	template<typename ObjectIdInserter>
	void GetToolIds(ObjectIdInserter Inserter) const
	{
		for (SvIe::SVTaskObjectPtrVector::const_iterator Iter = m_TaskObjectVector.begin(); Iter != m_TaskObjectVector.end(); ++Iter)
		{
			SVTaskObjectClass* pTask = (*Iter);

			if (nullptr != pTask)
			{
				// cppcheck-suppress redundantAssignment symbolName=Inserter ; cppCheck doesn't know insert_iterator
				Inserter = pTask->getObjectId();
			}
		}
	};

	void setTime(double time, ToolSetTimes timeType) { m_Times[timeType].SetValue(static_cast<__int64> (time)); }
	void setPpqPosition(long ppqPosition) { m_PPQIndexAtCompletion.SetValue(ppqPosition); }
	void setTriggerData(const SvTrig::TriggerData& rTriggerData);
	void setInspectionname(std::string Inspectionname) { m_InspectionName.SetValue(Inspectionname);};
	void setMissingImageCount(int missingeImageCount) { m_MissingImageCountTS.SetValue(missingeImageCount); }
	void setNotCompleteCount(int notCompleteCount) { m_NotCompleteCountTS.SetValue(notCompleteCount); }

	long getTriggerCount() const;
	SvPb::OverlayDesc getOverlayStruct(const SvOi::ISVImage& rImage) const;
	void getOverlays(SvIe::SVImageClass& rImage, SVExtentMultiLineStructVector& rMultiLineArray) const;
	
	DWORD GetObjectIdIndex() const { DWORD objectIdIndex {0}; m_ObjectIdIndex.GetValue(objectIdIndex); return objectIdIndex; }
	void SetObjectIdIndex(DWORD objectIdIndex);

#pragma region virtual method (IToolSet)
	virtual bool IsToolPreviousToSelected(uint32_t toolID) const override;
	/// Return the first band 0 image from the first color tool. If there are no color Tool it return nullptr.
	/// \returns SvOi::IObjectClass*
	virtual SvOi::IObjectClass* getBand0Image() const override;
#pragma endregion virtual method (IToolSet)
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	bool getConditionalResult(bool bRunMode = false) const;

	virtual bool Run( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject(SvIe::SVTaskObjectClass& rChildObject ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void init();

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

	void setPostRunStatus(double l_Timer, SvIe::RunStatus &rRunStatus);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvVol::SVBoolValueObjectClass m_RegressionTestMode;

	long m_SetNumber {0L};

	double m_StartTime {0.0};
	double m_EndTime {0.0};
	double m_AverageTime {0.0};
	double m_ProcessTime {0.0};


	bool m_bResetMinMaxToolsetTime {true};

	SvVol::SVBoolValueObjectClass  m_isObjectValid;	//	Embedded
	SvVol::SVBoolValueObjectClass m_Enabled;
	SvVol::SVBoolValueObjectClass m_ResetCountsObject;
	SvVol::SVLongValueObjectClass m_TriggerCount;

	SVResultList m_ResultList;

	SvVol::PassedWarnedFailedHelper m_pwf;

	//@TODO [Arvid][10.30][11.11.2022] the following four variables might also be added to SvVol::PassedWarnedFailedHelper
	SvVol::SVBoolValueObjectClass m_ExplicitFailed; 
	SvVol::SVLongValueObjectClass m_PassedCount;
	SvVol::SVLongValueObjectClass m_FailedCount;
	SvVol::SVLongValueObjectClass m_WarnedCount;

	SvVol::SVBoolValueObjectClass m_EnableAuxiliaryExtents;
	SvVol::SVLongValueObjectClass m_EnabledCount;
	SvVol::SVLongValueObjectClass m_ProcessedCount;

	// Conditional input
	SvOl::InputObject m_inputConditionBool;

	// Conditional tool set drawing flag.
	SvVol::SVEnumerateValueObjectClass m_DrawFlag;
	SvVol::SVTimerValueObjectClass m_ToolTime;

	SvVol::SVTimerValueObjectClass m_MinToolsetTime;
	SvVol::SVTimerValueObjectClass m_MaxToolsetTime;

	// Embedded Object:
	SvIe::SVMainImageClass m_MainImageObject;	// Main toolset image
	SvVol::SVLongValueObjectClass m_PPQIndexAtCompletion; // the PPQ position at which the product was located when at completion
	SvVol::SVTimerValueObjectClass m_Times[ToolSetTimes::MaxCount]; ///The times relevant to the tool set see the enums for detailed description
	SvVol::SVDoubleValueObjectClass m_Width;		//! The toolset image width			
	SvVol::SVDoubleValueObjectClass m_Height;		//! The toolset image height
	SvVol::SVDWordValueObjectClass m_ObjectID;		//! The object ID, which has been received by the PLC, to be inspected
	SvVol::SVDWordValueObjectClass m_ObjectType;	//! The objectType, which has been received by the PLC
	SvVol::SVDWordValueObjectClass m_TriggerIndex;	//! The trigger index, which has been received by the PLC
	SvVol::SVDWordValueObjectClass m_TriggerPerObjectID;//! The trigger total for this ObjectID
	SvVol::LinkedValue m_InspectedObjectID;			//! The inspected object ID (is always set to the incoming object ID), is no longer used must stay for compatibility reasons
	SvVol::SVDWordValueObjectClass m_ObjectIdIndex;	//! The object ID index 
	SvVol::SVBoolValueObjectClass m_LoopMode;		//! True when the PLC is in loop mode
	SvVol::SVDWordValueObjectClass m_RotationNumber;//! The rotation number in loop mode
	SvVol::SVDoubleValueObjectClass m_MeasurementValue;	//! The measurement value, which has been received by the PLC
	SvVol::SVStringValueObjectClass m_InspectionName;

	SvVol::SVLongValueObjectClass m_MissingImageCountTS; //MissingImageCount in ppq  at the time the inspection is started
	SvVol::SVLongValueObjectClass m_NotCompleteCountTS;//NotCompleteCount in ppq  at the time the inspection is started
#pragma endregion Member Variables
};

