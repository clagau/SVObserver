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
#include "SVTaskObjectValueInterface.h"

void SVShiftToolUtility::SetToolNormalize(SVShiftTool* pShiftTool)
{
	_variant_t l_Variant;
	long l_TranslationX = 0;
	long l_TranslationY = 0;

	SVTaskObjectValueInterface l_Interface( pShiftTool );

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, l_Variant.GetVARIANT() );

	l_TranslationX = l_Variant;

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, l_Variant.GetVARIANT() );

	l_TranslationY = l_Variant;

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
		l_hrOk = l_Interface.RunOnce();
	}
}

void SVShiftToolUtility::SetToolSetReference(SVShiftTool *pShiftTool)
{
	_variant_t l_Variant;
	long l_Left = 0;
	long l_Top = 0;
	long l_TranslationX = 0;
	long l_TranslationY = 0;

	SVTaskObjectValueInterface l_Interface( pShiftTool );

	// Normalize Extents
	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVExtentRelativeLeftPositionObjectGuid, l_Variant.GetVARIANT() );

	l_Left = l_Variant;

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVExtentRelativeTopPositionObjectGuid, l_Variant.GetVARIANT() );

	l_Top = l_Variant;

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVImageTransformDisplacementXGuid, l_Variant.GetVARIANT() );

	l_Left += static_cast< long >( l_Variant );

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVImageTransformDisplacementYGuid, l_Variant.GetVARIANT() );

	l_Top += static_cast< long >( l_Variant );

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationXResultObjectGuid, l_Variant.GetVARIANT() );

	l_TranslationX = l_Variant;

	l_Interface.GetValue( pShiftTool->GetUniqueObjectID(), SVOutputEvaluateTranslationYResultObjectGuid, l_Variant.GetVARIANT() );

	l_TranslationY = l_Variant;

	HRESULT l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVExtentRelativeLeftPositionObjectGuid, l_Left );

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVExtentRelativeTopPositionObjectGuid, l_Top );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, l_TranslationX );
	}

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
		l_hrOk = l_Interface.RunOnce();
	}
}


