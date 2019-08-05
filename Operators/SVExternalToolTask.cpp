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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxErrorEnum.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "Tools/SVTool.h"
#include "TableObject.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVExternalToolTask, SVExternalToolTaskGuid)

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

const SVExternalToolTaskData& SVExternalToolTaskData::operator = (const SVExternalToolTaskData& rhs)
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

		if (m_aInputObjectNames.size() != rhs.m_aInputObjectNames.size())	// if this is only a copy of the original, size will be 0
		{
			m_aInputObjectNames.resize(rhs.m_aInputObjectNames.size());
		}
		std::copy(rhs.m_aInputObjectNames.begin(), rhs.m_aInputObjectNames.end(), m_aInputObjectNames.begin());

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVExternalToolTaskObjectType;

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

	RegisterEmbeddedObject(&m_Data.m_voDllPath, SVDllFileNameGuid, IDS_OBJECTNAME_DLL_PATH, false, SvOi::SVResetItemTool);

	// Initialize Dll Dependencies 
	m_Data.m_aDllDependencies.resize(SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES);
	for (i = 0; i < m_Data.m_aDllDependencies.size(); i++)
	{
		RegisterEmbeddedObject(&m_Data.m_aDllDependencies[i], aSVDllDependencyFileNameGuid[i], IDS_OBJECTNAME_DLL_DEP_FILE_01 + static_cast<int>(i), false, SvOi::SVResetItemTool);
	}

	// init Tool Name
	RegisterEmbeddedObject(&m_Data.m_voToolName, SVDllToolNameGuid, IDS_OBJECTNAME_DLL_TOOL_NAME, false, SvOi::SVResetItemNone);
	m_Data.m_voToolName.setSaveValueFlag(false);

	// Init Tool Version
	RegisterEmbeddedObject(&m_Data.m_voToolVersion, SVDllToolVersionGuid, IDS_OBJECTNAME_DLL_TOOL_VERSION, false, SvOi::SVResetItemNone);
	m_Data.m_voToolVersion.setSaveValueFlag(false);

	// Init Input Object Info array
	m_Data.m_aInputObjects.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);
	m_Data.m_aInputObjectNames.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);

	for (i = 0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++)
	{


		RegisterEmbeddedObject(&m_Data.m_aInputObjects[i], aInputObjectGUID[i], IDS_OBJECTNAME_INPUT_01 + static_cast<int>(i), false, SvOi::SVResetItemTool);
		std::string ObjectName = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + static_cast<int>(i));
		ObjectName += SvDef::cLinkName;
		RegisterEmbeddedObject(&m_Data.m_aInputObjects[i].getLinkedName(), aInputObject_LinkedGUID[i], ObjectName.c_str(), false, SvOi::SVResetItemNone);

		RegisterEmbeddedObject(&m_Data.m_aInputObjectNames[i], aInputObjectNameGuid[i], IDS_OBJECTNAME_INPUT_01_NAME + static_cast<int>(i), false, SvOi::SVResetItemNone);

		// set default values
		VARIANT vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = VT_EMPTY;
		m_Data.m_aInputObjects[i].SetDefaultValue(vtTemp, true);


		m_Data.m_aInputObjectNames[i].SetDefaultValue(std::string(), true);
		m_Data.m_aInputObjectNames[i].setSaveValueFlag(false);
	}

	// Set Default Image Parameters for Result Images
	int l_pImageNames[] = {IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2, IDS_OBJECTNAME_IMAGE3, IDS_OBJECTNAME_IMAGE4};
	for (i = 0; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
	{
		SVImageInfoClass imageInfo;

		RegisterEmbeddedObject(&m_aResultImages[i], aSVVariantResultImageObjectGuid[i], l_pImageNames[i]);

		if (nullptr != GetTool())
		{
			imageInfo.SetOwner(GetTool()->GetUniqueObjectID());
		}

		imageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8);
		imageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8);

		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>(0.0, 0.0));
		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyWidth, 100);
		imageInfo.SetExtentProperty(SvPb::SVExtentPropertyHeight, 100);

		SvIe::SVImageClass* pImage = &(m_aResultImages[i]);
		pImage->InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);
		pImage->UpdateImage(GUID_NULL, imageInfo);
	}

	// Result Objects
	m_Data.m_aResultObjects.resize(SVExternalToolTaskData::NUM_RESULT_OBJECTS);

	for (i = 0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		// Register
		RegisterEmbeddedObject(&m_Data.m_aResultObjects[i], aSVVariantResultObjectGuid[i], IDS_OBJECTNAME_RESULT_01 + static_cast<int>(i), false, SvOi::SVResetItemNone);

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
		m_Data.m_aInputObjectNames[i].SetObjectAttributesAllowed(attribute, SvOi::OverwriteAttribute);
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
				m_Data.m_ResultTableObjects[i]->SetObjectAttributesAllowed(SvPb::taskObject, SvOi::AddAttribute);
			}
			else
			{
				m_Data.m_ResultTableObjects[i]->SetObjectAttributesAllowed(SvPb::taskObject, SvOi::RemoveAttribute);
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
	CloseObject();
	// remove embeddeds so that invalid objects won't be Closed
	m_embeddedList.clear();
}

