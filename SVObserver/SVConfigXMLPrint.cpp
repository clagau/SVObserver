//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigXMLPrint
//* .File Name       : $Workfile:   SVConfigXMLPrint.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   23 Jul 2014 10:34:38  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVConfigXMLPrint.h"
#include "ConfigurationOuttakes.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "RootObject.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVIODoc.h"
#include "SVToolGrouping.h"
#include "SVToolSet.h"
#include "AnalyzerOperators/BlobAnalyzer.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "ObjectInterfaces/IValueObject.h"
#include "Operators/SVResultDouble.h"
#include "Operators/SVShapeMaskHelperClass.h"
#include "SVFileAcquisitionDevice/SVFileAcquisitionLoadingModeEnum.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVArchiveTool.h"
#include "Tools/SVTool.h"
#include "Triggering/SVTriggerClass.h"
#include "Triggering/SVTriggerObject.h"
#pragma endregion Includes

const std::vector<SvPb::EmbeddedIdEnum>& NonPrintEmbeddeds();
const std::vector<SvPb::ClassIdEnum>& NonPrintClassIds();

const wchar_t* invalid = L"** I N V A L I D ! **";

const int cp_dflt = 1252;

const static wchar_t* XML_MonitorLists = L"RemoteMonitorLists";
const static wchar_t* XML_MonitorList = L"RemoteMonitorList";
const static wchar_t* XML_Name = L"Name";
const static wchar_t* XML_PPQName = L"PPQName";
const static wchar_t* XML_RejectQuueDepth = L"RejectQueueDepth";
const static wchar_t* XML_IsActive = L"IsActive";
const static wchar_t* XML_TRUE = L"TRUE";
const static wchar_t* XML_FALSE = L"FALSE";
const static wchar_t* XML_ProductValueItem = L"ProductValueItem";
const static wchar_t* XML_Item = L"Item";
const static wchar_t* XML_ProductImageList = L"ProductImageList";
const static wchar_t* XML_ProductImageItem = L"ProductImageItem";
const static wchar_t* XML_Image = L"Image";
const static wchar_t* XML_RejectConditionItem = L"RejectConditionItem";
const static wchar_t* XML_FailStatusList = L"FailStatusList";
const static wchar_t* XML_FailStatusItem = L"FailStatusItem";


namespace sv_xml
{

std::wstring space2underscore(std::wstring text)
{
	std::replace(text.begin(), text.end(), L' ', L'_');
	return text;
}

const std::wstring now()
{
	const std::wstring sep = L" ";
	typedef boost::posix_time::ptime ptime;
	typedef boost::posix_time::microsec_clock clock;
	ptime pt = clock::local_time();
	std::wstringstream ss;
	ss << pt.date() << sep << pt.time_of_day() << std::ends;
	return ss.str();
}

static SVToolGrouping GetToolGroupings(uint32_t inspectionId)
{
	// get the document that owns this inspection
	SVIPDoc* pDoc = GetIPDocByInspectionID(inspectionId);
	if (pDoc)
	{
		return pDoc->GetToolGroupings();
	}
	return SVToolGrouping();
}

static SVObjectClass* GetTool(const std::string& rName, const SvIe::SVTaskObjectListClass& rToolSet)
{
	SVObjectClass* pObject(nullptr);
	for (int i = 0; !pObject && i < rToolSet.GetSize(); i++)
	{
		std::string ToolName(rToolSet.getTaskObject(i)->GetName());
		if (ToolName == rName)
		{
			pObject = rToolSet.getTaskObject(i);
		}
	}
	return pObject;
}

const std::string SVConfigXMLPrint::Print() const
{
	m_cfo = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject(m_cfo);
	CComPtr<IXmlWriter> writer;
	/*HRESULT hr = */::CreateXmlWriter(__uuidof(IXmlWriter), reinterpret_cast<void**>(&writer), 0);
	writer->SetProperty(XmlWriterProperty_OmitXmlDeclaration, TRUE);
	CComPtr<IStream> stream;
	/*hr = */::CreateStreamOnHGlobal(nullptr, TRUE, &stream);

	/*hr = */writer->SetOutput(stream);
	PrintXMLDoc(writer);
	ULONG wrt;
	stream->Write("\0", 1, &wrt);
	HGLOBAL hg;
	/*hr = */::GetHGlobalFromStream(stream, &hg);
	return HG2String(hg)();
}

void SVConfigXMLPrint::PrintXMLDoc(Writer writer) const
{
	writer->WriteStartDocument(XmlStandalone::XmlStandalone_Yes);
	wchar_t head[] = L"<?xml version=\"1.0\"?>";
	writer->WriteRaw(head);

	{
		WriteStartEndElement Configuration(writer, nullptr, L"Configuration", nullptr);
		writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(getConfigFileName(), cp_dflt).c_str());
		writer->WriteAttributeString(nullptr, L"Path", nullptr, SvUl::to_utf16(getConfigFullFileName(), cp_dflt).c_str());
		writer->WriteAttributeString(nullptr, L"Timestamp", nullptr, sv_xml::now().c_str());

		{
			WriteStartEndElement Settings(writer, nullptr, L"Settings", nullptr);
			WriteTriggers(writer);
			WriteCameras(writer);
			WriteInspections(writer);
			WritePPQs(writer);
		}

		{
			WriteStartEndElement Details(writer, nullptr, L"Details", nullptr);
			WriteToolSets(writer);
			WriteIOSection(writer);
			WritePPQBar(writer);
			WriteExternalFiles(writer);
		}

	}
	writer->WriteEndDocument();
	writer->Flush();
}

