//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobDrawTask.h
/// This is the class for the Blob Analyzer 2 - Draw
//******************************************************************************


#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "BlobAnalyzer2.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{
	enum SV_BLOB_FILL_TYPE_ENUM
	{
		SV_BLOB_FILL_ALL = M_ALL_BLOBS,
		SV_BLOB_FILL_INCLUDED = M_INCLUDED_BLOBS,
		SV_BLOB_FILL_EXCLUDED = M_EXCLUDED_BLOBS
	};

	// Add String for SVEnumerateValueObjectClass
	const LPCSTR g_strBlobFillTypeEnums = _T("All=1073741824,Included=134217728,Excluded=536870912");

	class BlobDrawTask : public SvIe::SVTaskObjectListClass
	{
		SV_DECLARE_CLASS(BlobDrawTask);

	public:
		BlobDrawTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_BLOBDRAW);

		virtual ~BlobDrawTask();

		virtual bool CloseObject() override;

		virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		bool resetStepsForDeletedFeature(const std::set<MIL_ID>& deletedFeatureGroups, SvStl::MessageContainerVector* pErrorMessages);

		void setResultBufferId(MIL_ID resultBufferID) { m_ResultBufferID = resultBufferID; };

	private:
		void init();

		bool setDrawTypeEnums(const BlobAnalyzer2& rAnalyzer, SvStl::MessageContainerVector* pErrorMessages);

		std::string getStepName(int pos) const;
		
		MIL_ID			m_ResultBufferID = M_NULL;
		//value objects for Fill Blob option
		SvVol::SVBoolValueObjectClass  m_isFillBlobs;
		SvVol::SVByteValueObjectClass  m_blobFillColor;
		SvVol::SVEnumerateValueObjectClass  m_evoBlobType;

		SvIe::SVImageClass m_AdditionalImages;
		SvVol::SVBoolValueObjectClass  m_useAdditionalImage;
		SvVol::SVBoolValueObjectClass  m_isColorAdditionalImage;
		SvVol::SVByteValueObjectClass  m_backgroundColor1;
		SvVol::SVByteValueObjectClass  m_backgroundColor2;
		SvVol::SVByteValueObjectClass  m_backgroundColor3;
		SvVol::SVStringValueObjectClass  m_additionalImage_StepNames;
		SvVol::SVEnumerateValueObjectClass  m_additionalImage_DrawTypes;
		SvVol::SVEnumerateValueObjectClass  m_additionalImage_BlobTypes;
		SvVol::SVByteValueObjectClass  m_additionalImage_Color1;
		SvVol::SVByteValueObjectClass  m_additionalImage_Color2;
		SvVol::SVByteValueObjectClass  m_additionalImage_Color3;
	};

} //namespace SvAo
