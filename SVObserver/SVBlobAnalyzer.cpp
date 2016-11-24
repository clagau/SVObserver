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
#include "SVBlobAnalyzer.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVInspectionProcess.h"
#include "SVBlobAnalyzerDialog.h"
#include "SVGlobal.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVResultDouble.h"
#include "SVResultLong.h"
#include "SVRange.h"
#include "SVTool.h"
#include "SVSVIMStateClass.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

struct SVBlobFeatureConstants BlobFeatureConstants[]=
{
   SVEBlobArea, //M_AREA,
      &SVBlobAreaObjectGuid,
        IDS_OBJECTNAME_AREA,
   SVEBlobBoxXMax, //M_BOX_X_MAX,
      &SVBlobBoxXMaxObjectGuid,
        IDS_OBJECTNAME_BOXXMAX,
   SVEBlobBoxXMin, //M_BOX_X_MIN,
      &SVBlobBoxXMinObjectGuid,
        IDS_OBJECTNAME_BOXXMIN,
   SVEBlobBoxYMax, //M_BOX_Y_MAX,
      &SVBlobBoxYMaxObjectGuid,
      IDS_OBJECTNAME_BOXYMAX,
   SVEBlobBoxYMin, //M_BOX_Y_MIN,
      &SVBlobBoxYMinObjectGuid,
        IDS_OBJECTNAME_BOXYMIN,
   SVEBlobBreadth, // M_BREADTH,
      &SVBlobBreadthObjectGuid,
      IDS_OBJECTNAME_BREADTH,
   SVEBlobCenterOfGravityX, //M_CENTER_OF_GRAVITY_X,
      &SVBlobCenterOfGravityXObjectGuid,
      IDS_OBJECTNAME_CENTEROFGRAVITYX,
   SVEBlobCenterOfGravityY, //M_CENTER_OF_GRAVITY_Y,
      &SVBlobCenterOfGravityYObjectGuid,
      IDS_OBJECTNAME_CENTEROFGRAVITYY,
   SVEBlobConvexParameter, //M_CONVEX_PERIMETER,
      &SVBlobConvexPerimeterObjectGuid,
      IDS_OBJECTNAME_CONVEXPERIMETER,
   SVEBlobFeretElongation, //M_FERET_ELONGATION,
      &SVBlobFeretElongationObjectGuid,
      IDS_OBJECTNAME_FERETELONGATION,
   SVEBlobFeretMaxAngle, //M_FERET_MAX_ANGLE,
      &SVBlobFeretMaxAngleObjectGuid,
      IDS_OBJECTNAME_FERETMAXANGLE,
   SVEBlobFeretMaxDia, //M_FERET_MAX_DIAMETER,
      &SVBlobFeretMaxDiameterObjectGuid,
      IDS_OBJECTNAME_FERETMAXDIAMETER,
   SVEBlobFeretMeanDia, //M_FERET_MEAN_DIAMETER,
      &SVBlobFeretMeanDiameterObjectGuid,
      IDS_OBJECTNAME_FERETMEANDIAMETER,
   SVEBlobFeretMinAngle, //M_FERET_MIN_ANGLE,
      &SVBlobFeretMinAngleObjectGuid,
      IDS_OBJECTNAME_FERETMINANGLE,
   SVEBlobFeretMinDia, //M_FERET_MIN_DIAMETER,
      &SVBlobFeretMinDiameterObjectGuid,
        IDS_OBJECTNAME_FERETMINDIAMETER,
   SVEBlobFeretX, //M_FERET_X,
      &SVBlobFeretXObjectGuid,
      IDS_OBJECTNAME_FERETX,
   SVEBlobFeritY, //M_FERET_Y,
      &SVBlobFeretYObjectGuid,
      IDS_OBJECTNAME_FERETY,
   SVEBlobFirstPointX, //M_FIRST_POINT_X,
      &SVBlobFirstPointXObjectGuid,
      IDS_OBJECTNAME_FIRSTPOINTX,
   SVEBlobFirstPointY, //M_FIRST_POINT_Y,
      &SVBlobFirstPointYObjectGuid,
      IDS_OBJECTNAME_FIRSTPOINTY,
   SVEBlobLabelValue, //M_LABEL_VALUE,
      &SVBlobLabelObjectGuid,
      IDS_OBJECTNAME_LABEL,
   SVEBlobLength, //M_LENGTH,
      &SVBlobLengthObjectGuid,
      IDS_OBJECTNAME_LENGTH,
   SVEBlobNumberOfHoles, //M_NUMBER_OF_HOLES,
      &SVBlobNbrOfHolesObjectGuid,
      IDS_OBJECTNAME_NBROFHOLES,
   SVEBlobPerimeter, //M_PERIMETER,
      &SVBlobPerimeterObjectGuid,
      IDS_OBJECTNAME_PERIMETER,
   SVEBlobRoughness, //M_ROUGHNESS,
      &SVBlobRoughnessObjectGuid,
      IDS_OBJECTNAME_ROUGHNESS,
   SVEBlobSumPixel, //M_SUM_PIXEL,
      &SVBlobSumPixelObjectGuid,
        IDS_OBJECTNAME_SUMPIXEL,

	//
	// v3.1 Added Blob Features
	//
// The "chain" values do not seem to want to be excluded from the list based
// on their pass/fail criteria. And since I don't know what they do, I am 
// removing them. jab

