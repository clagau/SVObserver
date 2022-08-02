//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobDrawTask.h
/// This is the class for the Blob Analyzer 2 - Draw
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2.h"
#include "BlobDrawTask.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "InspectionEngine/RunStatus.h"
#include "SVMatroxLibrary/SVMatroxBlobInterface.h"
#include "ImageAnalyzer.h"
#include "ObjectInterfaces/NameValueVector.h"
#include "Definitions/SVImageFormatEnum.h"
#pragma endregion Includes

namespace
{
	const static SvOi::NameValueVector g_alwaysDrawType{
		{{"Blob"}, M_DRAW_BLOBS},
		{{"Blob Contour"}, M_DRAW_BLOBS_CONTOUR},
		{{"Box"}, M_DRAW_BOX},
		{{"Box Center"}, M_DRAW_BOX_CENTER},
		{{"Holes"}, M_DRAW_HOLES},
		{{"Holes Contour"}, M_DRAW_HOLES_CONTOUR} };

	const static std::vector<std::pair<long, SvOi::NameValuePair>> g_DrawTypeWithRequirements{
		//Requirement, Name, Long-Value
		{M_CONVEX_HULL, {{"Convex Hull"}, M_DRAW_CONVEX_HULL }},
		{M_CONVEX_HULL,{{"Convex Hull Contour"}, M_DRAW_CONVEX_HULL_CONTOUR }},
		{M_FERETS, {{"Feret Max"}, M_DRAW_FERET_MAX }},
		{M_FERETS, {{"Feret Min"}, M_DRAW_FERET_MIN }},
		{M_MIN_AREA_BOX, {{"Min Area Box"}, M_DRAW_MIN_AREA_BOX }},
		{M_CENTER_OF_GRAVITY, {{"Position"}, M_DRAW_POSITION }},
		{M_MIN_PERIMETER_BOX, {{"Min Perimeter Box"}, M_DRAW_MIN_PERIMETER_BOX }}
	};
}

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(BlobDrawTask, SvPb::BlobDrawClassId);

	BlobDrawTask::BlobDrawTask(SVObjectClass* POwner, int StringResourceID)
		: SVTaskObjectListClass(POwner, StringResourceID)
	{
		init();
	}

	BlobDrawTask::~BlobDrawTask()
	{
		BlobDrawTask::CloseObject();
	}

	bool BlobDrawTask::CloseObject()
	{

		return __super::CloseObject();
	}

	bool BlobDrawTask::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		try
		{
			if (!__super::onRun(rRunStatus, pErrorMessages))
			{
				return false;
			}

			// Now fill the blobs
			BOOL isFillBlob;
			m_isFillBlobs.GetValue(isFillBlob);
			if (isFillBlob)
			{
				auto* pAnalyzer = dynamic_cast<ImageAnalyzer*>(GetParent());
				if (nullptr == pAnalyzer)
				{
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					msg.Throw();
				}
				SvIe::SVImageClass* pInputImage = pAnalyzer->getInputImage(true);
				if (nullptr == pInputImage)
				{
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					msg.Throw();
				}
				//use readOnly even if it overwritten the image, because it is already changed by the Analyzer.
				SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
				if (nullptr == pImageBuffer)
				{
					assert(false);
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					msg.Throw();
				}
				if (pImageBuffer->isEmpty())
				{
					assert(false);
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					msg.Throw();
				}

				byte color;
				long type;
				m_blobFillColor.GetValue(color);
				m_evoBlobType.GetValue(type);
				/*MatroxCode =*/ SVMatroxBlobInterface::BlobFill(m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), static_cast<SVBlobControlEnum>(type), color);
			}// end if

			BOOL useAdditionalImage;
			m_useAdditionalImage.GetValue(useAdditionalImage);
			if (useAdditionalImage)
			{
				SvOi::ITRCImagePtr pImageBuffer = m_AdditionalImages.getImageToWrite(rRunStatus.m_triggerRecord);
				if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
				{
					assert(false);
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					msg.Throw();
				}

				BOOL isColorAdditionalImage;
				m_isColorAdditionalImage.GetValue(isColorAdditionalImage);
				byte background1;
				m_backgroundColor1.GetValue(background1);
				if (isColorAdditionalImage)
				{
					byte background2;
					m_backgroundColor2.GetValue(background2);
					byte background3;
					m_backgroundColor3.GetValue(background3);
					MbufClear(pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), M_RGB888(background1, background2, background3)); //M_COLOR_GREEN);
				}
				else
				{
					MbufClear(pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), background1);
				}

				int number = m_additionalImage_DrawTypes.getResultSize();
				for (int i = 0; i < number; ++i)
				{
					long drawType;
					m_additionalImage_DrawTypes.GetValue(drawType, i);
					long blobType;
					m_additionalImage_BlobTypes.GetValue(blobType, i);
					byte color1;
					m_additionalImage_Color1.GetValue(color1, i);
					if (isColorAdditionalImage)
					{
						byte color2;
						m_additionalImage_Color2.GetValue(color2, i);
						byte color3;
						m_additionalImage_Color3.GetValue(color3, i);
						MgraColor(M_DEFAULT, M_RGB888(color1, color2, color3));
					}
					else
					{
						MgraColor(M_DEFAULT, color1);
					}
					MblobDraw(M_DEFAULT, m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer().GetIdentifier(), drawType, blobType, M_DEFAULT);
				}
			}
		}
		catch (const SvStl::MessageContainer& e)
		{
			rRunStatus.SetInvalid();
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(e);
			}
			return false;
		}

		return true;
	}

	bool BlobDrawTask::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool result = __super::ResetObject(pErrorMessages);

		BOOL useAdditionalImage;
		m_useAdditionalImage.GetValue(useAdditionalImage);
		if (useAdditionalImage)
		{
			auto* pAnalyzer = dynamic_cast<BlobAnalyzer2*>(GetParent());
			if (nullptr == pAnalyzer)
			{
				if (pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
			SvIe::SVImageClass* pInputImage = pAnalyzer->getInputImage();
			if (nullptr == pInputImage)
			{
				if (pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
			BOOL isColorAdditionalImage;
			m_isColorAdditionalImage.GetValue(isColorAdditionalImage);
			auto imageInfo = pInputImage->GetImageInfo();
			imageInfo.SetImageProperty(SvDef::SVImagePropertyBandNumber, isColorAdditionalImage ? 3 : 1);
			imageInfo.SetImageProperty(SvDef::SVImagePropertyFormat, isColorAdditionalImage ? SvDef::SVImageFormatBGR888 : SvDef::SVImageFormatMono8);
			imageInfo.SetTranslation(SvPb::SVExtentTranslationNone);
			m_AdditionalImages.UpdateImage(SvDef::InvalidObjectId, imageInfo);
			m_AdditionalImages.SetObjectAttributesAllowed(SvPb::archivableImage | SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::OverwriteAttribute);

			result = setDrawTypeEnums(*pAnalyzer, pErrorMessages) && result;
		}
		else
		{
			m_AdditionalImages.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}

		return result;
	}

	bool BlobDrawTask::resetStepsForDeletedFeature(const std::set<MIL_ID>& deletedFeatureGroups, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool retValue{ true };
		for (int i = 0; i < m_additionalImage_DrawTypes.getResultSize(); ++i)
		{
			long value{ 0 };
			m_additionalImage_DrawTypes.GetValue(value, i);
			auto iter = std::find_if(g_DrawTypeWithRequirements.begin(), g_DrawTypeWithRequirements.end(), [value](const auto& rEntry) {return rEntry.second.second == value; });
			if (g_DrawTypeWithRequirements.end() != iter)
			{
				if (deletedFeatureGroups.end() != deletedFeatureGroups.find(iter->first))
				{
					retValue = false;
					if (pErrorMessages)
					{
						auto name = getStepName(i);
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidDrawTypeAfterDeletingFeature, { name }, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
			}
		}
		return retValue;
	}

	void BlobDrawTask::init()
	{
		//Identify our output type.
		m_ObjectTypeInfo.m_ObjectType = SvPb::BlobDrawObjectType;

		RegisterEmbeddedObject(&m_isFillBlobs, SvPb::BlobUseFillEId, IDS_BLOB_USE_FILL, false, SvOi::SVResetItemOwner);
		m_isFillBlobs.SetDefaultValue(BOOL(false), true);
		RegisterEmbeddedObject(&m_blobFillColor, SvPb::BlobFillColorEId, IDS_BLOB_FILL_COLOR, false, SvOi::SVResetItemNone);
		m_blobFillColor.SetDefaultValue(0, true);
		RegisterEmbeddedObject(&m_evoBlobType, SvPb::BlobFillTypeEId, IDS_BLOB_FILL_TYPE, false, SvOi::SVResetItemNone);
		m_evoBlobType.SetEnumTypes(g_strBlobFillTypeEnums);
		m_evoBlobType.SetDefaultValue(SV_BLOB_FILL_EXCLUDED, true);

		RegisterEmbeddedObject(&m_useAdditionalImage, SvPb::UseAdditionalImageEId, IDS_USE_ADDITIONAL_IMAGE, false, SvOi::SVResetItemOwner);
		m_useAdditionalImage.SetDefaultValue(BOOL(false), true);
		m_AdditionalImages.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypeIndependent);
		RegisterEmbeddedImage(&m_AdditionalImages, SvPb::OutputImageEId, "Additional Image");
		RegisterEmbeddedObject(&m_isColorAdditionalImage, SvPb::IsColorAdditionalImageEId, IDS_IS_COLOR_ADDITIONALIMAGE, false, SvOi::SVResetItemOwner);
		m_isColorAdditionalImage.SetDefaultValue(BOOL(false), true);
		RegisterEmbeddedObject(&m_backgroundColor1, SvPb::BackgroundColor1EId, IDS_BACKGROUND_COLOR1, false, SvOi::SVResetItemOwner);
		m_backgroundColor1.SetDefaultValue(0, false);
		RegisterEmbeddedObject(&m_backgroundColor2, SvPb::BackgroundColor2EId, IDS_BACKGROUND_COLOR2, false, SvOi::SVResetItemOwner);
		m_backgroundColor2.SetDefaultValue(0, false);
		RegisterEmbeddedObject(&m_backgroundColor3, SvPb::BackgroundColor3EId, IDS_BACKGROUND_COLOR3, false, SvOi::SVResetItemOwner);
		m_backgroundColor3.SetDefaultValue(0, false);

		RegisterEmbeddedObject(&m_additionalImage_StepNames, SvPb::DrawAdditionalImageStepNamesEId, IDS_ADDITONAL_IMAGE_STEPNAMES, false, SvOi::SVResetItemOwner);
		RegisterEmbeddedObject(&m_additionalImage_DrawTypes, SvPb::DrawTypesEId, IDS_ADDITONAL_IMAGE_DRAW_TYPES, false, SvOi::SVResetItemOwner);
		m_additionalImage_DrawTypes.SetEnumTypes(g_alwaysDrawType);
		m_additionalImage_DrawTypes.SetDefaultValue(M_DRAW_BLOBS);
		RegisterEmbeddedObject(&m_additionalImage_BlobTypes, SvPb::BlobTypesEId, IDS_ADDITONAL_IMAGE_BLOB_TYPES, false, SvOi::SVResetItemOwner);
		m_additionalImage_BlobTypes.SetEnumTypes(g_strBlobFillTypeEnums);
		m_additionalImage_BlobTypes.SetDefaultValue(SV_BLOB_FILL_INCLUDED);
		RegisterEmbeddedObject(&m_additionalImage_Color1, SvPb::Color1EId, IDS_ADDITONAL_IMAGE_COLOR1, false, SvOi::SVResetItemOwner);
		m_additionalImage_Color1.SetDefaultValue(255);
		RegisterEmbeddedObject(&m_additionalImage_Color2, SvPb::Color2EId, IDS_ADDITONAL_IMAGE_COLOR2, false, SvOi::SVResetItemOwner);
		m_additionalImage_Color2.SetDefaultValue(255);
		RegisterEmbeddedObject(&m_additionalImage_Color3, SvPb::Color3EId, IDS_ADDITONAL_IMAGE_COLOR3, false, SvOi::SVResetItemOwner);
		m_additionalImage_Color3.SetDefaultValue(255);
	}

	bool BlobDrawTask::setDrawTypeEnums(const BlobAnalyzer2& rAnalyzer, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool retValue = true;
		SvOi::NameValueVector drawList = g_alwaysDrawType;
		const auto& rGroupList = rAnalyzer.getFeatureGroups();
		for (const auto& rDrawType : g_DrawTypeWithRequirements)
		{
			auto iter = rGroupList.find(rDrawType.first);
			if (rGroupList.end() != iter)
			{
				drawList.push_back(rDrawType.second);
			}
		}
		m_additionalImage_DrawTypes.SetEnumTypes(drawList);
		for (int i = 0; i < m_additionalImage_DrawTypes.getResultSize(); ++i)
		{
			long value{ 0 };
			m_additionalImage_DrawTypes.GetValue(value, i);
			if (drawList.end() == std::find_if(drawList.begin(), drawList.end(), [value](const auto& rEntry) {return rEntry.second == value; }))
			{
				if (pErrorMessages)
				{
					auto name = getStepName(i);
					if (false == name.empty())
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidDrawTypeWithSelectedFeatures, { name }, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						pErrorMessages->push_back(Msg);
					}
					else
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
				retValue = false;
			}
		}
		return retValue;
	}

	std::string BlobDrawTask::getStepName(int pos) const
	{
		std::string names;
		m_additionalImage_StepNames.GetValue(names);
		for (int i = 0; i < pos; ++i)
		{
			auto findPos = names.find('|');
			if (std::string::npos != findPos)
			{
				names = names.substr(findPos + 1);
			}
			else
			{
				names = "";
				break;
			}
		}
		if (false == names.empty())
		{
			auto findPos = names.find('|');
			if (std::string::npos != findPos)
			{
				names = names.substr(0, findPos);
			}
		}
		return names;
	}
} //namespace SvAo

