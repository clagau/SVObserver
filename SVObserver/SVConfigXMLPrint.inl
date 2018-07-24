//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigXMLPrint
//* .File Name       : $Workfile:   SVConfigXMLPrint.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   23 Jul 2014 10:34:38  $
//******************************************************************************

#pragma region Includes
#include "SVUtilityLibrary/StringHelper.h"
#include "SVBlobAnalyzer.h"
#include "SVResultDouble.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "RootObject.h"
#include "SVObserver.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

std::pair<GUID **, size_t> NonPrintGuids();

typedef sv_xml::SVConfigXMLPrint SVConfigXMLPrint;

typedef sv_xml::SVDeviceParamConfigXMLHelper SVDeviceParamConfigXMLHelper;

const wchar_t * invalid = L"** I N V A L I D ! **";

const int cp_dflt = 1252;

const static wchar_t*   XML_MonitorLists = L"RemoteMonitorLists";
const static wchar_t*   XML_MonitorList = L"RemoteMonitorList";
const static wchar_t*   XML_Name = L"Name";
const static wchar_t*   XML_PPQName  = L"PPQName";
const static wchar_t*   XML_RejectQuueDepth =  	L"RejectQueueDepth";
const static wchar_t*   XML_IsActive=	L"IsActive";
const static wchar_t*   XML_TRUE=  L"TRUE";
const static wchar_t*   XML_FALSE =  	L"FALSE";
const static wchar_t*   XML_ProductValueItem = 	 L"ProductValueItem";
const static wchar_t*   XML_Item = 	  L"Item";
const static wchar_t*   XML_ProductImageList = 	  L"ProductImageList";
const static wchar_t*   XML_ProductImageItem	  =  L"ProductImageItem";
const static wchar_t*   XML_Image = L"Image";
const static wchar_t*   XML_RejectConditionItem  = 		L"RejectConditionItem";
const static wchar_t*   XML_FailStatusList =  L"FailStatusList";
const static wchar_t*   XML_FailStatusItem = L"FailStatusItem";



static SVToolGrouping GetToolGroupings(const SVGUID& rInspectionGuid)
{
	// get the document that owns this inspection
	SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(rInspectionGuid);
	if (pDoc)
	{
		return pDoc->GetToolGroupings();
	}
	return SVToolGrouping();
}

static SVObjectClass* GetTool(const std::string& rName, const SVTaskObjectListClass& rToolSet)
{
	SVObjectClass* pObject(nullptr);
	for (int i = 0; !pObject && i < rToolSet.GetSize(); i++)
	{
		std::string ToolName( rToolSet.GetAt(i)->GetName() );
		if ( ToolName == rName) 
		{
			pObject = rToolSet.GetAt(i);
		}
	}
	return pObject;
}

inline const std::string SVConfigXMLPrint::Print() const
{
	m_cfo = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject( m_cfo );
	CComPtr<IXmlWriter> writer;
	HRESULT hr = ::CreateXmlWriter(__uuidof(IXmlWriter), reinterpret_cast<void**>(&writer), 0);
	writer->SetProperty(XmlWriterProperty_OmitXmlDeclaration, TRUE);
	CComPtr<IStream> stream;
	hr = ::CreateStreamOnHGlobal(nullptr, TRUE, &stream);
	
	hr = writer->SetOutput(stream);
	PrintXMLDoc(writer);
	ULONG wrt;
	stream->Write("\0", 1, &wrt);
	HGLOBAL hg;
	hr = ::GetHGlobalFromStream(stream, &hg);
	return HG2String(hg)();
}

inline void SVConfigXMLPrint::PrintXMLDoc(Writer writer) const
{
	writer->WriteStartDocument(XmlStandalone::XmlStandalone_Yes);
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());
	wchar_t head[] = L"<?xml version=\"1.0\"?>";	
	writer->WriteRaw(head);
	writer->WriteStartElement(nullptr, L"Configuration", nullptr);
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pApp->getConfigFileName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"Path", nullptr, SvUl::to_utf16(pApp->getConfigFullFileName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"Timestamp", nullptr, sv_xml::now().c_str());
	writer->WriteStartElement(nullptr, L"Settings", nullptr);
	WriteTriggers(writer);
	WriteCameras(writer);
	WriteInspections(writer);
	WritePPQs(writer);
	writer->WriteEndElement();
	writer->WriteStartElement(nullptr, L"Details", nullptr);
	WriteToolSets(writer);
	WriteIOSection(writer);
	WritePPQBar(writer);
	writer->WriteEndElement();
	writer->WriteEndElement();
	writer->WriteEndDocument();
	writer->Flush();
}

