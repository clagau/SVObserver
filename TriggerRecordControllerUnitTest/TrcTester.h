//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once

#include <winerror.h>
#include <intsafe.h>

#include "TriggerRecordController\ITriggerRecordControllerRW.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "TriggerRecordController\ITriggerRecordRW.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVProtoBuf/TriggerRecordController.h"


/// configures the unit test
struct TrcTesterConfiguration
{
	int m_NumberOfUnittests = 20;
	int m_NoOfRepetitionsPerStep = 200;

	int m_NumberOfImagesPerInspection = 99;
	int m_NumberOfInspections = 5;
	int m_NumberOfBuffersPerInspection = 10;
	int m_NumberOfIndependentBuffers = 5;
};

/// controls the flow of the test
class TrcTester
{
	MIL_ID m_AppId;
	SvTrc::ITriggerRecordControllerRW &m_TRController;
	const struct TrcTesterConfiguration m_config;

public:
	TrcTester(const struct TrcTesterConfiguration &rConfig);
	~TrcTester();
	bool createInspections();
	bool setInspectionBuffers();
	bool setIndependentBuffers();
	bool hasAppId() { return m_AppId != M_NULL; }
};