	SVEBlobCompactness, //M_COMPACTNESS,
      &SVBlobCompactnessObjectGuid,
	   IDS_OBJECTNAME_BLOB_COMPACTNESS,
	SVEBlobNumberOfRuns, //M_NUMBER_OF_RUNS,
      &SVBlobNumberOfRunsObjectGuid,
	   IDS_OBJECTNAME_NUMBER_OF_RUNS,
	SVEBlobXMinAtYMin, //M_X_MIN_AT_Y_MIN,
      &SVBlobXMinAtYMinObjectGuid,
	   IDS_OBJECTNAME_XMIN_AT_YMIN,
	SVEBlobXMaxAtYMax, //M_X_MAX_AT_Y_MAX,
      &SVBlobXMaxAtYMaxObjectGuid,
	   IDS_OBJECTNAME_XMAX_AT_YMAX,
	SVEBlobYMinAtXMax, //M_Y_MIN_AT_X_MAX,
      &SVBlobYMinAtXMaxObjectGuid,
	   IDS_OBJECTNAME_YMIN_AT_XMAX,
	SVEBlobYMaxAtXMin, //M_Y_MAX_AT_X_MIN,
      &SVBlobYMaxAtXMinObjectGuid,
	   IDS_OBJECTNAME_YMAX_AT_XMIN,
	SVEBlobElongation, //M_ELONGATION,
      &SVBlobElongationObjectGuid,
	    IDS_OBJECTNAME_BLOB_ELONGATION,
	SVEBlobIntercept, //M_INTERCEPT_0,
      &SVBlobIntercept0ObjectGuid,
	   IDS_OBJECTNAME_BLOB_INTERCEPT0,
	SVEBlobIntercept45, //M_INTERCEPT_45,
      &SVBlobIntercept45ObjectGuid,
	   IDS_OBJECTNAME_BLOB_INTERCEPT45,
	SVEBlobIntercept90, //M_INTERCEPT_90,
      &SVBlobIntercept90ObjectGuid,
	   IDS_OBJECTNAME_BLOB_INTERCEPT90,
	SVEBlobIntercept135, //M_INTERCEPT_135,
      &SVBlobIntercept135ObjectGuid,
	   IDS_OBJECTNAME_BLOB_INTERCEPT135,
	SVEBlobMomentX0Y1, // M_MOMENT_X0_Y1,
      &SVBlobMomentX0Y1,
		IDS_OBJECTNAME_BLOB_MOMENT_X0Y1,
	SVEBlobMomentX1Y0, //M_MOMENT_X1_Y0,
      &SVBlobMomentX1Y0,
		IDS_OBJECTNAME_BLOB_MOMENT_X1Y0,
	SVEBlobMomentX1Y1, //M_MOMENT_X1_Y1,
      &SVBlobMomentX1Y1,
		IDS_OBJECTNAME_BLOB_MOMENT_X1Y1,
	SVEBlobMomentX0Y2, //M_MOMENT_X0_Y2,
      &SVBlobMomentX0Y2,
		IDS_OBJECTNAME_BLOB_MOMENT_X0Y2,
	SVEBlobMomentX2Y0, //M_MOMENT_X2_Y0,
      &SVBlobMomentX2Y0,
		IDS_OBJECTNAME_BLOB_MOMENT_X2Y0,
	SVEBlobMomentCentralX0Y2, // M_MOMENT_CENTRAL_X0_Y2,
      &SVBlobMomentCentralX0Y2,
		IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X0Y2,
	SVEBlobMomentCentralX2Y0, //M_MOMENT_CENTRAL_X2_Y0,
      &SVBlobMomentCentralX2Y0,
		IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X2Y0,
	SVEBlobMomentCentralX1Y1, //M_MOMENT_CENTRAL_X1_Y1,
      &SVBlobMomentCentralX1Y1,
		IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X1Y1,
	SVEBlobAxisPrincipalAngle, //M_AXIS_PRINCIPAL_ANGLE,
      &SVBlobAxisPrincipalAngle,
		IDS_OBJECTNAME_BLOB_AXIS_PRINCIPAL_ANGLE,
	SVEBlobAxisSecondaryAngle, //M_AXIS_SECONDARY_ANGLE,
      &SVBlobAxisSecondaryAngle,
		IDS_OBJECTNAME_BLOB_AXIS_SECONDARY_ANGLE,
    SVEBlobEulerNumber, //M_EULER_NUMBER,
      &SVBlobEulerNumber,
        IDS_OBJECTNAME_BLOB_EULER_NUMBER,
	SV_M_CENTER_BOUNDING_BOX_X,
	  &SVBlobCenterOfBoundingBoxXGuid,
		IDS_OBJECTNAME_CENTER_X_SOURCE,
	SV_M_CENTER_BOUNDING_BOX_Y,
	  &SVBlobCenterOfBoundingBoxYGuid,
		IDS_OBJECTNAME_CENTER_Y_SOURCE
};

