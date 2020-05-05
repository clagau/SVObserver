//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolTask
//* .File Name       : $Workfile:   SVExternalToolTask.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.10  $
//* .Check In Date   : $Date:   30 Sep 2014 15:41:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolTask.h"
#include "SVRange.h"
#include "Definitions/TextDefineSVDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVTaskObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVLibrary\SVOINIClass.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxErrorEnum.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVResult.h"
#include "SVVariantResultClass.h"
#include "Tools/SVTool.h"
#include "TableObject.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"


#pragma endregion Includes


namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVExternalToolTask, SvPb::ExternalToolTaskClassId)

SVExternalToolTaskData::SVExternalToolTaskData() : m_ResultTableObjects(NUM_RESULT_TABLE_OBJECTS, nullptr)
{
}

SVExternalToolTaskData::SVExternalToolTaskData(const SVExternalToolTaskData& src)
{
	*this = src;
}

SVExternalToolTaskData::~SVExternalToolTaskData()
{
}

	
SVExternalToolTaskData& SVExternalToolTaskData::operator = (const SVExternalToolTaskData& rhs)
{
	if (this != &rhs)
	{
		// EB 20040825 added these... why weren't they here before???
		m_voDllPath = rhs.m_voDllPath;
		m_aDllDependencies = rhs.m_aDllDependencies;

		m_voToolName = rhs.m_voToolName;
		m_voToolVersion = rhs.m_voToolVersion;

		m_aInputImageInfo = rhs.m_aInputImageInfo;

		//////////////////////////////////////////////////////
		// !! EB 20040820
		// These vectors contain registered objects which cannot be deallocated!
		// use std::copy which simply performs element assignment
		// instead of vector::operator =
		// note: operator = will work in VC6, but this is nonportable

		if (m_aInputObjects.size() != rhs.m_aInputObjects.size())	// if this is only a copy of the original, size will be 0
		{
			m_aInputObjects.resize(rhs.m_aInputObjects.size());
		}
		std::copy(rhs.m_aInputObjects.begin(), rhs.m_aInputObjects.end(), m_aInputObjects.begin());


		if (m_aResultObjects.size() != rhs.m_aResultObjects.size())	// if this is only a copy of the original, size will be 0
		{
			m_aResultObjects.resize(rhs.m_aResultObjects.size());
		}
		std::copy(rhs.m_aResultObjects.begin(), rhs.m_aResultObjects.end(), m_aResultObjects.begin());

		m_aResultImageDefinitions = rhs.m_aResultImageDefinitions;
		m_ResultDefinitions = rhs.m_ResultDefinitions;
		m_InputDefinitions = rhs.m_InputDefinitions;

		m_lNumInputImages = rhs.m_lNumInputImages;
		m_lNumInputValues = rhs.m_lNumInputValues;
		m_lNumResultImages = rhs.m_lNumResultImages;
		m_lNumResultValues = rhs.m_lNumResultValues;
		m_NumResultTables = rhs.m_NumResultTables;
		m_NumLinkedValue = rhs.m_NumLinkedValue;

		m_RangeResultData = rhs.m_RangeResultData;

		m_PropTreeState = rhs.m_PropTreeState;
	}

	return *this;
}





SVExternalToolTask::SVExternalToolTask(SVObjectClass* POwner, int StringResourceID)
	:SVTaskObjectListClass(POwner, StringResourceID), ISVCancel()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVExternalToolTaskObjectType;

	size_t i;

	SvLib::SVOINIClass m_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	m_bUseImageCopies = m_SvimIni.GetValueInt(_T("External Tool"), _T("UseImageCopy"), TRUE) != FALSE;

	// Initialize input images array.
	m_Data.m_aInputImageInfo.resize(SVExternalToolTaskData::NUM_INPUT_IMAGES);
	for (i = 0; i < m_Data.m_aInputImageInfo.size(); i++)
	{
		std::string l_Name = SvUl::Format(_T("ExternalToolTaskImage%d"), i);

		// this object will be filled in SVTaskObject::ConnectAllInputs
		// we supply the base object type (Image) and ConnectAllInputs finds the nearest match (Toolset main image)
		m_Data.m_aInputImageInfo[i].SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
		m_Data.m_aInputImageInfo[i].SetObject(GetObjectInfo());
		RegisterInputObject(&m_Data.m_aInputImageInfo[i], l_Name);
	}

	RegisterEmbeddedObject(&m_Data.m_voDllPath, SvPb::DllFileNameEId, IDS_OBJECTNAME_DLL_PATH, false, SvOi::SVResetItemTool);

	// Initialize Dll Dependencies 
	m_Data.m_aDllDependencies.resize(SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES);
	for (i = 0; i < m_Data.m_aDllDependencies.size(); i++)
	{
		RegisterEmbeddedObject(&m_Data.m_aDllDependencies[i], SvPb::DllDependencyFileNameEId+i, IDS_OBJECTNAME_DLL_DEP_FILE_01 + static_cast<int>(i), false, SvOi::SVResetItemTool);
	}

	// init Tool Name
	RegisterEmbeddedObject(&m_Data.m_voToolName, SvPb::DllToolNameEId, IDS_OBJECTNAME_DLL_TOOL_NAME, false, SvOi::SVResetItemNone);
	m_Data.m_voToolName.setSaveValueFlag(false);

	// Init Tool Version
	RegisterEmbeddedObject(&m_Data.m_voToolVersion, SvPb::DllToolVersionEId, IDS_OBJECTNAME_DLL_TOOL_VERSION, false, SvOi::SVResetItemNone);
	m_Data.m_voToolVersion.setSaveValueFlag(false);

	// Init Input Object Info array
	m_Data.m_aInputObjects.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);


	for (i = 0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++)
	{
		RegisterEmbeddedObject(&m_Data.m_aInputObjects[i], SvPb::ExternalInputEId+i, IDS_OBJECTNAME_INPUT_01 + static_cast<int>(i), false, SvOi::SVResetItemTool);
		std::string ObjectName = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + static_cast<int>(i));
		ObjectName += SvDef::cLinkName;
		RegisterEmbeddedObject(&m_Data.m_aInputObjects[i].getLinkedName(), SvPb::ExternalInputLinkedEId+i, ObjectName.c_str(), false, SvOi::SVResetItemNone);



		// set default values
		VARIANT vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = VT_EMPTY;
		m_Data.m_aInputObjects[i].SetDefaultValue(vtTemp, true);



	}

	// Set Default Image Parameters for Result Images
	int l_pImageNames[] = {IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2, IDS_OBJECTNAME_IMAGE3, IDS_OBJECTNAME_IMAGE4};
	for (i = 0; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
	{
		SVImageInfoClass imageInfo;

		RegisterEmbeddedObject(&m_aResultImages[i], SvPb::OutputImageEId+i, l_pImageNames[i]);

		if (nullptr != GetTool())
		{
			imageInfo.SetOwner(GetTool()->getObjectId());
		}

		imageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8);
		imageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);

		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>(0.0, 0.0));
		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, 100);
		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, 100);

		SvIe::SVImageClass* pImage = &(m_aResultImages[i]);
		pImage->InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);
		pImage->UpdateImage(SvDef::InvalidObjectId, imageInfo);
	}

	// Result Objects
	m_Data.m_aResultObjects.resize(SVExternalToolTaskData::NUM_RESULT_OBJECTS);

	for (i = 0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		// Register
		RegisterEmbeddedObject(&m_Data.m_aResultObjects[i], SvPb::ExternalResultEId+i, IDS_OBJECTNAME_RESULT_01 + static_cast<int>(i), false, SvOi::SVResetItemNone);

		// Defaults
		VARIANT vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = VT_EMPTY;
		m_Data.m_aResultObjects[i].SetDefaultValue(vtTemp, true);
		m_Data.m_aResultObjects[i].setSaveValueFlag(false);
	}

	addDefaultInputObjects();

	// default tool counts
	m_Data.m_lNumInputImages = 0;
	m_Data.m_lNumInputValues = 0;
	m_Data.m_lNumResultImages = 0;
	m_Data.m_lNumResultValues = 0;

	m_Data.m_NumLinkedValue = 0;
	m_Data.m_NumResultTables = 0;

	SetAllAttributes();
}
bool SVExternalToolTask::NoExFktInLoadVersion()
{
	return  SvOi::getLoadingVersion() < 0xA0000;
}