inline void SVConfigXMLPrint::WriteTriggers(Writer writer) const
{
	typedef std::map<std::string, SvTi::SVTriggerObject*> TriggerMap;
	TriggerMap triggers;
	long sz = 0;
	if( nullptr != m_cfo ){ sz = m_cfo->GetTriggerCount(); }
	for(long i = 0; i < sz; ++i)
	{
		SvTi::SVTriggerObject* pTrigger( nullptr );
		if( nullptr != m_cfo ){ pTrigger = m_cfo->GetTrigger(i); }
		if( nullptr != pTrigger )
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

inline void SVConfigXMLPrint::WriteTrigger(Writer writer, SvTi::SVTriggerObject* pTrigger) const
{
	ASSERT( nullptr != pTrigger );
	if(nullptr == pTrigger ){ return; }
	writer->WriteStartElement(nullptr, L"Trigger", nullptr);
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pTrigger->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"DeviceName", nullptr,
		(pTrigger->mpsvDevice)? SvUl::to_utf16(pTrigger->mpsvDevice->GetDeviceName(), cp_dflt).c_str() : L"** Device **");
	if(pTrigger->IsSoftwareTrigger())
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

inline void SVConfigXMLPrint::WriteCameras(Writer writer) const
{
	typedef std::map<std::string, SVVirtualCamera*> CameraMap;
	CameraMap cameras;
	long sz = 0;
	if( nullptr != m_cfo ) { sz = m_cfo->GetCameraCount(); }
	for(long i = 0; i < sz; ++i)
	{
		SVVirtualCamera* pCamera( nullptr );
		if( nullptr != m_cfo ) { pCamera = m_cfo->GetCamera( i ); }
		if( nullptr != pCamera )
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

inline const wchar_t * LoadingModeText(long mode)
{
	if(ContinuousMode == mode) return L"Continuous Load";
	if(SingleIterationMode == mode) return L"Single Iteration";
	return L"Single File";
}

inline void SVConfigXMLPrint::WriteCamera(Writer writer, SVVirtualCamera* pCamera) const
{
	ASSERT( nullptr != pCamera );
	if(nullptr == pCamera ){ return; }

	writer->WriteStartElement(nullptr, L"Camera", nullptr);
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"DeviceName", nullptr,
		(pCamera->mpsvDevice)? SvUl::to_utf16(pCamera->mpsvDevice->GetDeviceName(), cp_dflt).c_str() : L"** No Device **");
	writer->WriteAttributeString(nullptr, L"AcquisitionType", nullptr,
		(pCamera->IsFileAcquisition())?L"File Acquisition":L"Hardware Acquisition");
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

inline void SVConfigXMLPrint::WriteHardwareAcq(Writer writer, SVVirtualCamera* pCamera) const
{
	writer->WriteStartElement(nullptr, L"HardwareAcquisition", nullptr);
	SVFileNameArrayClass* pfnac = nullptr;
	SVLightReference* plrcDummy = nullptr;
	SVLut* plutDummy = nullptr;
	SVDeviceParamCollection* pDeviceParams = nullptr;
	SVAcquisitionClassPtr pAcqDevice;
	if( nullptr != pCamera ){ pAcqDevice = pCamera->GetAcquisitionDevice(); }
	if( nullptr != pAcqDevice && nullptr != m_cfo && m_cfo->GetAcquisitionDevice( pAcqDevice->DeviceName().c_str(), pfnac, plrcDummy, plutDummy, pDeviceParams ))
	{
		writer->WriteStartElement(nullptr, L"CameraFiles", nullptr);
		writer->WriteString(SvUl::to_utf16(pfnac->GetFileNameList(), cp_dflt).c_str());
		writer->WriteEndElement();
		if ( pDeviceParams && pDeviceParams->mapParameters.size() > 0 )
		{
			writer->WriteStartElement(nullptr, L"Parameters", nullptr);
			SVDeviceParamCollection l_CameraFileParams;
			HRESULT hr = pAcqDevice->GetCameraFileParameters(l_CameraFileParams);
			const int iDetailLevel = 0;
			SVDeviceParamConfigXMLHelper helper(writer, l_CameraFileParams);

			SVDeviceParamIndexer index( pDeviceParams->mapParameters );	// print in order
			SVDeviceParamIndexer::iterator iter;
			for (iter = index.begin(); iter != index.end(); ++iter)
			{
				SVDeviceParamWrapper& rw = pDeviceParams->GetParameter( *iter );
				SVDeviceParam* pParam = static_cast <SVDeviceParam*> (rw);
				if ( pParam )
				{
					SVDeviceParam* pCamFileParam = l_CameraFileParams.GetParameter( pParam->Type() );
					if ( nullptr == pCamFileParam )
						continue;
					if ( pCamFileParam->DetailLevel() > iDetailLevel )
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

inline void SVConfigXMLPrint::WriteFileAcq(Writer writer, SVVirtualCamera* pCamera) const
{
	ASSERT( nullptr != pCamera );
	if(nullptr == pCamera ){ return; }

	writer->WriteStartElement(nullptr, L"FileAcquisition", nullptr);
	writer->WriteAttributeString(nullptr, L"LoadingMode", nullptr, LoadingModeText(pCamera->GetFileLoadingMode()));
	writer->WriteStartElement(nullptr, XML_Image, nullptr);
	writer->WriteAttributeString(nullptr, L"Filename", nullptr, SvUl::to_utf16(pCamera->GetImageFilename(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"Directory", nullptr, SvUl::to_utf16(pCamera->GetImageDirectoryName(), cp_dflt).c_str());
	writer->WriteAttributeString(nullptr, L"SizeMode", nullptr, (pCamera->IsFileImageSizeEditModeFileBased())?L"Use Image Size from File":L"User Editable");
	if(pCamera->IsFileImageSizeEditModeFileBased())
	{
		wchar_t buff[64];
		writer->WriteAttributeString(nullptr, L"Width", nullptr, _itow(pCamera->GetFileImageWidth(), buff, 10));
		writer->WriteAttributeString(nullptr, L"Height", nullptr, _itow(pCamera->GetFileImageHeight(), buff, 10));
	}
	writer->WriteEndElement();
	writer->WriteEndElement();
}

inline const wchar_t * PPQModeText(SvDef::SVPPQOutputModeEnum mode)
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

namespace sv_xml
{
	inline bool less(SVVirtualCamera* lhs, SVVirtualCamera* rhs)
	{
		return std::string(lhs->GetName()) < std::string(rhs->GetName());
	}
}

inline void SVConfigXMLPrint::WritePPQs(Writer writer) const
{
	typedef std::map<std::string, SVPPQObject*> PPQMap;
	PPQMap ppqs;
	long lPPQ = 0;
	writer->WriteStartElement(nullptr, L"PPQs", nullptr);
	if( nullptr != m_cfo ) { lPPQ = m_cfo->GetPPQCount(); }
	for (long l = 0; l < lPPQ; l++)
	{
		SVPPQObject* pPPQ( nullptr );
		
		if( nullptr != m_cfo ) { pPPQ = m_cfo->GetPPQ(l); }
		if ( nullptr != pPPQ)
		{
			ppqs[pPPQ->GetName()] = pPPQ;
		}
	}

	for (PPQMap::const_iterator it = ppqs.begin(); it != ppqs.end(); ++it)
	{
		SVPPQObject* pPPQ = it->second;
		//If nullptr then do next in list
		if( nullptr == pPPQ ){ continue; }

		wchar_t buff[64];
		writer->WriteStartElement(nullptr, L"PPQ", nullptr);
		writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pPPQ->GetName(), cp_dflt).c_str());

		SvDef::SVPPQOutputModeEnum	enumPPQOutputMode;
		long	lPPQLength  = 0;	// PPQLength
		long	lResetDelay = 0;	// PPQOutputResetDelay
		long	lDelayTime  = 0;	// PPQOutputDelayTime
		
		pPPQ->GetPPQOutputMode(enumPPQOutputMode);
		pPPQ->GetPPQLength(lPPQLength);
		pPPQ->GetResetDelay(lResetDelay);
		pPPQ->GetOutputDelay(lDelayTime);

		writer->WriteAttributeString(nullptr, L"Mode", nullptr, PPQModeText(enumPPQOutputMode));
		writer->WriteAttributeString(nullptr, L"Length", nullptr, _itow(lPPQLength, buff, 10));
		writer->WriteAttributeString(nullptr, L"OutputResetDelay", nullptr, _itow(lResetDelay, buff, 10));
		writer->WriteAttributeString(nullptr, L"OutputDelayTime", nullptr, _itow(lDelayTime, buff, 10));
		
		SvTi::SVTriggerObject* pTrigger;
		pPPQ->GetTrigger(pTrigger);
		if ( nullptr != pTrigger )
		{
			writer->WriteStartElement(nullptr, L"Trigger", nullptr);
			writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pTrigger->GetName(), cp_dflt).c_str());
			writer->WriteEndElement();
		}

		WritePPQCameras(writer, pPPQ);
		WritePPQInspections(writer, pPPQ);
		writer->WriteEndElement();
	}
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WritePPQCameras(Writer writer, SVPPQObject* pPPQ) const
{
	std::deque< SVVirtualCamera* > l_Cameras;

	pPPQ->GetCameraList( l_Cameras );

	std::sort(l_Cameras.begin(), l_Cameras.end(), sv_xml::less);
	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while( l_Iter != l_Cameras.end() )
	{
		SVVirtualCamera* pCamera = ( *l_Iter );
		
		if ( nullptr != pCamera )
		{
			writer->WriteStartElement(nullptr, L"Camera", nullptr);
			writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str());
			writer->WriteEndElement();
		}

		++l_Iter;
	}
}

inline void SVConfigXMLPrint::WritePPQInspections(Writer writer, SVPPQObject* pPPQ) const
{
	InspectionMap inspections;
	long lSize = 0;
	pPPQ->GetInspectionCount(lSize);
	for (int intInspection = 0; intInspection < lSize; intInspection++)
	{
		SVInspectionProcess* pInspection( nullptr );
		pPPQ->GetInspection(intInspection, pInspection);
		
		if (nullptr != pInspection)
		{
			inspections[pInspection->GetName()] = pInspection;
		}
	}

	for (InspectionMap::const_iterator it = inspections.begin(); it != inspections.end(); ++it)
	{
		writer->WriteStartElement(nullptr, L"Inspection", nullptr);
		writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(it->second->GetName(), cp_dflt).c_str());
		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteInspections(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(nullptr, L"Inspections", nullptr);
	if( nullptr != m_cfo ) { lSize = m_cfo->GetInspectionCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspect( nullptr );
		
		if( nullptr != m_cfo ) { pInspect = m_cfo->GetInspection(l); }
		if( nullptr != pInspect)
		{
			inspections[pInspect->GetName()] = pInspect;
		}
	}

	for (InspectionMap::const_iterator it = inspections.begin(); it != inspections.end(); ++it)
	{
		writer->WriteStartElement(nullptr, L"Inspection", nullptr);
		writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(it->second->GetName(), cp_dflt).c_str());
		
		writer->WriteEndElement();
	}
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteToolSets(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(nullptr, L"InspectionProcesses", nullptr);
	if( nullptr != m_cfo ){ lSize = m_cfo->GetInspectionCount(); }
	for (long l = 0; l < lSize; l++)
	{
		SVInspectionProcess* pInspection( nullptr );
		
		if( nullptr != m_cfo ){ pInspection = m_cfo->GetInspection(l); }
		if( nullptr != pInspection )
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

inline void SVConfigXMLPrint::WriteToolSet(Writer writer, SVInspectionProcess* pInspection) const
{
	ASSERT( nullptr != pInspection );
	if( nullptr == pInspection ){ return;}

	nToolNumber = 0;
	writer->WriteStartElement(nullptr, L"InspectionProcess", nullptr);
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pInspection->GetName(), cp_dflt).c_str());
	WriteObject(writer, pInspection->GetToolSet());
	
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteIOSection(Writer writer) const
{
	writer->WriteStartElement(nullptr, L"InputOutput", nullptr);
	WriteModuleIO(writer);
	WriteResultIO(writer);
	WriteMonitorListSection(writer);
	WriteGlobalConstants(writer);
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteResultIO(Writer writer) const
{
	int					nFirstHeight = 0;
	int					nLastHeight  = 0;
	long				lSize        = 0;
	wchar_t buff[64];

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	if ( nullptr != pConfig && nullptr != pApp && pApp->GetIODoc())
	{
		SVPPQObject				*pPPQ;
		SVDigitalOutputObject	*pDigOutput;
		SVIOEntryHostStructPtrVector ppIOEntries;
		long lPPQSize = pConfig->GetPPQCount();
		
		// Print Result Output title...
		DWORD dwMaxOutput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount( dwMaxOutput );
		writer->WriteStartElement(nullptr, L"ResultOutputs", nullptr);
		writer->WriteAttributeString(nullptr, L"NumberOfOutputs", nullptr, _itow(dwMaxOutput, buff, 10));
		
		// Result Outputs
		for (long i = 0; i < (long) dwMaxOutput; ++i)
		{
			SVIOEntryHostStructPtr l_pModuleReady = pConfig->GetModuleReady();

			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( l_pModuleReady->m_IOId );
					
			// Check Module Ready first
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );
			if (pDigOutput)
			{
				if (i == pDigOutput->GetChannel())
				{
					writer->WriteStartElement(nullptr, L"DigitalOutput", nullptr);
					writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i+1, buff, 10));
					WriteIOEntryObject(writer, l_pModuleReady);
					//writer->WriteComment(L"First");
					writer->WriteEndElement();
					continue;
				}// end if
			}// end if
			
			for (int j = 0; j < lPPQSize; j++)
			{
				pPPQ = pConfig->GetPPQ(j);
				if ( nullptr != pPPQ )
				{
					// Get list of available outputs
					long	lIOEntries = 0;
					pPPQ->GetAllOutputs(ppIOEntries);
					
					lIOEntries = static_cast<long>(ppIOEntries.size());

					// Find each digital output
					for (int k = 0; k < lIOEntries; k++)
					{
						if (ppIOEntries[k]->m_ObjectType != IO_DIGITAL_OUTPUT)
							continue;

						SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(ppIOEntries[k]->m_IOId);

						pDigOutput = dynamic_cast<SVDigitalOutputObject*>(l_pObject);

						if (!pDigOutput)
							continue;

						if (i == pDigOutput->GetChannel())
						{
							writer->WriteStartElement(nullptr, L"DigitalOutput", nullptr);
							writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
							WriteIOEntryObject(writer, ppIOEntries[k]);
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

inline void SVConfigXMLPrint::WriteModuleIO(Writer writer) const
{
	int					nFirstHeight = 0;
	int					nLastHeight = 0;
	long				lSize = 0;
	CPoint				ptTemp(0, 0);
	wchar_t				buff[64];
	
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVObserverApp*         pApp    = dynamic_cast <SVObserverApp*> (AfxGetApp());
	
	// Print IODoc contents
	if (pApp->GetIODoc())
	{
		SVInputObjectList* pInputList = nullptr;
		SVDigitalInputObject	*pDigInput;
		SVIOEntryHostStructPtrVector ppIOEntries;
		
		// Get list of available inputs
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if ( nullptr != pInputList && pInputList->FillInputs( ppIOEntries ) )
		{
			lSize = static_cast< long >( ppIOEntries.size() );
			
			// Print module input title...
			DWORD dwMaxInput = 0;
			SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( dwMaxInput );

			writer->WriteStartElement(nullptr, L"DigitalInputs", nullptr);
			writer->WriteAttributeString(nullptr, L"NumberOfInputs", nullptr, _itow(dwMaxInput, buff, 10));
			
			// Module Inputs
			for (long i = 0; i < (long) dwMaxInput; ++i)
			{
				writer->WriteStartElement(nullptr, L"DigitalInput", nullptr);
				writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i+1, buff, 10));
				// Find each digital input
				for (int j = 0; j < lSize; j++)
				{
					if (ppIOEntries[j]->m_ObjectType != IO_DIGITAL_INPUT)
						continue;

					SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[j]->m_IOId );

					pDigInput = dynamic_cast< SVDigitalInputObject* >( l_pObject );

					if (!pDigInput)
						continue;

					if (i == pDigInput->GetChannel())
					{
						WriteIOEntryObject(writer, ppIOEntries[j]);
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
				if (ppIOEntries[i]->m_ObjectType == IO_REMOTE_INPUT)
					++j;
			}
			writer->WriteStartElement(nullptr, L"RemoteInputs", nullptr);
			writer->WriteAttributeString(nullptr, L"NumberOfInputs", nullptr, _itow(j, buff, 10));

			if (j)
			{
				for( int k = 0, l = 0; k < lSize; k++ )
				{
					if (ppIOEntries[k]->m_ObjectType != IO_REMOTE_INPUT)
						continue;

					writer->WriteStartElement(nullptr, L"RemoteInput", nullptr);
					writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow((l++) + 1, buff, 10));
					WriteIOEntryObject(writer, ppIOEntries[k]);
					writer->WriteEndElement();
				}
			}
			writer->WriteEndElement();
		}
	}
}

inline void SVConfigXMLPrint::WriteMonitorListSection(Writer writer) const
{
	wchar_t buff[64];
	int ItemCount = 0;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		writer->WriteStartElement(nullptr,XML_MonitorLists, nullptr);

		const RemoteMonitorListMap& remoteMonitorLists = pConfig->GetRemoteMonitorList();

		RemoteMonitorListMap::const_iterator iterMonitorList = remoteMonitorLists.begin();
		while (remoteMonitorLists.end() != iterMonitorList)
		{
			const std::string& ListName = iterMonitorList->first;
			const RemoteMonitorNamedList& monitorList = iterMonitorList->second;

			writer->WriteStartElement(nullptr,XML_MonitorList, nullptr);
			
			//Attribute monitorlistname
			writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(ListName.c_str(), cp_dflt).c_str());
			
			//Attribute  PPQname
			writer->WriteAttributeString(nullptr, XML_PPQName, nullptr, SvUl::to_utf16(monitorList.GetPPQName().c_str(), cp_dflt).c_str());

			//Attribute  Queue Depth
			int Depth = monitorList.GetRejectDepthQueue();
			writer->WriteAttributeString(nullptr, XML_RejectQuueDepth, nullptr, _itow(Depth, buff, 10));

			//write Active Flag 
			bool isActive = monitorList.IsActive();
			writer->WriteAttributeString(nullptr, XML_IsActive, nullptr, isActive? XML_TRUE :  XML_FALSE );

			//write Product Value List
			ItemCount = 0;
			const MonitoredObjectList& ValueList = monitorList.GetProductValuesList();
			MonitoredObjectList::const_iterator vlIt = ValueList.begin();
			writer->WriteStartElement(nullptr, L"ProductValueList", nullptr);
			while( vlIt != ValueList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *vlIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(nullptr, XML_ProductValueItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				vlIt++;
			}
			writer->WriteEndElement();

			// Write Product Image List
			ItemCount = 0;
			const MonitoredObjectList& ImageList = monitorList.GetProductImagesList();
			MonitoredObjectList::const_iterator ilIt = ImageList.begin();
			writer->WriteStartElement(nullptr, XML_ProductImageList, nullptr);
			while( ilIt != ImageList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *ilIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(nullptr, XML_ProductImageItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Image, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				ilIt++;
			}
			writer->WriteEndElement();

			// Write Reject Condition List
			ItemCount = 0;
			const MonitoredObjectList& RejectList = monitorList.GetRejectConditionList();
			MonitoredObjectList::const_iterator rlIt = RejectList.begin();
			writer->WriteStartElement(nullptr, L"RejectConditionList", nullptr);
			while( rlIt != RejectList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *rlIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(nullptr, XML_RejectConditionItem, nullptr);
					writer->WriteAttributeString(nullptr,XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				rlIt++;
			}
			writer->WriteEndElement();

			// Write Fail Status List
			ItemCount = 0;
			const MonitoredObjectList& FailList = monitorList.GetFailStatusList();
			MonitoredObjectList::const_iterator flIt = FailList.begin();
			writer->WriteStartElement(nullptr, XML_FailStatusList, nullptr);
			while( flIt != FailList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *flIt;
				const std::string& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(nullptr, XML_FailStatusItem, nullptr);
					writer->WriteAttributeString(nullptr, XML_Item, nullptr, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				flIt++;
			}
			writer->WriteEndElement();

			iterMonitorList++;

			writer->WriteEndElement();//RemoteMonitroList
		}

		writer->WriteEndElement();//RemoteMonitroLists
	}
}


inline void SVConfigXMLPrint::WritePPQBar(Writer writer) const
{
	writer->WriteStartElement(nullptr, L"PPQBar", nullptr);
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	int     nFirstHeight = 0;
	int     nLastHeight  = 0;
	wchar_t buff[64];
	
	long	lPPQ = 0;
	//The nullptr check here is enough because then lPPQ would be 0
	if( nullptr != pConfig){ lPPQ = pConfig->GetPPQCount(); }
	for (int intPPQ = 0; intPPQ < lPPQ; intPPQ++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(intPPQ);
		
		if (nullptr != pPPQ)
		{
			writer->WriteStartElement(nullptr, SvUl::to_utf16(pPPQ->GetName(), cp_dflt).c_str(), nullptr);
			long	lPPQLength = 0;
			pPPQ->GetPPQLength(lPPQLength);
			for (int intPPQPos = 0; intPPQPos < lPPQLength; intPPQPos++)
			{
				bool	bPosPrint = false;
				std::deque< SVVirtualCamera* > l_Cameras;

				pPPQ->GetCameraList( l_Cameras );
				std::sort(l_Cameras.begin(), l_Cameras.end());
				std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

				while( l_Iter != l_Cameras.end() )
				{
					SVVirtualCamera* pCamera = ( *l_Iter );
					
					if ( nullptr != pCamera )
					{
						long lPos = -1;

						pPPQ->GetCameraPPQPosition( lPos, pCamera );

						if (lPos == intPPQPos)
						{
							writer->WriteStartElement(nullptr, SvUl::to_utf16(pCamera->GetName(), cp_dflt).c_str(), nullptr);
							if (!bPosPrint)
							{
								writer->WriteAttributeString(nullptr, L"Position", nullptr, _itow(intPPQPos + 1, buff, 10));
								bPosPrint = true;
							}
							writer->WriteEndElement();
						}
					}

					++l_Iter;
				}
				
				long					lIOEntries;
				SVIOEntryHostStructPtrVector ppIOEntries;
				
				pPPQ->GetAllInputs( ppIOEntries );

				lIOEntries = static_cast< long >( ppIOEntries.size() );

				if (0 == lIOEntries)
				{
					writer->WriteComment(L"No IO Entries");
				}

				for (int intIOEntry = 0; intIOEntry < lIOEntries; intIOEntry++)
				{
					bool bValid = false;
					if (ppIOEntries[intIOEntry]->m_PPQIndex == intPPQPos)
					{
						SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[intIOEntry]->m_IOId );
		
						if ( nullptr != l_pObject )
						{
							if ( l_pObject->IsValid() )
							{
								writer->WriteStartElement(nullptr, SvUl::to_utf16(l_pObject->GetName(), cp_dflt).c_str(), nullptr);
								bValid = true;
							}//end if
						}
						else
						{
							if ( ppIOEntries[intIOEntry]->getObject()->IsValid() )
							{
								writer->WriteStartElement(nullptr, SvUl::to_utf16(ppIOEntries[intIOEntry]->getObject()->GetName(), cp_dflt).c_str(), nullptr);
								bValid = true;
							} // end if
						} //end else
						if (bValid && !bPosPrint)
						{
							writer->WriteAttributeString(nullptr, L"Position", nullptr, _itow(intPPQPos + 1, buff, 10));
							bPosPrint = true;
						}
						if (bValid)
						{
							writer->WriteEndElement();
						}
					}
				}
			}
			writer->WriteEndElement();
		}
	}
	writer->WriteEndElement();
}

inline std::wstring utf16(const std::string & str) { return SvUl::to_utf16(str.c_str(), cp_dflt); }

inline void SVConfigXMLPrint::WriteValueObject( Writer writer, SVObjectClass* pObj ) const
{
	if ( pObj->ObjectAttributesAllowed() & SvDef::SV_PRINTABLE )
	{
		writer->WriteStartElement(nullptr, SvUl::to_utf16(pObj->GetClassName(), cp_dflt).c_str(), nullptr);
		writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pObj->GetName(), cp_dflt).c_str());

		BOOL bGotValue = FALSE;
		std::string sValue;
		if ( SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SVDWordValueObjectClass*> ( pObj ) )
		{
			DWORD dwValue=0;
			bGotValue = (S_OK == pdwValueObject->GetValue( dwValue ) );
			sValue = SvUl::AsString( dwValue );
		}
		else
		{
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObj);
			bGotValue = (S_OK == pValueObject->getValue( sValue ));
		}

		if ( !bGotValue || sValue.empty())
		{
			writer->WriteAttributeString(nullptr, L"Value", nullptr, invalid);
		}
		else
		{
			writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(sValue, cp_dflt).c_str());
		}

		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteTool(Writer writer, SVToolClass * ts) const
{
	if (ts)
	{
		SVImageClass*           pCurrentSourceImage = nullptr;
		SVToolClass*            pTool = ts;
		SvOl::SVInObjectInfoStruct* l_psvImageInfo = nullptr;
		SvOl::SVInObjectInfoStruct* l_psvLastImageInfo = nullptr;

		while( nullptr == l_psvImageInfo && S_OK ==  pTool->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) )
		{
			if( nullptr != l_psvImageInfo )
			{
				if( l_psvImageInfo->IsConnected() )
				{
					pCurrentSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().getObject());

					WriteValueObject(writer, L"Property",  utf16(SvUl::LoadStdString(IDS_IMAGE_SOURCE_STRING)), SvUl::to_utf16(pCurrentSourceImage->GetObjectNameToObjectType().c_str(), cp_dflt).c_str());
				}
			}
			else
			{
				l_psvLastImageInfo = l_psvImageInfo;
				l_psvImageInfo = nullptr;
			}
		}
	}
}

inline void SVConfigXMLPrint::WriteArchiveTool(Writer writer, SVArchiveTool * ar) const
{
	if (ar)
	{
		wchar_t buff[64];
		int i, s;
		SVArchiveRecord* pRecord;
		
		writer->WriteStartElement(nullptr, L"Results", nullptr);
		s = ar->m_arrayResultsInfoObjectsToArchive.GetSize();
		for (i = 0; i < s; i++)
		{
			pRecord = ar->m_arrayResultsInfoObjectsToArchive.GetAt(i);
			if (nullptr != pRecord->GetObjectReference().getObject())
			{
				writer->WriteStartElement(nullptr, L"Result", nullptr);
				writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
				writer->WriteAttributeString(nullptr,XML_Name, nullptr, utf16(pRecord->GetObjectReference().GetCompleteName()).c_str());
				writer->WriteEndElement();
			}
		}
		writer->WriteEndElement();
		writer->WriteStartElement(nullptr, L"Images", nullptr);
		
		s = ar->m_arrayImagesInfoObjectsToArchive.GetSize();
		for (i = 0; i < s; i++)
		{
			pRecord = ar->m_arrayImagesInfoObjectsToArchive.GetAt(i);
			writer->WriteStartElement(nullptr, L"Result", nullptr);
			writer->WriteAttributeString(nullptr, L"Number", nullptr, _itow(i + 1, buff, 10));
			if (pRecord)
			{
				writer->WriteAttributeString(nullptr,XML_Name, nullptr, utf16(std::string(pRecord->GetImageObjectName())).c_str());
			}
			else
			{
				writer->WriteAttributeString(nullptr, L"Error", nullptr, invalid);
			}
			writer->WriteEndElement();
		}
		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteObject( Writer writer, SVObjectClass* pObject ) const
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
    int      nFirstHeight   = 0;
    int      nLastHeight    = 0;
	
	std::string sLabel, sValue;
    std::string  strType   = pObject->GetObjectName();
    std::string  strName   = pObject->GetName();
	wchar_t buff[64];
	
    GUID     guidObjID = pObject->GetClassID();
	
	BOOL	bWriteToolExtents = FALSE;		// Sri 2/17/00
	
	// If object is a value object, get embedded ID.
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject)  )
	{
		guidObjID = pObject->GetEmbeddedID();
	}
	
	std::pair<GUID**, size_t> nPrs = NonPrintGuids();
    // Check for non-printing object type.
	for (size_t nIndex = 0; nIndex < nPrs.second; nIndex++)
    {
        // If the GUID is for a non-printing object, Skip printing.
        if (guidObjID == *(nPrs.first[nIndex]))
        {
            return;
        }  // end if ( guidObjID == *pguidNonPrintArray [nIndex] )
    }  // end for ( int nIndex = 0; nIndex < nNPArraySize; nIndex++ )
	
	// This is to prevent the comments from being sent to the SVRC thru GetConfigReport
	if( guidObjID == SVToolCommentTypeObjectGuid )
	{
		return;
	}	
	
	// If object is a value object, print name and value.
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{
		WriteValueObject(writer, pObject);
	}
	else
	{
		do
		{
			if ( dynamic_cast <SVShapeMaskHelperClass*> (pObject) )
			{
				if ( !( pObject->ObjectAttributesAllowed() & SvDef::SV_PRINTABLE) )	// EB 20050818 - hack this instead of doing it right
				{
					break;
				}
			}

			writer->WriteStartElement(nullptr, SvUl::to_utf16(pObject->GetClassName(), cp_dflt).c_str(), nullptr);
			std::wstring objName = SvUl::to_utf16(pObject->GetName(), cp_dflt);
			writer->WriteAttributeString(nullptr,XML_Name, nullptr, objName.c_str());
			SVToolClass* pTool = dynamic_cast<SVToolClass*> (pObject);
			if ( nullptr != pTool )
			{
				// Increment even if disabled, to maintain count.  Starts with zero, so for first
				//    tool, will increment to 1.
				nToolNumber++;
				bWriteToolExtents = TRUE;		// Sri 2/17/00
				writer->WriteAttributeString(nullptr, L"ToolNumber", nullptr, _itow(nToolNumber, buff, 10));
			}
			
			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.empty())
			{
				writer->WriteAttributeString(nullptr, L"Type", nullptr, SvUl::to_utf16(strType, cp_dflt).c_str());
			}
			
			// Print the tool length, width, extends, etc here.
			if (bWriteToolExtents && nullptr != pTool)
			{
				bWriteToolExtents = FALSE;
				WriteTool(writer, pTool);
			} 
			
			if ( SVArchiveTool* pArchiveTool = dynamic_cast <SVArchiveTool*> (pObject) )
			{
				WriteArchiveTool(writer, pArchiveTool);
			}
			
			if ( SVStatisticsToolClass* pStatisticsTool = dynamic_cast<SVStatisticsToolClass*> (pObject) )
			{
				SVObjectReference refObject = pStatisticsTool->GetVariableSelected();
				if (refObject.getObject())
				{
					writer->WriteStartElement(nullptr,  L"Variable", nullptr);
					writer->WriteAttributeString(nullptr,XML_Name, nullptr, utf16(refObject.GetName()).c_str());
					writer->WriteEndElement();
				}
			}
			
			if (SVUserMaskOperatorClass* maskObj = dynamic_cast <SVUserMaskOperatorClass*>( pObject ))
			{
				if ( nullptr != maskObj )
				{
					SVImageClass* pImage = maskObj->getMaskInputImage();
					if (nullptr != pImage)
					{
						WriteValueObject(writer, L"Property",  utf16(SvUl::LoadStdString(IDS_IMAGE_SOURCE_STRING)), SvUl::to_utf16(pImage->GetCompleteName().c_str(), cp_dflt).c_str());
					}
				}
			}

			if (SVDoubleResultClass* pBlobResult = dynamic_cast<SVDoubleResultClass*>(pObject))
			{
				if (SV_IS_KIND_OF(pBlobResult->GetParent(),SVBlobAnalyzerClass))
				{  
					sLabel = SvUl::LoadStdString(IDS_BLOB_FEATURE_DEFAULT_VALUE) + _T(":");
					const SVDoubleValueObjectClass* pDoubleValueObj = dynamic_cast<const SVDoubleValueObjectClass*> (pBlobResult->getInput());
					if ( pDoubleValueObj )
					{
						double dVal = pDoubleValueObj->GetDefaultValue();
						sValue = SvUl::Format(_T("%lf"), dVal);
						//ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
						//PrintValueObject(pDC, ptCurPos, utf16(sLabel), utf16(sValue));

						WriteValueObject(writer, L"Property", utf16( sLabel ), utf16( sValue));

					}
				}
			}

			if (SV_IS_KIND_OF(pObject, SVTaskObjectClass))
			{
				if ( SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (pObject) )
				{
					sValue = pEquation->GetEquationText();
		
					// Write the equation text.
					if (sLabel.empty())
					{
						sLabel = _T("EquationText");
					}
					WriteValueObject(writer, L"Equation", utf16( sLabel ), utf16( sValue ));
				}
				WriteInputOutputList(writer, pObject);
			} 

			WriteFriends(writer, pObject);
			WriteChildren(writer, pObject);
			writer->WriteComment(objName.c_str());
			writer->WriteEndElement();
		} while (false);// end do
	}// End if else pObj kind of SVValueObjectClass

	//writer->WriteEndElement();
}  // end function void SVConfigXMLPrint:::PrintDetails( ... )

void SVConfigXMLPrint::WriteAllChildren(Writer writer, SVTaskObjectListClass* pTaskObj) const
{
	for (int nCnt = 0; nCnt < pTaskObj->GetSize(); nCnt++)
	{
		SVObjectClass* pChild = pTaskObj->GetAt(nCnt);
		if (pChild)
		{
			WriteObject(writer, pChild);
		}
	}
}

void SVConfigXMLPrint::WriteChildren( Writer writer, SVObjectClass* pObj ) const
{
	if ( SVTaskObjectListClass* pTaskObj = dynamic_cast <SVTaskObjectListClass*> (pObj) )
    {
		writer->WriteStartElement(nullptr, L"Children", nullptr);
		if (SVToolSetClass* pToolSet = dynamic_cast <SVToolSetClass *>(pObj))
		{
			SVToolGrouping& rToolGroupings = GetToolGroupings(pToolSet->GetInspection()->GetUniqueObjectID());
			if (rToolGroupings.size())
			{
				bool bToolGroupActive = false;
				for (SVToolGrouping::const_iterator it = rToolGroupings.begin();it != rToolGroupings.end();++it)
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
							writer->WriteAttributeString(nullptr,XML_Name, nullptr, name.c_str());
						}
						break;

						case ToolGroupData::EndOfGroup:
						{
							writer->WriteStartElement(nullptr, L"EndToolGrouping", nullptr);
							std::wstring name = SvUl::to_utf16(it->first.c_str(), cp_dflt);
							writer->WriteAttributeString(nullptr,XML_Name, nullptr, name.c_str());
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

void SVConfigXMLPrint::WriteFriends( Writer writer, SVObjectClass* pObj ) const
{
	const SVObjectInfoArrayClass& rFriendList = pObj->GetFriendList();
	size_t sz = rFriendList.size();

	if (sz > 0)
	{
		writer->WriteStartElement(nullptr, L"Friends", nullptr);
		for (size_t nCnt = 0; nCnt < sz; nCnt++)
		{
			const SVObjectInfoStruct& rObjInfo = rFriendList[nCnt];
			
			if (rObjInfo.getObject())
			{
				WriteObject(writer, rObjInfo.getObject());
			}  // end if( rObjInfo )
		}  // end for( int nCnt = 0; nCnt < sz; nCnt++ )
		writer->WriteEndElement();
	}
}  // end function void SVConfigXMLPrint:::PrintFriends( ... )

void SVConfigXMLPrint::WriteInputOutputList( Writer writer, SVObjectClass* pObj ) const
{
	SVTaskObjectClass* pTaskObj = dynamic_cast <SVTaskObjectClass*> (pObj);

	SVOutputInfoListClass l_OutputList;

	pTaskObj->GetOutputList( l_OutputList );

	for (int nCnt = 0; nCnt < l_OutputList.GetSize(); nCnt++)
	{
		SVOutObjectInfoStruct* pOutput = l_OutputList.GetAt(nCnt);
		
		if (pOutput->getObject()->GetParent() == pObj)
		{
			WriteObject(writer, pOutput->getObject());
		}  // end if( pOutput->PObject->GetOwner () == pObj )
	}  // end for( int nCnt = 0; nCnt < pOutputInfoList->GetSize(); nCnt++ )
}  // end function void SVConfigXMLPrint:::PrintInputOutputList( ... )

void SVConfigXMLPrint::WriteValueObject(Writer writer, const std::wstring tag, const std::wstring lpszName, const std::wstring lpszValue) const
{
	writer->WriteStartElement(nullptr, tag.c_str(), nullptr);
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, lpszName.c_str());
	writer->WriteAttributeString(nullptr, L"Value", nullptr, lpszValue.c_str());
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const
{
	SVDigitalInputObject	*pDigInput = nullptr;
	SVDigitalOutputObject	*pDigOutput = nullptr;
	std::wstring 			sValue;
	
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( IOEntry->m_IOId );
	writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(l_pObject->GetName(), cp_dflt).c_str());
	
	switch (IOEntry->m_ObjectType)
	{
		case IO_DIGITAL_INPUT:
			pDigInput = dynamic_cast< SVDigitalInputObject* >( l_pObject );
			if (pDigInput->IsForced())
				sValue = pDigInput->GetForcedValue() ? L"1" : L"0";
			else
				sValue = L" ";
			WriteValueObject(writer,  L"Property", L"Forced", sValue.c_str());
			
			WriteValueObject(writer,  L"Property", L"Inverted", pDigInput->IsInverted() ? L"1":L" ");
			break;

		case IO_DIGITAL_OUTPUT:
			pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );
			if (pDigOutput->IsForced())
				sValue = pDigOutput->GetForcedValue() ? L"1" : L"0";
			else
				sValue = L" ";
			WriteValueObject(writer,  L"Property", L"Forced", sValue.c_str());
			
			sValue = pDigOutput->IsInverted() ? L"1" : L" ";
			WriteValueObject(writer,  L"Property", L"Inverted", sValue.c_str());

			sValue = pDigOutput->IsCombined() ? L"1" : L" ";
			WriteValueObject(writer,  L"Property", L"Combined", sValue.c_str());
			
			sValue = pDigOutput->GetCombinedValue() ? L"AND w ACK" : L"OR w NAK";
			WriteValueObject(writer,  L"Property", L"Combined using", sValue.c_str());
			break;

		case IO_REMOTE_INPUT:
			break;

		case IO_REMOTE_OUTPUT:
			break;
	}
}

inline void SVConfigXMLPrint::WriteGlobalConstants(Writer writer) const
{
	std::string Value;
	int Index (0);

	BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvDef::FqnGlobal );

	writer->WriteStartElement(nullptr, L"GlobalConstants", nullptr);

	BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter )
	{
		const BasicValueObjectPtr& pGlobalConstant = *Iter;

		if(nullptr != pGlobalConstant)
		{
			Value = SvUl::Format( L"GlobalConstant%d", ++Index );
			writer->WriteStartElement(nullptr, SvUl::to_utf16( Value.c_str(), cp_dflt).c_str(), nullptr);
			//write Global Constant Name
			Value = pGlobalConstant->GetCompleteName();
			writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16( Value.c_str(), cp_dflt).c_str() );
			//write Global Constant Value
			pGlobalConstant->getValue( Value );
			writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16( Value.c_str(), cp_dflt).c_str() );
			//write Global Constant type
			Value = pGlobalConstant->getTypeName();
			writer->WriteAttributeString(nullptr, L"Type", nullptr, SvUl::to_utf16( Value.c_str(), cp_dflt).c_str() );
			writer->WriteEndElement();//GlobalConstantx
		}
		++Iter;
	}

	writer->WriteEndElement();//GlobalConstants
}


//////////////////////////////////////////////////////////////////
// Visitor helper

inline SVDeviceParamConfigXMLHelper::SVDeviceParamConfigXMLHelper(
	Writer writer, SVDeviceParamCollection& rCamFileParams)
	: m_writer(writer), m_rCamFileParams(rCamFileParams)
{
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit(SVDeviceParam& param )
{
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVLongValueDeviceParam& param )
{
	const SVLongValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.lValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				Value = iterOption->m_Description;
			}
		}
		if ( Value.empty() )
		{
			Value = SvUl::Format( _T("%lu%s"), static_cast<unsigned long> (param.GetScaledValue()), param.info.sUnits.empty() ? _T("") : std::string(_T(" ") + param.info.sUnits).c_str() );
		}
		m_writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVi64ValueDeviceParam& param )
{
	const SVi64ValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Text = SvUl::Format("%I64d", param.iValue );
		m_writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Text, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVBoolValueDeviceParam& param )
{
	const SVBoolValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.bValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				Value = iterOption->m_Description.c_str();
			}
		}
		if ( Value.empty() )
		{
			Value = param.bValue ? _T("TRUE") : _T("FALSE");
		}
		m_writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value, cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVStringValueDeviceParam& param )
{
	const SVStringValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		m_writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(param.strValue.c_str(), cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVParamListDeviceParam& param )
{
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVLutDeviceParam& param )
{
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVLightReferenceDeviceParam& param )
{
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVCameraFormatsDeviceParam& param )
{
	const SVCameraFormatsDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(nullptr, L"Param", nullptr);
		std::string Value;
		SVCameraFormat* pFormat = nullptr;
		if ( param.options.size() > 0 )
		{
			SVCameraFormat& rFormat = param.options[ param.strValue ];
			const SVCameraFormat& rCamFileFormat = pCamFileParam->options.find( param.strValue )->second;
			if ( rCamFileFormat.m_strName == param.strValue )
			{
				Value = rCamFileFormat.m_strDescription;
				pFormat = &rFormat;
			}
		}
		if ( Value.empty() )
		{
			Value = param.strValue;
		}
		m_writer->WriteAttributeString(nullptr,XML_Name, nullptr, SvUl::to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(nullptr, L"Value", nullptr, SvUl::to_utf16(Value.c_str(), cp_dflt).c_str());

		if ( nullptr != pFormat )
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

inline HRESULT SVDeviceParamConfigXMLHelper::Visit(SVCustomDeviceParam& param)
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

