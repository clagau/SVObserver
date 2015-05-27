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

#include "stdafx.h"
#include "SVBlobAnalyzer.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVInspectionProcess.h"
#include "SVBlobAnalyzerDialog.h"
#include "SVGlobal.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVResultDouble.h"
#include "SVResultLong.h"
#include "SVRange.h"
#include "SVTool.h"
#include "SVSVIMStateClass.h"

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

	msvError.ClearLastErrorCd();

	msvlDefaultAttributes = 0;
	m_lNumberOfBlobsFound = 0;
	m_lNumberOfBlobsToProcess = 0;
	m_pResultBlob = NULL;

	//Indentify our output type.
	outObjectInfo.ObjectTypeInfo.SubType = SVBlobAnalyzerObjectType;

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
		m_guidResults[i] = SVInvalidGUID;
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

	msvSortFeature.SetDefaultValue (-1, TRUE);

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

	SVDoubleResultClass*    pResult;

	msvError.ClearLastErrorCd ();

	//not a MIL Feature...  Just return
	if ( (aFeatureIndex == SV_CENTER_X_SOURCE)  || (aFeatureIndex == SV_CENTER_Y_SOURCE) )
		return 0;

	do
	{
		// Setup the result

		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = msvValue[aFeatureIndex].GetEmbeddedID();
		resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

		resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
		resultClassInfo.ClassId = SVDoubleResultClassGuid;
		resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
		strTitle = msvValue[aFeatureIndex].GetName(); //.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
		resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;

		// Construct the result class
		pResult = (SVDoubleResultClass *) resultClassInfo.Construct();
		m_guidResults[ aFeatureIndex ] = pResult->GetUniqueObjectID();

		if(!pResult)
		{
			msvError.msvlErrorCd = -1136;
			SV_TRAP_ERROR_BRK (msvError, 1136);
		}

		Add( pResult );

		SVObjectTypeInfoStruct info;
		info.ObjectType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;

		SVDoubleValueObjectClass* pValue = 
		      reinterpret_cast<SVDoubleValueObjectClass*>(SVSendMessage(pResult, 
		                                               SVM_GETFIRST_OBJECT, 
		                                               NULL, 
		                                              reinterpret_cast<DWORD_PTR>(&info)));

		if (!pValue)
		{
			msvError.msvlErrorCd = -1137;
			SV_TRAP_ERROR_BRK (msvError, 1137);
		}

		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;

		// Ensure this Object's inputs get connected
		::SVSendMessage( pResult, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pResult), NULL ) != SVMR_SUCCESS )
			{
				AfxMessageBox("Creation of Blob Analyzer Result Failed");
					
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( objectID != SVInvalidGUID )
					Delete( objectID );
				else
					delete pResult;
			}
		}

	} while (false);

	return msvError.GetLastErrorCd ();
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

	msvError.ClearLastErrorCd ();
	
	do
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_lvoNumberOfBlobsFound.GetEmbeddedID();
		resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
		resultClassInfo.ClassId = SVLongResultClassGuid;
		resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
		strTitle = m_lvoNumberOfBlobsFound.GetName();
		resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
		
		// Construct the result class
		m_pResultBlob = (SVLongResultClass *) resultClassInfo.Construct();
		
		if(!m_pResultBlob)
		{
			msvError.msvlErrorCd = -25025;
			SV_TRAP_ERROR_BRK (msvError, 25025);
		}
		
		Add( m_pResultBlob );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVLongValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		SVLongValueObjectClass* pValue = 
			reinterpret_cast<SVLongValueObjectClass*>(SVSendMessage(m_pResultBlob, 
			SVM_GETFIRST_OBJECT, 
			NULL, 
			reinterpret_cast<DWORD_PTR>(&info)));
		
		if (!pValue)
		{
			msvError.msvlErrorCd = 25026;
			SV_TRAP_ERROR_BRK (msvError, 25026);
		}
		
		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		
		// Ensure this Object's inputs get connected
		::SVSendMessage( m_pResultBlob, SVM_CONNECT_ALL_INPUTS, NULL, NULL );
		
		// And last - Create (initialize) it
		
		if( ! m_pResultBlob->IsCreated() )
		{
			// And finally try to create the child object...
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pResultBlob), NULL ) != SVMR_SUCCESS )
			{
				AfxMessageBox("Creation of Blob Analyzer Result Failed");
				
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = m_pResultBlob->GetUniqueObjectID();
				if( objectID != SVInvalidGUID )
					Delete( objectID );
				else
					delete m_pResultBlob;
				
			}
		}
		
	} while ( false );
	
	return msvError.GetLastErrorCd ();
}