void SVExternalToolTask::SetAllAttributes()
{
	for (int i = 0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++)
	{
		//Use add attributes because linked value objects can change the attributes if linked type 
		UINT attribute = (i < m_Data.m_NumLinkedValue) ? SvPb::remotelySetable | SvPb::setableOnline | SvPb::viewable | SvPb::selectableForEquation : SvPb::noAttributes;
		SvOi::SetAttributeType addOverwriteType = (i < m_Data.m_NumLinkedValue) ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::OverwriteAttribute;
		m_Data.m_aInputObjects[i].SetObjectAttributesAllowed(attribute, addOverwriteType);

		attribute = (i < m_Data.m_NumLinkedValue) ? SvDef::defaultValueObjectAttributes : SvPb::noAttributes;
		attribute &= ~SvPb::viewable;

	}

	for (int i = 0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		UINT attribute = (i < m_Data.m_lNumResultValues) ? SvDef::defaultValueObjectAttributes : SvPb::noAttributes;
		SVResultClass* pResultObject = GetResultRangeObject(i);
		if (nullptr != pResultObject)
		{
			pResultObject->SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
		}
		attribute &= ~SvPb::printable;
		m_Data.m_aResultObjects[i].SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
	}

	for (int i = 0; i < SVExternalToolTaskData::NUM_RESULT_TABLE_OBJECTS; i++)
	{
		if (m_Data.m_ResultTableObjects[i])
		{
			if (i < m_Data.getNumTableResults())
			{
				m_Data.m_ResultTableObjects[i]->Hide(false);
			}
			else
			{
				m_Data.m_ResultTableObjects[i]->Hide(true);
			}
		}
	}

	for (int i = 0; i < static_cast<long>(m_Data.m_aDllDependencies.size()); i++)
	{
		m_Data.m_aDllDependencies[i].SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
		m_Data.m_aDllDependencies[i].SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);
		std::string Temp;
		m_Data.m_aDllDependencies[i].GetValue(Temp);
		SvOi::SetAttributeType AddRemoveType = !Temp.empty() ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.m_aDllDependencies[i].SetObjectAttributesAllowed(SvPb::printable, AddRemoveType);
	}
	m_Data.m_voToolVersion.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_Data.m_voToolName.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::AddAttribute);
	m_Data.m_voDllPath.SetObjectAttributesAllowed(SvPb::printable | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
}

SVExternalToolTask::~SVExternalToolTask()
{
	SVExternalToolTask::CloseObject();
	// remove embeddeds so that invalid objects won't be Closed
	m_embeddedList.clear();
}

void SVExternalToolTask::SetResultArraySize()
{
	int ResultNum = __min(m_Data.getNumResults(), static_cast<int>(m_Data.m_aResultObjects.size()));
	assert(m_Data.getNumResults() <= m_Data.m_aResultObjects.size());
	for (int rn = 0; rn < ResultNum; rn++)
	{
		if (m_Data.m_ResultDefinitions[rn].getVT() & VT_ARRAY)
		{
			int nArraysize = m_Data.m_ResultDefinitions[rn].getMaxArraysize();
			m_Data.m_aResultObjects[rn].SetArraySize(nArraysize);
		}
	}
}
void SVExternalToolTask::CreateArrayInTable()
{
	int TableNum = m_Data.getNumTableResults();

	for (int t = 0; t < TableNum; t++)
	{
		if (t >= m_Data.m_ResultTableObjects.size() || m_Data.m_ResultTableObjects[t] == nullptr)
		{
			assert(false);
			break;
		}
		TableObject* pTab = m_Data.m_ResultTableObjects[t];
		int nRows = m_Data.m_TableResultDefinitions[t].getTableRowCount();
		int nCols = m_Data.m_TableResultDefinitions[t].getTableColoumnCount();
		std::vector<std::string> Columnames = m_Data.m_TableResultDefinitions[t].getColoumnNames();

		for (int i = 0; i < nCols; i++)
		{
			if (i >= c_maxTableColumn)
			{
				assert(false);
				break;
			}

			pTab->updateOrCreateColumn(SvPb::TableColumnValueEId+i, Columnames[i], nRows);
		}
		SvVol::ValueObjectSortContainer resultTableSortContainer;
		resultTableSortContainer.resize(nRows);
		for (int i = 0; i < nRows; i++)
		{
			resultTableSortContainer[i] = i;
		}
		pTab->setSortContainer(resultTableSortContainer, SVRunStatusClass());
	}
}
bool SVExternalToolTask::CreateTableObjects()
{
	bool result = true;
	int Index {0};
	for (SVTaskObjectClass* pOb : m_TaskObjectVector)
	{
		if (pOb && pOb->GetObjectType() == SvPb::TableObjectType && pOb->GetObjectSubType() == SvPb::SVNotSetSubObjectType)
		{
			assert(Index < SVExternalToolTaskData::NUM_RESULT_TABLE_OBJECTS);
			if (Index < SVExternalToolTaskData::NUM_RESULT_TABLE_OBJECTS)
			{
				m_Data.m_ResultTableObjects[Index++] = dynamic_cast<SvOp::TableObject*>(pOb);
			}
		}
	}

	for (int i = 0; i < SVExternalToolTaskData::NUM_RESULT_TABLE_OBJECTS; i++)
	{

		if (nullptr == m_Data.m_ResultTableObjects[i])
		{
			m_Data.m_ResultTableObjects[i] = new SvOp::TableObject(this);
			Add(m_Data.m_ResultTableObjects[i]);
			if (!CreateChildObject(m_Data.m_ResultTableObjects[i]))
			{
				result = false;
				SvStl::MessageContainer message;
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
				Msg.setMessage(message.getMessage());
				break;
			}
		}

	}
	return result;


}
bool SVExternalToolTask::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool ok = SVTaskObjectListClass::CreateObject(rCreateStructure);

	if (ok && nullptr != rCreateStructure.m_pTool)
	{
		if (nullptr != GetInspection() && nullptr != GetTool())
		{
			try
			{

				CreateTableObjects();

				Initialize();
			}
			catch (const SvStl::MessageContainer& /*e*/)
			{
				//hr = static_cast<HRESULT> (e.getMessage().m_MessageCode);
			}

			ok = true;
		}
	}

	m_isCreated = ok;

	return ok;
}
HRESULT SVExternalToolTask::InitializeResultObjects()
{
	// Initialize Result Objects
	long numResultObjects {0};
	HRESULT hr {S_OK};

	ResultValueDefinitionStruct* paResultValueDefs {nullptr};
	ResultValueDefinitionStructEx* paResultValueDefsEx {nullptr};
	if (m_dll.UseResultEx())
	{
		hr = m_dll.GetResultValueDefinitions(&numResultObjects, &paResultValueDefsEx);
	}
	else
	{
		m_dll.GetResultValueDefinitions(&numResultObjects, &paResultValueDefs);
	}

	m_Data.m_lNumResultValues = numResultObjects;
	if (S_OK != hr)
	{
		return  hr;
	}

	m_InspectionResultValues.resize(numResultObjects);
	m_Data.m_ResultDefinitions.resize(numResultObjects);

	std::unique_ptr<int[]> MaxArraySize(nullptr);
	if (m_dll.UseResultValuesMaxArraySize())
	{
		MaxArraySize = std::make_unique<int[]>(numResultObjects);
		m_dll.getResultValuesMaxArraySize(getObjectId(), numResultObjects, MaxArraySize.get());
	}

	for (int i = 0; i < numResultObjects; i++)
	{

		if (nullptr != paResultValueDefsEx)
		{
			m_Data.m_ResultDefinitions[i].setDefinition(paResultValueDefsEx[i], i);
		}
		else if (nullptr != paResultValueDefs)
		{
			m_Data.m_ResultDefinitions[i].setDefinition(paResultValueDefs[i], i);
		}
		if (MaxArraySize.get())
		{
			m_Data.m_ResultDefinitions[i].setMaxArraysize(MaxArraySize[i]);
		}

		long vt = m_Data.m_ResultDefinitions[i].getVT();
		if (vt != VT_BSTR)	// Do not allocate result if already exists....
		{
			SVResultClass* pResult = GetResultRangeObject(i);
			if (nullptr == pResult)
			{
				AllocateResult(i);
				pResult = GetResultRangeObject(i);
			}
			if (nullptr != pResult)
			{
				SvDef::SVObjectTypeInfoStruct info;
				info.m_ObjectType = SvPb::SVValueObjectType;
				info.m_SubType = SvPb::SVVariantValueObjectType;
				info.m_EmbeddedID = SvPb::ValueEId;
				SvVol::SVVariantValueObjectClass* pValue = dynamic_cast<SvVol::SVVariantValueObjectClass*>(pResult->getFirstObject(info));

				if (nullptr != pValue)
				{
					_variant_t defaultValue;

					defaultValue.ChangeType(static_cast<VARTYPE> (~(VT_ARRAY)& vt));
					pValue->SetDefaultValue(defaultValue, false);
				}
			}
		}


		if (m_Data.m_ResultDefinitions[i].UseDisplayNames())
		{
		
			std::string oldName = m_Data.m_aResultObjects[i].GetName();
			std::string resPrefix = SvUl::LoadStdString(IDS_EXTERNAL_DLL_RESULT_PREFIX);
			std::stringstream str;
			str << resPrefix << std::setfill('0') << std::setw(2) << i + 1 << "_" << m_Data.m_ResultDefinitions[i].getDisplayName();
			m_Data.m_aResultObjects[i].SetName(str.str().c_str());
			
			if (NoExFktInLoadVersion() && GetInspection())
			{
				GetInspection()->OnObjectRenamed(m_Data.m_aResultObjects[i], oldName);
			}
		
		}

		///Set Arraysize 
		SetResultArraySize();

	}
	if (nullptr != paResultValueDefsEx)
	{
		hr = m_dll.DestroyResultValueDefinitionStructures(paResultValueDefsEx);
	}
	else if (nullptr != paResultValueDefs)
	{
		hr = m_dll.DestroyResultValueDefinitionStructures(paResultValueDefs);
	}

	if (S_OK != hr)
	{
		return hr;
	}

	ResultTableDefinitionStruct* paResultTableDefs {nullptr};
	ResultTableDefinitionStructEx* paResultTableDefsEx {nullptr};
	long resultTableNum {0};
	std::unique_ptr<int[]> maxRowSizes(nullptr);
	if (m_dll.UseTableOutput())
	{
		if (m_dll.UseResultEx())
		{
			hr = m_dll.getResultTableDefinitions(&resultTableNum, &paResultTableDefsEx);
		}
		else
		{
			hr = m_dll.getResultTableDefinitions(&resultTableNum, &paResultTableDefs);
		}
		if (m_dll.UseResultTablesMaxRowSize())
		{
			maxRowSizes = std::make_unique<int[]>(resultTableNum);
			m_dll.getResultTablesMaxRowSize(getObjectId(), resultTableNum, maxRowSizes.get());
		}


		m_Data.m_NumResultTables = resultTableNum;
		m_Data.m_TableResultDefinitions.resize(resultTableNum);
		m_InspectionResultTables.resize(resultTableNum);

		for (int j = 0; j < resultTableNum; j++)
		{
			if (nullptr != paResultTableDefs)
			{
				m_Data.m_TableResultDefinitions[j].setDefinition(paResultTableDefs[j], j);
			}
			else if (nullptr != paResultTableDefsEx)
			{
				m_Data.m_TableResultDefinitions[j].setDefinition(paResultTableDefsEx[j], j);
			}

			if (maxRowSizes.get())
			{
				m_Data.m_TableResultDefinitions[j].setTableRowCount(maxRowSizes[j]);
			}



			if (m_Data.m_TableResultDefinitions[j].UseDisplayNames())
			{
				std::stringstream str;
				std::string tresPrefix = SvUl::LoadStdString(IDS_EXTERNAL_DLL_TRESULT_PREFIX);
				str << tresPrefix << std::setfill('0') << std::setw(2) << j + 1 << "_" << m_Data.m_TableResultDefinitions[j].getDisplayName();
				std::string name = str.str();
				if (GetResultTableObject(j))
				{
				
					std::string oldName = GetResultTableObject(j)->GetName();
					GetResultTableObject(j)->SetName(name.c_str());
					if (NoExFktInLoadVersion() && GetInspection())
					{
						GetInspection()->OnObjectRenamed(*GetResultTableObject(j), oldName);

					}
				}
			}
		}

		if (nullptr != paResultTableDefsEx)
		{
			hr = m_dll.destroyResultTableDefinitionStructures(paResultTableDefsEx);
		}
		else if (nullptr != paResultTableDefs)
		{
			hr = m_dll.destroyResultTableDefinitionStructures(paResultTableDefs);
		}
		paResultValueDefs = nullptr;
		paResultValueDefsEx = nullptr;
		CreateArrayInTable();
	}
	else
	{
		m_Data.m_NumResultTables = 0;
		m_Data.m_TableResultDefinitions.resize(0);
		m_InspectionResultTables.resize(0);

	}
	return hr;
}