void SVConfigXMLPrint::WriteTriggers(Writer writer) const
{
	typedef std::map<std::string, SvTrig::SVTriggerObject*> TriggerMap;
	TriggerMap triggers;
	long sz = 0;
	if (nullptr != m_cfo) { sz = m_cfo->GetTriggerCount(); }
	for (long i = 0; i < sz; ++i)
	{
		SvTrig::SVTriggerObject* pTrigger(nullptr);
		if (nullptr != m_cfo) { pTrigger = m_cfo->GetTrigger(i); }
		if (nullptr != pTrigger)
		{
			triggers[pTrigger->GetName()] = pTrigger;
		}
	}

	writer->WriteStartElement(nullptr, L"Triggers", nullptr);

	for (TriggerMap::const_iterator it = triggers.begin(); it != triggers.end(); ++it)
	{
		WriteTrigger(writer, it->second);
	}

	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteTrigger(Writer writer, SvTrig::SVTriggerObject* pTrigger) const
{
	ASSERT(nullptr != pTrigger);
	if (nullptr == pTrigger) { return; }
	writer->WriteStartElement(nullptr, L"Trigger", nullptr);
	writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pTrigger->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"DeviceName", nullptr,
		(nullptr != pTrigger->getDevice()) ? SvUl::to_utf16(pTrigger->getDevice()->GetDeviceName(), cp_dflt).c_str() : L"** Device **");
	if (SvDef::TriggerType::SoftwareTrigger == pTrigger->getType())
	{
		writer->WriteAttributeString(nullptr, L"Type", nullptr, L"Software");
		std::wostringstream os;
		os << pTrigger->GetSoftwareTriggerPeriod() << L" ms" << std::ends;
		writer->WriteAttributeString(nullptr, L"TimerPeriod", nullptr, os.str().c_str());
	}
	else
	{
		writer->WriteAttributeString(nullptr, L"Type", nullptr, L"Hardware");
	}
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteCameras(Writer writer) const
{
	typedef std::map<std::string, SvIe::SVVirtualCamera*> CameraMap;
	CameraMap cameras;
	long sz = 0;
	if (nullptr != m_cfo) { sz = m_cfo->GetCameraCount(); }
	for (long i = 0; i < sz; ++i)
	{
		SvIe::SVVirtualCamera* pCamera(nullptr);
		if (nullptr != m_cfo) { pCamera = m_cfo->GetCamera(i); }
		if (nullptr != pCamera)
		{
			cameras[pCamera->GetName()] = pCamera;
		}
	}

	writer->WriteStartElement(nullptr, L"Cameras", nullptr);

	for (CameraMap::const_iterator it = cameras.begin(); it != cameras.end(); ++it)
	{
		WriteCamera(writer, it->second);
	}

	writer->WriteEndElement();
}

const wchar_t* LoadingModeText(long mode)
{
	if (ContinuousMode == mode) return L"Continuous Load";
	if (SingleIterationMode == mode) return L"Single Iteration";
	return L"Single File";
}

void SVConfigXMLPrint::WriteCamera(Writer writer, SvIe::SVVirtualCamera* pCamera) const
{
	ASSERT(nullptr != pCamera);
	if (nullptr == pCamera) { return; }

	writer->WriteStartElement(nullptr, L"Camera", nullptr);
	writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"DeviceName", nullptr,
		(pCamera->GetAcquisitionDevice()) ? SvUl::to_utf16(pCamera->GetAcquisitionDevice()->GetDeviceName(), cp_dflt).c_str() : L"** No Device **");
	writer->WriteAttributeString(nullptr, L"AcquisitionType", nullptr,
		(pCamera->IsFileAcquisition()) ? L"File Acquisition" : L"Hardware Acquisition");
	if (pCamera->IsFileAcquisition())
	{
		WriteFileAcq(writer, pCamera);
	}
	else
	{
		WriteHardwareAcq(writer, pCamera);
	}
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteHardwareAcq(Writer writer, SvIe::SVVirtualCamera* pCamera) const
{
	writer->WriteStartElement(nullptr, L"HardwareAcquisition", nullptr);
	SVFileNameArrayClass* pfnac = nullptr;
	SVLightReference* plrcDummy = nullptr;
	SVLut* plutDummy = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;
	SvIe::SVAcquisitionClassPtr pAcqDevice;
	if (nullptr != pCamera) { pAcqDevice = pCamera->GetAcquisitionDevice(); }
	if (nullptr != pAcqDevice && nullptr != m_cfo && m_cfo->GetAcquisitionDevice(pAcqDevice->DeviceName().c_str(), pfnac, plrcDummy, plutDummy, pDeviceParams))
	{
		writer->WriteStartElement(nullptr, L"CameraFiles", nullptr);
		writer->WriteString(SvUl::to_utf16(pfnac->GetFileNameList(), cp_dflt).c_str());
		writer->WriteEndElement();
		if (pDeviceParams && pDeviceParams->mapParameters.size() > 0)
		{
			writer->WriteStartElement(nullptr, L"Parameters", nullptr);
			SVDeviceParamCollection l_CameraFileParams;
			/*HRESULT hr = */pAcqDevice->GetCameraFileParameters(l_CameraFileParams);
			const int iDetailLevel = 0;
			SVDeviceParamConfigXMLHelper helper(writer, l_CameraFileParams);

			SVDeviceParamIndexer index(pDeviceParams->mapParameters);	// print in order
			SVDeviceParamIndexer::iterator iter;
			for (iter = index.begin(); iter != index.end(); ++iter)
			{
				SVDeviceParamWrapper& rw = pDeviceParams->GetParameter(*iter);
				SVDeviceParam* pParam = static_cast <SVDeviceParam*> (rw);
				if (pParam)
				{
					SVDeviceParam* pCamFileParam = l_CameraFileParams.GetParameter(pParam->Type());
					if (nullptr == pCamFileParam)
						continue;
					if (pCamFileParam->DetailLevel() > iDetailLevel)
						continue;
					pParam->Accept(helper);
				}
			}
			writer->WriteEndElement();
		}
	}
	else
	{
		writer->WriteAttributeString(nullptr, L"Error", nullptr, L"** No acquisition device **");
	}
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteFileAcq(Writer writer, SvIe::SVVirtualCamera* pCamera) const
{
	ASSERT(nullptr != pCamera);
	if (nullptr == pCamera) { return; }

	writer->WriteStartElement(nullptr, L"FileAcquisition", nullptr);
	writer->WriteAttributeString(nullptr, L"LoadingMode", nullptr, LoadingModeText(pCamera->GetFileLoadingMode()));
	writer->WriteStartElement(nullptr, XML_Image, nullptr);
	writer->WriteAttributeString(nullptr, L"Filename", nullptr, SvUl::to_utf16(pCamera->GetImageFilename(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"Directory", nullptr, SvUl::to_utf16(pCamera->GetImageDirectoryName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"SizeMode", nullptr, (pCamera->IsFileImageSizeEditModeFileBased()) ? L"Use Image Size from File" : L"User Editable");
	if (pCamera->IsFileImageSizeEditModeFileBased())
	{
		wchar_t buff[64];
		writer->WriteAttributeString(nullptr, L"Width", nullptr, _itow(pCamera->GetFileImageWidth(), buff, 10));
		writer->WriteAttributeString(nullptr, L"Height", nullptr, _itow(pCamera->GetFileImageHeight(), buff, 10));
	}
	writer->WriteEndElement();
	writer->WriteEndElement();
}

const wchar_t* PPQModeText(SvDef::SVPPQOutputModeEnum mode)
{
	switch (mode)
	{
	case SvDef::SVPPQNextTriggerMode:	// Resets outputs on trigger. Writes outputs immediately.
		return L"NextTrigger";
	case SvDef::SVPPQTimeDelayMode:	// Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
		// Aborts waiting if new trigger occurs.
		return L"TimeDelay";
	case SvDef::SVPPQTimeDelayAndDataCompleteMode:	// Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
		// Aborts waiting if new trigger occurs.
		return L"TimeDelayDataCompletion";
	case SvDef::SVPPQExtendedTimeDelayMode:		// Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
														// Aborts waiting if product leaves PPQ.
		return L"ExtendedTimeDelayMode";
	case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode: // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
		// Aborts waiting if product leaves PPQ.
		return L"ExtendedTimeDelayDataCompletion";
	default:
		return invalid;
	}
}

void SVConfigXMLPrint::WritePPQs(Writer writer) const
{
	typedef std::map<std::string, SVPPQObject*> PPQMap;
	PPQMap ppqs;
	long lPPQ = 0;
	writer->WriteStartElement(nullptr, L"PPQs", nullptr);
	if (nullptr != m_cfo) { lPPQ = m_cfo->GetPPQCount(); }
	for (long l = 0; l < lPPQ; l++)
	{
		SVPPQObject* pPPQ(nullptr);

		if (nullptr != m_cfo) { pPPQ = m_cfo->GetPPQ(l); }
		if (nullptr != pPPQ)
		{
			ppqs[pPPQ->GetName()] = pPPQ;
		}
	}

	for (PPQMap::const_iterator it = ppqs.begin(); it != ppqs.end(); ++it)
	{
		SVPPQObject* pPPQ = it->second;
		//If nullptr then do next in list
		if (nullptr == pPPQ) { continue; }

		wchar_t buff[64];
		writer->WriteStartElement(nullptr, L"PPQ", nullptr);
		writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pPPQ->GetName(), cp_dflt).c_str());

		SvDef::SVPPQOutputModeEnum	enumPPQOutputMode{ pPPQ->getPPQOutputMode() };
		long	lPPQLength{ pPPQ->getPPQLength() };	// PPQLength
		long	lResetDelay{ pPPQ->getResetDelay() };	// PPQOutputResetDelay
		long	lDelayTime{ pPPQ->getOutputDelay() };	// PPQOutputDelayTime

		writer->WriteAttributeString(nullptr, L"Mode", nullptr, PPQModeText(enumPPQOutputMode));
		writer->WriteAttributeString(nullptr, L"Length", nullptr, _itow(lPPQLength, buff, 10));
		writer->WriteAttributeString(nullptr, L"OutputResetDelay", nullptr, _itow(lResetDelay, buff, 10));
		writer->WriteAttributeString(nullptr, L"OutputDelayTime", nullptr, _itow(lDelayTime, buff, 10));
		writer->WriteAttributeString(nullptr, L"MaxTriggerGap4Interest", nullptr, _itow(pPPQ->getMaxProcessingOffset4Interest(), buff, 10));

		if (nullptr != pPPQ->GetTrigger())
		{
			writer->WriteStartElement(nullptr, L"Trigger", nullptr);
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pPPQ->GetTrigger()->GetName(), cp_dflt).c_str());
			writer->WriteEndElement();
		}

		WritePPQCameras(writer, pPPQ);
		WritePPQInspections(writer, pPPQ);
		writer->WriteEndElement();
	}
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WritePPQCameras(Writer writer, SVPPQObject* pPPQ) const
{
	SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras(true);

	for (const auto* pCamera : cameraVector)
	{
		if (nullptr != pCamera)
		{
			writer->WriteStartElement(nullptr, L"Camera", nullptr);
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str());
			writer->WriteEndElement();
		}
	}
}

void SVConfigXMLPrint::WritePPQInspections(Writer writer, SVPPQObject* pPPQ) const
{
	InspectionMap inspections;
	long lSize = 0;
	pPPQ->GetInspectionCount(lSize);
	for (int intInspection = 0; intInspection < lSize; intInspection++)
	{
		SVInspectionProcess* pInspection(nullptr);
		pPPQ->GetInspection(intInspection, pInspection);

		if (nullptr != pInspection)
		{
			inspections[pInspection->GetName()] = pInspection;
		}
	}

	for (InspectionMap::const_iterator it = inspections.begin(); it != inspections.end(); ++it)
	{
		writer->WriteStartElement(nullptr, L"Inspection", nullptr);
		writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(it->second->GetName(), cp_dflt).c_str());
		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteInspections(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(nullptr, L"Inspections", nullptr);
	if (nullptr != m_cfo) { lSize = m_cfo->GetInspectionCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspect(nullptr);

		if (nullptr != m_cfo) { pInspect = m_cfo->GetInspection(l); }
		if (nullptr != pInspect)
		{
			inspections[pInspect->GetName()] = pInspect;
		}
	}

	for (InspectionMap::const_iterator it = inspections.begin(); it != inspections.end(); ++it)
	{
		writer->WriteStartElement(nullptr, L"Inspection", nullptr);
		writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(it->second->GetName(), cp_dflt).c_str());

		writer->WriteEndElement();
	}
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteToolSets(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(nullptr, L"InspectionProcesses", nullptr);
	if (nullptr != m_cfo) { lSize = m_cfo->GetInspectionCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspection(nullptr);

		if (nullptr != m_cfo) { pInspection = m_cfo->GetInspection(l); }
		if (nullptr != pInspection)
		{
			inspections[pInspection->GetName()] = pInspection;
		}
	}

	for (InspectionMap::const_iterator it = inspections.begin(); it != inspections.end(); ++it)
	{
		WriteToolSet(writer, it->second);
	}

	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteToolSet(Writer writer, SVInspectionProcess* pInspection) const
{
	ASSERT(nullptr != pInspection);
	if (nullptr == pInspection) { return; }

	nToolNumber = 0;
	writer->WriteStartElement(nullptr, L"InspectionProcess", nullptr);
	writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pInspection->GetName(), cp_dflt).c_str());
	WriteObject(writer, pInspection->GetToolSet());

	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteIOSection(Writer writer) const
{
	writer->WriteStartElement(nullptr, L"InputOutput", nullptr);
	WriteModuleIO(writer);
	WriteResultIO(writer);
	WriteMonitorListSection(writer);
	WriteGlobalConstants(writer);
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteResultIO(Writer writer) const
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());

	if (nullptr != pConfig && nullptr != pApp && GetTheIODoc())
	{
		long lPPQSize = pConfig->GetPPQCount();

		// Print Result Output title...
		long maxOutput {SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount()};
		writer->WriteStartElement(nullptr, L"ResultOutputs", nullptr);
		wchar_t buff[64];
		writer->WriteAttributeString(nullptr, L"NumberOfOutputs", nullptr, _itow(maxOutput, buff, 10));

		// Result Outputs
		for (long i = 0; i < maxOutput; ++i)
		{
			SVIOEntryHostStructPtr pModuleReady = pConfig->GetModuleReady();

			// Check Module Ready first
			SVDigitalOutputObject* pDigOutput = (nullptr == pModuleReady) ? nullptr : dynamic_cast<SVDigitalOutputObject*>(SVObjectManagerClass::Instance().GetObject(pModuleReady->m_IOId));
			if (pDigOutput)
			{
				if (i == pDigOutput->GetChannel())
				{
					writer->WriteStartElement(nullptr, L"DigitalOutput", nullptr);
					writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
					WriteIOEntryObject(writer, pModuleReady);
					//writer->WriteComment(L"First");
					writer->WriteEndElement();
					continue;
				}// end if
			}// end if

			for (int j = 0; j < lPPQSize; j++)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ(j);
				if (nullptr != pPPQ)
				{
					// Find each digital output
					for (const auto& pIoEntry : pPPQ->GetAllOutputs())
					{
						if (pIoEntry->m_ObjectType != IO_DIGITAL_OUTPUT)
						{
							continue;
						}

						pDigOutput = dynamic_cast<SVDigitalOutputObject*>(SVObjectManagerClass::Instance().GetObject(pIoEntry->m_IOId));

						if (!pDigOutput)
						{
							continue;
						}

						if (i == pDigOutput->GetChannel())
						{
							writer->WriteStartElement(nullptr, L"DigitalOutput", nullptr);
							writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
							WriteIOEntryObject(writer, pIoEntry);
							writer->WriteEndElement();
							continue;
						}// end if
					}
				}
			}// end for j
		}// end for
		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteModuleIO(Writer writer) const
{
	CPoint				ptTemp(0, 0);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	// Print IODoc contents
	if (GetTheIODoc())
	{
		SVInputObjectList* pInputList = nullptr;
		// Get list of available inputs
		if (nullptr != pConfig) { pInputList = pConfig->GetInputObjectList(); }
		if (nullptr != pInputList)
		{
			SVIOEntryHostStructPtrVector inputEntryVector = pInputList->getInputList();
			long lSize = static_cast<long>(inputEntryVector.size());

			// Print module input title...
			long maxInput {SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount()};

			writer->WriteStartElement(nullptr, L"DigitalInputs", nullptr);
			wchar_t				buff[64];
			writer->WriteAttributeString(nullptr, L"NumberOfInputs", nullptr, _itow(maxInput, buff, 10));

			// Module Inputs
			for (long i = 0; i < maxInput; ++i)
			{
				writer->WriteStartElement(nullptr, L"DigitalInput", nullptr);
				writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
				// Find each digital input
				for (int j = 0; j < lSize; j++)
				{
					if (inputEntryVector[j]->m_ObjectType != IO_DIGITAL_INPUT)
						continue;

					SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(inputEntryVector[j]->m_IOId);

					SVDigitalInputObject* pDigInput = dynamic_cast<SVDigitalInputObject*>(l_pObject);

					if (!pDigInput)
						continue;

					if (i == pDigInput->GetChannel())
					{
						WriteIOEntryObject(writer, inputEntryVector[j]);
						break;
					}
				}
				writer->WriteEndElement();
			}

			writer->WriteEndElement();

			// Find each Remote input
			int j = 0;
			for (int i = 0; i < lSize; i++)
			{
				if (inputEntryVector[i]->m_ObjectType == IO_REMOTE_INPUT)
				{
					++j;
				}
			}
			writer->WriteStartElement(nullptr, L"RemoteInputs", nullptr);
			writer->WriteAttributeString(nullptr, L"NumberOfInputs", nullptr, _itow(j, buff, 10));

			if (j)
			{
				for (int k = 0, l = 0; k < lSize; k++)
				{
					if (inputEntryVector[k]->m_ObjectType != IO_REMOTE_INPUT)
					{
						continue;
					}

					writer->WriteStartElement(nullptr, L"RemoteInput", nullptr);
					writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow((l++) + 1, buff, 10));
					WriteIOEntryObject(writer, inputEntryVector[k]);
					writer->WriteEndElement();
				}
			}
			writer->WriteEndElement();
		}
	}
}

void SVConfigXMLPrint::WriteMonitorListSection(Writer writer) const
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		writer->WriteStartElement(nullptr, XML_MonitorLists, nullptr);

		const RemoteMonitorListMap& remoteMonitorLists = pConfig->GetRemoteMonitorList();

		RemoteMonitorListMap::const_iterator iterMonitorList = remoteMonitorLists.begin();
		while (remoteMonitorLists.end() != iterMonitorList)
		{
			const std::string& ListName = iterMonitorList->first;
			const RemoteMonitorNamedList& monitorList = iterMonitorList->second;

			writer->WriteStartElement(nullptr, XML_MonitorList, nullptr);

			//Attribute monitorlistname
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(ListName.c_str(), cp_dflt).c_str());

			//Attribute  PPQname
			writer->WriteAttributeString(nullptr, XML_PPQName, nullptr, SvUl::to_utf16(monitorList.GetPPQName().c_str(), cp_dflt).c_str());

			//Attribute  Queue Depth
			int Depth = monitorList.GetRejectDepthQueue();
			wchar_t				buff[64];
			writer->WriteAttributeString(nullptr, XML_RejectQuueDepth, nullptr, _itow(Depth, buff, 10));

			//write Active Flag 
			bool isActive = monitorList.IsActive();
			writer->WriteAttributeString(nullptr, XML_IsActive, nullptr, isActive ? XML_TRUE : XML_FALSE);

			//write Product Value List
			int ItemCount = 0;
			const MonitoredObjectList& ValueList = monitorList.GetProductValuesList();
			MonitoredObjectList::const_iterator vlIt = ValueList.begin();
			writer->WriteStartElement(nullptr, L"ProductValueList", nullptr);
			while (vlIt != ValueList.end())
			{
				ItemCount++;
				const MonitoredObject& rObj = *vlIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if (!objectName.empty())
				{
					writer->WriteStartElement(nullptr, XML_ProductValueItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				++vlIt;
			}
			writer->WriteEndElement();

			// Write Product Image List
			ItemCount = 0;
			const MonitoredObjectList& ImageList = monitorList.GetProductImagesList();
			MonitoredObjectList::const_iterator ilIt = ImageList.begin();
			writer->WriteStartElement(nullptr, XML_ProductImageList, nullptr);
			while (ilIt != ImageList.end())
			{
				ItemCount++;
				const MonitoredObject& rObj = *ilIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if (!objectName.empty())
				{
					writer->WriteStartElement(nullptr, XML_ProductImageItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Image, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				++ilIt;
			}
			writer->WriteEndElement();

			// Write Reject Condition List
			ItemCount = 0;
			const MonitoredObjectList& RejectList = monitorList.GetRejectConditionList();
			MonitoredObjectList::const_iterator rlIt = RejectList.begin();
			writer->WriteStartElement(nullptr, L"RejectConditionList", nullptr);
			while (rlIt != RejectList.end())
			{
				ItemCount++;
				const MonitoredObject& rObj = *rlIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if (!objectName.empty())
				{
					writer->WriteStartElement(nullptr, XML_RejectConditionItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				++rlIt;
			}
			writer->WriteEndElement();

			// Write Fail Status List
			ItemCount = 0;
			const MonitoredObjectList& FailList = monitorList.GetFailStatusList();
			MonitoredObjectList::const_iterator flIt = FailList.begin();
			writer->WriteStartElement(nullptr, XML_FailStatusList, nullptr);
			while (flIt != FailList.end())
			{
				ItemCount++;
				const MonitoredObject& rObj = *flIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if (!objectName.empty())
				{
					writer->WriteStartElement(nullptr, XML_FailStatusItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				++flIt;
			}
			writer->WriteEndElement();

			++iterMonitorList;

			writer->WriteEndElement();//RemoteMonitroList
		}

		writer->WriteEndElement();//RemoteMonitroLists
	}
}


void SVConfigXMLPrint::WritePPQBar(Writer writer) const
{

	WriteStartEndElement writerPPQBar(writer, nullptr, L"PPQBar", nullptr);
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	long	lPPQ = 0;
	//The nullptr check here is enough because then lPPQ would be 0
	if (nullptr != pConfig) { lPPQ = pConfig->GetPPQCount(); }
	for (int intPPQ = 0; intPPQ < lPPQ; intPPQ++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(intPPQ);

		if (nullptr != pPPQ)
		{
			//std::wstring t = SvUl::to_utf16(pPPQ->GetName(), cp_dflt).c_str();
			WriteStartEndElement writerPPQName(writer, nullptr, SvUl::to_utf16(pPPQ->GetName(), cp_dflt).c_str(), nullptr);

			for (int intPPQPos = 0; intPPQPos < pPPQ->getPPQLength(); intPPQPos++)
			{
				bool	bPosPrint = false;
				SvIe::SVVirtualCameraPtrVector cameraVector = pPPQ->GetVirtualCameras(true);

				wchar_t				buff[64];
				for (const auto* const pCamera : cameraVector)
				{
					if (nullptr != pCamera)
					{
						long lPos = -1;
						pPPQ->GetCameraPPQPosition(lPos, pCamera);

						if (lPos == intPPQPos)
						{
							WriteStartEndElement writerCameraName(writer, nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str(), nullptr);

							if (!bPosPrint)
							{
								std::wstring temp = _itow(intPPQPos + 1, buff, 10);
								writer->WriteAttributeString(nullptr, L"Position", nullptr, _itow(intPPQPos + 1, buff, 10));
								bPosPrint = true;
							}

						}
					}
				}

				if (0 == pPPQ->GetAllInputs().size())
				{
					writer->WriteComment(L"No IO Entries");
				}

				for (const auto& pEntry : pPPQ->GetAllInputs())
				{
					if (pEntry->m_PPQIndex == intPPQPos)
					{

						SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pEntry->m_IOId);
						if (nullptr != l_pObject)
						{
							if (l_pObject->IsCreated())
							{
								WriteStartEndElement writerObjectName(writer, nullptr, SvUl::to_utf16(l_pObject->GetName(), cp_dflt).c_str(), nullptr);
								if (!bPosPrint)
								{
									std::wstring t = _itow(intPPQPos + 1, buff, 10);
									writer->WriteAttributeString(nullptr, L"Position", nullptr, _itow(intPPQPos + 1, buff, 10));
									bPosPrint = true;
								}

							}//end if
						}
						else
						{
							if (pEntry->getObject()->IsCreated())
							{
								//std::wstring t = SvUl::to_utf16(pEntry->getObject()->GetName(), cp_dflt);
								WriteStartEndElement writerObjectName(writer, nullptr, SvUl::to_utf16(pEntry->getObject()->GetName(), cp_dflt).c_str(), nullptr);
								if (!bPosPrint)
								{
									std::wstring t = _itow(intPPQPos + 1, buff, 10);
									writer->WriteAttributeString(nullptr, L"Position", nullptr, _itow(intPPQPos + 1, buff, 10));
									bPosPrint = true;
								}

							} // end if
						} //end else
					}
				}
			}

		}
	}

}

std::wstring utf16(const std::string& str) { return SvUl::to_utf16(str.c_str(), cp_dflt); }

void SVConfigXMLPrint::WriteValueObject(Writer writer, SVObjectClass* pObj) const
{
	if (pObj->ObjectAttributesAllowed() & SvPb::audittrail)
	{
		writer->WriteStartElement(nullptr, SvUl::to_utf16(pObj->GetClassName(), cp_dflt).c_str(), nullptr);
		writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pObj->GetName(), cp_dflt).c_str());

		bool bGotValue {false};
		std::string sValue;
		if (SvVol::SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SvVol::SVDWordValueObjectClass*> (pObj))
		{
			DWORD dwValue = 0;
			bGotValue = (S_OK == pdwValueObject->GetValue(dwValue));
			sValue = SvUl::AsString(dwValue);
		}
		else if (SvVol::LinkedValue* pLinkedValue = dynamic_cast<SvVol::LinkedValue*>(pObj))
		{
			writer->WriteAttributeString(nullptr, L"Type", nullptr, utf16(std::to_string(pLinkedValue->getSelectedOption())).c_str() );
			writer->WriteAttributeString(nullptr, L"Content", nullptr, SvUl::to_utf16(pLinkedValue->getContentStr(), cp_dflt).c_str());
		}
		else
		{
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObj);
			bGotValue = (S_OK == pValueObject->getValue(sValue));
		}

		if (bGotValue && !sValue.empty())
		{
			writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(sValue, cp_dflt).c_str());
		}

		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteArchiveTool(Writer writer, SvTo::SVArchiveTool* ar) const
{
	if (ar)
	{
		wchar_t buff[64];

		writer->WriteStartElement(nullptr, L"Results", nullptr);
		const auto& rRecVec = ar->m_ResultCollection.getRecordVec();
		for (int i = 0; i < rRecVec.size(); ++i)
		{
			if (nullptr != rRecVec[i].GetObjectReference().getObject())
			{
				writer->WriteStartElement(nullptr, L"Result", nullptr);
				writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
				writer->WriteAttributeString(nullptr, XML_Name, nullptr, utf16(rRecVec[i].GetObjectReference().GetCompleteName()).c_str());
				writer->WriteEndElement();
			}
		}

		writer->WriteEndElement();
		writer->WriteStartElement(nullptr, L"Images", nullptr);

		const auto& rRecImageVec = ar->m_ImageCollection.getRecordVec();
		for (int i = 0; i < rRecImageVec.size(); ++i)
		{
			writer->WriteStartElement(nullptr, L"Result", nullptr);
			writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, utf16(std::string(rRecImageVec[i].GetImageObjectName())).c_str());
			writer->WriteEndElement();
		}
		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteObject(Writer writer, SVObjectClass* pObject) const
{
	SvPb::ClassIdEnum classID = pObject->GetClassID();
	const auto& nonPCIds = NonPrintClassIds();
	auto result = std::find_if(std::begin(nonPCIds), std::end(nonPCIds), [&classID](const auto& rEntry) { return classID == rEntry; });
	if (result != std::end(nonPCIds))
	{
		return;
	}

	// If object is a value object, get embedded ID which is NonPrintable.
	if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
	{
		SvPb::EmbeddedIdEnum embeddedID = pObject->GetEmbeddedID();
		const auto& nPrs = NonPrintEmbeddeds();
		// Check for non-printing object type.
		auto iter = std::find_if(std::begin(nPrs), std::end(nPrs), [&embeddedID](const auto& rEntry) { return embeddedID == rEntry; });
		if (iter != std::end(nPrs))
		{
			return;
		}
	}

	// If object is a value object, print name and value.
	if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
	{
		WriteValueObject(writer, pObject);
	}
	else
	{
		std::string sLabel, sValue;
		std::string  strType = pObject->GetObjectName();
		std::string  strName = pObject->GetName();

		do
		{
			if (dynamic_cast <SvOp::SVShapeMaskHelperClass*> (pObject))
			{
				if (!(pObject->ObjectAttributesAllowed() & SvPb::audittrail))	// EB 20050818 - hack this instead of doing it right
				{
					break;
				}
			}

			writer->WriteStartElement(nullptr, SvUl::to_utf16(pObject->GetClassName(), cp_dflt).c_str(), nullptr);
			std::wstring objName = SvUl::to_utf16(pObject->GetName(), cp_dflt);
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, objName.c_str());
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject);
			if (nullptr != pTool)
			{
				// Increment even if disabled, to maintain count.  Starts with zero, so for first
				//    tool, will increment to 1.
				nToolNumber++;
				wchar_t				buff[64];
				writer->WriteAttributeString(nullptr, L"ToolNumber", nullptr, _itow(nToolNumber, buff, 10));
			}

			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.empty())
			{
				writer->WriteAttributeString(nullptr, L"Type", nullptr, SvUl::to_utf16(strType, cp_dflt).c_str());
			}

			if (SvTo::SVArchiveTool* pArchiveTool = dynamic_cast <SvTo::SVArchiveTool*> (pObject))
			{
				WriteArchiveTool(writer, pArchiveTool);
			}

			if (SvOp::SVDoubleResult* pBlobResult = dynamic_cast<SvOp::SVDoubleResult*>(pObject))
			{
				if (SV_IS_KIND_OF(pBlobResult->GetParent(), SvAo::BlobAnalyzer))
				{
					sLabel = SvUl::LoadStdString(IDS_BLOB_FEATURE_DEFAULT_VALUE) + _T(":");
					const SvVol::SVDoubleValueObjectClass* pDoubleValueObj = dynamic_cast<const SvVol::SVDoubleValueObjectClass*> (pBlobResult->getInput());
					if (pDoubleValueObj)
					{
						double dVal = pDoubleValueObj->GetDefaultValue();
						sValue = SvUl::Format(_T("%lf"), dVal);
						//ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
						//PrintValueObject(pDC, ptCurPos, utf16(sLabel), utf16(sValue));

						WriteValueObject(writer, L"Property", utf16(sLabel), utf16(sValue));

					}
				}
			}

			auto* pTask = dynamic_cast<SvIe::SVTaskObjectClass*>(pObject);
			if (nullptr != pTask)
			{
				if (SvOp::SVEquation* pEquation = dynamic_cast <SvOp::SVEquation*> (pObject))
				{
					sValue = pEquation->GetEquationText();

					// Write the equation text.
					if (sLabel.empty())
					{
						sLabel = _T("EquationText");
					}
					WriteValueObject(writer, L"Equation", utf16(sLabel), utf16(sValue));
				}
				WriteInputOutputList(writer, pTask);
				WriteFriends(writer, pTask);
			}

			WriteChildren(writer, pObject);
			writer->WriteComment(objName.c_str());
			writer->WriteEndElement();
		} while (false);// end do
	}// End if else pObj kind of SVValueObjectClass

	//writer->WriteEndElement();
}  // end function void SVConfigXMLPrint:::PrintDetails( ... )

void SVConfigXMLPrint::WriteAllChildren(Writer writer, SvIe::SVTaskObjectListClass* pTaskObj) const
{
	for (int nCnt = 0; nCnt < pTaskObj->GetSize(); nCnt++)
	{
		SVObjectClass* pChild = pTaskObj->getTaskObject(nCnt);
		if (pChild)
		{
			WriteObject(writer, pChild);
		}
	}
}

void SVConfigXMLPrint::WriteChildren(Writer writer, SVObjectClass* pObj) const
{
	if (SvIe::SVTaskObjectListClass* pTaskObj = dynamic_cast <SvIe::SVTaskObjectListClass*> (pObj))
	{
		writer->WriteStartElement(nullptr, L"Children", nullptr);
		if (SVToolSet* pToolSet = dynamic_cast <SVToolSet*>(pObj))
		{
			SVToolGrouping toolGroupings = GetToolGroupings(pToolSet->GetInspection()->getObjectId());
			if (toolGroupings.size())
			{
				bool bToolGroupActive = false;
				for (SVToolGrouping::iterator it = toolGroupings.begin(); it != toolGroupings.end(); ++it)
				{
					switch (it->second.m_type)
					{
					case ToolGroupData::StartOfGroup:
					{
						if (bToolGroupActive)
						{
							writer->WriteEndElement();
						}
						bToolGroupActive = true;
						writer->WriteStartElement(nullptr, L"ToolGrouping", nullptr);
						std::wstring name = SvUl::to_utf16(it->first.c_str(), cp_dflt);
						writer->WriteAttributeString(nullptr, XML_Name, nullptr, name.c_str());
					}
					break;

					case ToolGroupData::EndOfGroup:
					{
						writer->WriteStartElement(nullptr, L"EndToolGrouping", nullptr);
						std::wstring name = SvUl::to_utf16(it->first.c_str(), cp_dflt);
						writer->WriteAttributeString(nullptr, XML_Name, nullptr, name.c_str());
						writer->WriteEndElement();
						writer->WriteEndElement(); // end of start group
						bToolGroupActive = false;
					}
					break;

					case ToolGroupData::Tool:
					{
						SVObjectClass* pTool = GetTool(it->first.c_str(), *pTaskObj);
						if (pTool)
						{
							WriteObject(writer, pTool);
						}
					}
					break;
					}
				}
				if (bToolGroupActive)
				{
					writer->WriteEndElement();
				}
			}
			else
			{
				WriteAllChildren(writer, pTaskObj);
			}
		}
		else
		{
			WriteAllChildren(writer, pTaskObj);
		}
		writer->WriteEndElement();
	}
}  // end function void SVConfigXMLPrint:::PrintChildren( ... )

void SVConfigXMLPrint::WriteFriends(Writer writer, SvIe::SVTaskObjectClass* pObj) const
{
	auto rFriendList = pObj->GetFriendList().getContainerCopy();
	size_t sz = rFriendList.size();

	if (sz > 0)
	{
		writer->WriteStartElement(nullptr, L"Friends", nullptr);
		for (auto* pFriend : rFriendList)
		{
			if (pFriend)
			{
				WriteObject(writer, pFriend);
			}  // end if( rObjInfo )
		}  // end for( int nCnt = 0; nCnt < sz; nCnt++ )
		writer->WriteEndElement();
	}
}  // end function void SVConfigXMLPrint:::PrintFriends( ... )

void SVConfigXMLPrint::WriteInputOutputList(Writer writer, SvIe::SVTaskObjectClass* pTaskObj) const
{
	if (nullptr != pTaskObj)
	{
		const auto& rInputList = pTaskObj->GetPrivateInputList();
		if (0 < rInputList.size())
		{
			bool firstFlag = true;
			for (auto* pInput : rInputList)
			{
				if (nullptr != pInput && pInput->IsConnected() && SvPb::noAttributes != pInput->ObjectAttributesAllowed())
				{
					if (firstFlag)
					{
						writer->WriteStartElement(nullptr, L"Inputs", nullptr);
						firstFlag = false;
					}
					WriteValueObject(writer, L"Input", SvUl::to_utf16(pInput->GetName()), SvUl::to_utf16(pInput->GetInputObjectInfo().getObject()->GetObjectNameToObjectType().c_str(), cp_dflt).c_str());
				}
			}
			if (false == firstFlag)
			{
				writer->WriteEndElement();
			}
		}

		std::vector<SvOi::IObjectClass*> outputList;
		pTaskObj->getOutputList(std::back_inserter(outputList));
		for (auto* pIObject : outputList)
		{
			auto* pObject = dynamic_cast<SVObjectClass*> (pIObject);
			if (nullptr != pObject && pObject->GetParent() == pTaskObj)
			{
				WriteObject(writer, pObject);
			}
		}
	}
}  // end function void SVConfigXMLPrint:::PrintInputOutputList( ... )

void SVConfigXMLPrint::WriteValueObject(Writer writer, const std::wstring& rTag, const std::wstring& rName, const std::wstring& rValue) const
{
	writer->WriteStartElement(nullptr, rTag.c_str(), nullptr);
	writer->WriteAttributeString(nullptr, XML_Name, nullptr, rName.c_str());
	writer->WriteAttributeString(nullptr, L"Value", nullptr, rValue.c_str());
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const
{
	std::wstring 			sValue;
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(IOEntry->m_IOId);
	writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(l_pObject->GetName(), cp_dflt).c_str());

	switch (IOEntry->m_ObjectType)
	{
	case IO_DIGITAL_INPUT:
	{
		SVDigitalInputObject* pDigInput = dynamic_cast<SVDigitalInputObject*>(l_pObject);
		if (pDigInput->IsForced())
			sValue = pDigInput->GetForcedValue() ? L"1" : L"0";
		else
			sValue = L" ";
		WriteValueObject(writer, L"Property", L"Forced", sValue.c_str());

		WriteValueObject(writer, L"Property", L"Inverted", pDigInput->IsInverted() ? L"1" : L" ");
		break;
	}

	case IO_DIGITAL_OUTPUT:
	{
		SVDigitalOutputObject* pDigOutput = dynamic_cast<SVDigitalOutputObject*>(l_pObject);
		if (pDigOutput->IsForced())
			sValue = pDigOutput->GetForcedValue() ? L"1" : L"0";
		else
			sValue = L" ";
		WriteValueObject(writer, L"Property", L"Forced", sValue.c_str());

		sValue = pDigOutput->IsInverted() ? L"1" : L" ";
		WriteValueObject(writer, L"Property", L"Inverted", sValue.c_str());

		sValue = pDigOutput->isCombined() ? L"1" : L" ";
		WriteValueObject(writer, L"Property", L"Combined", sValue.c_str());

		sValue = pDigOutput->isAndACK() ? L"AND w ACK" : L"OR w NAK";
		WriteValueObject(writer, L"Property", L"Combined using", sValue.c_str());
		break;
	}

	case IO_REMOTE_INPUT:
		break;

	case IO_REMOTE_OUTPUT:
		break;
	}
}

void SVConfigXMLPrint::WriteGlobalConstants(Writer writer) const
{
	std::string Value;
	int Index(0);

	SvVol::BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList(GlobalConstantObjects, SvDef::FqnGlobal);

	writer->WriteStartElement(nullptr, L"GlobalConstants", nullptr);

	SvVol::BasicValueObjects::ValueVector::const_iterator Iter(GlobalConstantObjects.cbegin());
	while (GlobalConstantObjects.cend() != Iter)
	{
		const SvVol::BasicValueObjectPtr& pGlobalConstant = *Iter;

		if (nullptr != pGlobalConstant)
		{
			Value = SvUl::Format(L"GlobalConstant%d", ++Index);
			writer->WriteStartElement(nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str(), nullptr);
			//write Global Constant Name
			Value = pGlobalConstant->GetCompleteName();
			writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str());
			//write Global Constant Value
			pGlobalConstant->getValue(Value);
			writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str());
			//write Global Constant type
			Value = pGlobalConstant->getTypeName();
			writer->WriteAttributeString(nullptr, L"Type", nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str());
			writer->WriteEndElement();//GlobalConstantx
		}
		++Iter;
	}

	writer->WriteEndElement();//GlobalConstants
}


