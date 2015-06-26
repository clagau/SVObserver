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

#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVBlobAnalyzer.h"
#include "SVResultDouble.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "RootObject.h"

std::pair<GUID **, size_t> NonPrintGuids();

typedef sv_xml::SVConfigXMLPrint SVConfigXMLPrint;

typedef sv_xml::SVDeviceParamConfigXMLHelper SVDeviceParamConfigXMLHelper;

const wchar_t * invalid = L"** I N V A L I D ! **";

const int cp_dflt = 1252;

static SVToolGrouping GetToolGroupings(const SVGUID& rInspectionGuid)
{
	// get the document that owns this inspection
	SVIPDoc* pDoc = SVObjectManagerClass::Instance().GetIPDoc(rInspectionGuid);
	if (pDoc)
	{
		return pDoc->GetToolGroupings();
	}
	return SVToolGrouping();
}

static SVObjectClass* GetTool(const SVString& rName, const SVTaskObjectListClass& rToolSet)
{
	SVObjectClass* pObject(nullptr);
	for (int i = 0; !pObject && i < rToolSet.GetSize(); i++)
	{
		if (rToolSet.GetAt(i)->GetName() == rName) 
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
	hr = ::CreateStreamOnHGlobal(NULL, TRUE, &stream);
	//wchar_t head[] = L"<? xml version=\"1.0\" ?>";
	//ULONG wrtn = 0;
	//stream->Write(head, sizeof(head), &wrtn);
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
	writer->WriteStartElement(NULL, L"Configuration", NULL);
	writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pApp->getConfigFileName(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"Path", NULL, to_utf16(pApp->getConfigFullFileName(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"Timestamp", NULL, sv_xml::now().c_str());
	writer->WriteStartElement(NULL, L"Settings", NULL);
	WriteTriggers(writer);
	WriteCameras(writer);
	WriteInspections(writer);
	WritePPQs(writer);
	writer->WriteEndElement();
	writer->WriteStartElement(NULL, L"Details", NULL);
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
	typedef std::map<std::string, SVTriggerObject*> TriggerMap;
	TriggerMap triggers;
	long sz = 0;
	if( nullptr != m_cfo ){ sz = m_cfo->GetTriggerCount(); }
	for(long i = 0; i < sz; ++i)
	{
		SVTriggerObject* pTrigger( nullptr );
		if( nullptr != m_cfo ){ pTrigger = m_cfo->GetTrigger(i); }
		if( nullptr != pTrigger )
		{
			triggers[pTrigger->GetName()] = pTrigger;
		}
	}

	writer->WriteStartElement(NULL, L"Triggers", NULL);

	for (TriggerMap::const_iterator it = triggers.begin(); it != triggers.end(); ++it)
	{
		WriteTrigger(writer, it->second);
	}

	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteTrigger(Writer writer, SVTriggerObject* pTrigger) const
{
	ASSERT( nullptr != pTrigger );
	if(nullptr == pTrigger ){ return; }
	writer->WriteStartElement(NULL, L"Trigger", NULL);
	writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pTrigger->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"DeviceName", NULL,
		(pTrigger->mpsvDevice)?to_utf16(pTrigger->mpsvDevice->GetDeviceName(), cp_dflt).c_str(): L"** Device **");
	if(pTrigger->IsSoftwareTrigger())
	{
		writer->WriteAttributeString(NULL, L"Type", NULL, L"Software");
		std::wostringstream os;
		os << pTrigger->GetSoftwareTriggerPeriod() << L" ms" << std::ends;
		writer->WriteAttributeString(NULL, L"TimerPeriod", NULL, os.str().c_str());			
	}
	else
	{
		writer->WriteAttributeString(NULL, L"Type", NULL, L"Hardware");
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

	writer->WriteStartElement(NULL, L"Cameras", NULL);

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

	writer->WriteStartElement(NULL, L"Camera", NULL);
	writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamera->GetName(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"DeviceName", NULL,
		(pCamera->mpsvDevice)?to_utf16(pCamera->mpsvDevice->GetDeviceName(), cp_dflt).c_str(): L"** No Device **");
	writer->WriteAttributeString(NULL, L"AcquisitionType", NULL,
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
	writer->WriteStartElement(NULL, L"HardwareAcquisition", NULL);
	SVFileNameArrayClass* pfnac = NULL;
	SVLightReference* plrcDummy = NULL;
	SVLut* plutDummy = NULL;
	SVDeviceParamCollection* pDeviceParams = NULL;
	SVAcquisitionClassPtr pAcqDevice;
	if( nullptr != pCamera ){ pAcqDevice = pCamera->GetAcquisitionDevice(); }
	if( !( pAcqDevice.empty() ) && nullptr != m_cfo && m_cfo->GetAcquisitionDevice( pAcqDevice->GetRootDeviceName(), pfnac, plrcDummy, plutDummy, pDeviceParams ))
	{
		writer->WriteStartElement(NULL, L"CameraFiles", NULL);
		writer->WriteString(to_utf16(pfnac->GetFileNameList(), cp_dflt).c_str());
		writer->WriteEndElement();
		if ( pDeviceParams && pDeviceParams->mapParameters.size() > 0 )
		{
			writer->WriteStartElement(NULL, L"Parameters", NULL);
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
					if ( pCamFileParam == NULL)
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
		writer->WriteAttributeString(NULL, L"Error", NULL, L"** No acquisition device **");
	}
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteFileAcq(Writer writer, SVVirtualCamera* pCamera) const
{
	ASSERT( nullptr != pCamera );
	if(nullptr == pCamera ){ return; }

	writer->WriteStartElement(NULL, L"FileAcquisition", NULL);
	writer->WriteAttributeString(NULL, L"LoadingMode", NULL, LoadingModeText(pCamera->GetFileLoadingMode()));
	writer->WriteStartElement(NULL, L"Image", NULL);
	writer->WriteAttributeString(NULL, L"Filename", NULL, to_utf16(pCamera->GetImageFilename(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"Directory", NULL, to_utf16(pCamera->GetImageDirectoryName(), cp_dflt).c_str());
	writer->WriteAttributeString(NULL, L"SizeMode", NULL, (pCamera->IsFileImageSizeEditModeFileBased())?L"Use Image Size from File":L"User Editable");
	if(pCamera->IsFileImageSizeEditModeFileBased())
	{
		wchar_t buff[64];
		writer->WriteAttributeString(NULL, L"Width", NULL, _itow(pCamera->GetFileImageWidth(), buff, 10));
		writer->WriteAttributeString(NULL, L"Height", NULL, _itow(pCamera->GetFileImageHeight(), buff, 10));
	}
	writer->WriteEndElement();
	writer->WriteEndElement();
}

inline const wchar_t * PPQModeText(SVPPQOutputModeEnum mode)
{
	switch (mode)
	{
		case SVPPQNextTriggerMode:	// Resets outputs on trigger. Writes outputs immediately.
			return L"NextTrigger";
		case SVPPQTimeDelayMode:	// Resets outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
			// Aborts waiting if new trigger occurs.
			return L"TimeDelay";
		case SVPPQTimeDelayAndDataCompleteMode:	// Resets outputs on trigger. Writes outputs after delay time is over and data is complete.
			// Aborts waiting if new trigger occurs.
			return L"TimeDelayDataCompletion";
		case SVPPQExtendedTimeDelayMode:		// Doesn't reset outputs on trigger. Writes outputs after delay time is over, even if data isn't complete.
															// Aborts waiting if product leaves PPQ.
			return L"ExtendedTimeDelayMode";
		case SVPPQExtendedTimeDelayAndDataCompleteMode: // Doesn't reset outputs on trigger. Writes outputs after delay time is over and data is complete.
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
	writer->WriteStartElement(NULL, L"PPQs", NULL);
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
		writer->WriteStartElement(NULL, L"PPQ", NULL);
		writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pPPQ->GetName(), cp_dflt).c_str());

		SVPPQOutputModeEnum	enumPPQOutputMode;
		long	lPPQLength  = 0;	// PPQLength
		long	lResetDelay = 0;	// PPQOutputResetDelay
		long	lDelayTime  = 0;	// PPQOutputDelayTime
		
		pPPQ->GetPPQOutputMode(enumPPQOutputMode);
		pPPQ->GetPPQLength(lPPQLength);
		pPPQ->GetResetDelay(lResetDelay);
		pPPQ->GetOutputDelay(lDelayTime);

		writer->WriteAttributeString(NULL, L"Mode", NULL, PPQModeText(enumPPQOutputMode));
		writer->WriteAttributeString(NULL, L"Length", NULL, _itow(lPPQLength, buff, 10));
		writer->WriteAttributeString(NULL, L"OutputResetDelay", NULL, _itow(lResetDelay, buff, 10));
		writer->WriteAttributeString(NULL, L"OutputDelayTime", NULL, _itow(lDelayTime, buff, 10));
		
		SVTriggerObject* pTrigger;
		pPPQ->GetTrigger(pTrigger);
		if ( nullptr != pTrigger )
		{
			writer->WriteStartElement(NULL, L"Trigger", NULL);
			writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pTrigger->GetName(), cp_dflt).c_str());
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
			writer->WriteStartElement(NULL, L"Camera", NULL);
			writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamera->GetName(), cp_dflt).c_str());
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
		writer->WriteStartElement(NULL, L"Inspection", NULL);
		writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(it->second->GetName(), cp_dflt).c_str());
		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteInspections(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(NULL, L"Inspections", NULL);
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
		writer->WriteStartElement(NULL, L"Inspection", NULL);
		writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(it->second->GetName(), cp_dflt).c_str());
		
		writer->WriteEndElement();
	}
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteToolSets(Writer writer) const
{
	InspectionMap inspections;
	long lSize = 0;
	writer->WriteStartElement(NULL, L"InspectionProcesses", NULL);
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
	writer->WriteStartElement(NULL, L"InspectionProcess", NULL);
	writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pInspection->GetName(), cp_dflt).c_str());
	WriteObject(writer, pInspection->GetToolSet());
	
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteIOSection(Writer writer) const
{
	writer->WriteStartElement(NULL, L"InputOutput", NULL);
	WriteModuleIO(writer);
	WriteResultIO(writer);
	WriteMonitorListSection(writer);
	WriteGlobalConstants(writer);
	writer->WriteEndElement();
}

inline void SVConfigXMLPrint::WriteResultIO(Writer writer) const
{
	CString				label, value;
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
		SVIOEntryHostStructPtrList ppIOEntries;
		long lPPQSize = pConfig->GetPPQCount();
		
		// Print Result Output title...
		DWORD dwMaxOutput = 0;
		SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount( dwMaxOutput );
		writer->WriteStartElement(NULL, L"ResultOutputs", NULL);
		writer->WriteAttributeString(NULL, L"NumberOfOutputs", NULL, _itow(dwMaxOutput, buff, 10));
		
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
					writer->WriteStartElement(NULL, L"DigitalOutput", NULL);
					writer->WriteAttributeString(NULL, L"Number", NULL, _itow(i+1, buff, 10));
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
					if (pPPQ->GetAllOutputs( ppIOEntries ))
					{
						lIOEntries = static_cast< long >( ppIOEntries.size() );
						
						// Find each digital output
						for (int k = 0; k < lIOEntries; k++)
						{
							if (ppIOEntries[k]->m_ObjectType != IO_DIGITAL_OUTPUT)
								continue;
							
							SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( ppIOEntries[k]->m_IOId );

							pDigOutput = dynamic_cast< SVDigitalOutputObject* >( l_pObject );
							
							if (!pDigOutput)
								continue;
							
							if (i == pDigOutput->GetChannel())
							{
								writer->WriteStartElement(NULL, L"DigitalOutput", NULL);
								writer->WriteAttributeString(NULL, L"Number", NULL, _itow(i+1, buff, 10));
								WriteIOEntryObject(writer, ppIOEntries[k]);
								writer->WriteEndElement();
								continue;
							}// end if
						}
					}
				}
			}// end for j
		}// end for
		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteModuleIO(Writer writer) const
{
	CString				label, value;
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
		SVIOEntryHostStructPtrList ppIOEntries;
		
		// Get list of available inputs
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if ( nullptr != pInputList && pInputList->FillInputs( ppIOEntries ) )
		{
			lSize = static_cast< long >( ppIOEntries.size() );
			
			// Print module input title...
			DWORD dwMaxInput = 0;
			SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( dwMaxInput );

			writer->WriteStartElement(NULL, L"DigitalInputs", NULL);
			writer->WriteAttributeString(NULL, L"NumberOfInputs", NULL, _itow(dwMaxInput, buff, 10));
			
			// Module Inputs
			for (long i = 0; i < (long) dwMaxInput; ++i)
			{
				writer->WriteStartElement(NULL, L"DigitalInput", NULL);
				writer->WriteAttributeString(NULL, L"Number", NULL, _itow(i+1, buff, 10));
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
			writer->WriteStartElement(NULL, L"RemoteInputs", NULL);
			writer->WriteAttributeString(NULL, L"NumberOfInputs", NULL, _itow(j, buff, 10));

			if (j)
			{
				for( int k = 0, l = 0; k < lSize; k++ )
				{
					if (ppIOEntries[k]->m_ObjectType != IO_REMOTE_INPUT)
						continue;

					writer->WriteStartElement(NULL, L"RemoteInput", NULL);
					writer->WriteAttributeString(NULL, L"Number", NULL, _itow((l++) + 1, buff, 10));
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
	CString sLabel, sValue;
	wchar_t				buff[64];
	int ItemCount = 0;

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

	if( nullptr != pConfig )
	{
		writer->WriteStartElement(NULL, L"RemoteMonitorList", NULL);
		
		const RemoteMonitorList& remoteMonitorLists = pConfig->GetRemoteMonitorList();

		RemoteMonitorList::const_iterator iterMonitorList = remoteMonitorLists.begin();
		while (remoteMonitorLists.end() != iterMonitorList)
		{
			const SVString& ListName = iterMonitorList->first;
			const RemoteMonitorNamedList& monitorList = iterMonitorList->second;
			
			//write Monitor List Name
			writer->WriteStartElement(NULL,L"MonitorListName",NULL);
			writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(ListName.c_str(), cp_dflt).c_str());
			writer->WriteEndElement();//Monitor List Name

			//write out PPQ
			writer->WriteStartElement(NULL,L"PPQName",NULL);
			writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(monitorList.GetPPQName().c_str(), cp_dflt).c_str());
			writer->WriteEndElement();//PPQ

			//write Queue Depth
			int Depth = monitorList.GetRejectDepthQueue();
			writer->WriteStartElement(NULL,L"RejectQueueDepth",NULL);
			writer->WriteAttributeString(NULL, L"Value", NULL, _itow(Depth, buff, 10));
			writer->WriteEndElement();//queue depth

			//write Product Value List
			ItemCount = 0;
			const MonitoredObjectList& ValueList = monitorList.GetProductValuesList();
			MonitoredObjectList::const_iterator vlIt = ValueList.begin();
			writer->WriteStartElement(NULL,L"ProductValueList",NULL);
			while( vlIt != ValueList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *vlIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(NULL,L"ProductValueItem",NULL);
					writer->WriteAttributeString(NULL, L"Item", NULL, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				vlIt++;
			}
			writer->WriteEndElement();

			// Write Product Image List
			ItemCount = 0;
			const MonitoredObjectList& ImageList = monitorList.GetProductImagesList();
			MonitoredObjectList::const_iterator ilIt = ImageList.begin();
			writer->WriteStartElement(NULL,L"ProductImageList",NULL);
			while( ilIt != ImageList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *ilIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(NULL,L"ProductImageItem",NULL);
					writer->WriteAttributeString(NULL, L"Image", NULL, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				ilIt++;
			}
			writer->WriteEndElement();

			// Write Reject Condition List
			ItemCount = 0;
			const MonitoredObjectList& RejectList = monitorList.GetRejectConditionList();
			MonitoredObjectList::const_iterator rlIt = RejectList.begin();
			writer->WriteStartElement(NULL,L"RejectConditionList",NULL);
			while( rlIt != RejectList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *rlIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(NULL,L"RejectConditionItem",NULL);
					writer->WriteAttributeString(NULL, L"Item", NULL, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				rlIt++;
			}
			writer->WriteEndElement();

			// Write Fail Status List
			ItemCount = 0;
			const MonitoredObjectList& FailList = monitorList.GetFailStatusList();
			MonitoredObjectList::const_iterator flIt = FailList.begin();
			writer->WriteStartElement(NULL,L"FailStatusList",NULL);
			while( flIt != FailList.end() )
			{
				ItemCount++;
				const MonitoredObject& rObj = *flIt;
				const SVString& objectName = RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj);
				if ( !objectName.empty() )
				{
					writer->WriteStartElement(NULL,L"FailStatusItem",NULL);
					writer->WriteAttributeString(NULL, L"Item", NULL, _itow(ItemCount, buff, 10));
					writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(objectName.c_str(), cp_dflt).c_str());
					writer->WriteEndElement();
				}
				flIt++;
			}
			writer->WriteEndElement();

			iterMonitorList++;
		}


		writer->WriteEndElement();//RemoteMonitroList
	}
}


inline void SVConfigXMLPrint::WritePPQBar(Writer writer) const
{
	writer->WriteStartElement(NULL, L"PPQBar", NULL);
	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	
	CString	sLabel, sValue;
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
			writer->WriteStartElement(NULL, to_utf16(pPPQ->GetName(), cp_dflt).c_str(), NULL);
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
							writer->WriteStartElement(NULL, to_utf16(pCamera->GetName(), cp_dflt).c_str(), NULL);
							if (!bPosPrint)
							{
								writer->WriteAttributeString(NULL, L"Position", NULL, _itow(intPPQPos + 1, buff, 10));
								bPosPrint = true;
							}
							writer->WriteEndElement();
						}
					}

					++l_Iter;
				}
				
				long					lIOEntries;
				SVIOEntryHostStructPtrList ppIOEntries;
				
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
		
						if ( l_pObject != NULL )
						{
							if ( l_pObject->IsValid() )
							{
								writer->WriteStartElement(NULL, to_utf16(l_pObject->GetName(), cp_dflt).c_str(), NULL);
								bValid = true;
							}//end if
						}
						else
						{
							if ( ppIOEntries[intIOEntry]->m_pValueObject->IsValid() )
							{
								writer->WriteStartElement(NULL, to_utf16(ppIOEntries[intIOEntry]->m_pValueObject->GetName(), cp_dflt).c_str(), NULL);
								bValid = true;
							} // end if
						} //end else
						if (bValid && !bPosPrint)
						{
							writer->WriteAttributeString(NULL, L"Position", NULL, _itow(intPPQPos + 1, buff, 10));
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

inline std::wstring utf16(const SVString & str) { return to_utf16(str.c_str(), cp_dflt); }

inline void SVConfigXMLPrint::WriteValueObject( Writer writer, SVValueObjectClass* pObj ) const
{
	if ( pObj->ObjectAttributesAllowed() & SV_PRINTABLE )
	{
		writer->WriteStartElement(NULL, to_utf16(pObj->GetClassName(), cp_dflt).c_str(), NULL);
		writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pObj->GetName(), cp_dflt).c_str());
		CString sLabel = pObj->GetName();

		if (sLabel.Find(_T("OCR Gray Scale Parameters")) >= 0)
		{
			CString  strParameters;
			
			if (pObj->GetValue(strParameters) == S_OK )
				WriteOCRGrayScaleParameters(writer, strParameters.GetBuffer());
		}
		else if (sLabel.Find(_T("OCR Parameters")) >= 0)
		{
			CString  strParameters;
			
			if (pObj->GetValue(strParameters) == S_OK )
				WriteOCRParameters(writer, strParameters.GetBuffer());
		}
		else
		{
			BOOL bGotValue = FALSE;
			CString sValue;
			if ( SVDWordValueObjectClass* pdwValueObject = dynamic_cast <SVDWordValueObjectClass*> ( pObj ) )
			{
				DWORD dwValue=0;
				bGotValue = (pdwValueObject->GetValue( dwValue ) == S_OK );
				sValue = AsString( dwValue );
			}
			else
			{
				bGotValue = (pObj->GetValue( sValue ) == S_OK );
			}

			if ( !bGotValue || sValue.IsEmpty())
			{
				writer->WriteAttributeString(NULL, L"Value", NULL, invalid);
			}
			else
			{
				writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(sValue.GetBuffer(), cp_dflt).c_str());
			}
		}  // end else
		writer->WriteEndElement();
	}   // end if ( pObj->uObjectAttributesAllowed & SV_PRINTABLE )
}

