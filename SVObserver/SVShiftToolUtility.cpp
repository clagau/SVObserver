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
#include "SVShiftTool.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

void SVShiftToolUtility::SetToolNormalize(SVShiftTool* pShiftTool)
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

		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_Values{ SvOg::BoundValues{pShiftTool->GetInspection()->GetUniqueObjectID(), pShiftTool->GetUniqueObjectID()} };

		m_Values.Set<double>(SVShiftToolReferenceXObjectGuid, static_cast<double> (lTranslationX));
		m_Values.Set<double>(SVShiftToolReferenceYObjectGuid, static_cast<double> (lTranslationY));

		m_Values.Commit();
	}
}

void SVShiftToolUtility::SetToolSetReference(SVShiftTool *pShiftTool)
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

		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;

		Controller m_Values{ SvOg::BoundValues{ pShiftTool->GetInspection()->GetUniqueObjectID(), pShiftTool->GetUniqueObjectID() } };

		long lLeft = static_cast<long> (m_Values.Get<double>(SVExtentRelativeLeftPositionObjectGuid));
		long lTop = static_cast<long> (m_Values.Get<double>(SVExtentRelativeTopPositionObjectGuid));
		long lOffset = static_cast<long> (m_Values.Get<double>(SVImageTransformDisplacementXGuid));
		lLeft += lOffset;
		lOffset = static_cast<long> (m_Values.Get<double>(SVImageTransformDisplacementYGuid));
		lTop += lOffset;

		m_Values.Set<double>(SVExtentRelativeLeftPositionObjectGuid, static_cast<double> (lLeft));
		m_Values.Set<double>(SVExtentRelativeTopPositionObjectGuid, static_cast<double> (lTop));
		m_Values.Set<double>(SVShiftToolReferenceXObjectGuid, static_cast<double> (lTranslationX));
		m_Values.Set<double>(SVShiftToolReferenceYObjectGuid, static_cast<double> (lTranslationY));

		m_Values.Commit();
	}
}