HRESULT SVExternalToolTask::Initialize(SVDllLoadLibraryCallback fnNotify)
{

	HRESULT hr = S_FALSE;
	BSTR bstrName = nullptr;
	HRESULT hrUninitializeRun = m_dll.UninitializeRun(getObjectId());
	HRESULT hrClose = m_dll.Close();
	HRESULT hrUninitialize = Uninitialize();

	m_aInspectionInputImages.clear();
	m_InspectionInputValues.clear();
	m_InspectionResultValues.clear();
	m_aInspectionResultImages.clear();

	m_Data.m_InputDefinitions.clear();
	m_Data.m_ResultDefinitions.clear();
	m_Data.m_aResultImageDefinitions.clear();

	// try to connect to DLL
	std::string DllPath;
	m_Data.m_voDllPath.GetValue(DllPath);

	try
	{
		hr = m_dll.Open(DllPath.c_str(), fnNotify);
	}
	catch (const SvStl::MessageContainer& e)
	{
		SvStl::MessageContainer newMessage;
		newMessage.setMessage(e.getMessage(), getObjectId());
		throw newMessage;
	}

	if (S_OK == hr)
	{
		try
		{
			///////////////////////////////////////
			//    Get DLL information
			///////////////////////////////////////

			hr = m_dll.GetToolName(&bstrName);
			if (S_OK != hr)
			{
				throw hr;
			}

			std::string Name = SvUl::createStdString(bstrName);
			m_Data.m_voToolName.SetValue(Name);

			SysFreeString(bstrName);

			long lVersion = 0;
			hr = m_dll.GetToolVersion(&lVersion);

			if (S_OK != hr)
			{
				throw hr;
			}
			m_Data.m_voToolVersion.SetValue(lVersion);

			///////////////////////////////////////
			//    Initialize Inputs
			///////////////////////////////////////

			// Initialize Input Images

			hr = m_dll.GetNumberOfInputImages(&m_Data.m_lNumInputImages);
			if (S_OK != hr)
			{
				throw hr;
			}

			std::vector<SVImageDefinitionStruct> aInputImages;	// used in m_dll.InitializeRun

			m_aInspectionInputImages.resize(m_Data.m_lNumInputImages);
			m_aInspectionInputHBMImages.resize(m_Data.m_lNumInputImages);
			aInputImages.resize(m_Data.m_lNumInputImages);
			m_aInputImageInformationStructs.resize(m_Data.m_lNumInputImages);

			hr = m_dll.GetInputImageInformation(&m_aInputImageInformationStructs);
			if (S_OK != hr)
			{
				throw hr;
			}

			m_aPreviousInputImageRect.clear();
			for (int i = 0; i < m_Data.m_lNumInputImages; i++)
			{
				SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_Data.m_aInputImageInfo[i].GetInputObjectInfo().getObject());
				if (pImage)
				{
					SVImageInfoClass imageInfo = pImage->GetImageInfo();
					GetImageDefinitionStruct(aInputImages[i], imageInfo);

					RECT rect;
					imageInfo.GetOutputRectangle(rect);
					m_aPreviousInputImageRect.push_back(rect);

					// remember MIL handle
					SvTrc::IImagePtr pImageBuffer = pImage->getLastImage(true);
					if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
					{
						// this cast assumes that a mil handle will never be larger than 32 bits.
						m_aInspectionInputImages[i] = static_cast<long>(pImageBuffer->getHandle()->GetBuffer().GetIdentifier());
					}

					if (!m_dll.UseMil())
					{
						m_aInputImagesCopy[i] = nullptr;
						if (nullptr != m_aInspectionInputHBMImages[i].hbm)
						{
							::DeleteObject(m_aInspectionInputHBMImages[i].hbm);
							m_aInspectionInputHBMImages[i].FreeBitmapInfo();
							m_aInspectionInputHBMImages[i].Clear();
						}
						hr = SVIHBitmapUtilities::SVImageInfoToNewDIB(imageInfo, m_aInspectionInputHBMImages[i]);
						if (S_OK == hr)
						{
							SvIe::SVImageProcessingClass::convertToMILBuffer(m_aInspectionInputHBMImages[i].hbm, m_aInputImagesCopy[i]);
							SvOi::SVImageBufferHandlePtr l_ImageBufferCopy = m_aInputImagesCopy[i];
							if (nullptr != l_ImageBufferCopy && !l_ImageBufferCopy->empty())
							{
								MbufClear(l_ImageBufferCopy->GetBuffer().GetIdentifier(), 0);
							}
							else
							{
								hr = E_FAIL;
							}
						}
						if (S_OK != hr)
						{
							throw hr;
						}
					}
					else
					{
						if (m_bUseImageCopies)
						{
							SVImageInfoClass imageInfoCopy;
							GetImageInfo(&(aInputImages[i]), imageInfoCopy);	// get the physical info from our definition struct

							if (nullptr != GetTool())
							{
								imageInfoCopy.SetOwner(GetTool()->getObjectId());
							}
							else
							{
								imageInfoCopy.SetOwner(SvDef::InvalidObjectId);
							}

							// create buffer
							SvIe::SVImageProcessingClass::CreateImageBuffer(imageInfoCopy, m_aInputImagesCopy[i]);
						}
					}
				}// end if( pImage )
				else // can't find image!
				{
					// @TODO - add error handling
				}
			}// end for( int i = 0 ; i < m_Data.m_lNumInputImages ; i++ )


			///////////////////////////////////////
			// Initialize Input Objects
			///////////////////////////////////////

			InputValueDefinitionStructEx* paInputValueDefsEx = nullptr;
			InputValueDefinitionStruct* paInputValueDefs = nullptr;
			long ArraySize {0};
			if (m_dll.UseInputEx())
			{
				hr = m_dll.GetInputValueDefinitions(&ArraySize, &paInputValueDefsEx);
				if (hr == S_OK)
				{
					m_Data.SetInputValueDefinitions(ArraySize, paInputValueDefsEx);
				}
			}
			else
			{
				hr = m_dll.GetInputValueDefinitions(&ArraySize, &paInputValueDefs);
				if (hr == S_OK)
				{
					m_Data.SetInputValueDefinitions(ArraySize, paInputValueDefs);
				}
			}

			if (S_OK != hr)
			{
				throw hr;
			}
			m_InspectionInputValues.resize(ArraySize);

			m_Data.InitializeInputs(this);
			for (int i = 0; i < ArraySize; i++)
			{
				HRESULT hres = ::VariantChangeTypeEx(&m_InspectionInputValues[i], &m_InspectionInputValues[i], SvDef::LCID_USA, 0, static_cast<VARTYPE>(m_Data.m_InputDefinitions[i].getVt()));
				if (S_OK != hres)
				{
					///cant change variant empty to safe array
					m_InspectionInputValues[i] = m_Data.m_InputDefinitions[i].getDefaultValue();
				}
			}
			if (m_dll.UseInputEx())
			{
				hr = m_dll.DestroyInputValueDefinitionStructures(paInputValueDefsEx);
			}
			else
			{
				hr = m_dll.DestroyInputValueDefinitionStructures(paInputValueDefs);
			}
			if (S_OK != hr)
			{
				throw hr;
			}
			paInputValueDefs = nullptr;
			paInputValueDefsEx = nullptr;
			 
			SetIndirectValueSaveFlag();
			collectInputValues();


			hr = m_dll.InitializeRun(getObjectId(), (long)aInputImages.size(), aInputImages.size() ? &(aInputImages[0]) : nullptr,
				(long)m_InspectionInputValues.size(), m_InspectionInputValues.size() ? &(m_InspectionInputValues[0]) : nullptr);
			if (S_OK != hr)
			{
				throw hr;
			}
			hr = InitializeResultObjects();
			if (S_OK != hr)
			{
				throw hr;
			}

			if (m_dll.UseMil())
			{
				// This must be done after InitializeRun so the Dll has a id for this tool
				hr = m_dll.SetMILInputImages(getObjectId(), (long)m_aInspectionInputImages.size(), m_aInspectionInputImages.size() ? &(m_aInspectionInputImages[0]) : nullptr);
				if (S_OK != hr)
				{
					throw hr;
				}
			}
			///////////////////////////////////////
			// Initialize Result Images
			///////////////////////////////////////

			SVImageDefinitionStruct* paResultImageDefs = nullptr;
			long NumResultImages {0};
			hr = m_dll.GetResultImageDefinitions(getObjectId(), &NumResultImages, &paResultImageDefs);
			if (S_OK != hr)
			{
				throw hr;
			}
			m_Data.m_lNumResultImages = NumResultImages;
			m_aInspectionResultImages.resize(NumResultImages);
			m_aInspectionResultHBMImages.resize(NumResultImages);
			m_Data.m_aResultImageDefinitions.resize(NumResultImages);

			int l_pImageNames[] = {IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2, IDS_OBJECTNAME_IMAGE3, IDS_OBJECTNAME_IMAGE4};
			for (int i = 0; i < m_Data.m_aResultImageDefinitions.size(); i++)
			{
				m_Data.m_aResultImageDefinitions[i] = paResultImageDefs[i];

				{// begin block
				 // create buffer
					SvIe::SVImageClass* pImage = &(m_aResultImages[i]);
					pImage->SetObjectAttributesAllowed(SvPb::archivableImage | SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::OverwriteAttribute);
					//! Check is Feature already in embedded list	
					SVObjectPtrVector::const_iterator Iter = std::find_if(m_embeddedList.begin(), m_embeddedList.end(), [i](const SVObjectPtrVector::value_type pEntry)->bool
					{
						return (pEntry->GetEmbeddedID() == SvPb::OutputImageEId+i);
					}
					);
					if (m_embeddedList.end() == Iter)
					{
						RegisterEmbeddedObject(pImage, SvPb::OutputImageEId+i, l_pImageNames[i]);
					}

					// get image info
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if (nullptr != GetTool())
					{
						imageInfo.SetOwner(GetTool()->getObjectId());
					}
					else
					{
						imageInfo.SetOwner(SvDef::InvalidObjectId);
					}

					pImage->InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);
					if (!m_dll.UseMil())
					{
						imageInfo.setDibBufferFlag(true);
					}
					pImage->UpdateImage(SvDef::InvalidObjectId, imageInfo);
				}// end block
				if (m_bUseImageCopies)
				{
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if (nullptr != GetTool())
					{
						imageInfo.SetOwner(GetTool()->getObjectId());
					}
					else
					{
						imageInfo.SetOwner(SvDef::InvalidObjectId);
					}

					// create buffer
					SvIe::SVImageProcessingClass::CreateImageBuffer(imageInfo, m_aResultImagesCopy[i]);
				}// end if ( m_bUseImageCopies )

			}// end for ( int i = 0 ; i < m_Data.m_lNumResultImages; i++)

			for (int i = m_Data.m_lNumResultImages; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
			{
				RemoveEmbeddedObject(&(m_aResultImages[i]));
				SvIe::SVImageClass* pImage = &(m_aResultImages[i]);
				pImage->SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::OverwriteAttribute);
			}

			hr = m_dll.DestroyImageDefinitionStructure(paResultImageDefs);
			if (S_OK != hr)
			{
				throw hr;
			}
			paResultImageDefs = nullptr;

			if (m_dll.UseMil())
			{
				hr = m_dll.SetMILResultImages(getObjectId(), m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr);
				if (S_OK != hr)
				{
					throw hr;
				}
			}

			SetAllAttributes();
		}// end try
		catch (HRESULT)
		{
		}
		catch (int)
		{
			hr = S_FALSE;// Error Initializing Tool
		}
		catch (std::exception&)
		{
			hr = S_FALSE;
		}