inline void SVConfigXMLPrint::WriteTool(Writer writer, SVToolClass * ts) const
{
	wchar_t buff[64];
	if (ts)
	{
	    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
		SVInputInfoListClass    toolInputList;
		SVImageClass*           pCurrentSourceImage = NULL;
		SVInObjectInfoStruct*   pImageInputInfo = NULL;
		SVToolClass*            pTool = ts;
		
		SVInObjectInfoStruct* l_psvImageInfo = NULL;
		SVInObjectInfoStruct* l_psvLastImageInfo = NULL;

		while( l_psvImageInfo == NULL && pTool->FindNextInputImageInfo( l_psvImageInfo, l_psvLastImageInfo ) == S_OK )
		{
			if( l_psvImageInfo != NULL )
			{
				pImageInputInfo = l_psvImageInfo;

				if( l_psvImageInfo->IsConnected() )
				{
					pCurrentSourceImage = dynamic_cast <SVImageClass*> (l_psvImageInfo->GetInputObjectInfo().PObject);

					WriteValueObject(writer, L"Property",  utf16(pApp->GetStringResource(IDS_IMAGE_SOURCE_STRING)), to_utf16(pCurrentSourceImage->GetCompleteObjectNameToObjectType().GetBuffer(), cp_dflt).c_str());
				}
			}
			else
			{
				l_psvLastImageInfo = l_psvImageInfo;
				l_psvImageInfo = NULL;
			}
		}

		SVImageInfoClass* pImageInfo = NULL;
		if (ts->GetObjectName() != pApp->GetStringResource(IDS_CLASSNAME_SVGAGETOOL))
			pImageInfo = reinterpret_cast <SVImageInfoClass*> ( ::SVSendMessage(pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL) );
		
		if (pImageInfo)
		{
			POINT l_oPoint;
			POINT l_oOutputPoint;
			CString sLabel;
			CString sValue;

			long l_lWidth = 0;
			long l_lHeight = 0;

			if ( pImageInfo->GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint ) == S_OK &&
				 pImageInfo->GetExtentProperty( SVExtentPropertyOutputPositionPoint, l_oOutputPoint ) == S_OK &&
				 pImageInfo->GetExtentProperty( SVExtentPropertyWidth, l_lWidth ) == S_OK &&
				 pImageInfo->GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) == S_OK )
			{
				sLabel = pApp->GetStringResource(IDS_TOOL_LENGTH_STRING);
				sValue.Format("%d", l_lWidth);
				WriteValueObject(writer,  L"Property", utf16(sLabel), _itow(l_lWidth, buff, 10));
				
				sLabel = pApp->GetStringResource(IDS_TOOL_HEIGHT_STRING);
				sValue.Format("%d", l_lHeight);
				WriteValueObject(writer,  L"Property", utf16(sLabel), _itow(l_lHeight, buff, 10));
				
				sLabel = pApp->GetStringResource(IDS_ABSOLUTE_POS_STRING);
				sValue.Format("(%d, %d)", l_oPoint.x, l_oPoint.y);
				WriteValueObject(writer,  L"Property", utf16(sLabel), utf16(sValue));
				
				sLabel = pApp->GetStringResource(IDS_RELATIVE_POS_STRING);
				sValue.Format("(%d, %d)", l_oOutputPoint.x, l_oOutputPoint.y);
				WriteValueObject(writer,  L"Property", utf16(sLabel), utf16(sValue));
			}
		} // End, if( pImageInfo )
	}
}