SV_IMPLEMENT_CLASS( SVBlobAnalyzerClass, SVBlobAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
//
//
SVBlobAnalyzerClass::SVBlobAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass(FALSE, POwner, StringResourceID ) 
{
	init();
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVBlobAnalyzerClass::init()
{
	SVBlobFeatureEnum i;


	msvlDefaultAttributes = 0;
	m_lNumberOfBlobsFound = 0;
	m_lNumberOfBlobsToProcess = 0;
	m_pResultBlob = nullptr;

	//Indentify our output type.
	m_outObjectInfo.ObjectTypeInfo.SubType = SVBlobAnalyzerObjectType;

	//Register embedded objects.
	RegisterEmbeddedObject(
		&msvPersistantFeaturesEnabled, 
		SVBlobEnabledFeaturesObjectGuid,
		IDS_OBJECTNAME_ENABLEDFEATURES,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&m_lvoBlobSampleSize, 
		SVNbrOfBlobsObjectGuid,
		IDS_OBJECTNAME_NBROFBLOBS,
		false, SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_lvoMaxBlobDataArraySize, 
		SVMaxBlobDataArraySizeObjectGuid,
		IDS_OBJECTNAME_MAX_BLOB_DATA_ARRAY_SIZE,
		false, SVResetItemOwner );

	RegisterEmbeddedObject(
		&msvSortFeature, 
		SVSortFeatureObjectGuid,
		IDS_OBJECTNAME_SORTFEATURE,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&m_lvoNumberOfBlobsFound, 
		SVNbrOfBlobsFoundObjectGuid,
		IDS_OBJECTNAME_NBROFBLOBSFOUND,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&msvSortAscending, 
		SVSortAscendingObjectGuid,
		IDS_OBJECTNAME_SORTASCENDING,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&msvbExcludeFailed, 
		SVExcludeFailedObjectGuid,
		IDS_OBJECTNAME_EXCLUDEFAILED,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&m_bvoFillBlobs,
		SVBlobUseFillGuid,
		IDS_BLOB_USE_FILL,
		false, SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_colorBlobEnumValue,
		SVBlobIsBlackGuid,
		IDS_BLACK_BLOBS,
		false, SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_evoBlobFillColor,
		SVBlobFillColorGuid,
		IDS_BLOB_FILL_COLOR,
		false, SVResetItemNone );

	RegisterEmbeddedObject(
		&m_evoBlobType,
		SVBlobFillTypeGuid,
		IDS_BLOB_FILL_TYPE,
		false, SVResetItemNone );

	for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		RegisterEmbeddedObject(
			&msvValue[i], 
			*BlobFeatureConstants[i].pEmbeddedID,
			BlobFeatureConstants[i].NewStringResourceID,
			false, SVResetItemNone );

		msvValue[i].SetDefaultValue(0, TRUE);

		msvszFeaturesEnabled [i] = _T('1');             // Not enabled.
		m_guidResults[i] = SV_GUID_NULL;
	}

	msvszFeaturesEnabled [SV_TOPOF_LIST] = _T('\0');    // Null termination.

	msvSortAscending.SetDefaultValue(FALSE,TRUE);
	msvbExcludeFailed.SetDefaultValue(FALSE,TRUE);

	msvlDefaultAttributes = msvValue[0].ObjectAttributesAllowed();

	// Set default inputs and outputs
	addDefaultInputObjects();

	/*--- FEATURE LIST ---------------------------------------------------------*/
	/*--- The list of enabled features is kept in a string because, of the      */
	/*--- "value objects", the string appeared to contain the least overhead. --*/
	for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		msvszFeaturesEnabled [i] = _T('0');             // Not enabled.
		msvValue[i].ObjectAttributesAllowedRef() = 
			msvValue[i].ObjectAttributesAllowed() & (~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);
	}

	msvPersistantFeaturesEnabled.SetDefaultValue (msvszFeaturesEnabled, TRUE);
	/*--- End of FEATURE LIST. -------------------------------------------------*/

	m_lvoBlobSampleSize.SetDefaultValue (SV_MAX_NUMBER_OF_BLOBS, TRUE);
	m_lvoMaxBlobDataArraySize.SetDefaultValue(1, TRUE);

	msvSortFeature.SetDefaultValue (SV_AREA, TRUE);

	CreateArray();

	m_colorBlobEnumValue.SetEnumTypes(g_strBlobColorEnums);
	m_colorBlobEnumValue.SetDefaultValue(SV_BLOB_WHITE,TRUE);	
	//set default values for the BlobFill value objects
	m_bvoFillBlobs.SetDefaultValue(FALSE,TRUE);
	m_evoBlobFillColor.SetEnumTypes(g_strBlobFillColorEnums);
	m_evoBlobFillColor.SetDefaultValue(SV_BLOB_FILL_BLACK,TRUE);
	m_evoBlobType.SetEnumTypes(g_strBlobFillTypeEnums);
	m_evoBlobType.SetDefaultValue(SV_BLOB_FILL_EXCLUDED,TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
//
SVBlobAnalyzerClass::~SVBlobAnalyzerClass()
{
	CloseObject ();
}

#pragma region IEnumerateValueObject
SvOi::NameValueList SVBlobAnalyzerClass::getFeatureList(bool isSelected) const
{
	SvOi::NameValueList list;
	for (int i = SV_AREA; i < SV_TOPOF_LIST; i++)
	{	

		if ( (msvszFeaturesEnabled[i] == '1' && isSelected) || (msvszFeaturesEnabled[i] == '0' && !isSelected))
		{
			//
			// Skip the M_SUM_PIXEL blob feature if doing 'available' list.
			//
			if(i == SV_SUM_PIXEL && !isSelected)
			{
				continue;
			}
			list.push_back( SvOi::NameValuePair(msvValue [i].GetName(), i) );
		}
	}
	return list;
}
#pragma endregion IEnumerateValueObject

BOOL SVBlobAnalyzerClass::CloseObject ()
{
    SVImageAnalyzerClass::CloseObject ();
	
    if ( !msvResultBufferID.empty() )
    {
		SVMatroxBlobInterface::Destroy(msvResultBufferID);
    }

    if ( !msvFeatureListID.empty() )
    {
		SVMatroxBlobInterface::Destroy(msvFeatureListID);
    }
    return TRUE;
}

DWORD SVBlobAnalyzerClass::AllocateResult (SVBlobFeatureEnum aFeatureIndex)
{
	SVClassInfoStruct       resultClassInfo;
	CString                 strTitle;
	SVObjectTypeInfoStruct  interfaceInfo;

	SVDoubleResultClass*    pResult(nullptr);
	DWORD LastError(0);


	//not a MIL Feature...  Just return
	if ( (aFeatureIndex == SV_CENTER_X_SOURCE)  || (aFeatureIndex == SV_CENTER_Y_SOURCE) )
		return 0;

	do
	{
		// Setup the result

		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = msvValue[aFeatureIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
		resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
		resultClassInfo.m_ClassName = SvUl_SF::LoadString( IDS_OBJECTNAME_RESULT );
		strTitle = msvValue[aFeatureIndex].GetName(); //.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
		resultClassInfo.m_ClassName += SV_TSTR_SPACE + strTitle;

		// Construct the result class
		pResult = (SVDoubleResultClass *) resultClassInfo.Construct();
		m_guidResults[ aFeatureIndex ] = pResult->GetUniqueObjectID();

		if(!pResult)
		{	
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16111);
			LastError =  -SvOi::Err_16111;
			break;
		}

		Add( pResult );

		SVObjectTypeInfoStruct info;
		info.ObjectType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;

		SVDoubleValueObjectClass* pValue = dynamic_cast<SVDoubleValueObjectClass*>( getFirstObject( info ) );

		if (!pValue)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16112);
			LastError =   -SvOi::Err_16112 ; 
			break;
		}

		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;

		// Ensure this Object's inputs get connected
		pResult->ConnectAllInputs();

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(pResult) )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10041 ); 
					
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( SV_GUID_NULL != objectID )
				{
					Delete( objectID );
				}
				else
				{
					delete pResult;
				}
			}
		}

	} while (false);

	return LastError;	
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : AllocateBlobResult( )
// -----------------------------------------------------------------------------
// .Description : Associate the number of blobs found (m_lvoNumberOfBlobsFound) to a 
//              : Result Class object so that we can set a range on the number of  
//				: Blobs. Since it's not a feature associated with every blob, we have to
//				: handle it independant of other blob features.
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
DWORD SVBlobAnalyzerClass::AllocateBlobResult ()
{
	SVClassInfoStruct       resultClassInfo;
	CString                 strTitle;
	SVObjectTypeInfoStruct  interfaceInfo;

//    SVDoubleResultClass*    pResult;

	DWORD LastError(0);
	
	do
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_lvoNumberOfBlobsFound.GetEmbeddedID();
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
		resultClassInfo.m_ClassId = SVLongResultClassGuid;
		resultClassInfo.m_ClassName = SvUl_SF::LoadString( IDS_OBJECTNAME_RESULT );
		strTitle = m_lvoNumberOfBlobsFound.GetName();
		resultClassInfo.m_ClassName += SV_TSTR_SPACE + strTitle;
		
		// Construct the result class
		m_pResultBlob = (SVLongResultClass *) resultClassInfo.Construct();
		
		if(!m_pResultBlob)
		{
			SvStl::MessageMgrStd  Ex( SvStl::LogAndDisplay );
			Ex.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16113);
			LastError =   -SvOi::Err_16113 ; 
			break;
		}
		
		Add( m_pResultBlob );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVLongValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		SVLongValueObjectClass* pValue = dynamic_cast<SVLongValueObjectClass*>( getFirstObject( info ) );

		if (!pValue)
		{		
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16114);
			LastError =   -SvOi::Err_16114 ; 
			break;
		}
		
		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		
		// Ensure this Object's inputs get connected
		m_pResultBlob->ConnectAllInputs();
		
		// And last - Create (initialize) it
		
		if( ! m_pResultBlob->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(m_pResultBlob) )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10042 ); 
				
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = m_pResultBlob->GetUniqueObjectID();
				if( SV_GUID_NULL != objectID )
				{
					Delete( objectID );
				}
				else
				{
					delete m_pResultBlob;
				}
			}
		}
		
	} while ( false );
	
	return LastError;	
}

DWORD SVBlobAnalyzerClass::FreeResult (SVBlobFeatureEnum aFeatureIndex)
{
	SVResultClass*    pResult(nullptr);
	DWORD LastError(0);
	
	pResult = GetResultObject (aFeatureIndex);

	if (nullptr == pResult)
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16115);
		LastError =   -SvOi::Err_16115 ; 

	}
	else
	{


		m_guidResults[ aFeatureIndex ] = SV_GUID_NULL;

		DestroyChildObject(pResult, SVMFSetDefaultInputs);

		pResult = nullptr;
	}
	
	return LastError;
	
}

SVResultClass* SVBlobAnalyzerClass::GetResultObject(SVBlobFeatureEnum aFeatureIndex)
{
	return static_cast <SVResultClass*> ( SVObjectManagerClass::Instance().GetObject(m_guidResults[aFeatureIndex]) );
}

