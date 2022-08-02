//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobFeatureList.cpp
/// This is the class for the Blob Analyzer 2 - Feature List
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobFeatureList.h"
#include "BlobFeatureTask.h"
#include "Operators/TableObject.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Operators/SVResultLong.h"
#include "SVProtoBuf/ConverterHelper.h"
#include <iterator>
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
		{ M_FERET_GENERAL,				BlobFeatureData{ IDS_OBJECTNAME_FERET_GENERAL, M_FERET_GENERAL, false } },
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

	const std::initializer_list<MIL_ID> g_necessaryFeatures{ M_AREA, M_BOX_X_MIN, M_BOX_X_MAX, M_BOX_Y_MIN, M_BOX_Y_MAX };
	static std::map<MIL_ID, int> g_ResultColumPosMap = { { M_BOX_X_MIN, 0 }, { M_BOX_X_MAX, 1 }, { M_BOX_Y_MIN, 2 }, { M_BOX_Y_MAX, 3 } };
#pragma endregion Declarations

	SV_IMPLEMENT_CLASS(BlobFeatureList, SvPb::BlobFeatureListClassId);

	BlobFeatureList::BlobFeatureList(SVObjectClass* POwner, int StringResourceID)
		: SVTaskObjectListClass(POwner, StringResourceID)
	{
		init();
	}

	void BlobFeatureList::init()
	{
		//Indentify our output type.
		m_ObjectTypeInfo.m_ObjectType = SvPb::BlobFeatureListObjectType;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	BlobFeatureList::~BlobFeatureList()
	{
		BlobFeatureList::CloseObject();
	}

	SvPb::InspectionCmdResponse BlobFeatureList::getFeaturesData() const
	{
		SvPb::InspectionCmdResponse cmdResponse;
		SvPb::GetFeaturesResponse* pResponse = cmdResponse.mutable_getfeaturesresponse();
		for (const auto* pTask : m_featureTaskVec)
		{
			auto pFeatureData = pResponse->add_list();
			pFeatureData->set_type(pTask->getFeatureType());
			if (pTask->isCustomFeature())
			{
				pFeatureData->set_is_custom(true);
				pFeatureData->set_equationid(pTask->getEquationId());
			}
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
			pFeatureData->set_lower_bound_id(excludeData.m_rLowerBoundValue.getObjectId());
			auto* pValue = pFeatureData->mutable_lower_bound();
			excludeData.m_rLowerBoundValue.fillLinkedData(*pValue);
			pFeatureData->set_upper_bound_id(excludeData.m_rUpperBoundValue.getObjectId());
			pValue = pFeatureData->mutable_upper_bound();
			excludeData.m_rUpperBoundValue.fillLinkedData(*pValue);

			const auto& rangeData = pTask->getRangeData();
			pFeatureData->set_is_range(rangeData.m_isEnabled);
			pFeatureData->set_range_fail_high_id(rangeData.m_rRangeValues[RangeEnum::ER_FailHigh].getObjectId());
			pValue = pFeatureData->mutable_range_fail_high();
			rangeData.m_rRangeValues[RangeEnum::ER_FailHigh].fillLinkedData(*pValue);
			pFeatureData->set_range_warn_high_id(rangeData.m_rRangeValues[RangeEnum::ER_WarnHigh].getObjectId());
			pValue = pFeatureData->mutable_range_warn_high();
			rangeData.m_rRangeValues[RangeEnum::ER_WarnHigh].fillLinkedData(*pValue);
			pFeatureData->set_range_warn_low_id(rangeData.m_rRangeValues[RangeEnum::ER_WarnLow].getObjectId());
			pValue = pFeatureData->mutable_range_warn_low();
			rangeData.m_rRangeValues[RangeEnum::ER_WarnLow].fillLinkedData(*pValue);
			pFeatureData->set_range_fail_low_id(rangeData.m_rRangeValues[RangeEnum::ER_FailLow].getObjectId());
			pValue = pFeatureData->mutable_range_fail_low();
			rangeData.m_rRangeValues[RangeEnum::ER_FailLow].fillLinkedData(*pValue);
		}
		return cmdResponse;
	}

	SvPb::InspectionCmdResponse BlobFeatureList::setFeatures(SvPb::SetFeaturesRequest request, bool isGrayValid)
	{
		auto list = request.list();

		//If gray Image not set, remove the gray feature.
		if (false == isGrayValid)
		{
			auto iter = std::remove_if(list.begin(), list.end(), [](const auto& rEntry) {return rEntry.is_gray(); });
			if (list.end() != iter)
			{
				list.erase(iter, list.end());
				SvStl::MessageManager MesMan(SvStl::MsgType::Log);
				MesMan.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RemoveGrayFeature, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			}
		}

		SvPb::InspectionCmdResponse cmdResponse;
		SvPb::SetFeaturesResponse* pResponse = cmdResponse.mutable_setfeaturesresponse();
		int lastMilFeaturePos = 0;
		for (int i = 0; list.size() > i && m_featureTaskVec.size() > i; ++i)
		{
			if (static_cast<long>(list[i].type()) != m_featureTaskVec[i]->getFeatureType() || list[i].is_custom() != m_featureTaskVec[i]->isCustomFeature())
			{
				auto iter = std::find_if(m_featureTaskVec.begin(), m_featureTaskVec.end(), [list, i](const auto* pTask) { return static_cast<long>(list[i].type()) == pTask->getFeatureType() && list[i].is_custom() == pTask->isCustomFeature(); });
				if (m_featureTaskVec.end() != iter)
				{	// move to correct position
					auto pos = std::distance(m_featureTaskVec.begin(), iter);
					moveFeatureTasks(pos, i);
					m_pResultTable->MoveValueColumn(static_cast<int>(pos), i);
				}
				else
				{	//insert new feature at correct position
					auto* pNewTask = new BlobFeatureTask(this, list[i].is_custom() ? 0 : g_featureDataMap[list[i].type()].m_stringResourceID);
					pNewTask->setFeatureType(static_cast<long>(list[i].type()), list[i].is_custom());
					InsertBefore(m_featureTaskVec[i]->getObjectId(), pNewTask);
					m_featureTaskVec.insert(m_featureTaskVec.begin() + i, pNewTask);
					if (pNewTask && !pNewTask->is_Created())
					{
						CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
					}
					m_pResultTable->addColumnAtPos(i, list[i].name(), SvPb::TableColumnValueEId + m_pResultTable->getValueList().size());
				}
			}

			//Move mil-feature before custom feature
			auto newPos = i;
			if (!list[i].is_custom())
			{
				if (lastMilFeaturePos != i)
				{
					moveFeatureTasks(i, lastMilFeaturePos);
					m_pResultTable->MoveValueColumn(i, lastMilFeaturePos);
					newPos = lastMilFeaturePos;
				}
				++lastMilFeaturePos;
			}

			//set values
			int retVal = m_featureTaskVec[newPos]->setFeatureData(list[i]);
			if (0 < retVal)
			{
				auto* pError = pResponse->add_error_list();
				pError->set_row(newPos + 1);
				pError->set_fieldid(retVal);
			}
		}

		// add new feature
		for (int i = static_cast<int>(m_featureTaskVec.size()); i < list.size(); ++i)
		{
			auto* pNewTask = new BlobFeatureTask(this, list[i].is_custom() ? 0 : g_featureDataMap[list[i].type()].m_stringResourceID);
			Add(pNewTask);
			m_featureTaskVec.push_back(pNewTask);
			if (pNewTask && !pNewTask->is_Created())
			{
				CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
			}
			//must be after createChildObject, because for this method it must be set the tool-parameter
			pNewTask->setFeatureType(static_cast<long>(list[i].type()), list[i].is_custom());
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

		return cmdResponse;
	}

	bool BlobFeatureList::CloseObject()
	{
		m_customFeatureTaskExcludeVec.clear();
		for (auto* pTask : m_featureTaskVec)
		{
			pTask->setValueObject(nullptr);
		}
		m_featureTaskVec.clear();
		return __super::CloseObject();
	}


	bool BlobFeatureList::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		try
		{
			if (!__super::onRun(rRunStatus, pErrorMessages))
			{
				return false;
			}

			for (auto* pFeature : m_featureTaskVec)
			{
				assert(nullptr != pFeature);
				pFeature->fillValues(m_ResultBufferID);
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

	SvDef::StringVector BlobFeatureList::getAnalyzerResult(int blobIndex) const
	{
		SvDef::StringVector result;
		for (auto* pFeature : m_featureTaskVec)
		{
			auto pValue = pFeature->getValueObject();
			if (nullptr != pValue && pValue->getSortContainerSize() > blobIndex)
			{
				double value = 0;
				pValue->getValue(value, blobIndex);
				result.emplace_back(SvUl::Format(_T("%s\t%5.1f"), pValue->GetName(), value));
			}
		}
		return result;
	}

	void BlobFeatureList::fillFeatureTaskVec()
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

	void BlobFeatureList::addedMissingNeededFeatureTask()
	{
		std::set<MIL_ID> neededFeatures{ g_necessaryFeatures };
		for (auto* pTask : m_featureTaskVec)
		{
			//pointer in m_featureTaskVec always != nullptr
			if (!pTask->isCustomFeature())
			{
				neededFeatures.erase(pTask->getFeatureType());
			}
		}
		for (MIL_ID id : neededFeatures)
		{
			assert(g_featureDataMap.end() != g_featureDataMap.find(id));
			auto* pNewTask = new BlobFeatureTask(this, g_featureDataMap[id].m_stringResourceID);
			pNewTask->setFeatureType(static_cast<long>(id), false);
			Add(pNewTask);
			if (pNewTask && !pNewTask->is_Created())
			{
				CreateChildObject(*pNewTask, SvDef::SVMFResetObject);
			}
			m_featureTaskVec.push_back(pNewTask);
		}
	}

	void BlobFeatureList::updateTableDefinition(long maxArraySize, std::array<SvVol::DoubleSortValuePtr, 4>& resultColumnForOverlayArray, std::set<MIL_ID>& rRequiredFeatureGroup)
	{
		m_pResultTable->changeEIdinOrder(SvPb::TableColumnValueEId);
		auto currentCount = m_pResultTable->getValueList().size();
		for (auto i = m_featureTaskVec.size(); currentCount > i; ++i)
		{
			m_pResultTable->removeColumn(SvPb::TableColumnValueEId + i);
		}

		for (int i = 0; m_featureTaskVec.size() > i; ++i)
		{
			MIL_ID type = m_featureTaskVec[i]->getFeatureType();
			bool isCustomFeature = m_featureTaskVec[i]->isCustomFeature();
			if (isCustomFeature)
			{
				m_featureTaskVec[i]->setValueObject(m_pResultTable->updateOrCreateColumn(SvPb::TableColumnValueEId + i, m_featureTaskVec[i]->GetName(), maxArraySize));
			}
			else
			{
				assert(g_featureDataMap.end() != g_featureDataMap.find(type));
				m_featureTaskVec[i]->setValueObject(m_pResultTable->updateOrCreateColumn(SvPb::TableColumnValueEId + i, g_featureDataMap[type].m_stringResourceID, maxArraySize));
				auto iter = g_ResultColumPosMap.find(type);
				if (g_ResultColumPosMap.end() != iter)
				{
					resultColumnForOverlayArray[iter->second] = m_featureTaskVec[i]->getValueObject();
				}
				MIL_ID group = g_featureDataMap[type].m_FeatureGroup;
				if (M_NULL != group)
				{
					rRequiredFeatureGroup.emplace(group);
				}
			}
		}
	}

	bool BlobFeatureList::updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages, std::back_insert_iterator<std::vector<BlobExcludeData>> excludeDataVecInserter)
	{
		bool result = true;
		m_customFeatureTaskExcludeVec.clear();
		fillFeatureTaskVec();
		addedMissingNeededFeatureTask();

		//set exclude data
		for (auto* pFeatureTask : m_featureTaskVec)
		{
			auto excludeData = pFeatureTask->getExcludeData();
			if (excludeData.m_isEnabled)
			{
				if (SvPb::LinkedSelectedOption::DirectValue == excludeData.m_rLowerBoundValue.getSelectedOption() && SvPb::LinkedSelectedOption::DirectValue == excludeData.m_rUpperBoundValue.getSelectedOption())
				{ //check if lower < upper
					double lower, upper;
					excludeData.m_rLowerBoundValue.getValue(lower);
					excludeData.m_rUpperBoundValue.getValue(upper);
					if (upper < lower)
					{
						result = false;
						if (nullptr != pErrorMessages)
						{
							auto row = std::distance(m_featureTaskVec.begin(), std::find(m_featureTaskVec.begin(), m_featureTaskVec.end(), pFeatureTask)) + 1;
							SvDef::StringVector msgList;
							msgList.push_back(excludeData.m_name);
							msgList.push_back(std::to_string(row)); //the row number is not for display, but for getting the row in the error message.
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExcludeBlobUpperBoundLowerThanLowerBound, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
							pErrorMessages->push_back(Msg);
						}
					}
				}
				if (!pFeatureTask->isCustomFeature())
				{
					excludeDataVecInserter = excludeData;
				}
				else
				{
					m_customFeatureTaskExcludeVec.emplace_back(pFeatureTask);
				}
			}
		}

		//check range data
		for (auto* pFeatureTask : m_featureTaskVec)
		{
			auto rangeData = pFeatureTask->getRangeData();
			if (rangeData.m_isEnabled)
			{
				result &= rangeCheck(pErrorMessages, pFeatureTask, rangeData.m_rRangeValues[RangeEnum::ER_FailHigh], rangeData.m_rRangeValues[RangeEnum::ER_WarnHigh], SvStl::Tid_FailHigh, SvStl::Tid_WarnHigh);
				result &= rangeCheck(pErrorMessages, pFeatureTask, rangeData.m_rRangeValues[RangeEnum::ER_WarnHigh], rangeData.m_rRangeValues[RangeEnum::ER_WarnLow], SvStl::Tid_WarnHigh, SvStl::Tid_WarnLow);
				result &= rangeCheck(pErrorMessages, pFeatureTask, rangeData.m_rRangeValues[RangeEnum::ER_WarnLow], rangeData.m_rRangeValues[RangeEnum::ER_FailLow], SvStl::Tid_WarnLow, SvStl::Tid_FailLow);
			}
		}

		return result;
	}

	void BlobFeatureList::setSortControls(MIL_ID blobContextId) const
	{
		std::array<MIL_ID, SvDef::c_numberOfSortFeature> sortIdArray = { M_SORT1, M_SORT2, M_SORT3 };
		std::array<MIL_ID, SvDef::c_numberOfSortFeature> sortDirectionIdArray = { M_SORT1_DIRECTION, M_SORT2_DIRECTION, M_SORT3_DIRECTION };
		for (int i = 0; SvDef::c_numberOfSortFeature > i; ++i)
		{
			SortEnum sortValue = SortEnum::None;
			if (i < m_featureTaskVec.size())
			{
				sortValue = m_featureTaskVec[i]->getSortEnum();
			}
			switch (sortValue)
			{
			case SortEnum::Ascent:
				MblobControl(blobContextId, sortIdArray[i], m_featureTaskVec[i]->getFeatureType());
				MblobControl(blobContextId, sortDirectionIdArray[i], M_SORT_UP);
				break;
			case SortEnum::Descent:
				MblobControl(blobContextId, sortIdArray[i], m_featureTaskVec[i]->getFeatureType());
				MblobControl(blobContextId, sortDirectionIdArray[i], M_SORT_DOWN);
				break;
			case SortEnum::None:
			default:
				MblobControl(blobContextId, sortIdArray[i], M_NO_SORT);
				break;
			}
		}
	}

	bool BlobFeatureList::Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
	{
		bool retVal = __super::Run(rRunStatus, pErrorMessages);

		//exclude check only for custom features (for mil-feature is already done by mil)
		std::set<int> removeSet;
		for (auto* pFeatureTask : m_customFeatureTaskExcludeVec)
		{
			pFeatureTask->addExcludeBlobs(removeSet);
		}
		if (0 < removeSet.size())
		{
			auto container = m_pResultTable->getSortContainer();
			std::vector<double> valueArray;
			valueArray.resize(container.size(), 0);
			MblobGetResult(m_ResultBufferID, M_INCLUDED_BLOBS, M_LABEL_VALUE, valueArray);
			for (auto it = removeSet.crbegin(); it != removeSet.crend(); ++it)
			{
				MblobSelect(m_ResultBufferID, M_EXCLUDE, M_LABEL_VALUE, M_EQUAL, valueArray[*it], M_NULL);
				container.erase(container.begin() + *it);
			}
			m_pResultTable->setSortContainer(container);
			//set number of blobs new
			if (nullptr != m_pNumberOfBlobsObject)
			{
				m_pNumberOfBlobsObject->SetValue(static_cast<long>(container.size()));
			}
		}

		//do range
		for (auto* pFeatureTask : m_featureTaskVec)
		{
			SvIe::RunStatus ChildRunStatus;
			pFeatureTask->evalRange(ChildRunStatus);
			rRunStatus.updateState(ChildRunStatus, false);
		}

		setStatus(rRunStatus);

		return retVal;
	}

	void BlobFeatureList::moveFeatureTasks(size_t posFrom, size_t posTo)
	{
		assert(posFrom > posTo);

		auto iterFromR = std::find(m_TaskObjectVector.rbegin(), m_TaskObjectVector.rend(), m_featureTaskVec[posFrom]);
		auto iterToR = std::find(m_TaskObjectVector.rbegin(), m_TaskObjectVector.rend(), m_featureTaskVec[posTo]);

		auto size = m_featureTaskVec.size();
		std::rotate(m_featureTaskVec.rbegin() + (size - posFrom - 1), m_featureTaskVec.rbegin() + (size - posFrom), m_featureTaskVec.rbegin() + (size - posTo));
		if (m_TaskObjectVector.rend() != iterToR)
		{
			++iterToR;
		}
		std::rotate(iterFromR, iterFromR + 1, iterToR);
	}

	bool BlobFeatureList::rangeCheck(SvStl::MessageContainerVector* pErrorMessages, const BlobFeatureTask* pFeatureTask, const SvVol::LinkedValue& rHigherValue, const SvVol::LinkedValue& rLowerValue, SvStl::MessageTextEnum higherId, SvStl::MessageTextEnum lowerId)
	{
		if (SvPb::LinkedSelectedOption::DirectValue == rHigherValue.getSelectedOption() && SvPb::LinkedSelectedOption::DirectValue == rLowerValue.getSelectedOption())
		{ //check if lower < upper
			double higherValue, lowerValue;
			rHigherValue.getValue(higherValue);
			rLowerValue.getValue(lowerValue);
			if (higherValue < lowerValue)
			{
				if (nullptr != pErrorMessages)
				{
					auto row = std::distance(m_featureTaskVec.begin(), std::find(m_featureTaskVec.begin(), m_featureTaskVec.end(), pFeatureTask)) + 1;
					SvDef::StringVector msgList;
					msgList.push_back(pFeatureTask->GetName());
					msgList.push_back(SvStl::MessageData::convertId2AdditionalText(higherId));
					msgList.push_back(SvStl::MessageData::convertId2AdditionalText(lowerId));
					msgList.push_back(std::to_string(row)); //the row number is not for display, but for getting the row in the error message.
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeBlobUpperBoundLowerThanLowerBound, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
		return true;
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