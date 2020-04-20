//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShiftToolUtility
//* .File Name       : $Workfile:   SVShiftToolUtility.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:03:34  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVShiftToolUtility.h"
#include "Tools/SVShiftTool.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

void SVShiftToolUtility::SetToolNormalize(SvTo::SVShiftTool* pShiftTool)
{
	if (nullptr != pShiftTool)
	{
		long lTranslationX{ 0L };
		long lTranslationY{ 0L };
		if (nullptr != pShiftTool->GetTranslationXInput())
		{
			double Value{ 0.0 };
			pShiftTool->GetTranslationXInput()->GetValue(Value);
			lTranslationX = static_cast<long> (Value);
		}
		if (nullptr != pShiftTool->GetTranslationYInput())
		{
			double Value{ 0.0 };
			pShiftTool->GetTranslationYInput()->GetValue(Value);
			lTranslationY = static_cast<long> (Value);
		}

		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller m_Values{ SvOg::BoundValues{pShiftTool->GetInspection()->getObjectId(), pShiftTool->getObjectId()} };
		m_Values.Init();

		m_Values.Set<double>(SvPb::ShiftToolReferenceXEId, static_cast<double> (lTranslationX));
		m_Values.Set<double>(SvPb::ShiftToolReferenceYEId, static_cast<double> (lTranslationY));

		m_Values.Commit();
	}
}

void SVShiftToolUtility::SetToolSetReference(SvTo::SVShiftTool *pShiftTool)
{
	if (nullptr != pShiftTool)
	{
		long lTranslationX{ 0L };
		long lTranslationY{ 0L };
		if (nullptr != pShiftTool->GetTranslationXInput())
		{
			double Value{ 0.0 };
			pShiftTool->GetTranslationXInput()->GetValue(Value);
			lTranslationX = static_cast<long> (Value);
		}
		if (nullptr != pShiftTool->GetTranslationYInput())
		{
			double Value{ 0.0 };
			pShiftTool->GetTranslationYInput()->GetValue(Value);
			lTranslationY = static_cast<long> (Value);
		}

		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller m_Values{ SvOg::BoundValues{ pShiftTool->GetInspection()->getObjectId(), pShiftTool->getObjectId() } };

		m_Values.Init();
		long lLeft = static_cast<long> (m_Values.Get<double>(SvPb::ExtentRelativeLeftPositionEId));
		long lTop = static_cast<long> (m_Values.Get<double>(SvPb::ExtentRelativeTopPositionEId));
		long lOffset = static_cast<long> (m_Values.Get<double>(SvPb::ImageTransformDisplacementXEId));
		lLeft += lOffset;
		lOffset = static_cast<long> (m_Values.Get<double>(SvPb::ImageTransformDisplacementYEId));
		lTop += lOffset;

		m_Values.Set<double>(SvPb::ExtentRelativeLeftPositionEId, static_cast<double> (lLeft));
		m_Values.Set<double>(SvPb::ExtentRelativeTopPositionEId, static_cast<double> (lTop));
		m_Values.Set<double>(SvPb::ShiftToolReferenceXEId, static_cast<double> (lTranslationX));
		m_Values.Set<double>(SvPb::ShiftToolReferenceYEId, static_cast<double> (lTranslationY));

		m_Values.Commit();
	}
}