void SVBlobAnalyzerClass::RebuildResultObjectArray()
{
	SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;

	SVInObjectInfoStruct*	pResultInputInfo;

	SVDoubleResultClass*    pResult;
	SVObjectClass*          pSVObject;
	
	SVObjectTypeInfoStruct info;

	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultDoubleObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		pResult = dynamic_cast< SVDoubleResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		pResult->GetPrivateInputList( resultInputList );

		pResultInputInfo = resultInputList.GetAt( 0 );

		pSVObject = pResultInputInfo->GetInputObjectInfo().PObject;

		for ( int iFeature = SV_AREA; iFeature < SV_NUMBER_OF_BLOB_FEATURES; iFeature++ )
		{
			if (&msvValue[iFeature] == pSVObject)
			{
				m_guidResults[iFeature] = pResult->GetUniqueObjectID();
				break;
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetBlobResultObject( )
// -----------------------------------------------------------------------------
// .Description : Returns the Result class associated with the number of blobs found 
//              : (m_lvoNumberOfBlobsFound). If it couldn't find a Result class, create a   
//				: new Result class by calling AllocateBlobResult.
//				: 
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
SVLongResultClass* SVBlobAnalyzerClass::GetBlobResultObject()
{
	SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;

	SVInObjectInfoStruct*	pResultInputInfo;
	SVLongResultClass*    pResult = nullptr;
	SVObjectClass*          pSVObject;

	SVObjectTypeInfoStruct info;

	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultLongObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		pResult = dynamic_cast< SVLongResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		pResult->GetPrivateInputList( resultInputList );

		pResultInputInfo = resultInputList.GetAt( 0 );

		pSVObject = pResultInputInfo->GetInputObjectInfo().PObject;

		if (&m_lvoNumberOfBlobsFound == pSVObject)
		{
			break;
		}
	}

	return pResult;
}

/////////////////////////////////////////////////////////////////////////////
//
//
BOOL SVBlobAnalyzerClass::CreateObject(SVObjectLevelCreateStruct* PCreateStructure)
{
	CString             tempString;
	SVBlobFeatureEnum   i(SV_AREA);
	double              deflt(0.0);
	DWORD				LastError(0);
	
	
	
	SVMatroxBlobInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	
	do
	{
		if(!SVImageAnalyzerClass::CreateObject( PCreateStructure ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16116);
			LastError = - SvOi::Err_16116;
			break;
		}
		
		//
		// Restore manipulatable string from persistant string.
		//
		msvPersistantFeaturesEnabled.GetValue ( tempString );

		if ( tempString.GetLength() < SV_TOPOF_LIST )
		{
			tempString += "00";
			long l_lTempIndex = msvPersistantFeaturesEnabled.GetLastSetIndex();
			msvPersistantFeaturesEnabled.SetValue(l_lTempIndex,tempString);
		}
		
		_tcscpy (msvszFeaturesEnabled, tempString);
		
		m_lvoBlobSampleSize.GetValue( m_lBlobSampleSize );
		m_lvoMaxBlobDataArraySize.GetValue( m_lMaxBlobDataArraySize );
		
		//--- It is assumed, in the body of the class, that only enabled features
		//--- are registered.  All features needed to be register before now so that
		//--- the scripting would have access to them.
		for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
		{
			if (msvszFeaturesEnabled [i] == _T('0'))
			{
				hideEmbeddedObject (msvValue[i]);
				RemoveEmbeddedObject (&msvValue[i]);
			}
		}

		// EB 2005 01 28
		// add array capability to blob results
		for ( int iFeature = 0; iFeature < SV_NUMBER_OF_BLOB_FEATURES; iFeature++ )
		{
			msvValue[iFeature].SetArraySize( m_lMaxBlobDataArraySize );	// no longer sample size (max number of blobs found)
		}
		
		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();

		l_Code = SVMatroxBlobInterface::Create( msvResultBufferID );
		
		if ( msvResultBufferID.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16117);
			LastError = - SvOi::Err_16117;
			break;
		}
		
		l_Code = SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobIdentifier, static_cast<long>(SVValueBinary) );

		BOOL l_bUseFillBlob;
		m_bvoFillBlobs.GetValue( l_bUseFillBlob );
		if ( l_bUseFillBlob )
		{
			l_Code = SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable) );
		}
		else
		{
			l_Code = SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable) );
		}

		//    Restore selected features after recreation. msvFeatureListID get
		//    created (allocated) here.
		BuildFeatureListID ();

		// size to fit number of blobs MIL searches for
		m_vec2dBlobResults.resize(SV_NUMBER_OF_BLOB_FEATURES, m_lBlobSampleSize);

		// Sri	04-12-00
		// Features Box X Max, Box X Min, Box Y Max and Box Y Min should always be enabled,
		// since we have to draw bounding rectangles to all the blobs found.
		for (i = SV_AREA; i <= SV_BOXY_MIN; i = (SVBlobFeatureEnum)(i + 1))
		{
			if (msvszFeaturesEnabled [i] != _T('1'))
			{
				msvszFeaturesEnabled [i] = _T('1');
				EnableFeature (i);
				
				SVResultClass *pResult = GetResultObject (i);
				if(!pResult)
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16118);
					LastError = - SvOi::Err_16118;
					break; // Break out of for loop 
				}
				
				SVRangeClass *pRange = pResult->GetResultRange();
				if(!pRange)
				{
					SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16119);
					LastError = - SvOi::Err_16119;
					break; // Break out of for loop 
				}
				
				pRange->FailHigh.SetValue(1, 5000.0); // Set some high values, so that it doesn't fail
				pRange->WarnHigh.SetValue(1, 4999.0);
			}
		}


		if (0 != LastError )
		{
			break; // If any error had occurred in the for loop, break out of while loop also.	
		}


		// RDS 2002-03-26
		// Set currently existing features into the persistence string.
		// Mainly this should be SV_BOXX_MAX thru SV_BOXY_MIN
		msvPersistantFeaturesEnabled.SetValue ( 1, msvszFeaturesEnabled );

		long lSortIndex;
		msvSortFeature.GetValue (lSortIndex);
		if(lSortIndex < 0)
			msvSortFeature.SetValue (1, 1L);
		if(!m_pResultBlob && !(m_pResultBlob = (SVLongResultClass*)GetBlobResultObject()))
		{
			if (AllocateBlobResult() & SV_ERROR_CONDITION) 
				break; // Some error has occurred.
			
			SVRangeClass *pRange = m_pResultBlob->GetResultRange();
			if(!pRange)
			{
			
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16120);
				LastError = SvOi::Err_16120;
				break; 
			}
			pRange->FailLow.SetValue(1, m_defaultResultNumberOfBlobsLowFail); 
			pRange->WarnLow.SetValue(1, m_defaultResultNumberOfBlobsLowWarn);
			pRange->FailHigh.SetValue(1, m_defaultResultNumberOfBlobsHighFail); 
			pRange->WarnHigh.SetValue(1, m_defaultResultNumberOfBlobsHighWarn);
		}
		// End. Sri
		msvValue[0].GetDefaultValue (deflt);
		m_vec2dBlobResults.fill(deflt);
		
	} while ( false );

	
	if (0 != LastError )
	{
		m_isCreated = false;
	}
	else
	{
		m_isCreated = true;
	}

	// Set Embedded defaults
	for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		if( !msvValue[i].IsCreated() )
		{
			CreateChildObject( &msvValue[i] );
		}

		if ( msvszFeaturesEnabled[i] != _T('1') )
		{
			msvValue[i].ObjectAttributesAllowedRef() &= ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		}
		else	// Jira SVO-611
		{
			msvValue[i].ObjectAttributesAllowedRef() |= SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		}

		if ( SV_NO_ATTRIBUTES != msvValue[i].ObjectAttributesAllowed() )
		{
			msvValue[i].ObjectAttributesAllowedRef() |= SV_PRINTABLE;	// for older configs
		}
	}

	// add printable for older configs
	msvPersistantFeaturesEnabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lvoBlobSampleSize.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvSortFeature.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvSortAscending.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msvbExcludeFailed.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lvoNumberOfBlobsFound.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return m_isCreated;
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::EnableFeature (SVBlobFeatureEnum aIndex)
{
	msvValue[aIndex].ObjectAttributesAllowedRef() = msvlDefaultAttributes;

	RegisterEmbeddedObject( &msvValue[aIndex], *BlobFeatureConstants[aIndex].pEmbeddedID, BlobFeatureConstants[aIndex].NewStringResourceID, false, SVResetItemNone );
	
	dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
	
	AllocateResult (aIndex);

	dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
	if ( !(SV_CENTER_X_SOURCE == aIndex || SV_CENTER_Y_SOURCE == aIndex) )
	{
		BuildFeatureListID();
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::DisableFeature(SVBlobFeatureEnum aIndex)
{
	if ( SV_CENTER_X_SOURCE  != aIndex && SV_CENTER_Y_SOURCE != aIndex )
	{
		FreeResult (aIndex);
	}
	hideEmbeddedObject (msvValue[aIndex]);
	RemoveEmbeddedObject (&msvValue[aIndex]);
	dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();

	BuildFeatureListID();

	return 0;
}

BOOL SVBlobAnalyzerClass::OnValidate()
{
	if (!SVImageAnalyzerClass::OnValidate ())
	{
		SetInvalid();
		return false;
	}

	m_isObjectValid.SetValue(1, true);
	return true;
}

bool SVBlobAnalyzerClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	return (Result && __super::resetAllObjects( shouldNotifyFriends, silentReset ));
}

////////////////////////////////////////////////////////////////////////////////
//
//
BOOL SVBlobAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	
	DWORD LastError(0);
	long lSortFeature;
	bool l_bCenterXSet = false;
	bool l_bCenterYSet = false;

	SVImageClass* pImage(nullptr);

	SVMatroxBlobInterface::SVStatusCode l_Code;

	do
	{
		SVSmartHandlePointer ImageHandle;

		if(!SVImageAnalyzerClass::onRun(RRunStatus))
		{
			break;
		}

		//
		// Mil library determines the blobs and count of blobs.
		//
		pImage = getInputImage();

		if (!pImage)
		{

			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16135);
			LastError = - SvOi::Err_16135;
			break;
		}

		if ( ! pImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			ASSERT( FALSE );
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16136);
			LastError = - SvOi::Err_16136;
			break;;
		}

		SVImageBufferHandleImage l_MilBuffer;
		ImageHandle->GetData( l_MilBuffer );

		if ( l_MilBuffer.empty() )
		{
			ASSERT( FALSE );
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16137);
			LastError = - SvOi::Err_16137;
			break;
		}

		//
		// Analyze the image for blobs and features of blobs.
		//
		l_Code = SVMatroxBlobInterface::Execute( msvResultBufferID, l_MilBuffer.GetBuffer(), msvFeatureListID );

		if( l_Code != SVMEE_STATUS_OK )
		{
			ASSERT( FALSE );
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16138);
			LastError = - SvOi::Err_16138;
			break;

		}

		// Sri 04-12-00
		// Check whether we want to exclude the blobs, which has at least one of its features 
		// out of Range
		BOOL	bExclude;
		msvbExcludeFailed.GetValue(bExclude);
		if(bExclude)
		{
			for ( SVBlobFeatureEnum eFeature = SV_AREA; eFeature < SV_TOPOF_LIST; eFeature = (SVBlobFeatureEnum)(eFeature + 1) )
			{
				if (msvszFeaturesEnabled[eFeature] == _T('1'))
				{
					// check to see if the feature is SV_CENTER_X_SOURCE or SV_CENTER_Y_SOURCE
					// if so do not call the MIL functions.
					
					if ( (eFeature == SV_CENTER_X_SOURCE) || (eFeature == SV_CENTER_Y_SOURCE) )
					{
						continue;
					}
					
					SVResultClass* pResult = GetResultObject(eFeature);
					if(!pResult)
					{
						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16139);
						LastError = - SvOi::Err_16139;
						break;
					}
					SVRangeClass* pRange = pResult->GetResultRange();
					if(!pRange)
					{
						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16140);
						LastError = - SvOi::Err_16140;
						break;
					}

					double dLow(0), dHigh(0);
					pRange->getUpdatedRange(RangeEnum::ER_FailHigh, RRunStatus.m_lResultDataIndex).GetValue(dHigh);
					pRange->getUpdatedRange(RangeEnum::ER_FailLow, RRunStatus.m_lResultDataIndex).GetValue(dLow);

					pRange->getUpdatedRange(RangeEnum::ER_WarnLow,RRunStatus.m_lResultDataIndex);
					pRange->getUpdatedRange(RangeEnum::ER_WarnHigh,RRunStatus.m_lResultDataIndex);

					// Now that we have indirect high and low ranges it is possible that dLow is larger than dHigh.
					// This would cause the MIL function to return an error.  To avoid this exception, we set both to 0.
					// Desired behavior in this case is that ALL blobs are excluded.
					if( dLow > dHigh )
					{
						dLow = 0;
						dHigh = 0;
						// To exclude all blobs, also exclude any blobs which are in the range.
						l_Code = SVMatroxBlobInterface::BlobSelect( msvResultBufferID, 
							SVEBlobExclude, 
							BlobFeatureConstants[eFeature].MILFeatureDef, 
							SVECondInRange, 
							dLow, 
							dHigh );
					}

					// Exclude all blobs that are out of the range for this feature.
					l_Code = SVMatroxBlobInterface::BlobSelect(msvResultBufferID, 
						SVEBlobExclude, 
						BlobFeatureConstants[eFeature].MILFeatureDef, 
						SVECondOutRange, 
						dLow, 
						dHigh );

					if( l_Code != SVMEE_STATUS_OK )
					{

						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16141);
						LastError = - SvOi::Err_16141;
						break;
					}
				}
			}
			if (0 != LastError )
			{
				break; // If any error had occurred in the for loop, break out of while loop also.
			}
		}// end if(bExclude)
		// End. Sri

		//
		// Get the number blobs found by MblobCalculate()
		//
		l_Code = SVMatroxBlobInterface::GetNumber( msvResultBufferID, m_lNumberOfBlobsFound );

		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16142);
			LastError = - SvOi::Err_16142;
			break;
		}

		if (m_lNumberOfBlobsFound > m_lBlobSampleSize)
		{
			//Apparently the M_LABEL_VALUE given to each found blob is not 
			//consecutive from start to finish.   This is based on testing 
			//that indicated less then the desired number of selected blobs
			//after executing this function.  The check of the number of 
			//selected blobs after executing this function as well as the 
			//while loop, designed to achieve the correct nbr of blobs, 
			//below were added to correct this problem.	EJC 07/31/01
			long l_lValue=0;
			l_Code = SVMatroxBlobInterface::BlobSelect( msvResultBufferID,
				SVEBlobExclude,
				SVEBlobLabelValue,
				SVECondGreater,
				m_lBlobSampleSize, 
				l_lValue );		// Matrox was expecting a NULL here but our interface uses a ref.

			//
			// Limit the number of blobs to the maximum we can handle.
			//
			//check the number of blobs selected
			long lSelectedNbrOfBlobs = 0;
			l_Code = SVMatroxBlobInterface::GetNumber( msvResultBufferID, lSelectedNbrOfBlobs );

			if(lSelectedNbrOfBlobs == m_lBlobSampleSize)
			{
				m_lNumberOfBlobsFound = m_lBlobSampleSize;
			}
			else if(lSelectedNbrOfBlobs < m_lBlobSampleSize)
			{
				int x = 0;
				while(lSelectedNbrOfBlobs < m_lBlobSampleSize)
				{
					x += (m_lBlobSampleSize - lSelectedNbrOfBlobs);
					l_Code = SVMatroxBlobInterface::BlobSelect( msvResultBufferID,
						SVEBlobExcludeOnly,
						SVEBlobLabelValue,
						SVECondGreater,
						m_lBlobSampleSize + x,
						l_lValue);

					l_Code = SVMatroxBlobInterface::GetNumber( msvResultBufferID, lSelectedNbrOfBlobs);
				}
				m_lNumberOfBlobsFound = lSelectedNbrOfBlobs;

			}
			else //if(lSelectedNbrOfBlobs > m_lBlobSampleSize)
			{
				// this is an error condition!  i.e. selected more blobs then we asked for !
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16143);
				LastError = - SvOi::Err_16143;
				break;
			}

			if( l_Code != SVMEE_STATUS_OK )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16144);
				LastError = - SvOi::Err_16144;
				break;
			}
		}// end if (m_lNumberOfBlobsFound > m_lBlobSampleSize)

		m_lvoNumberOfBlobsFound.SetValue( RRunStatus.m_lResultDataIndex, m_lNumberOfBlobsFound );

		// EB 2005 01 28
		// add array capability to blob results
		m_lNumberOfBlobsToProcess = std::min(m_lNumberOfBlobsFound, m_lMaxBlobDataArraySize);	// no more results than max data array size
		for ( int iFeature = 0; iFeature < SV_NUMBER_OF_BLOB_FEATURES; iFeature++ )
		{
			if (msvszFeaturesEnabled[iFeature] == _T('1'))
			{
				msvValue[iFeature].SetResultSize( RRunStatus.m_lResultDataIndex, m_lNumberOfBlobsToProcess );
			}
		}

		// do the following even if no blobs have been found.
		//if number of blobs to process = 0,  make sure the feature do not have values from previous runs.
		if (0 == m_lNumberOfBlobsToProcess)
		{
			for (SVBlobFeatureEnum eFeature = SV_AREA; eFeature < SV_TOPOF_LIST; eFeature = (SVBlobFeatureEnum)(eFeature + 1))
			{	
				if (msvszFeaturesEnabled[ eFeature ] == _T('1'))
				{
					msvValue[ eFeature ].SetValue( RRunStatus.m_lResultDataIndex, 0.0 );
				}
			}
		}
	
		register SVBlobFeatureEnum eFeature;

		// Get the array of features values for each feature selected.
		//
		for (eFeature = SV_AREA; eFeature < SV_TOPOF_LIST; eFeature = (SVBlobFeatureEnum) (eFeature + 1))
		{
			if (msvszFeaturesEnabled[ eFeature ] == _T('1'))
			{
				if ( (eFeature == SV_CENTER_X_SOURCE) || (eFeature == SV_CENTER_Y_SOURCE) ) // not a MIL Feature, do not process
				{
					//calculate the values if the feature is enabled.  
					// do not continue with rest of feature code for these two
					// feature, since they are not MIL features.
					continue;
				}

				//
				// Get a pointer to the row of values assigned to this blob
				// feature.
				//

				vector2d<double>::row_type& row = m_vec2dBlobResults[eFeature];
				row.resize( m_lNumberOfBlobsFound );
				double* pData = nullptr;
				if(row.size() != 0)
				{
					pData = &(row[0]);
					memset( pData, 0L, sizeof( double ) * row.size() );
				}

				//
				// Check to make sure we don't try to get M_SUM_PIXEL result
				// on a binary image.  We can't do M_SUM_PIXEL
				//
				if(BlobFeatureConstants[eFeature].MILFeatureDef == SVEBlobSumPixel)
				{
					//
					// If we could find out if this was a binarized image. We could
					// possibly do more.  MBufInquire() had no option to return an 
					// indication a binarized image property.
					//
					// Zero the M_SUM_PIXEL blob features values
					//
					for ( int n = 0; n < m_lNumberOfBlobsFound; n++ )
					{
						pData[n] = 0.0;
					}
					continue;     // Next i;
				}
				//
				// Get the results for each feature for each blob found.
				//
				if (m_lNumberOfBlobsFound != 0 && ((eFeature != SV_CENTER_X_SOURCE) || (eFeature != SV_CENTER_Y_SOURCE)) )
				{
					l_Code = SVMatroxBlobInterface::GetResult( msvResultBufferID, 
						BlobFeatureConstants [eFeature]. MILFeatureDef,
						pData );
				}
			}// end if (msvszFeaturesEnabled [eFeature] == _T('1'))

		}// end for (eFeature = SV_AREA; eFeature < SV_TOPOF_LIST; eFeature = (SVBlobFeatureEnum) (eFeature + 1))
		if (m_lNumberOfBlobsFound != 0 && ((msvszFeaturesEnabled [SV_CENTER_X_SOURCE] == _T('1')) || 
				(msvszFeaturesEnabled [SV_CENTER_Y_SOURCE] == _T('1'))) )
		{
			double * pCenterXData = nullptr;
			bool l_bCenterXSet = false;
			if (msvszFeaturesEnabled [SV_CENTER_X_SOURCE] == _T('1') )
			{
				vector2d<double>::row_type& row = m_vec2dBlobResults[SV_CENTER_X_SOURCE];
				pCenterXData = &(row[0]);
				memset( pCenterXData, 0L, sizeof( double ) * row.size() );
				l_bCenterXSet = true;
			}

			double * pCenterYData = nullptr;
			bool l_bCenterYSet = false;
			if (msvszFeaturesEnabled [SV_CENTER_Y_SOURCE] == _T('1') )
			{
				vector2d<double>::row_type& row = m_vec2dBlobResults[SV_CENTER_Y_SOURCE];
				pCenterYData = &(row[0]);
				memset( pCenterYData, 0L, sizeof( double ) * row.size() );
				l_bCenterYSet = true;
			}

			double * pBoxXMaxData = &(m_vec2dBlobResults[SV_BOXX_MAX][0]);
			double * pBoxXMinData = &(m_vec2dBlobResults[SV_BOXX_MIN][0]);
			double * pBoxYMaxData = &(m_vec2dBlobResults[SV_BOXY_MAX][0]);
			double * pBoxYMinData = &(m_vec2dBlobResults[SV_BOXY_MIN][0]);

			for(int n=0; n < m_lBlobSampleSize; n++)
			{
				double l_dMaxX = 0.0;
				double l_dMinX = 0.0;
				double l_dMaxY = 0.0;
				double l_dMinY = 0.0;
				double l_dCenterX = 0.0;
				double l_dCenterY = 0.0;
				SVExtentPointStruct ptSt;

				l_dMaxX = pBoxXMaxData[n];
				l_dMinX = pBoxXMinData[n];
				l_dMaxY = pBoxYMaxData[n];
				l_dMinY = pBoxYMinData[n];

				l_dCenterX = (l_dMaxX - l_dMinX)/2 + l_dMinX;
				l_dCenterY = (l_dMaxY - l_dMinY)/2 + l_dMinY;

				ptSt.m_dPositionX = l_dCenterX;
				ptSt.m_dPositionY = l_dCenterY;

				SVImageClass* pTmpImage = nullptr;

				if ( pImage )
				{
					pTmpImage = pImage;
					while ( nullptr != pTmpImage->GetParentImage() )
					{
						pTmpImage = pTmpImage->GetParentImage();
					}
					pImage->TranslateFromOutputSpaceToImage(pTmpImage,ptSt,ptSt);

					if ( l_bCenterXSet )
					{
						pCenterXData[n] = ptSt.m_dPositionX;
					}
					if ( l_bCenterYSet )
					{
						pCenterYData[n] = ptSt.m_dPositionY;
					}
						
				} // if (pImage) 
			}//for number of blobs
		}//if the feature SV_CENTER_X(Y)_SOURCE is set

		msvSortFeature.GetValue( lSortFeature );
		msvlSortMap.SetSize( m_lNumberOfBlobsFound );
		// Check for Sort Feature
		// Note Sort Feature will be -1 if no features have been selected
		// otherwise if at least one feature has been selected
		// lSortFeature will be a valid feature index
		if( lSortFeature >= SV_AREA && lSortFeature < SV_TOPOF_LIST )
		{
			SortBlobs( lSortFeature, msvlSortMap.GetData(), static_cast< long >( msvlSortMap.GetSize() ) );
		}

		for (eFeature = SV_AREA; eFeature < SV_TOPOF_LIST; eFeature = (SVBlobFeatureEnum) (eFeature + 1))
		{
			if (msvszFeaturesEnabled [eFeature] == _T('1'))
			{
				if (m_lNumberOfBlobsToProcess > 0)
				{
					// EB 2005 01 28
					// add array capability to blob results
					for ( int iBlob = 0; iBlob < m_lNumberOfBlobsToProcess; iBlob++ )
					{
						msvValue[eFeature].SetValue( RRunStatus.m_lResultDataIndex, iBlob, m_vec2dBlobResults[eFeature][ msvlSortMap[iBlob] ] );
					}
				}
				else
				{
					//Arvid set array size to 1 even if no blobs are present so as not to cause errors in math tools in "old style" configurations
					//Arvid this will in practice, however, undo the changes of SVO-322
					msvValue[ eFeature ].SetResultSize( RRunStatus.m_lResultDataIndex, 1 ); 
					msvValue[ eFeature ].SetValue( RRunStatus.m_lResultDataIndex, 0.0 );
				}
			}
		}

		// Now fill the blobs
		BOOL l_bFillBlob;
		m_bvoFillBlobs.GetValue(l_bFillBlob);

		l_Code = SVMatroxBlobInterface::GetNumber( msvResultBufferID, m_lNumberOfBlobsFound );
		if ( l_bFillBlob )
		{
			long l_lColor;
			long l_lType;
			m_evoBlobFillColor.GetValue(RRunStatus.m_lResultDataIndex, l_lColor);
			m_evoBlobType.GetValue(RRunStatus.m_lResultDataIndex, l_lType);
			SVBlobControlEnum l_eCriterion = SVEBlobAll;
			switch( l_lType )
			{
				case SV_BLOB_FILL_ALL: 
				{
					l_eCriterion = SVEBlobAll;
					break;
				}
				case SV_BLOB_FILL_EXCLUDED :
				{
					l_eCriterion = SVEBlobExcludeBlobs;
					break;
				}
				case SV_BLOB_FILL_INCLUDED:
				{
					l_eCriterion = SVEBlobIncludeBlobs;
					break;
				}
				default:
				{
					// Do nothing.
					break;
				}
			}// end switch( l_lType )

			l_Code = SVMatroxBlobInterface::BlobFill( msvResultBufferID, l_MilBuffer.GetBuffer(), l_eCriterion, l_lColor);
		}// end if
	} while ( false );

	if (0 != LastError )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	return TRUE;
}