#ifndef _DEBUG
		catch (...)
		{
			hr = S_FALSE;
		}
#endif
	}// end if ( S_OK == hr )

	if (S_OK == hr)
	{
		SvIe::SVImageClass* pOutputImage = GetResultImage(0);
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
		if (nullptr != pTool && nullptr != pOutputImage)
		{
			hr = pTool->UpdateOffsetData(pOutputImage);
		}
	}

	return hr;
}

HRESULT SVExternalToolTask::Uninitialize()
{
	HRESULT hr = S_OK;

	for (std::vector<SVDIBITMAPINFO>::iterator it = m_aInspectionInputHBMImages.begin(); it != m_aInspectionInputHBMImages.end(); ++it)
	{
		if (nullptr != it->hbm)
		{
			::DeleteObject(it->hbm);
			it->FreeBitmapInfo();
			it->Clear();
		}
	}
	m_aInspectionInputHBMImages.clear();
	return hr;
}


bool SVExternalToolTask::CloseObject()
{
	Uninitialize();
	if (IsCreated())
	{
		m_dll.UninitializeRun(getObjectId());
		m_dll.Close();
		if (SVTaskObjectClass::CloseObject())
		{
			bool bRetVal = true; // m_svOutputImageObject.CloseObject();

			return bRetVal;
		}
		return false;
	}
	return true;
}

bool SVExternalToolTask::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool ok = true;

	if (m_dll.IsHandleNull())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitExternalTaskFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetInvalid();
		return false;
	}
	// All inputs and outputs must be validated first
	if (__super::onRun(rRunStatus, pErrorMessages))
	{
		try
		{
			// See Section 3.5.3.
			// SetInputValues()
			// SetMILInputImages()
			// SetMILResultImages()
			// RunTool()
			// GetResultValues()
			// EB 20051121
			// check input images to see if they have resized
			// If so, do a ResetObject first.
			if (anyImagesResized())
			{
				ok = ResetObject(pErrorMessages) && ok;
			}

			/////////////////////////////////////////////////////
			//   Inputs
			/////////////////////////////////////////////////////

			SvTrc::IImagePtr pResultImageBuffers[SVExternalToolTaskData::NUM_RESULT_IMAGES];
			HRESULT hr = S_OK;

			bool okToRun = prepareInput(pResultImageBuffers, rRunStatus);

			///////////////////////////////////////
			//    Run
			///////////////////////////////////////
			long lToolStatus = 0;

			if (okToRun)
			{
				hr = m_dll.RunTool(getObjectId(), &lToolStatus);
			}
			if (S_OK != hr)
			{
				throw hr;
			}

			///////////////////////////////////////
			//    Outputs
			///////////////////////////////////////

			// Result Values

			getResults(pResultImageBuffers);

			if (okToRun)
			{
				rRunStatus.SetPassed();
			}
			else
			{
				ok = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalTask_CannotRun, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}

		}	// end try
		catch (const SvStl::MessageContainer& rSvE)
		{
			ok = false;
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(rSvE);
			}
		}
		catch (HRESULT hr)
		{
			ok = false;
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector msgList;
				_bstr_t bMessage;
				HRESULT hrError = GetDLLMessageString(hr, bMessage.GetAddress());
				std::string dllMessageString = (hrError == E_NOTIMPL) ? std::string("<not implemented>") : SvUl::createStdString(bMessage);
				msgList.push_back(SvUl::Format(_T("%ld: %s"), hr, dllMessageString.c_str()));
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalDllError, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
		
#ifndef _DEBUG
		catch (...)
		{
			ok = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalTask_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}
#endif
	}// end if( SVTaskObjectListClass::onRun( rRunStatus ) )
	else
	{
		ok = false;
	}

	if (!ok)
	{
		rRunStatus.SetInvalid();
	}

	return ok;
}