DWORD SVBlobAnalyzerClass::FreeResult (SVBlobFeatureEnum aFeatureIndex)
{
	SVResultClass*    pResult;
	
	msvError.ClearLastErrorCd ();
	
	do
	{
		pResult = GetResultObject (aFeatureIndex);
		
		if (!pResult)
		{
			msvError.msvlErrorCd = -1140;
			SV_TRAP_ERROR_BRK (msvError, 1140);
		}
		
		m_guidResults[ aFeatureIndex ] = SVInvalidGUID;
		
		SVSendMessage (this, 
		               SVM_DESTROY_CHILD_OBJECT,
		              reinterpret_cast<DWORD_PTR>(pResult),
		               SVMFSetDefaultInputs);
		
		pResult = NULL;
		
	} while ( false );
	
	return msvError.GetLastErrorCd ();
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
	
	msvError.ClearLastErrorCd ();

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
	SVLongResultClass*    pResult = NULL;
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
	SVBlobFeatureEnum   i;
	double              deflt;
	
	msvError.ClearLastErrorCd ();
	
	SVMatroxBlobInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	
	do
	{
		if(!SVImageAnalyzerClass::CreateObject( PCreateStructure ))
		{
			msvError.msvlErrorCd = -1125;
			SV_TRAP_ERROR_BRK (msvError, 1125);
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
		
		GetInspection()->SetDefaultInputs();

		l_Code = SVMatroxBlobInterface::Create( msvResultBufferID );
		
		if ( msvResultBufferID.empty() )
		{
			msvError.msvlErrorCd = -1127;
			SV_TRAP_ERROR_BRK (msvError, 1127);
		}
		
		l_Code = SVMatroxBlobInterface::Set( msvResultBufferID, SVEBlobIdentifier, static_cast<long>(SVValueBinary) );

		//MblobControl(msvResultBufferID, M_IDENTIFIER_TYPE, M_BINARY );
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
		for (i = SV_BOXX_MAX; i <= SV_BOXY_MIN; i = (SVBlobFeatureEnum)(i + 1))
		{
			if (msvszFeaturesEnabled [i] != _T('1'))
			{
				msvszFeaturesEnabled [i] = _T('1');
				EnableFeature (i);
				
				SVResultClass *pResult = GetResultObject (i);
				if(!pResult)
				{
					msvError.msvlErrorCd = 25027;
					SV_TRAP_ERROR_BRK (msvError, 25027); // Break out of for loop
				}
				
				SVRangeClass *pRange = pResult->GetResultRange();
				if(!pRange)
				{
					msvError.msvlErrorCd = 25028;
					SV_TRAP_ERROR_BRK (msvError, 25028);// Break out of for loop
				}
				
				pRange->FailHigh.SetValue(1, 5000.0); // Set some high values, so that it doesn't fail
				pRange->WarnHigh.SetValue(1, 4999.0);
			}
		}

		if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
			break; // If any error had occurred in the for loop, break out of while loop also.
		
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
				msvError.msvlErrorCd = 25029;
				SV_TRAP_ERROR_BRK (msvError, 25029);
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

	if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
		isCreated = FALSE;
	else
		isCreated = TRUE;

	// Set Embedded defaults
	for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
	{
		if( !msvValue[i].IsCreated() )
		{
			::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>( msvValue + i ), NULL );
		}

		if ( msvszFeaturesEnabled[i] != _T('1') )
		{
			msvValue[i].ObjectAttributesAllowedRef() &= ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		}

		if ( msvValue[i].ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
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

	return isCreated;
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::EnableFeature (SVBlobFeatureEnum aIndex)
{
	msvValue[aIndex].ObjectAttributesAllowedRef() = msvlDefaultAttributes;

	RegisterEmbeddedObject(
		&msvValue[aIndex], 
		*BlobFeatureConstants[aIndex].pEmbeddedID,
		BlobFeatureConstants[aIndex].NewStringResourceID,
		false, SVResetItemNone );
	
	GetInspection()->SetDefaultInputs();
	
	AllocateResult (aIndex);

	GetInspection()->SetDefaultInputs();
	if ( !(aIndex == SV_CENTER_X_SOURCE || aIndex == SV_CENTER_Y_SOURCE) )
	{
		BuildFeatureListID ();
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::DisableFeature (SVBlobFeatureEnum aIndex)
{
//    msvValue[aIndex].uObjectAttributesAllowed = 
//        msvValue[aIndex].uObjectAttributesAllowed & 
//        (~SV_DEFAULT_ATTRIBUTES);

	// If picked for resultView, published, used in an Equation, or selected for Archiving
//	msvValue[aIndex].uObjectAttributesSet = 0;


	if ( aIndex != SV_CENTER_X_SOURCE && aIndex != SV_CENTER_Y_SOURCE )
		FreeResult (aIndex);
// HideEmbeddedObject () dissconnects all users of this object.
	hideEmbeddedObject (msvValue[aIndex]);
	RemoveEmbeddedObject (&msvValue[aIndex]);
	GetInspection()->SetDefaultInputs();


	BuildFeatureListID ();

	return 0;
}

BOOL SVBlobAnalyzerClass::OnValidate()
{
	msvError.ClearLastErrorCd ();
	msvError.msvlErrorCd = 0x00000000;
	
	do
	{
		if (!SVImageAnalyzerClass::OnValidate ())
		{
			//       Error code set inside SVImageAnalyzerClass::OnValidate ()
			//		 Next line commented out to remove message box.
			//       SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1124);
			break;
		}
	} while ( false );
	
	if( (msvError.GetLastErrorCd () & SV_ERROR_CONDITION) ||
		(msvError.msvlErrorCd       & SV_ERROR_CONDITION) )
	{
		SetInvalid ();
		return FALSE;
	}
	
	isObjectValid.SetValue (1, TRUE);
	return TRUE;
}

DWORD_PTR SVBlobAnalyzerClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR dwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				dwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				dwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return (dwResult | SVImageAnalyzerClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

////////////////////////////////////////////////////////////////////////////////
//
//
BOOL SVBlobAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	long lSortFeature;

	bool l_bCenterXSet = false;
	bool l_bCenterYSet = false;

	SVImageClass* pImage;

	msvError.ClearLastErrorCd();

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
			msvError.msvlErrorCd = -1135;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1135);
		}

		if ( ! pImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			ASSERT( FALSE );
			msvError.msvlErrorCd = -1135;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1135);
		}

		SVImageBufferHandleImage l_MilBuffer;
		ImageHandle->GetData( l_MilBuffer );

		if ( l_MilBuffer.empty() )
		{
			ASSERT( FALSE );
			msvError.msvlErrorCd = -1135;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1135);
		}

		//
		// Analyze the image for blobs and features of blobs.
		//
		l_Code = SVMatroxBlobInterface::Execute( msvResultBufferID, l_MilBuffer.GetBuffer(), msvFeatureListID );

		if( l_Code != SVMEE_STATUS_OK )
		{
			msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1131);
		}

		// Sri 04-12-00
		// Check whether we want to exclude the blobs, which has atleast one of its features 
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
						msvError.msvlErrorCd = 25030;
						SV_TRAP_ERROR_BRK (msvError, 25030); // Break out of for loop
					}
					SVRangeClass* pRange = pResult->GetResultRange();
					if(!pRange)
					{
						msvError.msvlErrorCd = 25031;
						SV_TRAP_ERROR_BRK (msvError, 25031);// Break out of for loop
					}

					double dLow;
					double dHigh;
					pRange->getUpdatedFailHigh(RRunStatus.m_lResultDataIndex).GetValue(dHigh);
					pRange->getUpdatedFailLow(RRunStatus.m_lResultDataIndex).GetValue(dLow);

					// Now that we have indirect high and low ranges it is possible that dLow is larger than dHigh.
					// This would cause the MIL function to return an error.  To avoid this exception, we set both to NULL (0).
					// Desired behavior in this case is that ALL blobs are excluded.
					if( dLow > dHigh )
					{
						dLow = NULL;
						dHigh = NULL;
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
						msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
						SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 25032);
					}
				}
			}
			if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
				break; // If any error had occurred in the for loop, break out of while loop also.
		}// end if(bExclude)
		// End. Sri

		//
		// Get the number blobs found by MblobCalculate()
		//
		l_Code = SVMatroxBlobInterface::GetNumber( msvResultBufferID, m_lNumberOfBlobsFound );

		if( l_Code != SVMEE_STATUS_OK )
		{
			msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1132);
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
				msvError.msvlErrorCd = SV_ERROR_CONDITION;
				SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1133);
			}

			if( l_Code != SVMEE_STATUS_OK )
			{
				msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
				SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1133);
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

		//
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
				//m_vec2dBlobResults[ eFeature ].resize( m_lNumberOfBlobsFound );
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
			double * pCenterXData = NULL;
			bool l_bCenterXSet = false;
			if (msvszFeaturesEnabled [SV_CENTER_X_SOURCE] == _T('1') )
			{
				vector2d<double>::row_type& row = m_vec2dBlobResults[SV_CENTER_X_SOURCE];
				pCenterXData = &(row[0]);
				memset( pCenterXData, 0L, sizeof( double ) * row.size() );
				l_bCenterXSet = true;
			}

			double * pCenterYData = NULL;
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

				SVImageClass* pTmpImage = NULL;

				if ( pImage )
				{
					pTmpImage = pImage;
					while ( pTmpImage->GetParentImage() != NULL )
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
				// EB 2005 01 28
				// add array capability to blob results
				for ( int iBlob = 0; iBlob < m_lNumberOfBlobsToProcess; iBlob++ )
				{
					msvValue[eFeature].SetValue( RRunStatus.m_lResultDataIndex, iBlob, m_vec2dBlobResults[eFeature][ msvlSortMap[iBlob] ] );
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

	if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
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

	int i;
// it is assumed that msvError.ClearLastErrorCd () is done in calling function.

	for (i = 0; i < p_lArraySize; i++)
	{
		alSortMap [i] = i;
	}

	BOOL ascending;
	msvSortAscending.GetValue(ascending);
	if (m_vec2dBlobResults[alSortFeature].size() > 0)
	{
		MapQuickSort (&(m_vec2dBlobResults[alSortFeature][0]),
	                alSortMap,
	                0, 
	                p_lArraySize - 1, 
#ifndef _DEBUG
	                ascending);                           // Ascending
#else
	                ascending,
	                m_vec2dBlobResults[alSortFeature]);     // Ascending
#endif
	}
	return msvError.GetLastErrorCd ();
}