DWORD SVBlobAnalyzerClass::SortBlobs (long alSortFeature, 
	long* alSortMap,
	long p_lArraySize )
{
	DWORD LastError(0);
	
	for (int i = 0; i < p_lArraySize; i++)
	{
		alSortMap [i] = i;
	}

	BOOL ascending(FALSE);
	msvSortAscending.GetValue(ascending);
	if (m_vec2dBlobResults[alSortFeature].size() > 0)
	{
		LastError =	MapQuickSort (&(m_vec2dBlobResults[alSortFeature][0]),
			alSortMap,
			0, 
			p_lArraySize - 1, 
		ascending);                           // Ascending

	}
	return LastError;
}

DWORD SVBlobAnalyzerClass::MapQuickSort (double*    aSortArray, 
	long*      alSortMap,
	long       alBeginning,
	long       alEnd,
	BOOL       abAscending)
{
	DWORD LastError(0);
	
	long    i = alEnd;
	long   	j = alBeginning;
	double  val(0.0);
	
	do
	{
		if (alEnd < 0)
		{
			//          Number of blobs is zero.  This is not a fatal error, but there is 
			//          nothing to sort.
			break;
		}

		val = aSortArray [alSortMap [(alBeginning + alEnd) / 2]];

		do
		{
			if (abAscending)
			{
				while (aSortArray [alSortMap [j]] < val)
				{
					j++;
				}
				while (aSortArray [alSortMap [i]] > val)
				{
					i--;
				}
			}
			else
			{
				while (aSortArray [alSortMap [j]] > val)
				{
					j++;
				}
				while (aSortArray [alSortMap [i]] < val) 
				{
					i--;
				}
			}

			if (i >= j)
			{
				if (i != j)
				{
					long lTemp = alSortMap [i];
					alSortMap [i] = alSortMap [j];
					alSortMap [j] = lTemp;
				}

				i --;
				j ++;
			}

		} while (j <= i);

		if (alBeginning < i)
		{
			LastError = MapQuickSort (aSortArray,
				alSortMap,
				alBeginning,
				i,
				abAscending);
			if(LastError)
			{

				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16146);
				LastError = - SvOi::Err_16146;
				break;
			}

			
		}
		if (j < alEnd)
		{
			LastError = MapQuickSort (aSortArray,
				alSortMap,
				j,
				alEnd,
				abAscending);
			if(LastError)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16147);
				LastError = - SvOi::Err_16147;
				break;
			}
		}

	} while ( false );

	return LastError;
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::BuildFeatureListID ()
{
	SVMatroxBlobInterface::SVStatusCode l_Code;
	DWORD LastError(0);

	if ( !msvFeatureListID.empty() )
	{
		l_Code = SVMatroxBlobInterface::Destroy( msvFeatureListID );

		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16148);
			LastError = - SvOi::Err_16148;
			return LastError;

		}
	}

	l_Code = SVMatroxBlobInterface::Create(msvFeatureListID );

	if ( msvFeatureListID.empty() )
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16149);
		LastError = - SvOi::Err_16149;
		return LastError;
	}

	for (SVBlobFeatureEnum i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		if (msvszFeaturesEnabled [i] == _T('1'))
		{
			if ( (i == SV_CENTER_X_SOURCE) || (i == SV_CENTER_Y_SOURCE) )
			{
				continue;
			}

			l_Code = SVMatroxBlobInterface::BlobSelectFeature( msvFeatureListID, 
				BlobFeatureConstants [i].MILFeatureDef);


			if( l_Code != SVMEE_STATUS_OK )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16150);
				LastError = - SvOi::Err_16150;
				return LastError;
			}
		}
	}



	return LastError;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsPtOverResult( CPoint point )