void SVExternalToolTask::SetPathName(const std::string& rPath)
{
	m_Data.m_voDllPath.SetValue(rPath);
}

HRESULT SVExternalToolTask::SetDependencies(const SvDef::StringVector& rDependencies)
{
	HRESULT hr = S_FALSE;
	int iSize = static_cast<int> (rDependencies.size());
	if (SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES >= iSize)
	{
		int i(0);

		for (i = 0; i < iSize; i++)
		{
			m_Data.m_aDllDependencies[i].SetValue(rDependencies[i]);
		}
		for (i = iSize; i < SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES; i++)
		{
			m_Data.m_aDllDependencies[i].SetValue(std::string());
		}

		hr = S_OK;
	}
	return hr;
}

HRESULT SVExternalToolTask::GetResultImageDefinitions(SVImageDefinitionStructArray& raResultImageDefinitions)
{
	raResultImageDefinitions = m_Data.m_aResultImageDefinitions;
	return S_OK;
}

HRESULT SVExternalToolTask::GetResultValueDefinitions(std::vector<ResultValueDefinition>&  raResultValueDefinitions)
{
	raResultValueDefinitions = m_Data.m_ResultDefinitions;
	return S_OK;
}



// ISVCancel interface function
bool SVExternalToolTask::CanCancel()
{
	return true;
}

// ISVCancel interface function
HRESULT SVExternalToolTask::GetCancelData(SVCancelData*& rpCancelData)
{
	SVExternalToolTaskData* pData = new SVExternalToolTaskData(m_Data);
	rpCancelData = pData;

	// set temp data
	std::vector<SVResultClass*> aRangeResults = GetResultRangeObjects();
	std::vector<SVResultClass*>::iterator iter;
	for (iter = aRangeResults.begin(); iter != aRangeResults.end(); ++iter)
	{
		SVCancelData* pCancelData = nullptr;
		(*iter)->GetCancelData(pCancelData);
		pData->m_RangeResultData.mapData[*iter] = pCancelData;
	}

	return S_OK;
}

// ISVCancel interface function
HRESULT SVExternalToolTask::SetCancelData(SVCancelData* pCancelData)
{
	SVExternalToolTaskData* pData = dynamic_cast<SVExternalToolTaskData*> (pCancelData);
	if (pData)
	{
		size_t i(0);

		// disconnect changed objects

		for (i = 0; i < m_Data.m_aInputImageInfo.size(); i++)
		{

			SvOl::SVInObjectInfoStruct* pImageInfo = &m_Data.m_aInputImageInfo[i];

			// Disconnect input info of input object...
			if (pImageInfo->IsConnected())
			{
				// Send to the Object we are using
				SVObjectManagerClass::Instance().DisconnectObjectInput(pImageInfo->GetInputObjectInfo().getObjectId(), pImageInfo);
			}

		}

		m_Data = *pData;

		// Set Range Result data
		SVMultiCancelData::MapType::iterator iter;
		for (iter = pData->m_RangeResultData.mapData.begin(); iter != pData->m_RangeResultData.mapData.end(); ++iter)
		{
			iter->first->SetCancelData(iter->second);
		}
		m_Data.m_RangeResultData.mapData.clear();	// don't remember this temp data;


		for (i = 0; i < m_Data.m_aInputImageInfo.size(); i++)
		{
			SvOl::SVInObjectInfoStruct* pImageInfo = &m_Data.m_aInputImageInfo[i];
			// reconnect changed objects
			// Connect input info to new input object...
			SVObjectManagerClass::Instance().ConnectObjectInput(pImageInfo->GetInputObjectInfo().getObjectId(), pImageInfo);
		}

		SvTo::SVToolClass* pTool = dynamic_cast <SvTo::SVToolClass*> (GetAncestor(SvPb::SVToolObjectType));
		// Reset all objects again...
		if (nullptr != pTool)
		{
			pTool->resetAllObjects();
		}

		return S_OK;
	}// end if ( pData )
	else
	{
		return S_FALSE;
	}
}

void SVExternalToolTask::SetIndirectValueSaveFlag()
{
	int size = static_cast<int>(std::min(m_Data.m_InputDefinitions.size(), m_Data.m_aInputObjects.size()));
	for (int i = 0; i < size; i++)
	{
		if (m_Data.m_aInputObjects[i].isIndirectValue())
		{
			m_Data.m_aInputObjects[i].setIndirectValueSaveFlag(true);
		}
	}
}


SvIe::SVImageClass* SVExternalToolTask::GetInputImage(int iIndex, bool bRunMode /*= false*/)
{
	if (iIndex >= 0 && iIndex < m_Data.m_lNumInputImages)
	{
		const SvOl::SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[iIndex];
		return SvOl::getInput<SvIe::SVImageClass>(rInfo, bRunMode);
	}
	return nullptr;
}

SvIe::SVImageClass* SVExternalToolTask::GetResultImage(int iIndex)
{
	if (iIndex >= 0 && iIndex < m_Data.m_lNumResultImages)
	{
		return &(m_aResultImages[iIndex]);
	}
	return nullptr;
}

SvVol::SVVariantValueObjectClass* SVExternalToolTask::GetResultValueObject(int iIndex)
{
	if (iIndex >= 0 && iIndex < m_Data.m_lNumResultValues)
	{
		return &(m_Data.m_aResultObjects[iIndex]);
	}
	return nullptr;
}

SvOp::TableObject*  SVExternalToolTask::GetResultTableObject(int iIndex)
{
	if (iIndex >= 0 && iIndex < m_Data.m_NumResultTables)
	{
		return  m_Data.m_ResultTableObjects[iIndex];
	}
	return nullptr;
}



HRESULT SVExternalToolTask::GetImageInfo(const SVImageDefinitionStruct* pDefinitionStruct, SVImageInfoClass& rInfo)
{
	HRESULT hr = S_OK;

	rInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8);
	rInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, pDefinitionStruct->eImageFormat);

	rInfo.SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>(0.0, 0.0));
	rInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, pDefinitionStruct->lWidth);
	rInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, pDefinitionStruct->lHeight);

	switch (pDefinitionStruct->eImageFormat)
	{
		case SvDef::SVImageFormatBGR888:
		case SvDef::SVImageFormatBGR888X:
			rInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3);
			break;
		default:
			rInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);
			break;
	}

	return hr;
}

HRESULT SVExternalToolTask::GetImageDefinitionStruct(SVImageDefinitionStruct& rImageDef, const SVImageInfoClass& rInfo)
{
	long l_lBandNumber = 1;
	long l_lFormat = SvDef::SVImageFormatUnknown;

	rInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputWidth, rImageDef.lWidth);
	rInfo.GetExtentProperty(SvPb::SVExtentPropertyEnum::SVExtentPropertyOutputHeight, rImageDef.lHeight);
	rInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_lFormat);
	rInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber);

	if ((rImageDef.eImageFormat == SvDef::SVImageFormatBGR888 ||
		rImageDef.eImageFormat == SvDef::SVImageFormatBGR888X) &&
		l_lBandNumber == 1)
	{
		rImageDef.eImageFormat = SvDef::SVImageFormatMono8;
	}
	else
	{
		rImageDef.eImageFormat = (SvDef::SVImageFormatEnum)l_lFormat;
	}

	return S_OK;
}