inline void SVConfigXMLPrint::WriteArchiveTool(Writer writer, SVArchiveTool * ar) const
{
	if (ar)
	{
		wchar_t buff[64];
		int i, s;
		SVArchiveRecord* pRecord;
		
		writer->WriteStartElement(NULL, L"Results", NULL);
		s = ar->m_arrayResultsInfoObjectsToArchive.GetSize();
		for (i = 0; i < s; i++)
		{
			pRecord = ar->m_arrayResultsInfoObjectsToArchive.GetAt(i);
			if (pRecord->GetObjectReference().Object() != NULL)
			{
				writer->WriteStartElement(NULL, L"Result", NULL);
				writer->WriteAttributeString(NULL, L"Number", NULL, _itow(i + 1, buff, 10));
				writer->WriteAttributeString(NULL, L"Name", NULL, utf16(pRecord->GetObjectReference().GetCompleteObjectName()).c_str());
				writer->WriteEndElement();
			}
		}
		writer->WriteEndElement();
		writer->WriteStartElement(NULL, L"Images", NULL);
		
		s = ar->m_arrayImagesInfoObjectsToArchive.GetSize();
		for (i = 0; i < s; i++)
		{
			pRecord = ar->m_arrayImagesInfoObjectsToArchive.GetAt(i);
			writer->WriteStartElement(NULL, L"Result", NULL);
			writer->WriteAttributeString(NULL, L"Number", NULL, _itow(i + 1, buff, 10));
			if (pRecord)
			{
				writer->WriteAttributeString(NULL, L"Name", NULL, utf16(pRecord->GetImageObjectName()).c_str());
			}
			else
			{
				writer->WriteAttributeString(NULL, L"Error", NULL, invalid);
			}
			writer->WriteEndElement();
		}
		writer->WriteEndElement();
	}
}