DWORD SVBlobAnalyzerClass::MapQuickSort (double*    aSortArray, 
                                         long*      alSortMap,
                                         long       alBeginning,
                                         long       alEnd,
#ifndef _DEBUG
                                         BOOL       abAscending)
#else
                                         BOOL       abAscending,
                                         std::vector<double>& SVA)
#endif
{
	long    i;
	long    j;
	long    lTemp;
	
	double  val;
	
	i = alEnd;
	j = alBeginning;
	
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
#ifdef _DEBUG
					//ASSERT( (j >= 0) && (j < SV_MAX_NUMBER_OF_BLOBS) );
					//ASSERT( (alSortMap [j] >= 0) && (alSortMap [j] < SVA.GetSize()) );
#endif
					j++;
				}
				while (aSortArray [alSortMap [i]] > val)
				{
#ifdef _DEBUG
					//ASSERT( (i >= 0) && (i < SV_MAX_NUMBER_OF_BLOBS) );
					//ASSERT( (alSortMap [i] >= 0) && (alSortMap [i] < SVA.GetSize()) );
#endif
					i--;
				}
			}
			else
			{
				while (aSortArray [alSortMap [j]] > val)
				{
#ifdef _DEBUG
					// ASSERT( (j >= 0) && (j < SV_MAX_NUMBER_OF_BLOBS) );
					// ASSERT( (alSortMap [j] >= 0) && (alSortMap [j] < SVA.GetSize()) );
#endif
					j++;
				}
				while (aSortArray [alSortMap [i]] < val) 
				{
#ifdef _DEBUG
					// ASSERT( (i >= 0) && (i < SV_MAX_NUMBER_OF_BLOBS) );
					// ASSERT( (alSortMap [i] >= 0) && (alSortMap [i] < SVA.GetSize()) );
#endif
					i--;
				}
			}
			
			if (i >= j)
			{
				if (i != j)
				{
					lTemp = alSortMap [i];
					alSortMap [i] = alSortMap [j];
					alSortMap [j] = lTemp;
				}
				
				i --;
				j ++;
			}
			
		} while (j <= i);
		
		if (alBeginning < i)
		{
			msvError.msvlErrorCd = MapQuickSort (aSortArray,
			                                     alSortMap,
			                                     alBeginning,
			                                     i,
#ifndef _DEBUG
			                                     abAscending);
#else
			                                     abAscending,
			                                     SVA);          // Ascending