void SVExternalToolTask::CreateArray()
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

			pTab->updateOrCreateColumn(TableColumnValueObjectGuid[i], Columnames[i], nRows);
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
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
	bool l_bOk = SVTaskObjectListClass::CreateObject(rCreateStructure);

	if (l_bOk && nullptr != rCreateStructure.m_pTool)
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

			l_bOk = true;
		}
	}

	m_isCreated = l_bOk;

	return l_bOk;
}

HRESULT SVExternalToolTask::Initialize(SVDllLoadLibraryCallback fnNotify)
{

	HRESULT hr = S_FALSE;
	BSTR bstrName = nullptr;

	GUID guid = GetUniqueObjectID();

	HRESULT hrUninitializeRun = m_dll.UninitializeRun(guid);
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
		newMessage.setMessage(e.getMessage(), GetUniqueObjectID());
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

			std::vector< SVImageDefinitionStruct> aInputImages;	// used in m_dll.InitializeRun

			m_aInspectionInputImages.resize(m_Data.m_lNumInputImages);
			m_aInspectionInputHBMImages.resize(m_Data.m_lNumInputImages);
			aInputImages.resize(m_Data.m_lNumInputImages);

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
						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionInputImages[i] = static_cast<long>(pImageBuffer->getHandle()->GetBuffer().GetIdentifier());
					}

					if (!m_dll.UseMil())
					{
						if (nullptr != m_aInspectionInputHBMImages[i].hbm)
						{
							::DeleteObject(m_aInspectionInputHBMImages[i].hbm);
							m_aInspectionInputHBMImages[i].FreeBitmapInfo();
							m_aInspectionInputHBMImages[i].Clear();
						}
						HRESULT hrInitialCopy = SVIHBitmapUtilities::SVImageInfoToNewDIB(imageInfo, m_aInspectionInputHBMImages[i]);
					}
					else
					{
						if (m_bUseImageCopies)
						{
							SVImageInfoClass imageInfoCopy;
							GetImageInfo(&(aInputImages[i]), imageInfoCopy);	// get the physical info from our definition struct

							if (nullptr != GetTool())
							{
								imageInfoCopy.SetOwner(GetTool()->GetUniqueObjectID());
							}
							else
							{
								imageInfoCopy.SetOwner(GUID_NULL);
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

			InputValueDefinitionStruct* paInputValueDefs = nullptr;
			long ArraySize {0};

			hr = m_dll.GetInputValueDefinitions(&ArraySize, &paInputValueDefs);
			m_Data.m_lNumInputValues = ArraySize;
			if (S_OK != hr)
			{
				throw hr;
			}
			m_InspectionInputValues.resize(ArraySize);
			m_Data.InitializeInputs(ArraySize, paInputValueDefs);
			for (int i = 0; i < ArraySize; i++)
			{
				HRESULT hres = ::VariantChangeTypeEx(&m_InspectionInputValues[i], &m_InspectionInputValues[i], SvDef::LCID_USA, 0, static_cast<VARTYPE>(paInputValueDefs[i].m_VT));
				if (S_OK != hres)
				{
					///cant change variant empty to safe array
					_variant_t temp(paInputValueDefs[i].m_DefaultValue);
					m_InspectionInputValues[i] = temp.Detach();
				}
			}


			hr = m_dll.DestroyInputValueDefinitionStructures(paInputValueDefs);
			if (S_OK != hr)
			{
				throw hr;
			}
			paInputValueDefs = nullptr;

			InspectionInputsToVariantArray();

			///////////////////////////////////////
			//    Initialize Results
			///////////////////////////////////////

			// Initialize Result Objects
			ResultValueDefinitionStruct* paResultValueDefs = nullptr;
			long NumResultObjects {0};
			hr = m_dll.GetResultValueDefinitions(&NumResultObjects, &paResultValueDefs);
			m_Data.m_lNumResultValues = NumResultObjects;
			if (S_OK != hr)
			{
				throw hr;
			}

			m_InspectionResultValues.resize(NumResultObjects);
			m_Data.m_ResultDefinitions.resize(NumResultObjects);


			for (int i = 0; i < NumResultObjects; i++)
			{
				m_Data.m_ResultDefinitions[i].setDefinition(paResultValueDefs[i], i);
				long vt = paResultValueDefs[i].m_VT;
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
						info.ObjectType = SvPb::SVValueObjectType;
						info.SubType = SvPb::SVVariantValueObjectType;
						info.EmbeddedID = SVValueObjectGuid;
						SvVol::SVVariantValueObjectClass* pValue = dynamic_cast<SvVol::SVVariantValueObjectClass*>(pResult->getFirstObject(info));

						if (nullptr != pValue)
						{
							_variant_t defaultValue;

							defaultValue.ChangeType(static_cast<VARTYPE> (~(VT_ARRAY)& vt));
							pValue->SetDefaultValue(defaultValue, false);
						}
					}
				}
			}
			hr = m_dll.DestroyResultValueDefinitionStructures(paResultValueDefs);
			if (S_OK != hr)
			{
				throw hr;
			}

			ResultTableDefinitionStruct* paResultTableDefs {nullptr};
			long ResultTableNum {0};
			if (m_dll.UseTableOutput())
			{
				hr = m_dll.getResultTableDefinitions(&ResultTableNum, &paResultTableDefs);

			}
			m_Data.m_NumResultTables = ResultTableNum;
			m_Data.m_TableResultDefinitions.resize(ResultTableNum);
			m_InspectionResultTables.resize(ResultTableNum);
			for (int j = 0; j < ResultTableNum; j++)
			{
				m_Data.m_TableResultDefinitions[j].setDefinition(paResultTableDefs[j], j);
			}

			hr = m_dll.destroyResultTableDefinitionStructures(paResultTableDefs);


			paResultValueDefs = nullptr;

			//Initialize TableObjects!
			CreateArray();

			///////////////////////////////////////
			//    InitializeRun
			///////////////////////////////////////

			hr = m_dll.InitializeRun(guid, (long)aInputImages.size(), aInputImages.size() ? &(aInputImages[0]) : nullptr,
				(long)m_InspectionInputValues.size(), m_InspectionInputValues.size() ? &(m_InspectionInputValues[0]) : nullptr);
			if (S_OK != hr)
			{
				throw hr;
			}
			if (m_dll.UseMil())
			{
				// This must be done after InitializeRun so the Dll has a guid for this tool
				hr = m_dll.SetMILInputImages(guid, (long)m_aInspectionInputImages.size(), (long)m_aInspectionInputImages.size() ? &(m_aInspectionInputImages[0]) : nullptr);
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
			hr = m_dll.GetResultImageDefinitions(guid, &NumResultImages, &paResultImageDefs);
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
						return (pEntry->GetEmbeddedID() == aSVVariantResultImageObjectGuid[i]);
					}
					);
					if (m_embeddedList.end() == Iter)
					{
						RegisterEmbeddedObject(pImage, aSVVariantResultImageObjectGuid[i], l_pImageNames[i]);
					}

					// get image info
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if (nullptr != GetTool())
					{
						imageInfo.SetOwner(GetTool()->GetUniqueObjectID());
					}
					else
					{
						imageInfo.SetOwner(GUID_NULL);
					}

					pImage->InitializeImage(SvDef::SVImageTypeEnum::SVImageTypePhysical);
					pImage->UpdateImage(GUID_NULL, imageInfo);
				}// end block
				if (m_bUseImageCopies)
				{
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if (nullptr != GetTool())
					{
						imageInfo.SetOwner(GetTool()->GetUniqueObjectID());
					}
					else
					{
						imageInfo.SetOwner(GUID_NULL);
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
				hr = m_dll.SetMILResultImages(guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr);
				if (S_OK != hr)
				{
					throw hr;
				}
			}

			SetAllAttributes();
		}// end try
		catch (HRESULT l_hr)
		{
			l_hr;	// remove compiler warning
			// logging ???
		}
		catch (int l_iErrorCode)
		{
			l_iErrorCode;	// remove compiler warning
			// logging ???
			hr = S_FALSE;// Error Initializing Tool
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
		m_dll.UninitializeRun(GetUniqueObjectID());
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
	bool l_bOk = true;

	if (m_dll.IsHandleNull())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitExternalTaskFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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
			HRESULT MatroxCode(S_OK);

			GUID guid = GetUniqueObjectID();
			int i;
			HRESULT hr = S_OK;

			// EB 20051121
			// check input images to see if they have resized
			// If so, do a ResetObject here first.
			// collect input images
			bool bNeedReset = false;

			if (m_aPreviousInputImageRect.size() == 0)
			{
				for (i = 0; i < m_Data.m_lNumInputImages; i++)
					m_aPreviousInputImageRect.push_back(RECT());
			}

			for (i = 0; i < m_Data.m_lNumInputImages; i++)
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
						bNeedReset = true;
						break;
					}
				}
			}

			if (bNeedReset)
			{
				l_bOk = ResetObject(pErrorMessages) && l_bOk;
			}

			/////////////////////////////////////////////////////
			//   Inputs
			/////////////////////////////////////////////////////

			// collect input values
			for (i = 0; i < m_Data.m_InputDefinitions.size(); i++)
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
						long SX = pTableObject->getColumNames(m_InspectionInputValues[i]);
					}
					else
					{
						m_InspectionInputValues[i] = m_Data.m_InputDefinitions[i].getDefaultValue();
					}

				}

			}
			// send input values to DLL
			long size = static_cast<long>(m_InspectionInputValues.size());
			hr = m_dll.SetInputValues(guid, size, size ? &(m_InspectionInputValues[0]) : nullptr);
			if (S_OK != hr)
			{
				throw hr;
			}

			bool l_bOkToRun = true;

			// collect input images
			for (i = 0; i < m_Data.m_lNumInputImages; i++)
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
									l_bOkToRun = false;
								}
							}
							else
							{
								SvOi::SVImageBufferHandlePtr l_ImageBufferCopy = m_aInputImagesCopy[i];
								if (nullptr != l_ImageBufferCopy && !l_ImageBufferCopy->empty())
								{
									MatroxCode = SVMatroxBufferInterface::CopyBuffer(l_ImageBufferCopy->GetBuffer(), pImageBuffer->getHandle()->GetBuffer());

									// This cast assumes that a Mil Id will never get larger than 32 bits in size.
									m_aInspectionInputImages[i] = static_cast<long>(l_ImageBufferCopy->GetBuffer().GetIdentifier());	// assign to copy handle
									if (m_aInspectionInputImages[i] == 0)
									{
										l_bOkToRun = false;
									}
								}
								else
								{
									l_bOkToRun = false;
								}
							}
						}// if( m_dll.UseMil() )
						else
						{
							MatroxCode = SVMatroxBufferInterface::CopyBuffer(m_aInspectionInputHBMImages[i].hbm, pImageBuffer->getHandle()->GetBuffer());
						}
					}
					else
					{
						l_bOkToRun = false;
					}
				}
			}

			if (m_dll.UseMil())
			{
				// send input images to DLL
				hr = m_dll.SetMILInputImages(guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : nullptr);
				if (S_OK != hr)
				{
					throw hr;
				}
			}
			else
			{
				std::vector<HBITMAP> aDIBHandles(m_Data.m_lNumInputImages);
				for (int i = 0; i < m_Data.m_lNumInputImages; i++)
					aDIBHandles[i] = m_aInspectionInputHBMImages.at(i).hbm;
				hr = m_dll.SetHBITMAPInputImages(guid, m_Data.m_lNumInputImages ? &(aDIBHandles[0]) : nullptr);
			}

			SvTrc::IImagePtr pResultImageBuffers[SVExternalToolTaskData::NUM_RESULT_IMAGES];
			for (i = 0; i < m_Data.m_lNumResultImages; i++)
			{
				pResultImageBuffers[i] = GetResultImage(i)->getImageToWrite(rRunStatus.m_triggerRecord);
			}
			if (m_dll.UseMil())
			{
				if (!m_bUseImageCopies)
				{
					// collect result images
					for (i = 0; i < m_Data.m_lNumResultImages; i++)
					{
						SVMatroxBuffer buffer;
						if (nullptr != pResultImageBuffers[i] && !pResultImageBuffers[i]->isEmpty())
						{
							buffer = pResultImageBuffers[i]->getHandle()->GetBuffer();
						}

						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionResultImages[i] = static_cast<long>(buffer.GetIdentifier());
						if (m_aInspectionResultImages[i] == 0)
						{
							l_bOkToRun = false;
						}
					}
				}// if ( !m_bUseImageCopies )
				else
				{
					// collect result image copies
					for (i = 0; i < m_Data.m_lNumResultImages; i++)
					{
						SvOi::SVImageBufferHandlePtr l_ImageBuffer = m_aResultImagesCopy[i];
						SVMatroxBuffer buffer;

						if (nullptr != l_ImageBuffer)
						{
							buffer = l_ImageBuffer->GetBuffer();
						}

						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionResultImages[i] = static_cast<long>(buffer.GetIdentifier());
						if (m_aInspectionResultImages[i] == 0)
						{
							l_bOkToRun = false;
						}
					}
				}

				// give result images to dll
				hr = m_dll.SetMILResultImages(guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr);
				if (S_OK != hr)
				{
					throw hr;
				}
			}// if( m_dll.UseMil() )
			else
			{ // HBitmap Images
			}

			///////////////////////////////////////
			//    Run
			///////////////////////////////////////
			long lToolStatus = 0;
			if (l_bOkToRun)
			{
				hr = m_dll.RunTool(guid, &lToolStatus);
			}
			if (S_OK != hr)
			{
				throw hr;
			}

			///////////////////////////////////////
			//    Outputs
			///////////////////////////////////////

			// Result Values
			long Resultsize = static_cast<long>(m_InspectionResultValues.size());
			hr = m_dll.GetResultValues(guid, Resultsize, Resultsize ? &(m_InspectionResultValues[0]) : nullptr);
			if (S_OK != hr)
			{
				throw hr;
			}

			for (i = 0; i < Resultsize; i++)
			{
				if (m_InspectionResultValues[i].vt & VT_ARRAY)
				{
					VARTYPE type = (m_InspectionResultValues[i].vt & (~VT_ARRAY));
					if (!GetResultValueObject(i)->isArray())
					{
						//if the value object is an array the correct size will be set in setValue
						GetResultValueObject(i)->SetArraySize(2);
					}
					if (!GetResultValueObject(i)->GetDefaultType() != type)
					{
						_variant_t var(0.0);
						var.ChangeType(type);
						GetResultValueObject(i)->SetDefaultValue(var);
					}
					GetResultValueObject(i)->setValue(m_InspectionResultValues[i]);

				}

				else
				{
					GetResultValueObject(i)->SetValue(m_InspectionResultValues[i]);
				}
				// Clear OleVariant that was created in Dll.
				m_InspectionResultValues[i].Clear();
			}

			/////////////////// TABLES RESULT 
			int NumTableResults = m_Data.getNumTableResults();
			if (S_OK == m_dll.getResultTables(GetUniqueObjectID(), NumTableResults, NumTableResults ? &(m_InspectionResultTables[0]) : nullptr))
			{
				for (int i = 0; i < NumTableResults; i++)
				{
					GetResultTableObject(i)->setTableValues(m_InspectionResultTables[i]);
					// Clear OleVariant that was created in Dll.
					m_InspectionResultTables[i].Clear();
				}
			}

			// Result Images

			if (m_Data.m_lNumResultImages > 0)
			{
				if (m_dll.UseMil())
				{
					if (m_bUseImageCopies)
					{
						// collect result images
						for (i = 0; i < m_Data.m_lNumResultImages; i++)
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
					hr = m_dll.GetHBITMAPResultImages(guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultHBMImages[0]) : nullptr);
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

			const int iBitmapInfoHeader = sizeof(BITMAPINFOHEADER);
			const int iDibSize = sizeof(DIBSECTION);

			if (l_bOkToRun)
			{
				rRunStatus.SetPassed();
			}
			else
			{
				l_bOk = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalTask_CheckToRunFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}

		}	// end try
		catch (const SvStl::MessageContainer& rSvE)
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(rSvE);
			}
		}
		catch (HRESULT hr)
		{
			hr;	// remove compiler warning
			// log hresult??

			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%X"), hr));
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalTask_UnknownException, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
#ifndef _DEBUG
		catch (...)
		{
			l_bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector msgList;
				msgList.push_back(_T(""));
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExternalTask_UnknownException, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
#endif
	}// end if( SVTaskObjectListClass::onRun( rRunStatus ) )
	else
	{
		l_bOk = false;
	}

	if (!l_bOk)
	{
		rRunStatus.SetInvalid();
	}

	return l_bOk;
}