// -----------------------------------------------------------------------------
// .Description : When the user right clicks inside a tool figure, checks whether its
//              : on a blob. If it finds a blob at that point, gets the index of the  
//				: blob and returns TRUE.
//				: Called from SVImageViewClass::OnContextMenu
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
bool SVBlobAnalyzerClass::IsPtOverResult( const POINT& rPoint )
{
	long l_lCurrentNbrOfBlobs = 0;

	m_nBlobIndex = -1;

	if (( S_OK == m_lvoNumberOfBlobsFound.GetValue( l_lCurrentNbrOfBlobs ) ) &&
		(0 != l_lCurrentNbrOfBlobs))
	{
		SVImageExtentClass l_svExtents;
		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		if (pTool)
		{
			HRESULT hr = pTool->GetImageExtent( l_svExtents );

			if (S_OK == hr )
			{
				double* pxMax = &(m_vec2dBlobResults[SV_BOXX_MAX][0]);
				double* pxMin = &(m_vec2dBlobResults[SV_BOXX_MIN][0]);
				double* pyMax = &(m_vec2dBlobResults[SV_BOXY_MAX][0]);
				double* pyMin = &(m_vec2dBlobResults[SV_BOXY_MIN][0]);

				int iMapSize = static_cast< int >( msvlSortMap.GetSize() );
			
				for (int i = 0; i < (int)l_lCurrentNbrOfBlobs && i < iMapSize ; i++)
				{
					RECT l_oRect;

					long l = msvlSortMap.GetAt(i);

					l_oRect.top = static_cast<long>(pyMin[l]);
					l_oRect.left = static_cast<long>(pxMin[l]);
					l_oRect.bottom = static_cast<long>(pyMax[l]);
					l_oRect.right = static_cast<long>(pxMax[l]);
				
					SVExtentFigureStruct l_svFigure = l_oRect;
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

				if( S_OK == l_svFigure.IsPointOverFigure( rPoint ) )
					{
						m_nBlobIndex = msvlSortMap.GetAt(i); 

						break;
					}
				}
			}
		}
	}

	return ( m_nBlobIndex != -1 );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DisplayAnalyzerResult()
// -----------------------------------------------------------------------------
// .Description : When the user right clicks on a blob inside a tool figure and 
//              : selects the Menu item 'Display Analyzer Result', display all the 
//				: selected features and its current values for this blob.
//				: Called from SVImageViewClass::OnCommand
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVBlobAnalyzerClass::DisplayAnalyzerResult()
{
	if(m_nBlobIndex < 0 || 	m_nBlobIndex >= msvlSortMap.GetSize())
		return;

	SVBlobAnalyzerResultDlg	dlg;
	dlg.m_pBlobAnalyzer = this;
	dlg.DoModal();
}

void SVBlobAnalyzerClass::CreateArray()
{
	m_lvoBlobSampleSize.GetValue(m_lBlobSampleSize);
	m_lvoMaxBlobDataArraySize.GetValue(m_lMaxBlobDataArraySize);

	msvlSortMap.SetSize(m_lBlobSampleSize);// = new long[m_lBlobSampleSize]; 

	m_vec2dBlobResults.resize( SV_NUMBER_OF_BLOB_FEATURES, m_lBlobSampleSize );

	// EB 2005 01 28
	// add array capability to blob results
	for ( int iFeature = 0; iFeature < SV_NUMBER_OF_BLOB_FEATURES; iFeature++ )
	{
		if ( msvszFeaturesEnabled[iFeature] == _T('1') )
		{
			msvValue[iFeature].SetArraySize( m_lMaxBlobDataArraySize );	// no longer sample size (max number of blobs found)
		}
		else
		{
			msvValue[iFeature].SetArraySize( 0 );
		}
	}
}

HRESULT SVBlobAnalyzerClass::ResetObject()
{
	HRESULT l_hr = SVImageAnalyzerClass::ResetObject();

	BOOL l_bIsFillBlob;
	m_bvoFillBlobs.GetValue( l_bIsFillBlob );

	if ( l_bIsFillBlob )
	{
		SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable) );
	}
	else
	{
		SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable) );
	}

	long colorBlobEnum;
	m_colorBlobEnumValue.GetValue( colorBlobEnum );
	SVMatroxBlobInterface::SetForeground( msvResultBufferID, SV_BLOB_BLACK == colorBlobEnum );

	CreateArray();

	RebuildResultObjectArray();

	return l_hr;
}