inline void SVConfigXMLPrint::WriteObject( Writer writer, SVObjectClass* pObj ) const
{
    SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
    int      nFirstHeight   = 0;
    int      nLastHeight    = 0;
	
	CString	sLabel, sValue;
    CString  strType   = pObj->GetObjectName();
    CString  strName   = pObj->GetName();
	wchar_t buff[64];
	
    GUID     guidObjID = pObj->GetClassID();
	
	BOOL	bWriteToolExtents = FALSE;		// Sri 2/17/00
	
    // If object is a value object, get embedded ID.
    if (SV_IS_KIND_OF(pObj, SVValueObjectClass))
    {
        guidObjID = pObj->GetEmbeddedID();
    }  // end if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) )
	
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
	if ( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pObj) )
	{
		WriteValueObject(writer, pValueObject);
	}  // end if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) )
	else
	{
		do
		{
			if ( dynamic_cast <SVShapeMaskHelperClass*> (pObj) )
				if ( !( pObj->ObjectAttributesAllowed() & SV_PRINTABLE) )	// EB 20050818 - hack this instead of doing it right
					break;

			writer->WriteStartElement(NULL, to_utf16(pObj->GetClassName(), cp_dflt).c_str(), NULL);
			std::wstring objName = to_utf16(pObj->GetName(), cp_dflt);
			writer->WriteAttributeString(NULL, L"Name", NULL, objName.c_str());
			if ( SVToolClass* pTool = dynamic_cast<SVToolClass*> (pObj) )
			{
				// Increment even if disabled, to maintain count.  Starts with zero, so for first
				//    tool, will increment to 1.
				nToolNumber++;
				bWriteToolExtents = TRUE;		// Sri 2/17/00
				writer->WriteAttributeString(NULL, L"ToolNumber", NULL, _itow(nToolNumber, buff, 10));
			}  // end if( SV_IS_KIND_OF( pObj, SVToolClass ) )
			
			// If the object type is different from the name, print the type.
			if (strName != strType && !strType.IsEmpty())
			{
				writer->WriteAttributeString(NULL, L"Type", NULL, to_utf16(strType.GetBuffer(), cp_dflt).c_str());
			}
			
			// Print the tool length, width, extends, etc here.
			if (bWriteToolExtents && (SV_IS_KIND_OF(pObj, SVToolClass)))
			{
				bWriteToolExtents = FALSE;
				WriteTool(writer, dynamic_cast<SVToolClass*> (pObj));
			} // End, if(bPrintToolExtents && ( SV_IS_KIND_OF( pObj, SVToolClass )))
			
			if ( SVArchiveTool* pTool = dynamic_cast <SVArchiveTool*> (pObj) )
			{
				WriteArchiveTool(writer, pTool);
			}// end if ( SVArchiveTool* pTool = dynamic_cast <SVArchiveTool*> (pObj) )
			
			if ( SVStatisticsToolClass* pTool = dynamic_cast<SVStatisticsToolClass*> (pObj) )
			{
				SVObjectReference refObject = pTool->GetVariableSelected();
				if (refObject.Object())
				{
					writer->WriteStartElement(NULL,  L"Variable", NULL);
					writer->WriteAttributeString(NULL, L"Name", NULL, utf16(refObject.GetName()).c_str());
					writer->WriteEndElement();
				}
			}  // end if( SV_IS_KIND_OF( pObj, SVStatisticsToolClass ) )
			
			if (SV_IS_KIND_OF(pObj, SVUserMaskOperatorClass))
			{
				SVUserMaskOperatorClass* maskObj = dynamic_cast <SVUserMaskOperatorClass*>( pObj );
				if ( NULL != maskObj )
				{
					SVImageClass* pImage = maskObj->getMaskInputImage();
					if (nullptr != pImage)
					{
						WriteValueObject(writer, L"Property",  utf16(pApp->GetStringResource(IDS_IMAGE_SOURCE_STRING)), to_utf16(pImage->GetCompleteObjectName().GetBuffer(), cp_dflt).c_str());
					}
				}
			}

			if (SV_IS_KIND_OF(pObj,SVDoubleResultClass))
			{
				SVDoubleResultClass* pBlobResult = dynamic_cast<SVDoubleResultClass*>(pObj);
				if (pBlobResult)
				{
					if (SV_IS_KIND_OF(pBlobResult->GetOwner(),SVBlobAnalyzerClass))
					{  
						sLabel = pApp->GetStringResource(IDS_BLOB_FEATURE_DEFAULT_VALUE) + _T(":");
						SVDoubleValueObjectClass* pDoubleValueObj = pBlobResult->getInputDouble();
						if ( pDoubleValueObj )
						{
							double dVal;
							HRESULT hr = pDoubleValueObj->GetDefaultValue(dVal);
							sValue.Format(_T("%lf"),dVal);
							//ptCurPos.x   = (nIndentLevel + 1) * m_shortTabPixels;
							//PrintValueObject(pDC, ptCurPos, utf16(sLabel), utf16(sValue));
		
							WriteValueObject(writer, L"Property", utf16(sLabel), utf16(sValue));

						}
					}
				}
			}

			if (SV_IS_KIND_OF(pObj, SVLineAnalyzerClass))
			{
				SVObjectInfoStruct objectInfo;
				objectInfo.ObjectTypeInfo.ObjectType = SVLineObjectType;
				
				SVLineClass* pLineClass = dynamic_cast <SVLineClass*> (reinterpret_cast<SVObjectClass*> (SVSendMessage( pObj, SVM_GETFIRST_OBJECT, reinterpret_cast<DWORD_PTR>(&objectInfo), NULL) ) );
				
				POINT l_oHeadPoint;
				POINT l_oTailPoint;

				if ( pLineClass != NULL &&
					 pLineClass->GetHeadPoint( l_oHeadPoint ) == S_OK &&
					 pLineClass->GetHeadPoint( l_oTailPoint ) == S_OK )
				{
					sLabel = pApp->GetStringResource(IDS_HEAD_POINT_STRING);
					sValue.Format(_T("(%d, %d)"), (int)(l_oHeadPoint.x), (int)(l_oHeadPoint.y));
					WriteValueObject(writer,  L"Property", utf16(sLabel), utf16(sValue));
					
					sLabel = pApp->GetStringResource(IDS_TAIL_POINT_STRING);
					sValue.Format(_T("(%d, %d)"), (int)(l_oTailPoint.x), (int)(l_oTailPoint.y));
					WriteValueObject(writer, L"Property", utf16(sLabel), utf16(sValue));
				} // End, if(pLineClass)
				
			} // End if(SV_IS_KIND_OF( pObj, SVLineAnalyzerClass))
			if (SV_IS_KIND_OF(pObj, SVTaskObjectClass))
			{
				if ( SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (pObj) )
				{
					pEquation->GetEquationText(sValue);
		
					// Write the equation text.
					if (sLabel.IsEmpty())
						sLabel = _T("EquationText");
					WriteValueObject(writer, L"Equation", utf16(sLabel), utf16(sValue));
				}  // end if( SV_IS_KIND_OF( pObj, SVEquationClass ) )
				WriteInputOutputList(writer, pObj);
			}  // end if( SV_IS_KIND_OF( pObj, SVTaskObjectClass )
			WriteFriends(writer, pObj);
			WriteChildren(writer, pObj);
			writer->WriteComment(objName.c_str());
			writer->WriteEndElement();
		} while (false);// end do
	}// End if( SV_IS_KIND_OF( pObj, SVValueObjectClass ) ) else

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
		writer->WriteStartElement(NULL, L"Children", NULL);
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
							std::wstring name = to_utf16(it->first.c_str(), cp_dflt);
							writer->WriteAttributeString(nullptr, L"Name", nullptr, name.c_str());
						}
						break;

						case ToolGroupData::EndOfGroup:
						{
							writer->WriteStartElement(nullptr, L"EndToolGrouping", nullptr);
							std::wstring name = to_utf16(it->first.c_str(), cp_dflt);
							writer->WriteAttributeString(nullptr, L"Name", nullptr, name.c_str());
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
	}  // end if( SV_IS_KIND_OF( pObj, SVTaskObjectListClass ) )
}  // end function void SVConfigXMLPrint:::PrintChildren( ... )

