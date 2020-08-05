//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzer
//* .File Name       : $Workfile:   SVBlobAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   23 Jan 2015 11:20:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2.h"
#include "BlobFeatureTask.h"
#include "SVMatroxLibrary/SVMatroxBlobInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "Definitions/Color.h"
#include "Definitions/TextDefineSVDef.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVRange.h"
#include "Operators/TableObject.h"
#include "Tools/SVTool.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Operators/SVResultLong.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	enum class BlobColorEnum
	{
		SV_BLOB_BLACK = 0,
		SV_BLOB_WHITE = 1
	};// end SV_BLOB_COLOR_ENUM
	const LPCSTR g_strBlobColorEnums = _T("Black=0,NonBlack=1");

	enum class BlobConnectivityEnum
	{
		NEIGHBORS_4 = 0,
		NEIGHBORS_8 = 1
	};// end SV_BLOB_COLOR_ENUM
	const LPCSTR g_strBlobConnectivityEnums = _T("4Neighbors=0,8Neighbors=1");

	struct BlobFeatureData
	{
		int    m_stringResourceID;
		MIL_ID m_FeatureGroup;
		bool m_isGrayValue = false;

		BlobFeatureData(int stringResourceId = 0, MIL_ID featureGroup = M_NULL, bool isGrayValue = false) : m_stringResourceID(stringResourceId), m_FeatureGroup(featureGroup), m_isGrayValue(isGrayValue) {};
	};

	static std::map<MIL_ID, BlobFeatureData> g_featureDataMap = {
		{ M_AREA,						BlobFeatureData{ IDS_OBJECTNAME_AREA, M_BOX, false}},
		{ M_BLOB_TOUCHING_IMAGE_BORDERS,BlobFeatureData{ IDS_OBJECTNAME_BLOB_TOUCHING_IMAGE_BORDERS, M_NULL, false}},
		{ M_BOX_AREA,					BlobFeatureData{ IDS_OBJECTNAME_BOX_AREA, M_BOX, false } },
		{ M_BOX_ASPECT_RATIO,			BlobFeatureData{ IDS_OBJECTNAME_BOX_ASPECT_RATIO, M_BOX, false}},
		{ M_BOX_FILL_RATIO,				BlobFeatureData{ IDS_OBJECTNAME_BOX_FILL_RATIO, M_BOX, false } },
		{ M_BOX_X_MAX,					BlobFeatureData{ IDS_OBJECTNAME_BOXXMAX, M_BOX, false }},
		{ M_BOX_X_MIN,					BlobFeatureData{ IDS_OBJECTNAME_BOXXMIN, M_BOX, false }},
		{ M_BOX_Y_MAX,					BlobFeatureData{ IDS_OBJECTNAME_BOXYMAX, M_BOX, false } },
		{ M_BOX_Y_MIN,					BlobFeatureData{ IDS_OBJECTNAME_BOXYMIN, M_BOX, false } },
		{ M_BREADTH,					BlobFeatureData{ IDS_OBJECTNAME_BREADTH, M_BOX, false } },
		{ M_COMPACTNESS,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_COMPACTNESS, M_COMPACTNESS, false } },
		{ M_CONVEX_HULL_AREA,			BlobFeatureData{ IDS_OBJECTNAME_CONVEX_HULL_AREA, M_CONVEX_HULL, false } },
		{ M_CONVEX_HULL_COG_X,			BlobFeatureData{ IDS_OBJECTNAME_CONVEX_HULL_COG_X, M_CONVEX_HULL, false } },
		{ M_CONVEX_HULL_COG_Y,			BlobFeatureData{ IDS_OBJECTNAME_CONVEX_HULL_COG_Y, M_CONVEX_HULL, false } },
		{ M_CONVEX_HULL_FILL_RATIO,		BlobFeatureData{ IDS_OBJECTNAME_CONVEX_HULL_FILL_RATIO, M_CONVEX_HULL, false } },
		{ M_CONVEX_HULL_PERIMETER,		BlobFeatureData{ IDS_OBJECTNAME_CONVEX_HULL_PERIMETER, M_CONVEX_HULL, false } },
		{ M_CONVEX_PERIMETER,			BlobFeatureData{ IDS_OBJECTNAME_CONVEXPERIMETER, M_CONVEX_PERIMETER, false } },
		{ M_ELONGATION,					BlobFeatureData{ IDS_OBJECTNAME_BLOB_ELONGATION, M_ELONGATION, false } },
		{ M_EULER_NUMBER,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_EULER_NUMBER, M_EULER_NUMBER, false } },
		{ M_FERET_ELONGATION,			BlobFeatureData{ IDS_OBJECTNAME_FERETELONGATION, M_FERETS, false } },
		{ M_FERET_GENERAL,				BlobFeatureData{ IDS_OBJECTNAME_FERET_GENERAL, M_FERETS, false } },
		{ M_FERET_MAX_ANGLE,			BlobFeatureData{ IDS_OBJECTNAME_FERETMAXANGLE, M_FERETS, false } },
		{ M_FERET_MAX_DIAMETER,			BlobFeatureData{ IDS_OBJECTNAME_FERETMAXDIAMETER, M_FERETS, false } },
		{ M_FERET_MAX_DIAMETER_ELONGATION,		BlobFeatureData{ IDS_OBJECTNAME_FERET_MAX_DIAMETER_ELONGATION, M_FERET_MAX_DIAMETER_ELONGATION, false } },
		{ M_FERET_MEAN_DIAMETER,		BlobFeatureData{ IDS_OBJECTNAME_FERETMEANDIAMETER, M_FERETS, false } },
		{ M_FERET_MIN_ANGLE,			BlobFeatureData{ IDS_OBJECTNAME_FERETMINANGLE, M_FERETS, false } },
		{ M_FERET_MIN_DIAMETER,			BlobFeatureData{ IDS_OBJECTNAME_FERETMINDIAMETER, M_FERETS, false } },
		{ M_FERET_MIN_DIAMETER_ELONGATION,	BlobFeatureData{ IDS_OBJECTNAME_FERET_MIN_DIAMETER_ELONGATION,  M_FERET_MIN_DIAMETER_ELONGATION, false } },
		{ M_FERET_PERPENDICULAR_TO_MAX_DIAMETER, BlobFeatureData{ IDS_OBJECTNAME_FERET_PERPENDICULAR_TO_MAX_DIAMETER, M_FERET_PERPENDICULAR_TO_MAX_DIAMETER, false } },
		{ M_FERET_PERPENDICULAR_TO_MIN_DIAMETER, BlobFeatureData{ IDS_OBJECTNAME_FERET_PERPENDICULAR_TO_MIN_DIAMETER, M_FERET_PERPENDICULAR_TO_MIN_DIAMETER, false } },
		{ M_FERET_X,					BlobFeatureData{ IDS_OBJECTNAME_FERETX, M_BOX, false } },
		{ M_FERET_Y,					BlobFeatureData{ IDS_OBJECTNAME_FERETY, M_BOX, false } },
		{ M_FIRST_POINT_X,				BlobFeatureData{ IDS_OBJECTNAME_FIRSTPOINTX, M_BOX, false } },
		{ M_FIRST_POINT_Y,				BlobFeatureData{ IDS_OBJECTNAME_FIRSTPOINTY, M_BOX, false } },
		{ M_INTERCEPT_0,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_INTERCEPT0, M_INTERCEPT, false } },
		{ M_INTERCEPT_45,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_INTERCEPT45, M_INTERCEPT, false } },
		{ M_INTERCEPT_90,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_INTERCEPT90, M_INTERCEPT, false } },
		{ M_INTERCEPT_135,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_INTERCEPT135, M_INTERCEPT, false } },
		{ M_LABEL_VALUE,				BlobFeatureData{ IDS_OBJECTNAME_LABEL, M_NULL, false } },
		{ M_LENGTH,						BlobFeatureData{ IDS_OBJECTNAME_LENGTH, M_LENGTH, false } },
		{ M_MIN_AREA_BOX_ANGLE,			BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_ANGLE, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_AREA,			BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_AREA, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_CENTER_X,		BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_CENTER_X, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_CENTER_Y,		BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_CENTER_Y, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_HEIGHT,		BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_HEIGHT, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_PERIMETER,		BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_PERIMETER, M_MIN_AREA_BOX, false } },
		{ M_MIN_AREA_BOX_WIDTH,			BlobFeatureData{ IDS_OBJECTNAME_MIN_AREA_BOX_WIDTH, M_MIN_AREA_BOX, false } },
		{ M_MIN_PERIMETER_BOX_ANGLE,	BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_ANGLE, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_AREA,		BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_AREA, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_CENTER_X,	BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_CENTER_X, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_CENTER_Y,	BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_CENTER_Y, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_HEIGHT,	BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_HEIGHT, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_PERIMETER,BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_PERIMETER, M_MIN_PERIMETER_BOX, false } },
		{ M_MIN_PERIMETER_BOX_WIDTH,	BlobFeatureData{ IDS_OBJECTNAME_MIN_PERIMETER_BOX_WIDTH, M_MIN_PERIMETER_BOX, false } },
		{ M_NUMBER_OF_CHAINED_PIXELS,	BlobFeatureData{ IDS_OBJECTNAME_NUMBER_OF_CHAINED_PIXELS, M_CHAINS, false } },
		{ M_NUMBER_OF_CONVEX_HULL_POINTS,	BlobFeatureData{ IDS_OBJECTNAME_NUMBER_OF_CONVEX_HULL_POINTS, M_CONVEX_HULL, false } },
		{ M_NUMBER_OF_HOLES,			BlobFeatureData{ IDS_OBJECTNAME_NBROFHOLES, M_NUMBER_OF_HOLES, false } },
		{ M_NUMBER_OF_RUNS ,			BlobFeatureData{ IDS_OBJECTNAME_NUMBER_OF_RUNS, M_RUNS, false } },
		{ M_PERIMETER,					BlobFeatureData{ IDS_OBJECTNAME_PERIMETER, M_PERIMETER, false } },
		{ M_RECTANGULARITY,				BlobFeatureData{ IDS_OBJECTNAME_NBROFHOLES, M_RECTANGULARITY, false } },
		{ M_ROUGHNESS,					BlobFeatureData{ IDS_OBJECTNAME_ROUGHNESS, M_ROUGHNESS, false } },
		{ M_X_MAX_AT_Y_MAX,				BlobFeatureData{ IDS_OBJECTNAME_XMAX_AT_YMAX, M_CONTACT_POINTS, false } },
		{ M_X_MAX_AT_Y_MIN,				BlobFeatureData{ IDS_OBJECTNAME_XMAX_AT_YMIN, M_CONTACT_POINTS, false } },
		{ M_X_MIN_AT_Y_MAX,				BlobFeatureData{ IDS_OBJECTNAME_XMIN_AT_YMAX, M_CONTACT_POINTS, false } },
		{ M_X_MIN_AT_Y_MIN,				BlobFeatureData{ IDS_OBJECTNAME_XMIN_AT_YMIN, M_CONTACT_POINTS, false } },
		{ M_Y_MAX_AT_X_MAX,				BlobFeatureData{ IDS_OBJECTNAME_YMAX_AT_XMax, M_CONTACT_POINTS, false } },
		{ M_Y_MAX_AT_X_MIN,				BlobFeatureData{ IDS_OBJECTNAME_YMAX_AT_XMIN, M_CONTACT_POINTS, false } },
		{ M_Y_MIN_AT_X_MAX,				BlobFeatureData{ IDS_OBJECTNAME_YMIN_AT_XMAX, M_CONTACT_POINTS, false } },
		{ M_Y_MIN_AT_X_MIN,				BlobFeatureData{ IDS_OBJECTNAME_YMIN_AT_XMIN, M_CONTACT_POINTS, false } },
		{ M_BLOB_CONTRAST,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_CONTRAST, M_BLOB_CONTRAST, true } },
		{ M_MAX_PIXEL,					BlobFeatureData{ IDS_OBJECTNAME_MAX_PIXEL, M_MAX_PIXEL, true } },
		{ M_MEAN_PIXEL,					BlobFeatureData{ IDS_OBJECTNAME_MEAN_PIXEL, M_MEAN_PIXEL, true } },
		{ M_MIN_PIXEL,					BlobFeatureData{ IDS_OBJECTNAME_MIN_PIXEL, M_MIN_PIXEL, true } },
		{ M_SIGMA_PIXEL,				BlobFeatureData{ IDS_OBJECTNAME_SIGMA_PIXEL, M_SIGMA_PIXEL, true } },
		{ M_SUM_PIXEL,					BlobFeatureData{ IDS_OBJECTNAME_SUMPIXEL, M_SUM_PIXEL, true } },
		{ M_SUM_PIXEL_SQUARED,			BlobFeatureData{ IDS_OBJECTNAME_SUM_PIXEL_SQUARED, M_SUM_PIXEL_SQUARED, true } },

		{ M_AXIS_PRINCIPAL_ANGLE + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_AXIS_PRINCIPAL_ANGLE, M_MOMENT_SECOND_ORDER, false } },
		{ M_AXIS_PRINCIPAL_ANGLE + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_AXIS_PRINCIPAL_ANGLE_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_AXIS_SECONDARY_ANGLE + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_AXIS_SECONDARY_ANGLE, M_MOMENT_SECOND_ORDER, false } },
		{ M_AXIS_SECONDARY_ANGLE + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_AXIS_SECONDARY_ANGLE_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_CENTER_OF_GRAVITY_X + M_BINARY,			BlobFeatureData{ IDS_OBJECTNAME_CENTEROFGRAVITYX, M_CENTER_OF_GRAVITY, false } },
		{ M_CENTER_OF_GRAVITY_X + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_CENTEROFGRAVITYX_GRAY, M_CENTER_OF_GRAVITY, true } },
		{ M_CENTER_OF_GRAVITY_Y + M_BINARY,			BlobFeatureData{ IDS_OBJECTNAME_CENTEROFGRAVITYY, M_CENTER_OF_GRAVITY, false } },
		{ M_CENTER_OF_GRAVITY_Y + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_CENTEROFGRAVITYY_GRAY, M_CENTER_OF_GRAVITY, true } },
		{ M_FERET_AT_PRINCIPAL_AXIS_ANGLE + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_FERET_AT_PRINCIPAL_AXIS_ANGLE, M_FERET_AT_PRINCIPAL_AXIS_ANGLE, false } },
		{ M_FERET_AT_PRINCIPAL_AXIS_ANGLE + M_GRAYSCALE,	BlobFeatureData{ IDS_OBJECTNAME_FERET_AT_PRINCIPAL_AXIS_ANGLE_GRAY, M_FERET_AT_PRINCIPAL_AXIS_ANGLE, true } },
		{ M_FERET_AT_SECONDARY_AXIS_ANGLE + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_FERET_AT_SECONDARY_AXIS_ANGLE, M_FERET_AT_SECONDARY_AXIS_ANGLE, false } },
		{ M_FERET_AT_SECONDARY_AXIS_ANGLE + M_GRAYSCALE,	BlobFeatureData{ IDS_OBJECTNAME_FERET_AT_SECONDARY_AXIS_ANGLE_GRAY, M_FERET_AT_SECONDARY_AXIS_ANGLE, true } },
		{ M_FERET_PRINCIPAL_AXIS_ELONGATION + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_FERET_PRINCIPAL_AXIS_ELONGATION, M_FERET_PRINCIPAL_AXIS_ELONGATION, false } },
		{ M_FERET_PRINCIPAL_AXIS_ELONGATION + M_GRAYSCALE,	BlobFeatureData{ IDS_OBJECTNAME_FERET_PRINCIPAL_AXIS_ELONGATION_GRAY, M_FERET_PRINCIPAL_AXIS_ELONGATION, true } },
		{ M_MOMENT_CENTRAL_X0_Y2 + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X0Y2, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_CENTRAL_X0_Y2 + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X0Y2_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_MOMENT_CENTRAL_X1_Y1 + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X1Y1, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_CENTRAL_X1_Y1 + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X1Y1_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_MOMENT_CENTRAL_X2_Y0 + M_BINARY,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X2Y0, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_CENTRAL_X2_Y0 + M_GRAYSCALE,		BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X2Y0_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_MOMENT_GENERAL + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_MOMENT_GENERAL, M_MOMENT_GENERAL, false } },
		{ M_MOMENT_GENERAL + M_GRAYSCALE,			BlobFeatureData{ IDS_OBJECTNAME_MOMENT_GENERAL_GRAY, M_MOMENT_GENERAL, true } },
		{ M_MOMENT_X0_Y1 + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X0Y1, M_MOMENT_FIRST_ORDER, false } },
		{ M_MOMENT_X0_Y1 + M_GRAYSCALE,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X0Y1_GRAY, M_MOMENT_FIRST_ORDER, true } },
		{ M_MOMENT_X0_Y2 + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X0Y2, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_X0_Y2 + M_GRAYSCALE,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X0Y2_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_MOMENT_X1_Y0 + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X1Y0, M_MOMENT_FIRST_ORDER, false } },
		{ M_MOMENT_X1_Y0 + M_GRAYSCALE,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X1Y0_GRAY, M_MOMENT_FIRST_ORDER, true } },
		{ M_MOMENT_X1_Y1 + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X1Y1, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_X1_Y1 + M_GRAYSCALE,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X1Y1_GRAY, M_MOMENT_SECOND_ORDER, true } },
		{ M_MOMENT_X2_Y0 + M_BINARY,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X2Y0, M_MOMENT_SECOND_ORDER, false } },
		{ M_MOMENT_X2_Y0 + M_GRAYSCALE,				BlobFeatureData{ IDS_OBJECTNAME_BLOB_MOMENT_X2Y0_GRAY, M_MOMENT_SECOND_ORDER, true } },
	};

