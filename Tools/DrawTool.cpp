//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the Draw-tool
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "DrawTool.h"
#include "Definitions/SVImageFormatEnum.h"
#include "InspectionEngine/RunStatus.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "Operators/DrawTask.h"
#pragma endregion Includes

namespace SvTo
{
#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int cDefaultRingBufferDepth = 8;
constexpr long cDefaultIndexValue[] = {1, 2};
constexpr VARTYPE cVarType_imageIndex = VT_I4;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(DrawTool, SvPb::DrawToolClassId );

#pragma region Constructor
DrawTool::DrawTool( SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass(ToolExtType::All,pOwner, stringResourceID )
{
	BuildInputObjectList();
	BuildEmbeddedObjectList();
	BuildAvailableTaskList();

	// Set up your type... in this case this will reference that this tool is a 
	// Resize Tool.
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawToolObjectType;
}

DrawTool::~DrawTool(void)
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool DrawTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject( rCreateStructure );

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationNone);

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	// These values will not be exposed for this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = bOk;

	return bOk;
}

bool DrawTool::DoesObjectHaveExtents() const
{
	return false;
}

bool DrawTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);
	if (Result)
	{
		BOOL boolTmp;
		m_useBGImageObject.GetValue(boolTmp);
		m_useBGImage = boolTmp;
		m_autoFitSizeObject.GetValue(boolTmp);
		m_useAutofit = boolTmp;
		m_backgroundColor1Object.GetValue(m_backgroundColor1);
		m_backgroundColor2Object.GetValue(m_backgroundColor2);
		m_backgroundColor3Object.GetValue(m_backgroundColor3);

		if (m_useBGImage)
		{
			m_InputBGImage.validateInput();
			m_InputBGImage.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
			SvIe::SVImageClass* pInputImage = m_InputBGImage.getInput<SvIe::SVImageClass>(true);
			if (nullptr != pInputImage)
			{
				if ( m_pEmbeddedExtents.get() && m_useAutofit)
				{
					SVImageInfoClass inputImageInfo = pInputImage->GetImageInfo();
					m_pEmbeddedExtents->m_ExtentWidth.SetValue(inputImageInfo.getOutputWidth());
					m_pEmbeddedExtents->m_ExtentHeight.SetValue(inputImageInfo.getOutputHeight());
					getToolExtent().updateImageExtent(true);
				}
			}
			else
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoSourceImage, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			m_InputBGImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}

		m_isColorImageObject.GetValue(boolTmp);
		m_isColorImage = boolTmp;

		SVImageInfoClass imageInfo;
		imageInfo.SetExtents(getToolExtent().getImageExtent());
		imageInfo.SetImageProperty(SvDef::SVImagePropertyBandNumber, m_isColorImage ? 3 : 1);
		imageInfo.SetImageProperty(SvDef::SVImagePropertyFormat, m_isColorImage ? SvDef::SVImageFormatBGR888 : SvDef::SVImageFormatMono8);
		imageInfo.SetTranslation(SvPb::SVExtentTranslationNone);
		m_OutputImage.UpdateImageSetParentAndImageInfo(SvDef::InvalidObjectId, imageInfo);

		for (auto* pTaskObj : m_TaskObjectVector)
		{
			auto* pTask = dynamic_cast<SvOp::DrawTask*>(pTaskObj);
			if (nullptr != pTask)
			{
				pTask->setImageInfo(imageInfo);
			}
		}
	}

	return Result;
}

std::vector<std::string> DrawTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 3> cToolAdjustNameList
	{
		_T("Draw"),
		_T("Conditional"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

bool DrawTool::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	bool bRetVal = __super::Run(rRunStatus, pErrorMessages);

	if (bRetVal)
	{
		// Run yourself...
		auto pOutputImage = onRunLocal(rRunStatus, pErrorMessages);

		if (nullptr != pOutputImage && false == pOutputImage->isEmpty())
		{
			SvIe::RunStatus ChildRunStatus;
			ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
			ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;
			
			for (auto* pTaskObj : m_TaskObjectVector)
			{
				auto* pTask = dynamic_cast<SvOp::DrawTask*>(pTaskObj);
				if (nullptr != pTask)
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();
					bRetVal = pTask->Run(pOutputImage->getHandle(), ChildRunStatus, pErrorMessages) && bRetVal;
					rRunStatus.updateState(ChildRunStatus);
				}
				else
				{
					assert(false);
				}				
			}
			rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);
		}
		else
		{
			bRetVal = false;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			m_RunErrorMessages.push_back(Msg);
		}
	}

	if (!bRetVal)
	{
		// Something was wrong...
		rRunStatus.SetInvalid();
	}

	setStatus(rRunStatus);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}
#pragma endregion Public Methods