#endif

			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1134);
		}
		if (j < alEnd)
		{
			msvError.msvlErrorCd = MapQuickSort (aSortArray,
			                                     alSortMap,
			                                     j,
			                                     alEnd,
#ifndef _DEBUG
			                                     abAscending);
#else
			                                     abAscending,
			                                     SVA);          // Ascending
#endif

			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1135);
		}

	} while ( false );

	return msvError.GetLastErrorCd ();
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::BuildFeatureListID ()
{
	SVBlobFeatureEnum i;
	
	
	msvError.ClearLastErrorCd ();

	
	SVMatroxBlobInterface::SVStatusCode l_Code;

	do
	{
		if ( !msvFeatureListID.empty() )
		{
			l_Code = SVMatroxBlobInterface::Destroy( msvFeatureListID );
			
			if( l_Code != SVMEE_STATUS_OK )
			{
				msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
				SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1128);
			}
		}
		
		l_Code = SVMatroxBlobInterface::Create(msvFeatureListID );
		
		if ( msvFeatureListID.empty() )
		{
			msvError.msvlErrorCd = -1129;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1129);
		}
		
		for (i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum) (i + 1))
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
					msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
					SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1130);
				}
				
			}
			else
			{
			}
		}
		
		// "Chain" features were removed. jab
		//--- Each of the following features define below (chained pixel features)
		//--- require M_SAVE_RUNS to be enabled.  When M_SAVE_RUNS is enabled, it 
		//--- adds to processing time, so when not needed, it is dissabled. The code
		//--- has been added to BuildFeatureListID () because it is a common location
		//--- that is called every time a feature is enabled or disabled.
		//      if ((msvszFeaturesEnabled [SV_NBRCHAINED_PIXELS] == _T('1')) ||
		//          (msvszFeaturesEnabled [SV_CHAIN_INDEX] == _T('1')) ||
		//          (msvszFeaturesEnabled [SV_CHAIN_X] == _T('1')) ||
		//          (msvszFeaturesEnabled [SV_CHAIN_Y] == _T('1')))
		//      {
		//   		MblobControl(msvResultBufferID, M_SAVE_RUNS, M_ENABLE );
		//      }
		//      else
		//      {
		//   		MblobControl(msvResultBufferID, M_SAVE_RUNS, M_DISABLE );
		//      }
			
			
	} while ( false );
	
	return msvError.GetLastErrorCd ();
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
BOOL SVBlobAnalyzerClass::IsPtOverResult( CPoint point )
{
	long l_lCurrentNbrOfBlobs = 0;

	m_nBlobIndex = -1;

	if (( m_lvoNumberOfBlobsFound.GetValue( l_lCurrentNbrOfBlobs ) == S_OK ) &&
		(l_lCurrentNbrOfBlobs != 0))
	{
		SVImageExtentClass l_svExtents;
		HRESULT hr = GetTool()->GetImageExtent( l_svExtents );

		if (hr == S_OK)
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

				if( l_svFigure.IsPointOverFigure( point ) == S_OK )
				{
					m_nBlobIndex = msvlSortMap.GetAt(i); 

					break;
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
	if (GetTool()->WasEnabled())
	{
		long l_lCurrentNbrOfBlobs = 0;
		m_lvoNumberOfBlobsFound.GetValue( l_lCurrentNbrOfBlobs );

		if ( l_lCurrentNbrOfBlobs > 0 )
		{
			SVImageExtentClass l_svExtents;
			GetTool()->GetImageExtent(l_svExtents);

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBlobAnalyzer.cpp_v  $
 * 
 *    Rev 1.9   23 Jan 2015 11:20:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed method onRun to exclude ALL blobs when Fail High is less than Fail Low.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Dec 2014 09:48:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed m_colorBlobEnumValue.SetDefaultValue Flag to reset all.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Nov 2014 05:07:48   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Add enum SV_BLOB_COLOR_ENUM and g_strBlobColorEnums.
 * Rename m_isBlackBlobValue into m_colorBlobEnumValue and change the type from bool-value to enum-value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   12 Nov 2014 07:02:32   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add m_isBlackBlobValue
 * set foreground to blob interface
 * rename m_evoBlobFillColor instead m_evoBlobColor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Oct 2014 02:54:26   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  924
 * SCR Title:  Default Parameters in Blob Analyzer Result Number of Blobs
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add const for m_defaultResultNumberOfBlobs...(low and High, Fail and Warn) and change the value from 1->0 and from 2->9999
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 10:19:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 10:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 12:16:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:15:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   02 May 2013 11:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:43:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   02 Nov 2012 12:44:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  797
 * SCR Title:  Fix Analyzer Overlay Issues
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Set Overlay color.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   01 Aug 2012 12:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   30 Jul 2012 13:10:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated analyzer to use new extent hit test fucntionality for analyzer results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   30 Jul 2012 12:53:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   27 Jul 2012 08:10:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   25 Jul 2012 14:04:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   18 Jul 2012 13:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   16 Jul 2012 08:55:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   09 Jul 2012 13:59:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   02 Jul 2012 16:36:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   18 Jun 2012 17:52:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   26 May 2011 13:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated array size to 1 when no blobs are found when value objects are being set to a default value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   30 Mar 2011 15:04:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   10 Feb 2011 14:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   27 Jan 2011 10:47:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   08 Dec 2010 07:40:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   04 Nov 2010 13:18:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   01 Jun 2010 14:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   15 Dec 2009 15:00:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   03 Sep 2009 09:59:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   20 Oct 2008 11:11:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved SVBlobAnalyzerResultDlg to it's own module(s).
 * Revised onUpdateFigure and onCollectOverlays to use correct dataIndex and to check if tool or tooset was disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   09 Apr 2008 07:33:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   24 Jul 2007 11:26:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   21 Jun 2007 11:56:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49.1.0   07 Apr 2008 07:32:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   19 Feb 2007 15:47:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   21 Jun 2006 14:06:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  563
 * SCR Title:  Fix problem with Blob Analyzer changing Max Number of Blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with Max Number of Blobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   20 Jan 2006 15:07:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
 * added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
 * added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
 * 
 * fixed handling of feature attributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   20 Jan 2006 07:54:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix compilier warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   08 Dec 2005 09:42:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  532
 * SCR Title:  Fix error popup in Window Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed bogus error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   11 Nov 2005 10:14:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  526
 * SCR Title:  Fix performance problems with Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed the GetResultObject to use cached GUIDs instead of using the GETFIRST / GETNEXT messages
 * added RebuildResultObjectArray
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   13 Oct 2005 13:58:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.  Added processMessage method to call the ResetObject functionality.  Updated some value objects to force an owner reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   20 Sep 2005 14:09:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with loading old configurations that had a blob analyzer.  the string length was not correct for the enabled features.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   14 Sep 2005 14:33:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with calculating of Box X,Y ToolSet Source. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   02 Sep 2005 14:22:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed MaxBlobDataArraySize from 100 to 1 (avoid performance hit)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   01 Sep 2005 10:35:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added a second SetDefaultInputs call in the enableFeature method to fix the problem of the result feature not showing up in the result picker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   29 Jul 2005 12:03:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   19 Jul 2005 14:39:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for Max Blob Data Array Size
 * renamed member variables to be more understandable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   19 Jul 2005 07:32:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with Center X Y on Source when exclude failed blobs is checked.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   29 Jun 2005 10:07:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed ASSERT to not check CENTER_X_SOURCE and CENTER_Y_SOURCE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   27 Jun 2005 13:05:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   22 Jun 2005 12:35:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed compile errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   22 Jun 2005 11:36:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented array blobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   20 May 2005 07:36:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new blob features to get the center of the bounding box as releated to the Source Image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   17 Feb 2005 13:18:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   13 Feb 2004 14:28:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed BlobAnalyzer overlay figures to not crash.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   11 Feb 2004 17:30:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   03 Feb 2004 17:34:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed bug in onUpdateFigure
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   19 Dec 2003 12:29:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made change to only sort blobs that are included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   19 Dec 2003 10:27:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug to reset blob results during each iteration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   15 Dec 2003 12:43:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed blob analyzer's fill settings defaults.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   05 Dec 2003 09:57:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug to always fill blobs and to always update display figures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   04 Dec 2003 12:58:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code for Max Number of Blobs.  The value is now setable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   06 Nov 2003 07:51:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   took code out to enable save runs during the onRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   06 Nov 2003 07:40:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the layout of the dialog and changed logic based on if fill blobs is turned on
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   29 Oct 2003 14:06:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new value objects for FillBlobs, BlobColor and BlobType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   11 Jul 2003 07:57:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated BuildFeatureListID and CreateObject method to handle the change in where the display object is stored.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   17 Apr 2003 16:54:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   29 Jan 2003 11:43:56   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated onRun and onUpdateFigure to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   10 Jan 2003 13:44:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   09 Dec 2002 18:59:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Nov 2002 10:48:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * Changed ValueObject semantics to work with buckets
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   03 Apr 2002 14:26:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  284
 * SCR Title:  Blob Analyzer has bogus result objects for Box X Min/Max and Box Y Min/Max
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code in SVBlobAnalyzerClass::CreateObject to save Box X/Y Min/Max result object status into persistance string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   30 Jan 2002 16:10:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  278
 * SCR Title:  Error message is displayed when removing result objects used by other tools
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified ::OnValidate to not call the SVErrorClass::Trap error macro which results in a nebulous message box. Instead the error code is just checked and the tool marked invalid if there was an error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   13 Aug 2001 07:46:24   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  219
 * SCR Title:  Fix Dr. Watson error with Blob Analyzer when the SVIM goes on-line.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the "onRun" function so that blobs that are out
 * of range are deleted rather then excluded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   31 Jul 2001 12:48:48   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  219
 * SCR Title:  Fix Dr. Watson error with Blob Analyzer when the SVIM goes on-line.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified code in "onRun" function so that limiting selected blobs
 * to 100 functions as required.   Also added debug code to the 
 * functions "SortBlobs" and "MapQuickSort".  The only difference
 * between this version of this file and the previous version is the 
 * file header info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   31 Jul 2001 11:35:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  219
 * SCR Title:  Fix Dr. Watson error with Blob Analyzer when the SVIM goes on-line.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   10 May 2001 15:24:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * onUpdateFigure
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   27 Jul 2000 13:16:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  171
 * SCR Title:  Fix Blob Analyzer Memory Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix run mode and test mode to remove all blobs but first from display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   26 Jul 2000 14:19:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  171
 * SCR Title:  Fix Blob Analyzer Memory Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated onUpdateFigure to remove the allocation and deallocation of memory during test mode and run mode.
 * 
 * Removed call to onUpdateFigure in onRun function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   27 Apr 2000 08:34:36   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  131
 * SCR Title:  Bug fixes for blob analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Hard coded the Error Ids.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2000 18:09:36   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The structure SVBlobFeature Constants was changed;
 *   "chain" results were removed.
 *   SVBlobXMinAtXMinObjectGuid  was changed to 
 *     SVBlobYMaxAtXMinObjectGuid.
 * 
 * SVBlobFeatureClass was removed and SVBlobFeatureEnum was enabled, affecting the following functions:
 *   SVBlobAnalyzerClass::init ()
 *   SVBlobAnalyzerClass::~SVBlobAnalyzerClass ()
 *   SVBlobAnalyzerClass::AllocateResult ()
 *   SVBlobAnalyzerClass::FreeResult ()
 *   SVBlobAnalyzerClass::GetResultObject ()
 *   SVBlobAnalyzerClass::CreateObject ()
 *   SVBlobAnalyzerClass::EnableFeature ()
 *   SVBlobAnalyzerClass::DisableFeature ()
 *   SVBlobAnalyzerClass::OnRun ()
 *   SVBlobAnalyzerClass::BuildFeatureIDList ()
 *   SVBlobAnalyzerResultDlg::OnInitDialog ()
 * 
 * 
 * 
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   14 Apr 2000 10:01:34   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  123, 122
 * SCR Title:  Add Grahpical Interface to show all blobs found
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added blob feature 'Center of Gravity X'.
 * Added a result class object for the Number of Blobs found and added the functions 'AllocateBlobResult' and 'GetBlobResultObject' for managing the class.
 * Modified 'CreateObject' to make blob features Box X Max, Box X Min, Box Y Max and Box Y Min enabled all the time and also to set a range for the NbrOfBlobs.
 * Modified 'onRun' to exclude or include failed blobs according to user choice.
 * Added functions 'onUpdateFigure', 'IsPtOverResult' and 'DisplayAnalyzerResult'.
 * Added a new Dialog class 'SVBlobAnalyzerResultDlg'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 16 2000 16:17:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to add 26 more Blob features that SVObserver can calculate using the MIL 6.0 library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:05:42   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Nov 16 1999 13:20:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed unnecessary #include "SVBlobAnalyzerResult.h".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   Nov 16 1999 11:07:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed M_SUM_PIXEL blob feature which caused a 'crash' on binarized images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Nov 10 1999 12:25:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  57
 * SCR Title:  Version 3.00 Beta 18 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Document dirty flags set when dialogs OnOK button pressed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   20 Oct 1999 18:07:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised DisableFeatures to notify Objects using this output
 * that it is no longer available.
 * Revised DisableFeatures to reset objectAttributesSet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Oct 1999 09:12:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to correct problem in CloseObject where MIL
 * handles were not cleared after freeing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   13 Oct 1999 09:34:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to corrected problem when no features selected.
 * (Check SortFeature for -1 before calling Sort)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Oct 1999 09:10:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to handle No Blobs Encountered
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Oct 1999 10:56:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Call to SVIPDoc::SetDefaultInputs() after sending
 * SVM_DESTROY_CHILD_OBJECT message to refresh
 * dependencies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Oct 1999 08:51:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Embedded object for sort direction.
 * Corrected Sort problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 1999 15:06:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Corrected improperly dimensioned sortMap variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 1999 11:06:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to call BuildFeatureListID from EnableFeature and
 * DisableFeature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Sep 1999 10:28:10   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Finishing up Blob Analyzer.  Bringing sort feature out to the operator.  
 * 
 * Still buggy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 17:25:44   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Sep 1999 22:04:28   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Massive overhaul for new storage(scripting) techniques.  Should be pretty much done.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
