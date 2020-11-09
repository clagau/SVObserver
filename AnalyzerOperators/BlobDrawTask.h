//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobDrawTask.h
/// This is the class for the Blob Analyzer 2 - Draw
//******************************************************************************


#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma endregion Includes


namespace SvAo
{
	enum SV_BLOB_FILL_TYPE_ENUM
	{
		SV_BLOB_FILL_ALL = 0x8000,
		SV_BLOB_FILL_INCLUDED = 0x2000,
		SV_BLOB_FILL_EXCLUDED = 0x1000
	};

	// Add String for SVEnumerateValueObjectClass
	const LPCSTR g_strBlobFillTypeEnums = _T("All=32768,Included=8192,Excluded=4096");

	class BlobDrawTask : public SvIe::SVTaskObjectListClass
	{
		SV_DECLARE_CLASS(BlobDrawTask);

	public:
		BlobDrawTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_BLOBDRAW);

		virtual ~BlobDrawTask();

		virtual bool CloseObject() override;

		virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		void setResultBufferId(MIL_ID resultBufferID) { m_ResultBufferID = resultBufferID; };

	private:
		void init();
		
		MIL_ID			m_ResultBufferID = M_NULL;
		//value objects for Fill Blob option
		SvVol::SVBoolValueObjectClass  m_isFillBlobs;
		SvVol::SVByteValueObjectClass  m_blobFillColor;
		SvVol::SVEnumerateValueObjectClass  m_evoBlobType;
	};

} //namespace SvAo