HRESULT SVExternalToolTask::AllocateResult(int iIndex)
{
	SvIe::SVClassInfoStruct       resultClassInfo;
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

	SVVariantResultClass*    pResult;

	HRESULT hr = S_OK;

	try
	{
		// Setup the result

		// Declare Input Interface of Result...
		interfaceInfo.m_EmbeddedID = m_Data.m_aResultObjects[iIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputVector.push_back(interfaceInfo);

		resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultVariantObjectType;
		resultClassInfo.m_ClassId = SvPb::VariantResultClassId;
		resultClassInfo.m_ClassName = _T("Range");
		std::string strTitle = m_Data.m_aResultObjects[iIndex].GetName();
		resultClassInfo.m_ClassName += _T(" ") + strTitle;

		// Construct the result class
		pResult = dynamic_cast<SVVariantResultClass *> (resultClassInfo.Construct());

		if (!pResult)
		{
			throw - 12346;
		}

		Add(pResult);

		SvDef::SVObjectTypeInfoStruct info;
		info.m_ObjectType = SvPb::SVValueObjectType;
		info.m_SubType = SvPb::SVVariantValueObjectType;
		info.m_EmbeddedID = SvPb::ValueEId;

		SvVol::SVVariantValueObjectClass* pValue = dynamic_cast<SvVol::SVVariantValueObjectClass*>(pResult->getFirstObject(info));

		if (!pValue)
		{
			throw - 12347;
		}

		constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
		pValue->SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

		// Ensure this Object's inputs get connected
		pResult->ConnectAllInputs();

		// And last - Create (initialize) it

		if (!pResult->IsCreated())
		{
			// And finally try to create the child object...
			if (!CreateChildObject(pResult, SvDef::SVMFResetObject))
			{
				// Remove it from the TaskObjectList ( Destruct it )
				uint32_t objectID = pResult->getObjectId();
				if (SvDef::InvalidObjectId != objectID)
				{
					Delete(objectID);
				}
				else
				{
					delete pResult;
				}

				throw - 12348;
			}
		}

	}// end try
	catch (int iErrorCode)
	{
		hr = iErrorCode;
	}
#ifndef _DEBUG
	catch (...)
	{
		hr = S_FALSE;
	}
#endif
	return hr;
}



SVResultClass* SVExternalToolTask::GetResultRangeObject(int iIndex)
{
	SVVariantResultClass*   pResult = nullptr;

	SvDef::SVObjectTypeInfoStruct  info(SvPb::SVResultObjectType, SvPb::SVResultVariantObjectType);
	SVGetObjectDequeByTypeVisitor l_Visitor(info);
	SVObjectManagerClass::Instance().VisitElements(l_Visitor, getObjectId());

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;
	bool objectFound = false;
	for (l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end() && !objectFound; ++l_Iter)
	{
		pResult = dynamic_cast<SVVariantResultClass*>(const_cast<SVObjectClass*>(*l_Iter));
		SvOl::SVInputInfoListClass	resultInputList;
		pResult->GetPrivateInputList(resultInputList);

		SVObjectClass* pSVObject = resultInputList[0]->GetInputObjectInfo().getObject();
		if (&m_Data.m_aResultObjects[iIndex] == pSVObject)
		{
			objectFound = true;
		}
		else
		{
			pResult = nullptr;
		}
	}

	return pResult;
}

std::vector<SVResultClass*> SVExternalToolTask::GetResultRangeObjects()
{
	std::vector<SVResultClass*> aObjects;

	SvDef::SVObjectTypeInfoStruct  info(SvPb::SVResultObjectType, SvPb::SVResultVariantObjectType);
	SVGetObjectDequeByTypeVisitor l_Visitor(info);
	SVObjectManagerClass::Instance().VisitElements(l_Visitor, getObjectId());

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;
	for (l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter)
	{
		SVVariantResultClass* pResult = dynamic_cast<SVVariantResultClass*>(const_cast<SVObjectClass*>(*l_Iter));

		aObjects.push_back(pResult);
	}

	return aObjects;
}

HRESULT SVExternalToolTask::ClearData()
{
	HRESULT hr = S_OK;
	int i = 0;

	for (i = 0; i < m_Data.m_lNumResultValues; i++)
	{
		_variant_t vt;
		m_Data.m_aResultObjects[i].SetDefaultValue(vt, true);
	}

	std::vector< SVResultClass*> apResults = GetResultRangeObjects();
	for (std::vector<SVResultClass*>::iterator iter = apResults.begin(); iter != apResults.end(); ++iter)
	{
		HRESULT hrTmp = RemoveChild(*iter);
		if (S_OK != hrTmp)
			hr = hrTmp;
	}

	m_Data.m_lNumInputValues = 0;
	m_Data.m_lNumResultValues = 0;
	m_Data.m_lNumResultImages = 0;
	m_Data.m_lNumInputImages = 0;

	return hr;
}

HRESULT SVExternalToolTask::SetDefaultValues()
{
	HRESULT hr = S_OK;

	int size = m_Data.getNumInputs();
	for (int i = 0; i < size; i++)
	{

		InputValueDefinition& rInputDef = m_Data.m_InputDefinitions[i];
		if (rInputDef.getType() == SvOp::ExDllInterfaceType::Scalar || rInputDef.getType() == SvOp::ExDllInterfaceType::Array)
		{
			int LinkValueIndex = rInputDef.getLinkedValueIndex();

			m_Data.m_aInputObjects[LinkValueIndex].SetDefaultValue(rInputDef.getDefaultValue(), true);
		}

	}

	return hr;
}

bool SVExternalToolTask::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVTaskObjectListClass::ResetObject(pErrorMessages);

	for (int i = 0; i < m_Data.m_lNumInputImages; i++)
	{
		SvOl::SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
		SvOl::ValidateInput(rInfo);
	}

	SetIndirectValueSaveFlag();
	collectInputValues();

	// Data Definition List stuff
	collectInputImageNames();

	try
	{
		HRESULT hr = Initialize([](LPCTSTR) {});
		if (S_OK != hr)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitExternalTaskFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}

		int NumResults = m_Data.getNumResults();
		if (S_OK == m_dll.GetResultValues(getObjectId(), NumResults, NumResults ? &(m_InspectionResultValues[0]) : nullptr))
		{
			for (int i = 0; i < NumResults; i++)
			{
				GetResultValueObject(i)->SetValue(m_InspectionResultValues[i]);

				// Clear OleVariant that was created in Dll.
				m_InspectionResultValues[i].Clear();
			}
		}
		int NumTableResults = m_Data.getNumTableResults();
		if (S_OK == m_dll.getResultTables(getObjectId(), NumTableResults, NumTableResults ? &(m_InspectionResultTables[0]) : nullptr))
		{
			for (int i = 0; i < NumTableResults; i++)
			{
				GetResultTableObject(i)->setTableValues(m_InspectionResultTables[i]);
				// Clear OleVariant that was created in Dll.
				m_InspectionResultTables[i].Clear();
			}
		}

	}
	catch (const SvStl::MessageContainer& e)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(e);
		}
	}
	catch (...)
	{
		Result = false;
	}

	return Result;
}

bool SVExternalToolTask::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	bool Result = __super::resetAllObjects(pErrorMessages);
	CreateArrayInTable();
	return Result;

}
// compare cancel data (original) with current data.
HRESULT SVExternalToolTask::FindInvalidatedObjects(SVObjectPtrVector& rList, const SVCancelData* pCancelData, FindEnum eWhich)
{
	const SVExternalToolTaskData* pOriginalData = dynamic_cast <const SVExternalToolTaskData*> (pCancelData);
	SVExternalToolTaskData* pNewData = &m_Data;

	if (eWhich & FindEnum::FIND_VALUES)
	{
		// inputs
		if (pOriginalData->m_lNumInputValues > pNewData->m_lNumInputValues)
		{
			for (int i = pNewData->m_lNumInputValues; i < pOriginalData->m_lNumInputValues; i++)
			{
				rList.push_back(&(m_Data.m_aInputObjects[i]));
			}
		}

		// outputs
		if (pOriginalData->m_lNumResultValues > pNewData->m_lNumResultValues)
		{
			for (int i = pNewData->m_lNumResultValues; i < pOriginalData->m_lNumResultValues; i++)
			{
				rList.push_back(&(m_Data.m_aResultObjects[i]));
			}
		}
	}// end if ( eWhich & FIND_VALUES )

	if (eWhich & FindEnum::FIND_INPUT_IMAGES)
	{
		// input images
		if (pOriginalData->m_lNumInputImages > pNewData->m_lNumInputImages)
		{
			for (int i = pNewData->m_lNumInputImages; i < pOriginalData->m_lNumInputImages; i++)
			{
				const SvOl::SVInObjectInfoStruct* pStruct = &(pOriginalData->m_aInputImageInfo[i]);
				rList.push_back(pStruct->GetInputObjectInfo().getObject());
			}
		}
	}

	if (eWhich & FindEnum::FIND_RESULT_IMAGES)
	{
		// result images
		if (pOriginalData->m_lNumResultImages > pNewData->m_lNumResultImages)
		{
			for (int i = pNewData->m_lNumResultImages; i < pOriginalData->m_lNumResultImages; i++)
			{
				rList.push_back(&(m_aResultImages[i]));
			}
		}
	}// end if ( eWhich & FIND_RESULT_IMAGES )

	return S_OK;
}

HRESULT SVExternalToolTask::GetDLLMessageString(HRESULT hr, BSTR* bstrMessage) const
{
	return m_dll.GetMessageString(hr, bstrMessage);
}

bool SVExternalToolTask::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result(false);
	if (nullptr != pObjectInInfo)
	{
		SVObjectClass* pObject = pObjectInInfo->GetInputObjectInfo().getObject();
		if (SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (pObject))
		{
			// find object
			for (int i = 0; i < SVExternalToolTaskData::NUM_INPUT_IMAGES; i++)
			{
				SvOl::SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
				if (rInfo.GetInputObjectInfo().getObject() == pImage)
				{
					// replace with tool set image
					SvOi::IInspectionProcess* pInspectionInterface = GetInspectionInterface();
					SvOi::IObjectClass* pInspection = dynamic_cast<SvOi::IObjectClass*> (pInspectionInterface);
					SvDef::SVObjectTypeInfoStruct imageObjectInfo;
					imageObjectInfo.m_ObjectType = SvPb::SVImageObjectType;
					SvIe::SVImageClass* pToolSetImage = (nullptr != pInspection) ? dynamic_cast <SvIe::SVImageClass*> (pInspection->getFirstObject(imageObjectInfo)) : nullptr;

					rInfo.SetInputObject(pToolSetImage);
					//@Todo[MEC][8.20] [17.07.2019] EXTERNAL TOOL Check for null pointer!
					//See svb 290
					rInfo.GetInputObjectInfo().getObject()->ConnectObjectInput(&rInfo);
					break;
				}
			}

			Result = true;
		}
	}
	return __super::DisconnectObjectInput(pObjectInInfo) && Result;
}

