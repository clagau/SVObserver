//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Definition of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once

#include "stdafx.h"

#include "TriggerRecordControllerUnitTest.h"

#include "TrcTester.h"
//#include "SVStatusLibrary\MessageContainer.h"


SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor);

TrcTester::TrcTester(const struct TrcTesterConfiguration &rConfig) :
	m_config(rConfig),
	m_AppId(MappAlloc(M_DEFAULT, M_NULL)),
	m_TRController(SvTrc::getTriggerRecordControllerRWInstance())
{

}

TrcTester::~TrcTester()
{
	m_TRController.clearAll(); // avoids error message when MappFree() is called.
	MappFree(m_AppId);
	google::protobuf::ShutdownProtobufLibrary();
}


bool TrcTester::createInspections()
{
	SvPb::InspectionList inspList;

	for (int i = 0; i < m_config.m_NumberOfInspections; i++)
	{
		auto* pInspStruct = inspList.add_list();
		if (nullptr == pInspStruct)
		{
			wprintf(L"Could not create %d. inspection!\n", i);
			return false;
		}
		pInspStruct->set_numberofrecords(m_config.m_NumberOfImagesPerInspection); //ABX Anzahl der TRs in dieser Inspektion
	}
	return m_TRController.setInspections(inspList); //ABXX dem TRC die Liste von Inspektionen übergeben
}



bool TrcTester::setInspectionBuffers()
{
	GUID guid = GUID_NULL;

	for (int i = 0; i < m_config.m_NumberOfInspections; i++)
	{
		try
		{
			m_TRController.startResetTriggerRecordStructure(i);

			for (int j = 0; j < m_config.m_NumberOfBuffersPerInspection; j++)
			{
				UuidCreateSequential(&guid);

				m_TRController.addOrChangeImage(guid, specifyBuffer(1 + j));
			}

			m_TRController.finishResetTriggerRecordStructure(); //ABXX die Änderung der in Arbeit befindlichen Inspektion wird abgeschlossen
		}
		//catch (const SvStl::MessageContainer& rExp)
		catch(...)
		{
			wprintf(L"Parts of setInspectionBuffer throw an exception: #%d!", i);
			return false;
		}
	}

	return true;
}


bool TrcTester::setIndependentBuffers()
{
	GUID guid = GUID_NULL;

	try
	{
		m_TRController.startResetTriggerRecordStructure();
		bool independentOk = true;
		for (int i = 0; i < m_config.m_NumberOfBuffersPerInspection; i++)
		{
			UuidCreateSequential(&guid);
			independentOk &= m_TRController.removeAllImageBuffer(guid);
			independentOk &= m_TRController.addImageBuffer(guid, specifyBuffer(m_config.m_NumberOfIndependentBuffers), 1);
		}
		m_TRController.finishResetTriggerRecordStructure();

		if (!independentOk)
		{
			wprintf(L"ERROR when creating independent buffer!\n");
			return false;
		}
	}
	//catch (const SvStl::MessageContainer& rExp)
	catch(...)
	{
		wprintf(L"ERROR when creating independent buffer!\n");
		return false;
	}

	return true;
}


SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor)
{
	if (sizeFactor < 1)
	{
		sizeFactor = 1;
	}
	SVMatroxBufferCreateStruct bufferStruct;
	int sizeX = sizeFactor * 64;
	int sizeY = sizeFactor * 48;
	bufferStruct.m_lSizeX = sizeX;
	bufferStruct.m_lSizeY = sizeY;
	bufferStruct.m_eAttribute = SVBufAttImageProcDib;
	bufferStruct.m_eType = SV8BitUnsigned;

	return bufferStruct;
}