HRESULT SVExternalToolTask::SetPathName(const std::string& rPath)
{
	HRESULT hr = S_OK;
	m_Data.m_voDllPath.SetValue(rPath);
	return hr;
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
				SVObjectManagerClass::Instance().DisconnectObjectInput(pImageInfo->GetInputObjectInfo().getUniqueObjectID(), pImageInfo);
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
			SVObjectManagerClass::Instance().ConnectObjectInput(pImageInfo->GetInputObjectInfo().getUniqueObjectID(), pImageInfo);
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

HRESULT SVExternalToolTask::InspectionInputsToVariantArray()
{
	long size = static_cast<long>(m_Data.m_InputDefinitions.size());
	for (int i = 0; i < size; i++)
	{
		if (i < (int)m_Data.m_aInputObjects.size() && m_Data.m_aInputObjects[i].isIndirectValue())
		{
			m_Data.m_aInputObjects[i].setIndirectValueSaveFlag(true);
		}

		_variant_t Value;

		int LVIndex = m_Data.m_InputDefinitions[i].getLinkedValueIndex();
		if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::Scalar)
		{
			m_Data.m_aInputObjects[LVIndex].GetValue(Value);
			HRESULT hrChangeType = ::VariantChangeTypeEx(&Value, &Value, SvDef::LCID_USA, 0, static_cast<VARTYPE>(m_Data.m_InputDefinitions[i].getVt()));
			if (S_OK != hrChangeType)
			{
				Value.Clear();
				Value.ChangeType(static_cast<VARTYPE>(m_Data.m_InputDefinitions[i].getVt()));
			}
		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::Array)
		{
			/// if we have an array the typ must be correct

			m_Data.m_aInputObjects[LVIndex].GetArrayValue(Value);
		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::TableArray)
		{
			const TableObject* pTableObject = dynamic_cast<const TableObject*>(m_Data.m_aInputObjects[LVIndex].GetLinkedObject());
			if (pTableObject)
			{
				long SX {0}, SY {0};
				pTableObject->getTableValues(Value, &SX, &SY);
			}
			else
			{
				Value = m_Data.m_InputDefinitions[i].getDefaultValue();
			}

		}
		else if (m_Data.m_InputDefinitions[i].getType() == SvOp::ExDllInterfaceType::TableNames)
		{
			const TableObject* pTableObject = dynamic_cast<const TableObject*>(m_Data.m_aInputObjects[LVIndex].GetLinkedObject());
			if (pTableObject)
			{
				pTableObject->getColumNames(Value);
			}
			else
			{
				Value = m_Data.m_InputDefinitions[i].getDefaultValue();
			}

		}

		m_InspectionInputValues[i] = Value;

	}
	return S_OK;
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
		case SvDef::SVImageFormatRGB888:
		case SvDef::SVImageFormatRGB8888:
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

	rInfo.GetExtentProperty(SvPb::SVExtentPropertyWidth, rImageDef.lWidth);
	rInfo.GetExtentProperty(SvPb::SVExtentPropertyHeight, rImageDef.lHeight);

	rInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_lFormat);
	rInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandNumber);

	if ((rImageDef.eImageFormat == SvDef::SVImageFormatRGB888 ||
		rImageDef.eImageFormat == SvDef::SVImageFormatRGB8888) &&
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
		interfaceInfo.EmbeddedID = m_Data.m_aResultObjects[iIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputVector.push_back(interfaceInfo);

		resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultVariantObjectType;
		resultClassInfo.m_ClassId = SVVariantResultClassGuid;
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
		info.ObjectType = SvPb::SVValueObjectType;
		info.SubType = SvPb::SVVariantValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;

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
				GUID objectID = pResult->GetUniqueObjectID();
				if (GUID_NULL != objectID)
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
	SVObjectManagerClass::Instance().VisitElements(l_Visitor, GetUniqueObjectID());

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
	SVObjectManagerClass::Instance().VisitElements(l_Visitor, GetUniqueObjectID());

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
		m_Data.m_aInputObjectNames[i].SetDefaultValue(_T(""), true);
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

	InspectionInputsToVariantArray();

	// Data Definition List stuff
	CollectInputImageNames();

	try
	{
		HRESULT hr = Initialize([](LPCTSTR) {});
		if (S_OK != hr)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitExternalTaskFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}

		int NumResults = m_Data.getNumResults();
		if (S_OK == m_dll.GetResultValues(GetUniqueObjectID(), NumResults, NumResults ? &(m_InspectionResultValues[0]) : nullptr))
		{
			for (int i = 0; i < NumResults; i++)
			{
				GetResultValueObject(i)->SetValue(m_InspectionResultValues[i]);

				// Clear OleVariant that was created in Dll.
				m_InspectionResultValues[i].Clear();
			}
		}
		int NumTableResults = m_Data.getNumTableResults();
		if (S_OK == m_dll.getResultTables(GetUniqueObjectID(), NumTableResults, NumTableResults ? &(m_InspectionResultTables[0]) : nullptr))
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

void SVExternalToolTask::GetDLLMessageString(HRESULT hr, BSTR* bstrMessage) const
{
	m_dll.GetMessageString(hr, bstrMessage);
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
					imageObjectInfo.ObjectType = SvPb::SVImageObjectType;
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
		if (GUID_NULL != rInfo.GetInputObjectInfo().getUniqueObjectID())
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

HRESULT SVExternalToolTask::CollectInputImageNames()
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


void SVExternalToolTaskData::InitializeInputs(long ArraySize, InputValueDefinitionStruct  InputValueDefs[])
{

	m_lNumInputValues = ArraySize;
	m_InputDefinitions.resize(ArraySize);
	m_NumLinkedValue = 0;

	for (int i = 0; i < ArraySize; i++)
	{
		m_InputDefinitions[i].setDefinition(InputValueDefs[i], &m_NumLinkedValue);

		InputValueDefinition& rInputDef = m_InputDefinitions[i];
		if (rInputDef.getType() == SvOp::ExDllInterfaceType::Scalar || rInputDef.getType() == SvOp::ExDllInterfaceType::Array)
		{
			int LinkValueIndex = rInputDef.getLinkedValueIndex();
			SvVol::LinkedValue& rInputValue = m_aInputObjects[LinkValueIndex];

			if (rInputValue.GetDefaultType() == VT_EMPTY)
			{
				bool bSetVal {true};
				if (VT_BSTR == rInputValue.GetValueType() || (rInputDef.getDefaultValue().vt & ~VT_ARRAY) == rInputValue.GetValueTypeEx())
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


		// get Input object name
		SvVol::SVStringValueObjectClass& rvo = m_aInputObjectNames[i];
		rvo.SetDefaultValue(rInputDef.getDisplayName(), true);

	}
}




} //namespace SvOp