HRESULT SVExternalToolTask::DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	return __super::DisconnectInputsOutputs(rListOfObjects);
}

HRESULT SVExternalToolTask::HideInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	return __super::HideInputsOutputs(rListOfObjects);
}

bool SVExternalToolTask::ConnectAllInputs()
{
	bool l_bRunConnect = true;
	int i(0);

	// Check if input info is Ok for Input Images..
	for (i = 0; i < m_Data.m_lNumInputImages; i++)
	{
		l_bRunConnect = false;
		SvOl::SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
		if (SvDef::InvalidObjectId != rInfo.GetInputObjectInfo().getObjectId())
		{
			l_bRunConnect = true;
			break;
		}
	}

	if (l_bRunConnect)
	{
		return SVTaskObjectListClass::ConnectAllInputs();
	}
	return true;
}

HRESULT SVExternalToolTask::collectInputImageNames()
{
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	if (nullptr != pTool)
	{
		SvVol::SVStringValueObjectClass* pImageNames = pTool->GetInputImageNames();
		if (nullptr != pImageNames)
		{
			pImageNames->SetArraySize(m_Data.m_lNumInputImages);
			for (int i = 0; i < m_Data.m_lNumInputImages; i++)
			{
				SvIe::SVImageClass* l_pImage = GetInputImage(i);
				if (l_pImage)
				{
					std::string Name = l_pImage->GetCompleteName();

					pImageNames->SetValue(Name, i);
				}
			}
		}
	}
	return S_OK;
}
void SVExternalToolTaskData::SetInputValueDefinitions(long ArraySize, InputValueDefinitionStruct  InputValueDefs[])
{
	m_lNumInputValues = ArraySize;
	m_InputDefinitions.resize(ArraySize);
	m_NumLinkedValue = 0;

	for (int i = 0; i < ArraySize; i++)
	{
		m_InputDefinitions[i].setDefinition(InputValueDefs[i], &m_NumLinkedValue);
	}
}
void SVExternalToolTaskData::SetInputValueDefinitions(long ArraySize, InputValueDefinitionStructEx  InputValueDefs[])
{

	m_lNumInputValues = ArraySize;
	m_InputDefinitions.resize(ArraySize);
	m_NumLinkedValue = 0;

	for (int i = 0; i < ArraySize; i++)
	{
		m_InputDefinitions[i].setDefinition(InputValueDefs[i], &m_NumLinkedValue);
	}


}
void SVExternalToolTaskData::InitializeInputs(SVExternalToolTask*  pExternalToolTask)
{

	for (int i = 0; i < m_InputDefinitions.size(); i++)
	{
		InputValueDefinition& rInputDef = m_InputDefinitions[i];
		if (rInputDef.getType() == SvOp::ExDllInterfaceType::Scalar || rInputDef.getType() == SvOp::ExDllInterfaceType::Array)
		{
			int LinkValueIndex = rInputDef.getLinkedValueIndex();
			SvVol::LinkedValue& rInputValue = m_aInputObjects[LinkValueIndex];

			if (rInputValue.GetDefaultType() == VT_EMPTY)
			{
				bool bSetVal {true};
				if (VT_BSTR == rInputValue.GetValueType() || (rInputDef.getDefaultValue().vt & ~VT_ARRAY) == rInputValue.GetValueType())
				{
					bSetVal = false;
				}

				rInputValue.SetDefaultValue(rInputDef.getDefaultValue(), bSetVal);

			}
			//The linkedValues must be reset (to set the object reference correctly), before used them for get values (in InspectionInputsToVariantArray). 
			//But this method will called also in Create-process and there is not a reset called before.
			rInputValue.resetAllObjects();
		}
		else
		{
			int LinkValueIndex = rInputDef.getLinkedValueIndex();
			SvVol::LinkedValue& rLinkedObject = m_aInputObjects[LinkValueIndex];


			rLinkedObject.resetAllObjects();
		}

		if (rInputDef.UseDisplayNames())
		{
			
			std::string oldname = m_aInputObjects[i].GetName();
			std::string InputPrefix = SvUl::LoadStdString(IDS_EXTERNAL_DLL_INPUT_PREFIX);
			std::stringstream str;
			str << InputPrefix << std::setfill('0') << std::setw(2) << i +1 << "_" << rInputDef.getDisplayName();
			std::string name = str.str();
			m_aInputObjects[i].SetName(name.c_str());
			if (nullptr != pExternalToolTask &&  pExternalToolTask->NoExFktInLoadVersion() && pExternalToolTask->GetInspection())
			{
				pExternalToolTask->GetInspection()->OnObjectRenamed(m_aInputObjects[i], oldname);
			}
			
		}


	}

}




bool SVExternalToolTask::prepareInput(SvTrc::IImagePtr pResultImageBuffers[], SVRunStatusClass& rRunStatus)
{
	collectInputValues();

	// send input values to DLL
	long size = static_cast<long>(m_InspectionInputValues.size());
	HRESULT hr = m_dll.SetInputValues(getObjectId(), size, size ? &(m_InspectionInputValues[0]) : nullptr);
	if (S_OK != hr)
	{
		throw hr;
	}

	bool okToRun = collectInputImages(rRunStatus);

	if (m_dll.UseMil())
	{
		// send input images to DLL
		hr = m_dll.SetMILInputImages(getObjectId(), m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : nullptr);
		if (S_OK != hr)
		{
			throw hr;
		}
	}
	else
	{
		std::vector<HBITMAP> aDIBHandles(m_Data.m_lNumInputImages);
		for (int i = 0; i < m_Data.m_lNumInputImages; i++)
		{
			aDIBHandles[i] = m_aInspectionInputHBMImages.at(i).hbm;
		}
		/*hr = */m_dll.SetHBITMAPInputImages(getObjectId(), m_Data.m_lNumInputImages ? &(aDIBHandles[0]) : nullptr);
	}

	for (int i = 0; i < m_Data.m_lNumResultImages; i++)
	{
		pResultImageBuffers[i] = GetResultImage(i)->getImageToWrite(rRunStatus.m_triggerRecord);
	}

	if (m_dll.UseMil())
	{
		if (!collectMilResultBuffers(pResultImageBuffers))
		{
			okToRun = false;
		}

		// give result images to dll
		hr = m_dll.SetMILResultImages(getObjectId(), m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr);
		if (S_OK != hr)
		{
			throw hr;
		}

	}// if( m_dll.UseMil() )
	else
	{ // HBitmap Images
	}

	return okToRun;
}


void SVExternalToolTask::getResults(SvTrc::IImagePtr pResultImageBuffers[])
{
	collectResultValues();

	int NumTableResults = m_Data.getNumTableResults();
	if (S_OK == m_dll.getResultTables(getObjectId(), NumTableResults, NumTableResults ? &(m_InspectionResultTables[0]) : nullptr))
	{
		for (int i = 0; i < NumTableResults; i++)
		{
			GetResultTableObject(i)->setTableValues(m_InspectionResultTables[i]);
			// Clear OleVariant that was created in Dll.
			m_InspectionResultTables[i].Clear();
		}
	}

	collectResultImages(pResultImageBuffers);
}


bool SVExternalToolTask::anyImagesResized()
{
	if (m_aPreviousInputImageRect.size() == 0)
	{
		for (int i = 0; i < m_Data.m_lNumInputImages; i++)
		{
			m_aPreviousInputImageRect.push_back(RECT());
		}
	}

	for (int i = 0; i < m_Data.m_lNumInputImages; i++)
	{
		SvIe::SVImageClass* pInputImage = GetInputImage(i, true);
		if (pInputImage)
		{
			RECT rectInput;
			pInputImage->GetImageExtents().GetOutputRectangle(rectInput);

			RECT rectPrevious(m_aPreviousInputImageRect[i]);

			if (rectInput.left != rectPrevious.left ||
				rectInput.right != rectPrevious.right ||
				rectInput.top != rectPrevious.top ||
				rectInput.bottom != rectPrevious.bottom)
			{
				return true;
			}
		}
	}

	return false;
}