#pragma warning (push)
#pragma warning (disable : 4592)
	//@TODO[MZA][10.10][08.07.2020] https://stackoverflow.com/questions/34013930/error-c4592-symbol-will-be-dynamically-initialized-vs2015-1-static-const-std
	const std::initializer_list<MIL_ID> g_necessaryFeatures{ M_AREA, M_BOX_X_MIN, M_BOX_X_MAX, M_BOX_Y_MIN, M_BOX_Y_MAX };
	static std::map<MIL_ID, int> g_ResultColumPosMap = {{ M_BOX_X_MIN, 0 }, { M_BOX_X_MAX, 1 }, { M_BOX_Y_MIN, 2 }, { M_BOX_Y_MAX, 3 }};
#pragma warning (pop)
#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(BlobAnalyzer2, SvPb::BlobAnalyzer2ClassId);

	BlobAnalyzer2::BlobAnalyzer2(SVObjectClass* POwner, int StringResourceID)
		: SVImageAnalyzerClass(POwner, StringResourceID)
		, m_pResultTable(nullptr)
	{
		init();
	}

	void BlobAnalyzer2::init()
	{
		//Indentify our output type.
		m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::BlobAnalyzer2ObjectType;

		RegisterEmbeddedObject(&m_maxBlobDataArraySize, SvPb::MaxBlobDataArraySizeEId, IDS_OBJECTNAME_MAX_BLOB_DATA_ARRAY_SIZE, false, SvOi::SVResetItemOwner);
		RegisterEmbeddedObject(&m_numberOfBlobsFound, SvPb::NbrOfBlobsFoundEId, IDS_OBJECTNAME_NBROFBLOBSFOUND, false, SvOi::SVResetItemNone);
		m_numberOfBlobsFound.setSaveValueFlag(false);

		RegisterEmbeddedObject(	&m_isFillBlobs,	SvPb::BlobUseFillEId, IDS_BLOB_USE_FILL, false, SvOi::SVResetItemOwner );

		RegisterEmbeddedObject(&m_isGrayImageValue, SvPb::IsGrayImageEId, IDS_BLOB_IS_GRAY_IMAGE, false, SvOi::SVResetItemOwner);
		RegisterEmbeddedObject(&m_colorBlobEnumValue, SvPb::BlobColorEId, IDS_BLACK_BLOBS, false, SvOi::SVResetItemOwner);
		RegisterEmbeddedObject(&m_connectivityEnumValue, SvPb::ConnectivityBlobEId, IDS_BLOB_CONNECTIVITY, false, SvOi::SVResetItemOwner);

		RegisterEmbeddedObject(&m_blobFillColor, SvPb::BlobFillColorEId, IDS_BLOB_FILL_COLOR, false, SvOi::SVResetItemNone );
		RegisterEmbeddedObject(&m_evoBlobType, SvPb::BlobFillTypeEId, IDS_BLOB_FILL_TYPE, false, SvOi::SVResetItemNone );

		m_grayImageInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
		m_grayImageInfo.SetObject(GetObjectInfo());
		RegisterInputObject(&m_grayImageInfo, SvDef::GrayImageConnectionName);

		// Set default inputs and outputs
		addDefaultInputObjects();

		m_maxBlobDataArraySize.SetDefaultValue(100, true);
		m_isGrayImageValue.SetDefaultValue(BOOL(false), true);
		m_colorBlobEnumValue.SetEnumTypes(g_strBlobColorEnums);
		m_colorBlobEnumValue.SetDefaultValue(static_cast<long>(BlobColorEnum::SV_BLOB_WHITE), true);
		m_connectivityEnumValue.SetEnumTypes(g_strBlobConnectivityEnums);
		m_connectivityEnumValue.SetDefaultValue(static_cast<long>(BlobConnectivityEnum::NEIGHBORS_8), true);
		//set default values for the BlobFill value objects
		m_isFillBlobs.SetDefaultValue(BOOL(false), true);
		m_blobFillColor.SetDefaultValue(0, true);
		m_evoBlobType.SetEnumTypes(g_strBlobFillTypeEnums);
		m_evoBlobType.SetDefaultValue(SV_BLOB_FILL_EXCLUDED, true);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	BlobAnalyzer2::~BlobAnalyzer2()
	{
		BlobAnalyzer2::CloseObject();
	}

#pragma region IBlobAnalyzer2
	SvPb::InspectionCmdResponse BlobAnalyzer2::getFeaturesData() const
	{
		SvPb::InspectionCmdResponse cmdResponse;
		SvPb::GetFeaturesResponse* pResponse = cmdResponse.mutable_getfeaturesresponse();
		for (const auto* pTask : m_featureTaskVec)
		{
			auto pFeatureData = pResponse->add_list();
			pFeatureData->set_type(pTask->getFeatureType());
			pFeatureData->set_name(pTask->GetName());
			pFeatureData->set_is_gray(g_featureDataMap[pTask->getFeatureType()].m_isGrayValue);
			switch (pTask->getSortEnum())
			{
			case SortEnum::Ascent:
				pFeatureData->set_is_sort(true);
				pFeatureData->set_is_ascent(true);
				break;
			case SortEnum::Descent:
				pFeatureData->set_is_sort(true);
				pFeatureData->set_is_ascent(false);
				break;
			default:
				pFeatureData->set_is_sort(false);
				break;
			}
			const auto& excludeData = pTask->getExcludeData();
			pFeatureData->set_is_exclude(excludeData.m_isEnabled);
			pFeatureData->set_is_exclude_inner(excludeData.m_isInner);
			double tmpDouble = 0;
			excludeData.m_rLowerBoundValue.getValue(tmpDouble);
			pFeatureData->set_lower_bound(tmpDouble);
			std::string tmpStr;
			excludeData.m_rLowerBoundValue.getLinkedName().getValue(tmpStr);
			pFeatureData->set_lower_bound_indirect(tmpStr);
			excludeData.m_rUpperBoundValue.getValue(tmpDouble);
			pFeatureData->set_upper_bound(tmpDouble);
			excludeData.m_rUpperBoundValue.getLinkedName().getValue(tmpStr);
			pFeatureData->set_upper_bound_indirect(tmpStr);

			const auto& rangeData = pTask->getRangeData();
			pFeatureData->set_is_range(rangeData.m_isEnabled);
			pFeatureData->set_range_fail_high(rangeData.m_failHigh);
			pFeatureData->set_range_fail_high_indirect(rangeData.m_failHighIndirect);
			pFeatureData->set_range_warn_high(rangeData.m_warnHigh);
			pFeatureData->set_range_warn_high_indirect(rangeData.m_warnHighIndirect);
			pFeatureData->set_range_warn_low(rangeData.m_warnLow);
			pFeatureData->set_range_warn_low_indirect(rangeData.m_warnLowIndirect);
			pFeatureData->set_range_fail_low(rangeData.m_failLow);
			pFeatureData->set_range_fail_low_indirect(rangeData.m_failLowIndirect);
		}
		return cmdResponse;
	}

	SvPb::InspectionCmdResponse BlobAnalyzer2::setFeatures(SvPb::SetFeaturesRequest request)
	{
		auto list = request.list();

		//If gray Image not set, remove the gray feature.
		BOOL isGrayValid = false;
		m_isGrayImageValue.GetValue(isGrayValid);
		if (FALSE == isGrayValid)
		{
			auto iter = std::remove_if(list.begin(), list.end(), [](const auto& rEntry) {return rEntry.is_gray(); });
			if (list.end() != iter)
			{
				list.erase(iter, list.end());
				SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RemoveGrayFeature, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			}
		}
		
		SvPb::InspectionCmdResponse cmdResponse;
		SvPb::SetFeaturesResponse* pResponse = cmdResponse.mutable_setfeaturesresponse();
		for (int i = 0; list.size() > i && m_featureTaskVec.size() > i; ++i)
		{
			if (static_cast<long>(list[i].type()) != m_featureTaskVec[i]->getFeatureType())
			{
				auto iter = std::find_if(m_featureTaskVec.begin(), m_featureTaskVec.end(), [list, i](const auto* pTask) { return static_cast<long>(list[i].type()) == pTask->getFeatureType(); });
				if (m_featureTaskVec.end() != iter)
				{	// move to correct position
					auto* pTask = *iter;
					auto* pTaskAfter = m_featureTaskVec[i];
					m_featureTaskVec.erase(iter);
					m_featureTaskVec.insert(m_featureTaskVec.begin() + i, pTask);
					auto taskIter = std::find(m_TaskObjectVector.begin(), m_TaskObjectVector.end(), pTask);
					if (m_TaskObjectVector.end() != taskIter)
					{
						m_TaskObjectVector.erase(taskIter);
					}
					auto newIter = std::find(m_TaskObjectVector.begin(), m_TaskObjectVector.end(), pTaskAfter);
					if (m_TaskObjectVector.end() != newIter)
					{
						m_TaskObjectVector.insert(newIter, pTask);
					}
					else
					{
						m_TaskObjectVector.push_back(pTask);
						assert(false);
					}
				}
				else
				{	//insert new feature at correct position
					auto* pNewTask = new BlobFeatureTask(this, g_featureDataMap[list[i].type()].m_stringResourceID);
					pNewTask->setFeatureType(static_cast<long>(list[i].type()));
					InsertBefore(m_featureTaskVec[i]->getObjectId(), pNewTask);
					m_featureTaskVec.insert(m_featureTaskVec.begin()+i, pNewTask);
					if (pNewTask && !pNewTask->is_Created())
					{
						CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
					}
				}
			}
			//set values
			int retVal = m_featureTaskVec[i]->setFeatureData(list[i]);
			if (0 < retVal)
			{
				auto* pError = pResponse->add_error_list();
				pError->set_row(i + 1);
				pError->set_fieldid(retVal);
			}
		}

		// add new feature
		for (int i = static_cast<int>(m_featureTaskVec.size()); i < list.size(); ++i)
		{
			auto* pNewTask = new BlobFeatureTask(this, g_featureDataMap[list[i].type()].m_stringResourceID);
			pNewTask->setFeatureType(static_cast<long>(list[i].type()));
			Add(pNewTask);
			m_featureTaskVec.push_back(pNewTask);
			if (pNewTask && !pNewTask->is_Created())
			{
				CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
			}
			int retVal = m_featureTaskVec[i]->setFeatureData(list[i]);
			if (0 < retVal)
			{
				auto* pError = pResponse->add_error_list();
				pError->set_row(i + 1);
				pError->set_fieldid(retVal);
			}
		}
		
		//delete not longer needed tasks.
		while (list.size() < m_featureTaskVec.size())
		{
			auto* pTask = m_featureTaskVec[list.size()];
			m_featureTaskVec.erase(m_featureTaskVec.begin() + list.size());
			RemoveChild(pTask);
		}

		if (0 == pResponse->error_list_size())
		{
			SvStl::MessageContainerVector errorMessages;
			ResetObject(&errorMessages);
			for (const auto& rMessage : errorMessages)
			{
				if (SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound == rMessage.getMessage().m_AdditionalTextId && 0 < rMessage.getMessage().m_AdditionalTextList.size())
				{
					try
					{
						int row = std::stoi(rMessage.getMessage().m_AdditionalTextList[0]);
						auto* pError = pResponse->add_error_list();
						pError->set_row(row);
						pError->set_fieldid(SvPb::FeatureData::kLowerBoundIndirectFieldNumber);
						SvPb::setMessageToMessagePB(rMessage, pResponse->mutable_messages()->add_messages());
					}
					catch(...)
					{
						SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
						Msg.setMessage(rMessage.getMessage());
					}
				}
			}
		}
		
		return cmdResponse;
	}
#pragma endregion IBlobAnalyzer

	bool BlobAnalyzer2::CloseObject()
	{
		m_featureTaskVec.clear();
		m_ResultTableColumnVector.clear();
		SVImageAnalyzerClass::CloseObject();

		SVMatroxBlobInterface::DestroyResult(m_ResultBufferID);
		SVMatroxBlobInterface::DestroyContext(m_BlobContextID);

		return true;
	}


	bool BlobAnalyzer2::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool Result{ true };

		try
		{
			if (!SVImageAnalyzerClass::CreateObject(rCreateStructure))
			{
				SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16116, getObjectId());
				throw;
			}

			m_pResultTable = dynamic_cast<SvOp::TableObject*>(SvOi::FindObject(getObjectId(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::SVNotSetSubObjectType)));
			if (nullptr == m_pResultTable)
			{
				m_pResultTable = new SvOp::TableObject(this);
				Add(m_pResultTable);
				if (!CreateChildObject(m_pResultTable))
				{
					SvStl::MessageContainer message;
					message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
					Msg.setMessage(message.getMessage());
					throw;
				}
			}

			HRESULT MatroxCode = SVMatroxBlobInterface::CreateResult(m_ResultBufferID);

			if (S_OK != MatroxCode || M_NULL == m_ResultBufferID)
			{
				SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16117, getObjectId());
				throw;
			}

			MatroxCode = SVMatroxBlobInterface::CreateContext(m_BlobContextID);

			if (S_OK != MatroxCode || M_NULL == m_BlobContextID)
			{
				SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16148);
				throw;
			}

			updateBlobFeatures();
			allocateBlobNumberResult();

			SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
			if (nullptr != pInspection)
			{
				pInspection->SetDefaultInputs();
			}
		}
		catch (...)
		{
			Result = false;
		}


		if (!Result)
		{
			m_isCreated = false;
		}
		else
		{
			m_isCreated = true;
		}

		return m_isCreated;
	}

	bool BlobAnalyzer2::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
	{
		try
		{
			if (!__super::onRun(rRunStatus, pErrorMessages))
			{
				return false;
			}

			SvIe::SVImageClass* pInputImage = getInputImage(true);
			if (nullptr == pInputImage)
			{
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}
			SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
			if (nullptr == pImageBuffer)
			{
				assert(false);
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}
			if (pImageBuffer->isEmpty())
			{
				assert(false);
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}

			//get grayImageId if should use for blob calc
			MIL_ID grayImageId = M_NULL;
			BOOL isGrayValueUsed{ false };
			m_isGrayImageValue.GetValue(isGrayValueUsed);
			if (isGrayValueUsed && m_grayImageInfo.IsConnected())
			{
				SvIe::SVImageClass* pGrayImage = SvOl::getInput<SvIe::SVImageClass>(m_grayImageInfo, true);
				if (pGrayImage)
				{
					SvTrc::IImagePtr pGrayImageBuffer = pGrayImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());
					if (nullptr != pGrayImageBuffer && !pGrayImageBuffer->isEmpty())
					{
						grayImageId = pGrayImageBuffer->getHandle()->GetBuffer().GetIdentifier();
					}
					else
					{
						assert(false);
					}
				}
				else
				{
					assert(false);
				}
			}

			HRESULT MatroxCode = SVMatroxBlobInterface::Execute(m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), m_BlobContextID, grayImageId);
			if (S_OK != MatroxCode)
			{
				assert(false);
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}

			for (auto data : m_excludeDataVec)
			{
				double lowerBound = 0;
				data.m_rLowerBoundValue.getValue(lowerBound);
				double upperBound = 0;
				data.m_rUpperBoundValue.getValue(upperBound);
				if (lowerBound <= upperBound)
				{
					MblobSelect(m_ResultBufferID, M_EXCLUDE, data.m_featureType, data.m_isInner ? M_IN_RANGE : M_OUT_RANGE, lowerBound, upperBound);
				}
				else
				{
					auto row = std::distance(m_featureTaskVec.begin(), std::find_if(m_featureTaskVec.begin(), m_featureTaskVec.end(), [data](const auto& rEntry) { return data.m_featureType == rEntry->getFeatureType(); }));
					SvDef::StringVector msgList;
					msgList.push_back(std::to_string(row));
					SvStl::MessageMgrStd  Ex(SvStl::MsgType::Log);
					Ex.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				}
			}

			//
			// Get the number blobs found by MblobCalculate()
			//
			long numberOfBlobs = 0;
			MatroxCode = SVMatroxBlobInterface::GetNumber(m_ResultBufferID, numberOfBlobs);

			if (S_OK != MatroxCode)
			{
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}

			long maxBlobDataArraySize = 0;
			m_maxBlobDataArraySize.GetValue(maxBlobDataArraySize);

			m_numberOfBlobsFound.SetValue(numberOfBlobs);
			if (numberOfBlobs > maxBlobDataArraySize)
			{
				SvStl::MessageMgrStd msg(SvStl::MsgType::Data);
				msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Too_Many_Blobs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				msg.Throw();
			}

			m_pResultTable->setSortContainerDummy(SvVol::DummySortContainer(numberOfBlobs));
			if (0 < numberOfBlobs)
			{
				for (const auto& resultPair : m_ResultTableColumnVector)
				{
					std::vector<double> valueArray;
					MblobGetResult(m_ResultBufferID, M_INCLUDED_BLOBS, resultPair.first, valueArray);
					resultPair.second->SetArrayValues(valueArray);
				}
			}
			else
			{
				for (const auto& resultPair : m_ResultTableColumnVector)
				{
					resultPair.second->SetResultSize(0);
				}
			}

			//range check
			auto rangeState = checkRange();
			if (rangeState.first)
			{
				rRunStatus.SetFailed();
			}
			else if (rangeState.second)
			{
				rRunStatus.SetWarned();
			}
			else 
			{
				rRunStatus.SetPassed();
			}

			// Now fill the blobs
			BOOL isFillBlob;
			m_isFillBlobs.GetValue( isFillBlob );

			//MatroxCode = SVMatroxBlobInterface::GetNumber( m_ResultBufferID, numberOfBlobs);
			if ( isFillBlob )
			{
				byte color;
				long type;
				m_blobFillColor.GetValue(color);
				m_evoBlobType.GetValue(type);
				SVBlobControlEnum eCriterion = SVEBlobAll;
				switch( type )
				{
					case SV_BLOB_FILL_ALL: 
					{
						eCriterion = SVEBlobAll;
						break;
					}
					case SV_BLOB_FILL_EXCLUDED :
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

				MatroxCode = SVMatroxBlobInterface::BlobFill( m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), eCriterion, color);
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

	bool BlobAnalyzer2::IsPtOverResult( const POINT& rPoint )
	{
		long currentNbrOfBlobs = 0;
	
		m_nBlobIndex = -1;
	
		if ( S_OK == m_numberOfBlobsFound.GetValue(currentNbrOfBlobs) && 0 != currentNbrOfBlobs && 
			nullptr != m_ResultColumnForOverlayArray[0] && nullptr != m_ResultColumnForOverlayArray[1] && nullptr != m_ResultColumnForOverlayArray[2] && nullptr != m_ResultColumnForOverlayArray[3])
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
			if (nullptr != pTool)
			{
				std::vector<double> minXArray;
				m_ResultColumnForOverlayArray[0]->GetArrayValues(minXArray);
				std::vector<double> maxXArray;
				m_ResultColumnForOverlayArray[1]->GetArrayValues(maxXArray);
				std::vector<double> minYArray;
				m_ResultColumnForOverlayArray[2]->GetArrayValues(minYArray);
				std::vector<double> maxYArray;
				m_ResultColumnForOverlayArray[3]->GetArrayValues(maxYArray);
	
				size_t number = std::min({ static_cast<size_t>(currentNbrOfBlobs), minXArray.size(), maxXArray.size(), minYArray.size(), maxYArray.size() });
				
				for (int i = 0; i < number; i++)
				{
					RECT l_oRect;
					l_oRect.top = static_cast<long>(minYArray[i]);
					l_oRect.left = static_cast<long>(minXArray[i]);
					l_oRect.bottom = static_cast<long>(maxYArray[i]);
					l_oRect.right = static_cast<long>(maxXArray[i]);
					
					SVExtentFigureStruct l_svFigure{ l_oRect };
					pTool->GetImageExtent().TranslateFromOutputSpace( l_svFigure, l_svFigure );
	
					if( S_OK == l_svFigure.IsPointOverFigure( SVPoint<double>(rPoint) ) )
					{
						m_nBlobIndex = i; 	
						break;
					}
				}
			}
		}
	
		return ( m_nBlobIndex != -1 );
	}

	bool BlobAnalyzer2::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
	{
		bool result = __super::ResetObject(pErrorMessages);

		if (nullptr == m_pResultBlob)
		{
			result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(Msg);
			}
		}

		result &= updateBlobFeatures(pErrorMessages);

		std::array<MIL_ID, 3> sortIdArray = { M_SORT1, M_SORT2, M_SORT3 };
		std::array<MIL_ID, 3> sortDirectionIdArray = { M_SORT1_DIRECTION, M_SORT2_DIRECTION, M_SORT3_DIRECTION };
		for (int i = 0; 3 < i; ++i)
		{
			SortEnum sortValue = SortEnum::None;
			if (i < m_featureTaskVec.size())
			{
				sortValue = m_featureTaskVec[i]->getSortEnum();
			}
			switch (sortValue)
			{
			case SortEnum::Ascent:
				MblobControl(m_BlobContextID, sortIdArray[i], m_featureTaskVec[i]->getFeatureType());
				MblobControl(m_BlobContextID, sortDirectionIdArray[i], M_SORT_UP);
				break;
			case SortEnum::Descent:
				MblobControl(m_BlobContextID, sortIdArray[i], m_featureTaskVec[i]->getFeatureType());
				MblobControl(m_BlobContextID, sortDirectionIdArray[i], M_SORT_DOWN);
				break;
			case SortEnum::None:
			default:
				MblobControl(m_BlobContextID, sortIdArray[i], M_NO_SORT);
				break;
			}
		}

		SvOl::ValidateInput(m_grayImageInfo);
		BOOL isGrayValueUsed{ false };
		m_isGrayImageValue.GetValue(isGrayValueUsed);
		if (isGrayValueUsed)
		{
			if (nullptr == SvOl::getInput<SvIe::SVImageClass>(m_grayImageInfo))
			{
				result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}

		long connectivityBlobEnum;
		m_connectivityEnumValue.GetValue(connectivityBlobEnum);
		SVMatroxBlobInterface::Set(m_BlobContextID, M_CONNECTIVITY, (static_cast<long>(BlobConnectivityEnum::NEIGHBORS_4) == connectivityBlobEnum) ? M_4_CONNECTED : M_8_CONNECTED);

		long colorBlobEnum;
		m_colorBlobEnumValue.GetValue(colorBlobEnum);
		SVMatroxBlobInterface::SetForeground(m_BlobContextID, BlobColorEnum::SV_BLOB_BLACK == static_cast<BlobColorEnum>(colorBlobEnum));

		return result;
	}

	HRESULT BlobAnalyzer2::onCollectOverlays(SvIe::SVImageClass*, SVExtentMultiLineStructVector& rMultiLineArray)
	{
		// only if ToolSet/Tool was not Disabled
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
		if (pTool && pTool->WasEnabled())
		{
			if (nullptr == m_ResultColumnForOverlayArray[0] || nullptr == m_ResultColumnForOverlayArray[1] || nullptr == m_ResultColumnForOverlayArray[2] || nullptr == m_ResultColumnForOverlayArray[3])
			{
				assert(false);
				return E_FAIL;
			}
			std::vector<double> minXArray;
			m_ResultColumnForOverlayArray[0]->GetArrayValues(minXArray);
			std::vector<double> maxXArray;
			m_ResultColumnForOverlayArray[1]->GetArrayValues(maxXArray);
			std::vector<double> minYArray;
			m_ResultColumnForOverlayArray[2]->GetArrayValues(minYArray);
			std::vector<double> maxYArray;
			m_ResultColumnForOverlayArray[3]->GetArrayValues(maxYArray);

			size_t numberOfBlobs = std::min({ minXArray.size(), maxXArray.size(), minYArray.size(), maxYArray.size() });
			const SVImageExtentClass& rImageExtents = pTool->GetImageExtent();

			// if running only show N Blob Figures according to the specified
			// MaxBlobDataArraySize variable
			if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && 1 < numberOfBlobs)
			{
				numberOfBlobs = 1;
			}

			for (size_t i = 0; i < numberOfBlobs; ++i)
			{
				RECT l_oRect;
				l_oRect.top = static_cast<long> (minYArray[i]);
				l_oRect.left = static_cast<long> (minXArray[i]);
				l_oRect.bottom = static_cast<long> (maxYArray[i]+1);
				l_oRect.right = static_cast<long> (maxXArray[i]+1);

				SVExtentFigureStruct l_svFigure{ l_oRect };
				rImageExtents.TranslateFromOutputSpace(l_svFigure, l_svFigure);

				SVExtentMultiLineStruct l_multiLine;
				l_multiLine.m_Color = SvDef::DefaultSubFunctionColor1;

				l_multiLine.AssignExtentFigure(l_svFigure, SvDef::DefaultSubFunctionColor1);

				UpdateOverlayIDs(l_multiLine);

				rMultiLineArray.push_back(l_multiLine);
			}
			
		}
		return S_OK;
	}

	void BlobAnalyzer2::addOverlayGroups(const SvIe::SVImageClass*, SvPb::Overlay& rOverlay) const
	{
		auto* pGroup = rOverlay.add_shapegroups();
		pGroup->set_name("Blobs");
		pGroup->set_detaillevel(SvPb::Level1);
		auto* pShape = pGroup->add_shapes();
		pShape->mutable_color()->set_value(SvDef::DefaultSubFunctionColor1);
		auto* pRectArray = pShape->mutable_rectarray()->mutable_x12data();
		pRectArray->set_x1trpos(m_ResultColumnForOverlayArray[0]->getTrPos() + 1);
		pRectArray->set_x2trpos(m_ResultColumnForOverlayArray[1]->getTrPos() + 1);
		pRectArray->set_y1trpos(m_ResultColumnForOverlayArray[2]->getTrPos() + 1);
		pRectArray->set_y2trpos(m_ResultColumnForOverlayArray[3]->getTrPos() + 1);
	}

	SvDef::StringVector BlobAnalyzer2::getAnalyzerResult()
	{
		SvDef::StringVector result;
		long currentNbrOfBlobs = 0;
		m_numberOfBlobsFound.GetValue(currentNbrOfBlobs);
		if (m_nBlobIndex >= 0 && m_nBlobIndex < currentNbrOfBlobs)
		{
			for (const auto& rValue : m_ResultTableColumnVector)
			{
				if (rValue.second->getSortContainerSize() > m_nBlobIndex)
				{
					double value = 0;
					rValue.second->getValue(value, m_nBlobIndex);
					result.emplace_back(SvUl::Format(_T("%s\t%5.1f"), rValue.second->GetName(), value));
				}
			}
	
		}
		return result;
	}

	bool BlobAnalyzer2::updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool result = true;
		fillFeatureTaskVec();
		addedMissingNeededFeatureTask();

		m_excludeDataVec.clear();
		for (auto* pFeatureTask : m_featureTaskVec)
		{
			auto excludeData = pFeatureTask->getExcludeData();
			if (excludeData.m_isEnabled)
			{
				std::string lowerStr, upperStr;
				excludeData.m_rLowerBoundValue.getLinkedName().getValue(lowerStr);
				excludeData.m_rUpperBoundValue.getLinkedName().getValue(upperStr);
				if (lowerStr.empty() && upperStr.empty())
				{ //check if lower < upper
					double lower, upper;
					excludeData.m_rLowerBoundValue.getValue(lower);
					excludeData.m_rUpperBoundValue.getValue(upper);
					if (upper < lower)
					{
						result = false;
						if (nullptr != pErrorMessages)
						{
							auto row = std::distance(m_featureTaskVec.begin(), std::find(m_featureTaskVec.begin(), m_featureTaskVec.end(), pFeatureTask))+1;
							SvDef::StringVector msgList;
							msgList.push_back(std::to_string(row));
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
							pErrorMessages->push_back(Msg);
						}
					}
				}
				m_excludeDataVec.emplace_back(excludeData);
			}
		}

		auto featureGroups = updateTableDefinition();

		m_rangeDataVec.clear();
		for (auto* pFeatureTask : m_featureTaskVec)
		{
			auto rangeData = pFeatureTask->getRangeData();
			if (rangeData.m_isEnabled)
			{
				auto iter = std::find_if(m_ResultTableColumnVector.begin(), m_ResultTableColumnVector.end(), [rangeData](const auto& rEntry) { return rangeData.m_featureType == rEntry.first; });
				if (m_ResultTableColumnVector.end() != iter)
				{
					m_rangeDataVec.emplace_back(iter->second, rangeData);
				}
				else
				{
					assert(false);
				}
			}
		}

		MblobControl(m_BlobContextID, M_ALL_FEATURES, M_DISABLE);
		for (auto group : featureGroups)
		{
			MblobControl(m_BlobContextID, group, M_ENABLE);
		}
		return result;
	}

	void BlobAnalyzer2::fillFeatureTaskVec()
	{
		m_featureTaskVec.clear();
		for (auto* pTask : m_TaskObjectVector)
		{
			auto* pFeatureTask = dynamic_cast<BlobFeatureTask*>(pTask);
			if (nullptr != pFeatureTask)
			{
				m_featureTaskVec.push_back(pFeatureTask);
			}
		}
	}

	void BlobAnalyzer2::addedMissingNeededFeatureTask()
	{
		std::set<MIL_ID> neededFeatures{ g_necessaryFeatures };
		for (auto* pTask : m_featureTaskVec)
		{
			//pointer in m_featureTaskVec always != nullptr
			neededFeatures.erase(pTask->getFeatureType());
		}
		for (MIL_ID id : neededFeatures)
		{
			assert(g_featureDataMap.end() != g_featureDataMap.find(id));
			auto* pNewTask = new BlobFeatureTask(this, g_featureDataMap[id].m_stringResourceID);
			pNewTask->setFeatureType(static_cast<long>(id));
			Add(pNewTask);
			if (pNewTask && !pNewTask->is_Created())
			{
				CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
			}
			m_featureTaskVec.push_back(pNewTask);
		}
	}

	std::set<MIL_ID> BlobAnalyzer2::updateTableDefinition()
	{
		for (auto i = m_featureTaskVec.size(); m_ResultTableColumnVector.size() > i; ++i)
		{
			m_pResultTable->removeColumn(SvPb::TableColumnValueEId + i);
		}
		m_ResultTableColumnVector.resize(m_featureTaskVec.size(), { M_NULL, nullptr });

		long maxBlobDataArraySize = 0;
		m_maxBlobDataArraySize.GetValue(maxBlobDataArraySize);
		std::set<MIL_ID> featureGroups;
		for (int i = 0; m_featureTaskVec.size() > i; ++i)
		{
			MIL_ID type = m_featureTaskVec[i]->getFeatureType();
			auto iter = g_ResultColumPosMap.find(type);
			assert(g_featureDataMap.end() != g_featureDataMap.find(type));
			m_ResultTableColumnVector[i] = { type, m_pResultTable->updateOrCreateColumn(SvPb::TableColumnValueEId + i, g_featureDataMap[type].m_stringResourceID, maxBlobDataArraySize) };
			if (g_ResultColumPosMap.end() != iter)
			{
				m_ResultColumnForOverlayArray[iter->second] = m_ResultTableColumnVector[i].second;
			}
			MIL_ID group = g_featureDataMap[type].m_FeatureGroup;
			if (M_NULL != group)
			{
				featureGroups.emplace(group);
			}
		}
		return featureGroups;
	}

	void BlobAnalyzer2::allocateBlobNumberResult()
	{
		SvDef::SVObjectTypeInfoStruct  interfaceInfo;
		interfaceInfo.m_EmbeddedID = m_numberOfBlobsFound.GetEmbeddedID();
		SvIe::SVClassInfoStruct       resultClassInfo;
		resultClassInfo.m_DesiredInputVector.push_back(interfaceInfo);

		resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultLongObjectType;
		resultClassInfo.m_ClassId = SvPb::LongResultClassId;
		resultClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT);
		std::string Title = m_numberOfBlobsFound.GetName();
		resultClassInfo.m_ClassName += _T(" ") + Title;

		// Construct the result class
		m_pResultBlob = dynamic_cast<SvOp::SVLongResultClass*> (resultClassInfo.Construct());

		if (nullptr == m_pResultBlob)
		{
			SvStl::MessageMgrStd  Ex(SvStl::MsgType::Log);
			Ex.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			return;
		}

		Add(m_pResultBlob);

		m_pResultBlob->ConnectAllInputs();

		// And last - Create (initialize) it

		if (!m_pResultBlob->IsCreated())
		{
			// And finally try to create the child object...
			if (!CreateChildObject(m_pResultBlob))
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());

				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				uint32_t objectID = m_pResultBlob->getObjectId();
				if (SvDef::InvalidObjectId != objectID)
				{
					Delete(objectID);
				}
				else
				{
					delete m_pResultBlob;
				}
				m_pResultBlob = nullptr;
			}
		}
	}

	std::pair<bool, bool> BlobAnalyzer2::checkRange()
	{
		bool isWarn = false;
		for (auto& rData : m_rangeDataVec)
		{
			std::vector<double> values;
			rData.first->GetArrayValues(values);
			for (auto value : values)
			{
				if (rData.second.m_failHigh < value || rData.second.m_failLow > value)
				{
					return { true, false };
				}
				if (!isWarn && (rData.second.m_warnHigh < value || rData.second.m_warnLow > value))
				{
					isWarn = true;
				}
			}
		}
		return { false, isWarn };
	}
} //namespace SvAo

namespace SvOi
{
	SvPb::InspectionCmdResponse getAvailableFeatures()
	{
		static SvPb::InspectionCmdResponse response;
		if (!response.has_getavailablefeaturesresponse())
		{
			auto* list = response.mutable_getavailablefeaturesresponse();
			for (const auto& rData : SvAo::g_featureDataMap)
			{
				auto* pData = list->add_list();
				pData->set_id(static_cast<uint32_t>(rData.first));
				pData->set_name(SvUl::LoadStdString(rData.second.m_stringResourceID));
				bool isN = SvAo::g_necessaryFeatures.end() != std::find(SvAo::g_necessaryFeatures.begin(), SvAo::g_necessaryFeatures.end(), rData.first);
				pData->set_isnecessary(isN);
				pData->set_isgray(rData.second.m_isGrayValue);
			}
		}
		return response;
	}
}