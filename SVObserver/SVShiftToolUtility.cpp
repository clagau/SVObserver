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

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, l_TranslationY );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
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

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVExtentRelativeTopPositionObjectGuid, l_Top );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceXObjectGuid, l_TranslationX );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequest( pShiftTool->GetUniqueObjectID(), SVShiftToolReferenceYObjectGuid, l_TranslationY );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Interface.RunOnce();
	}
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVShiftToolUtility.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:03:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Jan 2013 12:37:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/