void SVExternalToolTask::collectInputValues()
{
	// collect input values
	for (int i = 0; i < m_Data.m_InputDefinitions.size(); i++)
	{
		int LVIndex = m_Data.m_InputDefinitions[i].getLinkedValueIndex();
		if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::Scalar)
		{
			m_Data.m_aInputObjects[LVIndex].GetValue(m_InspectionInputValues[i]);
			HRESULT hrChangeType = ::VariantChangeTypeEx(&m_InspectionInputValues[i], &m_InspectionInputValues[i], SvDef::LCID_USA, 0, static_cast<VARTYPE>(m_Data.m_InputDefinitions[i].getVt()));
			if (S_OK != hrChangeType)
			{
				m_InspectionInputValues[i].Clear();
				m_InspectionInputValues[i].ChangeType(static_cast<VARTYPE>(m_Data.m_InputDefinitions[i].getVt()));
			}
		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::Array)
		{
			/// if we have an array the typ must be correct

			m_Data.m_aInputObjects[LVIndex].GetArrayValue(m_InspectionInputValues[i]);
		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::TableArray)
		{
			const TableObject* pTableObject = dynamic_cast<const TableObject*>(m_Data.m_aInputObjects[LVIndex].GetLinkedObject());
			if (pTableObject)
			{
				long SX {0}, SY {0};
				pTableObject->getTableValues(m_InspectionInputValues[i], &SX, &SY);
			}
			else
			{
				m_InspectionInputValues[i] = m_Data.m_InputDefinitions[i].getDefaultValue();
			}

		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::TableNames)
		{
			const TableObject* pTableObject = dynamic_cast<const TableObject*>(m_Data.m_aInputObjects[LVIndex].GetLinkedObject());
			if (pTableObject)
			{
				pTableObject->getColumNames(m_InspectionInputValues[i]);
			}
			else
			{
				m_InspectionInputValues[i] = m_Data.m_InputDefinitions[i].getDefaultValue();
			}

		}
	}
}


bool SVExternalToolTask::collectInputImages(SVRunStatusClass& rRunStatus)
{
	bool okToRun = true;

	// collect input images
	for (int i = 0; i < m_Data.m_lNumInputImages; i++)
	{
		SvIe::SVImageClass* pInputImage = GetInputImage(i, true);
		if (pInputImage)
		{
			SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
			if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
			{
				if (m_dll.UseMil())
				{
					if (!m_bUseImageCopies)
					{
						// This cast assumes that a mil handle will never get larger than a 32 bit long.
						m_aInspectionInputImages[i] = static_cast<long>(pImageBuffer->getHandle()->GetBuffer().GetIdentifier());	// assign to internal buffer handle
						if (m_aInspectionInputImages[i] == 0)
						{
							okToRun = false;
						}
					}
					else
					{
						SvOi::SVImageBufferHandlePtr l_ImageBufferCopy = m_aInputImagesCopy[i];
						if (nullptr != l_ImageBufferCopy && !l_ImageBufferCopy->empty())
						{
							SVMatroxBufferInterface::CopyBuffer(l_ImageBufferCopy->GetBuffer(), pImageBuffer->getHandle()->GetBuffer());

							// This cast assumes that a Mil Id will never get larger than 32 bits in size.
							m_aInspectionInputImages[i] = static_cast<long>(l_ImageBufferCopy->GetBuffer().GetIdentifier());	// assign to copy handle
							if (m_aInspectionInputImages[i] == 0)
							{
								okToRun = false;
							}
						}
						else
						{
							okToRun = false;
						}
					}
				}// if( m_dll.UseMil() )
				else
				{
					SvOi::SVImageBufferHandlePtr l_ImageBufferCopy = m_aInputImagesCopy[i];
					if (nullptr != l_ImageBufferCopy && !l_ImageBufferCopy->empty())
					{
						SVMatroxBufferInterface::CopyBuffer(l_ImageBufferCopy->GetBuffer(), pImageBuffer->getHandle()->GetBuffer());
					}
					else
					{
						okToRun = false;
					}
				}
			}
			else
			{
				okToRun = false;
			}
		}
	}
	return okToRun;
}

bool SVExternalToolTask::collectMilResultBuffers(SvTrc::IImagePtr pResultImageBuffers[])
{
	bool allMilResultsAreOk = true;
	if (!m_bUseImageCopies)
	{
		// collect result images
		for (int i = 0; i < m_Data.m_lNumResultImages; i++)
		{
			SVMatroxBuffer buffer;
			if (nullptr != pResultImageBuffers[i] && !pResultImageBuffers[i]->isEmpty())
			{
				buffer = pResultImageBuffers[i]->getHandle()->GetBuffer();
			}

			// this cast assumes that a mil handle will never be larger than 32 bits.
			m_aInspectionResultImages[i] = static_cast<long>(buffer.GetIdentifier());
			if (m_aInspectionResultImages[i] == 0)
			{
				allMilResultsAreOk = false;
			}
		}
	}// if ( !m_bUseImageCopies )
	else
	{
		// collect result image copies
		for (int i = 0; i < m_Data.m_lNumResultImages; i++)
		{
			SvOi::SVImageBufferHandlePtr l_ImageBuffer = m_aResultImagesCopy[i];
			SVMatroxBuffer buffer;

			if (nullptr != l_ImageBuffer)
			{
				buffer = l_ImageBuffer->GetBuffer();
			}

			// this cast assumes that a mil handle will never be larger than 32 bits.
			m_aInspectionResultImages[i] = static_cast<long>(buffer.GetIdentifier());
			if (m_aInspectionResultImages[i] == 0)
			{
				allMilResultsAreOk = false;
			}
		}
	}
	return allMilResultsAreOk;
}

void SVExternalToolTask::collectResultValues()
{
	long Resultsize = static_cast<long>(m_InspectionResultValues.size());
	HRESULT hr = m_dll.GetResultValues(getObjectId(), Resultsize, Resultsize ? &(m_InspectionResultValues[0]) : nullptr);
	if (S_OK != hr)
	{
		throw hr;
	}

	for (int i = 0; i < Resultsize; i++)
	{
		if (m_InspectionResultValues[i].vt & VT_ARRAY)
		{
			try
			{
				VARTYPE type = (m_InspectionResultValues[i].vt & (~VT_ARRAY));
				if (!GetResultValueObject(i)->isArray())
				{
					//if the value object is an array the correct size will be set in setValue
					GetResultValueObject(i)->SetArraySize(2);
					assert(false);
				}

				if (GetResultValueObject(i)->GetDefaultType() != type)
				{
					_variant_t var(0.0);
					var.ChangeType(type);
					GetResultValueObject(i)->SetDefaultValue(var);
				}
				GetResultValueObject(i)->setValue(m_InspectionResultValues[i], -1, true);
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				m_InspectionResultValues[i].Clear();
				GetResultValueObject(i)->SetResultSize(0);
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(rSvE.getMessage());
			}
			
			catch (...)
			{
				m_InspectionResultValues[i].Clear();
				GetResultValueObject(i)->SetResultSize(0);
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));
			}

		}
		else if (m_InspectionResultValues[i].vt == VT_EMPTY && GetResultValueObject(i)->isArray())
		{
			GetResultValueObject(i)->SetResultSize(0);

		}
		else
		{
			GetResultValueObject(i)->SetValue(m_InspectionResultValues[i]);
		}
		// Clear OleVariant that was created in Dll.
		m_InspectionResultValues[i].Clear();
	}
}

void SVExternalToolTask::collectResultImages(SvTrc::IImagePtr pResultImageBuffers[])
{
	if (m_Data.m_lNumResultImages > 0)
	{
		if (m_dll.UseMil())
		{
			if (m_bUseImageCopies)
			{
				// collect result images
				for (int i = 0; i < m_Data.m_lNumResultImages; i++)
				{
					SvOi::SVImageBufferHandlePtr l_ImageBufferCopy = m_aResultImagesCopy[i];
					if (nullptr != pResultImageBuffers[i] && !pResultImageBuffers[i]->isEmpty() && nullptr != l_ImageBufferCopy && !l_ImageBufferCopy->empty())
					{
						// Ask Joe: what if different sizes??
						SVMatroxBufferInterface::CopyBuffer(pResultImageBuffers[i]->getHandle()->GetBuffer(), l_ImageBufferCopy->GetBuffer());
					}
				}
			}// end if ( m_bUseImageCopies )
		}// if( m_dll.UseMil() )
		else
		{ // Use HBitmaps 
			HRESULT MatroxCode(S_OK);

			HRESULT hr = m_dll.GetHBITMAPResultImages(getObjectId(), m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultHBMImages[0]) : nullptr);
			for (int i = 0; i < m_Data.m_lNumResultImages; i++)
			{
				if (nullptr != pResultImageBuffers[i] && !pResultImageBuffers[i]->isEmpty())
				{
					MatroxCode = SVMatroxBufferInterface::CopyBuffer(pResultImageBuffers[i]->getHandle()->GetBuffer(), m_aInspectionResultHBMImages[i]);
				}
				else
				{
					MatroxCode = E_FAIL;
				}
				if (S_OK == hr)
				{
					if (S_OK != MatroxCode)
					{
						hr = MatroxCode | SVMEE_MATROX_ERROR;
					}
				}
				m_aInspectionResultHBMImages[i] = 0;

			}// end for( int i = 0 ; i < m_Data.m_lNumResultImages ; i++ )
		}// end else use HBITMAP
	}// end if ( m_Data.m_lNumResultImages > 0 )

}

} //namespace SvOp
