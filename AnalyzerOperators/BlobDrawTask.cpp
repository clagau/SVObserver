//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobDrawTask.h
/// This is the class for the Blob Analyzer 2 - Draw
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobDrawTask.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVMatroxLibrary/SVMatroxBlobInterface.h"
#include "SVImageAnalyzerClass.h"
#pragma endregion Includes

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

	void BlobDrawTask::init()
	{
		//Indentify our output type.
		m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::BlobDrawObjectType;

		RegisterEmbeddedObject(&m_isFillBlobs, SvPb::BlobUseFillEId, IDS_BLOB_USE_FILL, false, SvOi::SVResetItemOwner);
		RegisterEmbeddedObject(&m_blobFillColor, SvPb::BlobFillColorEId, IDS_BLOB_FILL_COLOR, false, SvOi::SVResetItemNone);
		RegisterEmbeddedObject(&m_evoBlobType, SvPb::BlobFillTypeEId, IDS_BLOB_FILL_TYPE, false, SvOi::SVResetItemNone);

		//set default values for the BlobFill value objects
		m_isFillBlobs.SetDefaultValue(BOOL(false), true);
		m_blobFillColor.SetDefaultValue(0, true);
		m_evoBlobType.SetEnumTypes(g_strBlobFillTypeEnums);
		m_evoBlobType.SetDefaultValue(SV_BLOB_FILL_EXCLUDED, true);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	BlobDrawTask::~BlobDrawTask()
	{
		BlobDrawTask::CloseObject();
	}

	bool BlobDrawTask::CloseObject()
	{

		return __super::CloseObject();
	}

	bool BlobDrawTask::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
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
				auto* pAnalyzer = dynamic_cast<SVImageAnalyzerClass*>(GetOwnerInfo().getObject());
				if (nullptr == pAnalyzer)
				{
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					msg.Throw();
				}
				SvIe::SVImageClass* pInputImage = pAnalyzer->getInputImage(true);
				if (nullptr == pInputImage)
				{
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					msg.Throw();
				}
				//use readOnly even if it overwritten the image, because it is already changed by the Analyzer.
				SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
				if (nullptr == pImageBuffer)
				{
					assert(false);
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					msg.Throw();
				}
				if (pImageBuffer->isEmpty())
				{
					assert(false);
					SvStl::MessageManager msg(SvStl::MsgType::Data);
					msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					msg.Throw();
				}

				byte color;
				long type;
				m_blobFillColor.GetValue(color);
				m_evoBlobType.GetValue(type);
				SVBlobControlEnum eCriterion = SVEBlobAll;
				switch (type)
				{
				case SV_BLOB_FILL_ALL:
				{
					eCriterion = SVEBlobAll;
					break;
				}
				case SV_BLOB_FILL_EXCLUDED:
				{
					eCriterion = SVEBlobExcludeBlobs;
					break;
				}
				case SV_BLOB_FILL_INCLUDED:
				{
					eCriterion = SVEBlobIncludeBlobs;
					break;
				}
				default:
				{
					// Do nothing.
					break;
				}
				}// end switch()

				/*MatroxCode =*/ SVMatroxBlobInterface::BlobFill(m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), eCriterion, color);
			}// end if
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
} //namespace SvAo