//////////////////////////////////////////////////////////////////
// Visitor helper

SVDeviceParamConfigXMLHelper::SVDeviceParamConfigXMLHelper(
	Writer writer, SVDeviceParamCollection& rCamFileParams)
	: m_writer(writer), m_rCamFileParams(rCamFileParams)
{
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVDeviceParam&)
{
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVLongValueDeviceParam& param)
{
	const SVLongValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pCamFileParam);
	if (pCamFileParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		if (pCamFileParam->info.options.size() > 0)
		{
			SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find(pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.lValue);
			if (iterOption != pCamFileParam->info.options.end())
			{
				Value = iterOption->m_Description;
			}
		}
		if (Value.empty())
		{
			Value = SvUl::Format(_T("%lu%s"), static_cast<unsigned long> (param.GetScaledValue()), param.info.sUnits.empty() ? _T("") : std::string(_T(" ") + param.info.sUnits).c_str());
		}
		m_writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVi64ValueDeviceParam& param)
{
	const SVi64ValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pCamFileParam);
	if (pCamFileParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Text = SvUl::Format("%I64d", param.iValue);
		m_writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Text, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVBoolValueDeviceParam& param)
{
	const SVBoolValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pCamFileParam);
	if (pCamFileParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		if (pCamFileParam->info.options.size() > 0)
		{
			SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find(pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.bValue);
			if (iterOption != pCamFileParam->info.options.end())
			{
				Value = iterOption->m_Description.c_str();
			}
		}
		if (Value.empty())
		{
			Value = param.bValue ? _T("TRUE") : _T("FALSE");
		}
		m_writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVStringValueDeviceParam& param)
{
	const SVStringValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pCamFileParam);
	if (pCamFileParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		m_writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(param.strValue.c_str(), cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVParamListDeviceParam&)
{
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVLutDeviceParam&)
{
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVLightReferenceDeviceParam&)
{
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVCameraFormatsDeviceParam& param)
{
	const SVCameraFormatsDeviceParam* pCamFileParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pCamFileParam);
	if (pCamFileParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		SVCameraFormat* pFormat = nullptr;
		if (param.options.size() > 0)
		{
			SVCameraFormat& rFormat = param.options[param.strValue];
			const SVCameraFormat& rCamFileFormat = pCamFileParam->options.find(param.strValue)->second;
			if (rCamFileFormat.m_strName == param.strValue)
			{
				Value = rCamFileFormat.m_strDescription;
				pFormat = &rFormat;
			}
		}
		if (Value.empty())
		{
			Value = param.strValue;
		}
		m_writer->WriteAttributeString(nullptr, XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str());

		if (nullptr != pFormat)
		{
			m_writer->WriteAttributeString(nullptr, L"Left", nullptr, SvUl::to_utf16(SvUl::AsString(pFormat->m_lHPos).c_str(), cp_dflt).c_str());
			m_writer->WriteAttributeString(nullptr, L"Top", nullptr, SvUl::to_utf16(SvUl::AsString(pFormat->m_lVPos).c_str(), cp_dflt).c_str());
			m_writer->WriteAttributeString(nullptr, L"Width", nullptr, SvUl::to_utf16(SvUl::AsString(pFormat->m_lWidth).c_str(), cp_dflt).c_str());
			m_writer->WriteAttributeString(nullptr, L"Height", nullptr, SvUl::to_utf16(SvUl::AsString(pFormat->m_lHeight).c_str(), cp_dflt).c_str());
		}
		m_writer->WriteEndElement();
	}
	return S_OK;
}

