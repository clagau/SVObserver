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
#include "StdAfx.h"
#include "SVShiftToolUtility.h"
#include "SVShiftTool.h"
#include "InspectionEngine/SVTaskObjectValueInterface.h"

void SVShiftToolUtility::SetToolNormalize(SVShiftTool* pShiftTool)
{
	_variant_t Value;
	long l_TranslationX = 0;
	long l_TranslationY = 0;

	SVTaskObjectValueInterface l_Interface( pShiftTool );

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, Value );

	l_TranslationX = Value.lVal;

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, Value );

	l_TranslationY = Value.lVal;

	HRESULT l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, l_TranslationX );

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, l_TranslationY );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequestMarker();
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.RunOnce(pShiftTool->GetUniqueObjectID());
	}
}

void SVShiftToolUtility::SetToolSetReference(SVShiftTool *pShiftTool)
{
	_variant_t Value;
	long Left( 0L );
	long Top( 0L );
	long TranslationX( 0L );
	long TranslationY( 0L );

	SVTaskObjectValueInterface l_Interface( pShiftTool );

	// Normalize Extents
	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVExtentRelativeLeftPositionObjectGuid, Value );

	Left = static_cast<long> (Value);

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVExtentRelativeTopPositionObjectGuid, Value );

	Top = static_cast<long> (Value);

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVImageTransformDisplacementXGuid, Value );

	Left += static_cast<long> (Value);

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVImageTransformDisplacementYGuid, Value );

	Top += static_cast<long> (Value);

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, Value );

	TranslationX = static_cast<long> (Value);

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, Value );

	TranslationY = static_cast<long> (Value);

	HRESULT l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVExtentRelativeLeftPositionObjectGuid, Left );

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVExtentRelativeTopPositionObjectGuid, Top );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, TranslationX );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, TranslationY );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequestMarker();
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.RunOnce(pShiftTool->GetUniqueObjectID());
	}
}