#pragma region Private Methods
SvOi::ITRCImagePtr DrawTool::onRunLocal( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = SVToolClass::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		auto pImageBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
		if (pImageBuffer && false == pImageBuffer->isEmpty())
		{
			if (m_isColorImage)
			{
				MbufClear(pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), M_RGB888(m_backgroundColor1, m_backgroundColor2, m_backgroundColor3));
			}
			else
			{
				MbufClear(pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), m_backgroundColor1);
			}

			if (m_useBGImage)
			{
				SvIe::SVImageClass* pInputImage = m_InputBGImage.getInput<SvIe::SVImageClass>(true);
				if (nullptr != pInputImage)
				{
					SvOi::ITRCImagePtr pInputBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
					if (nullptr != pInputBuffer && false == pInputBuffer->isEmpty())
					{
						int offsetX = 0;
						int offsetY = 0;
						if (false == m_useAutofit)
						{
							double tmp = 0.;
							m_BackgroundImageX.getValue(tmp);
							offsetX = -static_cast<int>(tmp);
							m_BackgroundImageY.getValue(tmp);
							offsetY = -static_cast<int>(tmp);
						}
						
						if (0 == offsetX && 0 == offsetY)
						{
							SVMatroxBufferInterface::CopyBuffer(pImageBuffer->getHandle()->GetBuffer(), pInputBuffer->getHandle()->GetBuffer());
						}
						else
						{
							SVMatroxBufferInterface::CopyBuffer(pImageBuffer->getHandle()->GetBuffer(), pInputBuffer->getHandle()->GetBuffer(), offsetX, offsetY);
						}
					}
				}
			}
		}
		return pImageBuffer;
	}

	return nullptr;
}

void DrawTool::BuildInputObjectList ()
{
	// Source Image (SVNotSetSubObjectType means both mono and color type)
	m_InputBGImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVNotSetSubObjectType);
	registerInputObject( &m_InputBGImage, SvDef::SourceImageInputName, SvPb::ImageInputEId);
	m_InputBGImage.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);
	m_InputBGImage.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
}


void DrawTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject(&m_useBGImageObject, SvPb::UseBackgroundImageEId, IDS_USE_BACKGROUND_IMAGE, false, SvOi::SVResetItemOwner);
	m_useBGImageObject.SetDefaultValue(BOOL(false), true);
	RegisterEmbeddedObject(&m_isColorImageObject, SvPb::IsColorImageEId, IDS_IS_COLOR_IMAGE, false, SvOi::SVResetItemOwner);
	m_isColorImageObject.SetDefaultValue(BOOL(false), true);
	RegisterEmbeddedObject(&m_autoFitSizeObject, SvPb::AutoFitSizeEId, IDS_USE_AUTOFIT, false, SvOi::SVResetItemOwner);
	m_autoFitSizeObject.SetDefaultValue(BOOL(false), true);

	RegisterEmbeddedObject(&m_BackgroundImageX, SvPb::PositionXEId, IDS_POSITION_X, false, SvOi::SVResetItemTool);
	variant_t vtTemp {0l};
	m_BackgroundImageX.SetDefaultValue(vtTemp);
	RegisterEmbeddedObject(&m_BackgroundImageY, SvPb::PositionYEId, IDS_POSITION_Y, false, SvOi::SVResetItemTool);
	m_BackgroundImageY.SetDefaultValue(vtTemp);

	RegisterEmbeddedObject(&m_backgroundColor1Object, SvPb::Color1EId, IDS_BACKGROUND_COLOR1, false, SvOi::SVResetItemOwner);
	m_backgroundColor1Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_backgroundColor2Object, SvPb::Color2EId, IDS_BACKGROUND_COLOR2, false, SvOi::SVResetItemOwner);
	m_backgroundColor2Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_backgroundColor3Object, SvPb::Color3EId, IDS_BACKGROUND_COLOR3, false, SvOi::SVResetItemOwner);
	m_backgroundColor3Object.SetDefaultValue(0, false);

	m_OutputImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypeIndependent);
	RegisterEmbeddedImage(&m_OutputImage, SvPb::OutputImageEId, IDS_OBJECTNAME_IMAGE1);
}

void DrawTool::BuildAvailableTaskList()
{
	// Populate the available list
	SvIe::SVClassInfoStruct taskClassInfo;

	// Add the rectangle task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawRectangleTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawRectangleClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_RECTANGLE);
	m_availableChildren.push_back(taskClassInfo);

	// Add the oval task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawOvalTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawOvalClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_OVAL);
	m_availableChildren.push_back(taskClassInfo);

	// Add the segment task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawSegmentTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawSegmentClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_SEGMENT);
	m_availableChildren.push_back(taskClassInfo);

	// Add the triangle task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawTriangleTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawTriangleClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_TRIANGLE);
	m_availableChildren.push_back(taskClassInfo);

	// Add the lines task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawLineTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawLineClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_LINES);
	m_availableChildren.push_back(taskClassInfo);

	// Add the points task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawPointsTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawPointsClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_POINTS);
	m_availableChildren.push_back(taskClassInfo);

	// Add the polygon task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawPolygonTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawPolygonClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_POLYGON);
	m_availableChildren.push_back(taskClassInfo);

	// Add the Text task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawTextTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawTextClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_TEXT);
	m_availableChildren.push_back(taskClassInfo);

	// Add the BucketFill task
	taskClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	taskClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::DrawBucketFillTaskType;
	taskClassInfo.m_ClassId = SvPb::DrawBucketFillClassId;
	taskClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_DRAW_BUCKETFILL);
	m_availableChildren.push_back(taskClassInfo);
}
#pragma endregion Private Methods

} //namespace SvTo