HRESULT SVDeviceParamConfigXMLHelper::Visit(SVCustomDeviceParam& param)
{
	const SVCustomDeviceParam* pParam = m_rCamFileParams.Parameter(param.Type()).DerivedValue(pParam);
	if (pParam)
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		SVDeviceParamWrapper w(pParam->GetHeldParam());
		SVDeviceParam* p = w;
		Visit(*p);
		m_writer->WriteEndElement();
	}
	return S_OK;
}


void SVConfigXMLPrint::WriteExternalFiles(Writer writer) const
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr == pConfig)
	{
		return;
	}
	pConfig->UpdateAuditFiles(true);
	{
		WriteStartEndElement ExternalFiles(writer, nullptr, L"External_Files", nullptr);
		auto DefaultList = pConfig->GetAuditDefaultList();
		for (auto& Element : DefaultList)
		{
			if (Element.bignore == false)
			{
				WriteStartEndElement FilesElement(writer, nullptr, L"File", nullptr);

				writer->WriteAttributeString(nullptr, L"Name", nullptr, SvUl::to_utf16(Element.GetFilename().c_str(), cp_dflt).c_str());
				{
					WriteStartEndElement FullNameElement(writer, nullptr, L"FullName", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFullname().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement WriteDataElement(writer, nullptr, L"WriteDate", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFormatedWriteDate().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement SizeElement(writer, nullptr, L"Size", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFormatedSize().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement HashElement(writer, nullptr, L"hash", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetHashvalue().c_str(), cp_dflt).c_str());
				}

			}

		}
	}

	{
		WriteStartEndElement AdditionalExternalFiles(writer, nullptr, L"Aditional_External_Files", nullptr);
		auto WhiteList = pConfig->GetAuditWhiteList();
		for (auto& Element : WhiteList)
		{
			if (Element.bignore == false)
			{
				WriteStartEndElement FileElement(writer, nullptr, L"File", nullptr);
				writer->WriteAttributeString(nullptr, L"Name", nullptr, SvUl::to_utf16(Element.GetFilename().c_str(), cp_dflt).c_str());

				{
					WriteStartEndElement FullNameElement(writer, nullptr, L"FullName", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFullname().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement WriteDateElement(writer, nullptr, L"WriteDate", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFormatedWriteDate().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement SizeElement(writer, nullptr, L"Size", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetFormatedSize().c_str(), cp_dflt).c_str());

				}
				{
					WriteStartEndElement HashElement(writer, nullptr, L"hash", nullptr);
					writer->WriteString(SvUl::to_utf16(Element.GetHashvalue().c_str(), cp_dflt).c_str());
				}
			}
		}

	}
}

} //namespace sv_xml