void SVConfigXMLPrint::WriteFriends( Writer writer, SVObjectClass* pObj ) const
{
	const SVObjectInfoArrayClass& rFriendList = pObj->GetFriendList();
	size_t sz = rFriendList.size();

	if (sz > 0)
	{
		writer->WriteStartElement(NULL, L"Friends", NULL);
		for (size_t nCnt = 0; nCnt < sz; nCnt++)
		{
			const SVObjectInfoStruct& rObjInfo = rFriendList[nCnt];
			
			if (rObjInfo.PObject)
			{
				WriteObject(writer, rObjInfo.PObject);
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
		
		if (pOutput->PObject->GetOwner() == pObj)
		{
			WriteObject(writer, pOutput->PObject);
		}  // end if( pOutput->PObject->GetOwner () == pObj )
	}  // end for( int nCnt = 0; nCnt < pOutputInfoList->GetSize(); nCnt++ )
}  // end function void SVConfigXMLPrint:::PrintInputOutputList( ... )

void SVConfigXMLPrint::WriteOCRParameters(Writer writer, const std::string & strParameters) const
{
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	
	SVString  sLabel, sValue;
	
	SVOCRParamStruct*	pOCRParam = new SVOCRParamStruct();
	
	if (pOCRParam && !strParameters.empty())
	{
		pOCRParam->ConvertStringToParams(strParameters.c_str());
		
		writer->WriteStartElement(NULL, L"OCRParameters", NULL);
		
		sValue.Format(_T("%f"), pOCRParam->fFeaturesUsed);
		WriteValueObject(writer,  L"Param", L"Features Used", utf16(sValue));
		sValue.Format(_T("%f"), pOCRParam->fmaxAngle);
		WriteValueObject(writer,  L"Param", L"Max Angle", utf16(sValue));
		sValue.Format(_T("%f"), pOCRParam->fmaxScale);
		WriteValueObject(writer,  L"Param", L"Max Scale", utf16(sValue));
		sValue.Format(_T("%f"), pOCRParam->forientationSensitivity);
		WriteValueObject(writer,  L"Param", L"Orientation Sensitivity", utf16(sValue));
		sValue.Format(_T("%f"), pOCRParam->fthreshold);
		WriteValueObject(writer,  L"Param", L"Threshold", utf16(sValue));
		sValue.Format(_T("%s"), pOCRParam->ignoreNoMatch ? _T("Yes") : _T("No"));
		WriteValueObject(writer,  L"Param", L"Ignore No Match", utf16(sValue));
		sValue.Format(_T("(%d, %d)"), pOCRParam->minHeight, pOCRParam->maxHeight);
		WriteValueObject(writer,  L"Param", L"Height (min, max)", utf16(sValue));
		sValue.Format(_T("(%d, %d)"), pOCRParam->minWidth, pOCRParam->maxWidth);
		WriteValueObject(writer,  L"Param", L"Width (min, max)", utf16(sValue));
		sValue.Format(_T("(%d, %d)"), pOCRParam->nMinPixelsInBlob, pOCRParam->nMaxPixelsInBlob);
		WriteValueObject(writer,  L"Param", L"Pixels in Blob (min, max)", utf16(sValue));
		pOCRParam->STRmatchString;
		pOCRParam->STRnoMatchLabel;
		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteOCRGrayScaleParameters(Writer writer, const std::string & strParameters) const
{
	SVObserverApp* pApp = dynamic_cast <SVObserverApp*> (AfxGetApp());
	int      nFirstHeight   = 0;
	int      nLastHeight    = 0;
	
	CString  sLabel, sValue;
	
	SVOCRGrayParamStruct* pOCRGrayParam = new SVOCRGrayParamStruct();
	
	if (pOCRGrayParam && !strParameters.empty())
	{
		pOCRGrayParam->ConvertStringToParams(strParameters.c_str());
		
		writer->WriteStartElement(NULL, L"OCRGrayScaleParameters",NULL);
		
		WriteValueObject(writer,  L"Param", L"Match Label", utf16(pOCRGrayParam->STRnoMatchLabel));
		WriteValueObject(writer,  L"Param", L"Match String", utf16(pOCRGrayParam->STRmatchString));
		
		sValue.Format(_T("%d"), pOCRGrayParam->useMatchFile);	// int
		WriteValueObject(writer,  L"Param", L"Use Match File", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->maxMatches);		// int
		WriteValueObject(writer,  L"Param", L"Max Matches", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->xVicinity);	// int
		WriteValueObject(writer,  L"Param", L"X Vicinity", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->yVicinity);	// int
		WriteValueObject(writer,  L"Param", L"Y Vicinity", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->rejectThresh);	// float
		WriteValueObject(writer,  L"Param", L"Reject Threshold", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->acceptThresh);	// float
		WriteValueObject(writer,  L"Param", L"Accept Threshold", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->relThresh);	// float
		WriteValueObject(writer,  L"Param", L"Relative Threshold", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->minContrast);	// float
		WriteValueObject(writer,  L"Param", L"Min Contrast", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->scale);	// int
		WriteValueObject(writer,  L"Param", L"Scale", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->maxCandidates);	// int
		WriteValueObject(writer,  L"Param", L"Max Candidates", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->candsInVicinity);	// int
		WriteValueObject(writer,  L"Param", L"Candidates in Vicinity", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->candidateThresh);	// float
		WriteValueObject(writer,  L"Param", L"Candidate Threshold", utf16(sValue));
		sValue.Format(_T("%f"), pOCRGrayParam->candidateRelThresh);	// float
		WriteValueObject(writer,  L"Param", L"Candidate Relative Threshold", utf16(sValue));
		sValue.Format(_T("%d"), pOCRGrayParam->output);	// int
		WriteValueObject(writer,  L"Param", L"Output", utf16(sValue));
		writer->WriteEndElement();
	}
}

void SVConfigXMLPrint::WriteValueObject(Writer writer, const std::wstring tag, const std::wstring lpszName, const std::wstring lpszValue) const
{
	writer->WriteStartElement(NULL, tag.c_str(), NULL);
	writer->WriteAttributeString(NULL, L"Name", NULL, lpszName.c_str());
	writer->WriteAttributeString(NULL, L"Value", NULL, lpszValue.c_str());
	writer->WriteEndElement();
}

void SVConfigXMLPrint::WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const
{
	SVDigitalInputObject	*pDigInput = NULL;
	SVDigitalOutputObject	*pDigOutput = NULL;
	std::wstring 			sValue;
	
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( IOEntry->m_IOId );
	writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(l_pObject->GetName(), cp_dflt).c_str());
	
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
	SVString Value;
	int Index (0);

	BasicValueObjects::ValueVector GlobalConstantObjects;
	RootObject::getRootChildObjectList( GlobalConstantObjects, SvOl::FqnGlobal );

	writer->WriteStartElement(NULL, L"GlobalConstants", NULL);

	BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
	while ( GlobalConstantObjects.cend() != Iter )
	{
		const BasicValueObjectPtr& pGlobalConstant = *Iter;

		if( !pGlobalConstant.empty() )
		{
			Value.Format( L"GlobalConstant%d", ++Index );
			writer->WriteStartElement(NULL, to_utf16( Value.c_str(), cp_dflt).c_str(), NULL);
			//write Global Constant Name
			Value = pGlobalConstant->GetCompleteObjectName();
			writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16( Value.c_str(), cp_dflt).c_str() );
			//write Global Constant Value
			pGlobalConstant->getValue( Value );
			writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16( Value.c_str(), cp_dflt).c_str() );
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
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		CString s;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVLongValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.lValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				s = iterOption->strDescription.ToString();
			}
		}
		if ( s.IsEmpty() )
			s.Format("%lu%s", (unsigned long) param.GetScaledValue(), param.info.sUnits.IsEmpty() ? "" : " " + param.info.sUnits );
		m_writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(s.GetString(), cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVi64ValueDeviceParam& param )
{
	const SVi64ValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		CString s;
		s.Format("%I64d", param.iValue );
		m_writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(s.GetString(), cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVBoolValueDeviceParam& param )
{
	const SVBoolValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		CString s;
		if ( pCamFileParam->info.options.size() > 0 )
		{
			SVBoolValueDeviceParam::OptionsType::const_iterator iterOption;
			iterOption = std::find( pCamFileParam->info.options.begin(), pCamFileParam->info.options.end(), param.bValue );
			if ( iterOption != pCamFileParam->info.options.end() )
			{
				s = iterOption->strDescription.ToString();
			}
		}
		if ( s.IsEmpty() )
			s.Format("%s", param.bValue ? _T("TRUE") : _T("FALSE") );
		m_writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(s.GetString(), cp_dflt).c_str());
		m_writer->WriteEndElement();
	}
	return S_OK;
}

inline HRESULT SVDeviceParamConfigXMLHelper::Visit( SVStringValueDeviceParam& param )
{
	const SVStringValueDeviceParam* pCamFileParam = m_rCamFileParams.Parameter( param.Type() ).DerivedValue( pCamFileParam );
	if ( pCamFileParam )
	{
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		m_writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(param.strValue.c_str(), cp_dflt).c_str());
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
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		CString s;
		SVCameraFormat* pFormat = NULL;
		if ( param.options.size() > 0 )
		{
			SVCameraFormat& rFormat = param.options[ param.strValue ];
			const SVCameraFormat& rCamFileFormat = pCamFileParam->options.find( param.strValue )->second;
			if ( rCamFileFormat.m_strName == param.strValue )
			{
				s = rCamFileFormat.strDescription.ToString();
				pFormat = &rFormat;
			}
		}
		if ( s.IsEmpty() )
			s = param.strValue.ToString();
		m_writer->WriteAttributeString(NULL, L"Name", NULL, to_utf16(pCamFileParam->Name(), cp_dflt).c_str());
		m_writer->WriteAttributeString(NULL, L"Value", NULL, to_utf16(s.GetString(), cp_dflt).c_str());

		if ( pFormat )
		{
			m_writer->WriteAttributeString(NULL, L"Left", NULL, to_utf16(AsString(pFormat->lHPos).GetString(), cp_dflt).c_str());
			m_writer->WriteAttributeString(NULL, L"Top", NULL, to_utf16(AsString(pFormat->lVPos).GetString(), cp_dflt).c_str());
			m_writer->WriteAttributeString(NULL, L"Width", NULL, to_utf16(AsString(pFormat->lWidth).GetString(), cp_dflt).c_str());
			m_writer->WriteAttributeString(NULL, L"Height", NULL, to_utf16(AsString(pFormat->lHeight).GetString(), cp_dflt).c_str());
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
		m_writer->WriteStartElement(NULL, L"Param", NULL);
		SVDeviceParamWrapper w(pParam->GetHeldParam());
		SVDeviceParam* p = w;
		Visit(*p);
		m_writer->WriteEndElement();
	}
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConfigXMLPrint.inl_v  $
 * 
 *    Rev 1.15   23 Jul 2014 10:34:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  876
 * SCR Title:  Add user mask image to print configuration
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed WriteObject for the SVUserMaskOperatorClass to check to make sure that the SVImageClass is not NULL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Jul 2014 07:54:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method WriteMonitorListSection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Jul 2014 14:53:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added WriteAllChildren method.
 * Removed GetToolGroup function.
 * Aded GetToolGroupings function.
 * Added getTool function.
 * Revised WriteObject method.
 * Revised WriteChildren method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   14 Jul 2014 08:28:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  913
 * SCR Title:  Add Blob Feature Default Value to the Configuration Print (SVO-214)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed PrintDetails - to check for BlobAnalyzerResults and to print the default values for the features
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Jul 2014 10:02:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetToolGroup static function.
 * Revised WriteObject for printing Tool Groupings.
 * Revised WriteChildren for printing Tool Groupings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 May 2014 11:10:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Feb 2014 13:27:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed GetSECFileName to getConfigFileName and GetSECFullFileName to getConfigFullFileName in the PrintXMLDoc method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Feb 2014 10:32:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Dec 2013 13:55:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  876
 * SCR Title:  Add user mask image to print configuration
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed WriteTool method to get "Image Source" from string table (IDS_IMAGE_SOURCE_STRING) and print the image source for SVUserMaskOperatorClass.
 * Removed empty lines and dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Oct 2013 12:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Aug 2013 09:52:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include of SVObjectLibrary.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2013 13:26:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2013 13:23:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Solve the duplicate path names issue (issue 5 from SVRC issue list).  Added code page flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2013 19:29:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Changed special characters in "I N V A L I D", "Device", "No Device" and "No acquisition device" strings to avoid UTF-8 issue with XML writer.
 *   Changed "Title" to "Name" for InspectionProcess node.
 *   Changed to omit unassigned DigitalOutput nodes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2013 07:32:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jun 2013 11:33:00   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed nodes out of order. Fixed strange characters at the end of report.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2013 19:22:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2013 03:16:08   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