HRESULT SVBlobAnalyzerClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray )
{
	// only if ToolSet/Tool was not Disabled
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if (pTool && pTool->WasEnabled())
	{
		long l_lCurrentNbrOfBlobs = 0;
		m_lvoNumberOfBlobsFound.GetValue( l_lCurrentNbrOfBlobs );

		if ( l_lCurrentNbrOfBlobs > 0 )
		{
			SVImageExtentClass l_svExtents;
			pTool->GetImageExtent(l_svExtents);

			// if running only show N Blob Figures according to the specified
			// MaxBlobDataArraySize variable
			if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
			{
				long numBlobs = 0;
				m_lvoMaxBlobDataArraySize.GetValue(numBlobs);

				for (int i = 0; i < (int)numBlobs; i++)
				{
					RECT l_oRect;

					// Get Values from Bucketized (managed) data
					msvValue[SV_BOXX_MIN].GetValue(msvValue[SV_BOXX_MIN].GetLastSetIndex(), i, l_oRect.left);
					msvValue[SV_BOXX_MAX].GetValue(msvValue[SV_BOXX_MAX].GetLastSetIndex(), i, l_oRect.right);
					msvValue[SV_BOXY_MIN].GetValue(msvValue[SV_BOXY_MIN].GetLastSetIndex(), i, l_oRect.top);
					msvValue[SV_BOXY_MAX].GetValue(msvValue[SV_BOXY_MAX].GetLastSetIndex(), i, l_oRect.bottom);

					SVExtentFigureStruct l_svFigure = l_oRect;
					
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

					SVExtentMultiLineStruct l_multiLine;

					l_multiLine.m_Color = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
					
					l_multiLine.AssignExtentFigure( l_svFigure, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

					UpdateOverlayIDs( l_multiLine );

					p_rMultiLineArray.Add( l_multiLine );
				}
			}
			else // if not running show all
			{
				double* pxMax = &(m_vec2dBlobResults[1][0]);
				double* pxMin = &(m_vec2dBlobResults[2][0]);
				double* pyMax = &(m_vec2dBlobResults[3][0]);
				double* pyMin = &(m_vec2dBlobResults[4][0]);
				
				for (int i = 0; i < (int)l_lCurrentNbrOfBlobs; i++)
				{
					long l = msvlSortMap.GetAt(i);
					RECT l_oRect;

					l_oRect.top = static_cast<long>(pyMin[l]);
					l_oRect.left = static_cast<long>(pxMin[l]);
					l_oRect.bottom = static_cast<long>(pyMax[l]);
					l_oRect.right = static_cast<long>(pxMax[l]);

					SVExtentFigureStruct l_svFigure = l_oRect;
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

					SVExtentMultiLineStruct l_multiLine;
					l_multiLine.m_Color = SV_DEFAULT_SUB_FUNCTION_COLOR_1;

					l_multiLine.AssignExtentFigure( l_svFigure, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

					UpdateOverlayIDs( l_multiLine );

					p_rMultiLineArray.Add( l_multiLine );
				}
			}
		}
	}
	return S_OK;
}

void SVBlobAnalyzerClass::addDefaultInputObjects( BOOL BCallBaseClass, SVInputInfoListClass* PInputListToFill )
{
	SVImageAnalyzerClass::addDefaultInputObjects( BCallBaseClass, PInputListToFill );

#ifdef _DEBUG
	for( SVBlobFeatureEnum i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		UINT uiAttributes = msvValue[i].ObjectAttributesAllowed();
		TCHAR tchEnabled = msvszFeaturesEnabled[i];
		BOOL l_bOk = ( tchEnabled == _T('1') && 
			( uiAttributes & SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES) != SV_NO_ATTRIBUTES ) ||
			( tchEnabled == _T('0') && 
			(uiAttributes & SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES) == SV_NO_ATTRIBUTES );

		// if this ASSERT fires, verify that the attributes are being set correctly!!!!!!! jms & eb 2006 01 20
		ASSERT( !IsCreated() || l_bOk || (i == SV_CENTER_X_SOURCE || i == SV_CENTER_Y_SOURCE) );
	}
#endif
}

