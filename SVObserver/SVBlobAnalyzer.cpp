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
#include "Definitions/Color.h"

#include "SVInspectionProcess.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVResultDouble.h"
#include "SVResultLong.h"
#include "SVRange.h"
#include "SVGlobal.h"
#include "InspectionEngine/SVTool.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations

struct SVBlobFeatureConstant BlobFeatureConstants[]=
{
	{SVEBlobArea /*M_AREA*/, SVBlobFeatureGuids[SvOi::SV_AREA], IDS_OBJECTNAME_AREA},
	{SVEBlobBoxXMax /*M_BOX_X_MAX*/, SVBlobFeatureGuids[SvOi::SV_BOXX_MAX], IDS_OBJECTNAME_BOXXMAX},
	{SVEBlobBoxXMin /*M_BOX_X_MIN*/, SVBlobFeatureGuids[SvOi::SV_BOXX_MIN], IDS_OBJECTNAME_BOXXMIN},
	{SVEBlobBoxYMax /*M_BOX_Y_MAX*/, SVBlobFeatureGuids[SvOi::SV_BOXY_MAX], IDS_OBJECTNAME_BOXYMAX},
	{SVEBlobBoxYMin /*M_BOX_Y_MIN*/, SVBlobFeatureGuids[SvOi::SV_BOXY_MIN], IDS_OBJECTNAME_BOXYMIN},
	{SVEBlobBreadth /*M_BREADTH*/, SVBlobFeatureGuids[SvOi::SV_BREADTH], IDS_OBJECTNAME_BREADTH},
	{SVEBlobCenterOfGravityX /*M_CENTER_OF_GRAVITY_X*/, SVBlobFeatureGuids[SvOi::SV_CENTEROFGRAVITY_X], IDS_OBJECTNAME_CENTEROFGRAVITYX},
	{SVEBlobCenterOfGravityY /*M_CENTER_OF_GRAVITY_Y*/, SVBlobFeatureGuids[SvOi::SV_CENTEROFGRAVITY_Y], IDS_OBJECTNAME_CENTEROFGRAVITYY},
	{SVEBlobConvexParameter /*M_CONVEX_PERIMETER*/, SVBlobFeatureGuids[SvOi::SV_CONVEX_PERIMETER], IDS_OBJECTNAME_CONVEXPERIMETER},
	{SVEBlobFeretElongation /*M_FERET_ELONGATION*/, SVBlobFeatureGuids[SvOi::SV_FERET_ELONGATION], IDS_OBJECTNAME_FERETELONGATION},
	{SVEBlobFeretMaxAngle /*M_FERET_MAX_ANGLE*/, SVBlobFeatureGuids[SvOi::SV_FERETMAX_ANGLE], IDS_OBJECTNAME_FERETMAXANGLE},
	{SVEBlobFeretMaxDia /*M_FERET_MAX_DIAMETER*/, SVBlobFeatureGuids[SvOi::SV_FERETMAX_DIAMETER], IDS_OBJECTNAME_FERETMAXDIAMETER},
	{SVEBlobFeretMeanDia/*M_FERET_MEAN_DIAMETER*/, SVBlobFeatureGuids[SvOi::SV_FERETMEAN_DIAMETER], IDS_OBJECTNAME_FERETMEANDIAMETER},
	{SVEBlobFeretMinAngle /*M_FERET_MIN_ANGLE*/, SVBlobFeatureGuids[SvOi::SV_FERETMIN_ANGLE], IDS_OBJECTNAME_FERETMINANGLE},
	{SVEBlobFeretMinDia /*M_FERET_MIN_DIAMETER*/, SVBlobFeatureGuids[SvOi::SV_FERETMIN_DIAMETER], IDS_OBJECTNAME_FERETMINDIAMETER},
	{SVEBlobFeretX /*M_FERET_X*/, SVBlobFeatureGuids[SvOi::SV_FERET_X], IDS_OBJECTNAME_FERETX},
	{SVEBlobFeretY /*M_FERET_Y*/, SVBlobFeatureGuids[SvOi::SV_FERET_Y], IDS_OBJECTNAME_FERETY},
	{SVEBlobFirstPointX /*M_FIRST_POINT_X*/, SVBlobFeatureGuids[SvOi::SV_FIRSTPOINT_X], IDS_OBJECTNAME_FIRSTPOINTX},
	{SVEBlobFirstPointY /*M_FIRST_POINT_Y*/, SVBlobFeatureGuids[SvOi::SV_FIRSTPOINT_Y], IDS_OBJECTNAME_FIRSTPOINTY},
	{SVEBlobLabelValue /*M_LABEL_VALUE*/, SVBlobFeatureGuids[SvOi::SV_LABEL], IDS_OBJECTNAME_LABEL},
	{SVEBlobLength /*M_LENGTH*/, SVBlobFeatureGuids[SvOi::SV_LENGTH], IDS_OBJECTNAME_LENGTH},
	{SVEBlobNumberOfHoles /*M_NUMBER_OF_HOLES*/, SVBlobFeatureGuids[SvOi::SV_NBROF_HOLES], IDS_OBJECTNAME_NBROFHOLES},
	{SVEBlobPerimeter /*M_PERIMETER*/, SVBlobFeatureGuids[SvOi::SV_PERIMETER], IDS_OBJECTNAME_PERIMETER},
	{SVEBlobRoughness /*M_ROUGHNESS*/, SVBlobFeatureGuids[SvOi::SV_ROUGHNESS], IDS_OBJECTNAME_ROUGHNESS},
	{SVEBlobSumPixel /*M_SUM_PIXEL*/, SVBlobFeatureGuids[SvOi::SV_SUM_PIXEL], IDS_OBJECTNAME_SUMPIXEL},
	{SVEBlobCompactness /*M_COMPACTNESS*/, SVBlobFeatureGuids[SvOi::SV_COMPACTNESS], IDS_OBJECTNAME_BLOB_COMPACTNESS},
	{SVEBlobNumberOfRuns /*M_NUMBER_OF_RUNS*/, SVBlobFeatureGuids[SvOi::SV_NBR_RUNS], IDS_OBJECTNAME_NUMBER_OF_RUNS},
	{SVEBlobXMinAtYMin /*M_X_MIN_AT_Y_MIN*/, SVBlobFeatureGuids[SvOi::SV_XMINAT_YMIN], IDS_OBJECTNAME_XMIN_AT_YMIN},
	{SVEBlobXMaxAtYMax /*M_X_MAX_AT_Y_MAX*/, SVBlobFeatureGuids[SvOi::SV_XMAXAT_YMAX], IDS_OBJECTNAME_XMAX_AT_YMAX},
	{SVEBlobYMinAtXMax /*M_Y_MIN_AT_X_MAX*/, SVBlobFeatureGuids[SvOi::SV_YMINAT_XMAX], IDS_OBJECTNAME_YMIN_AT_XMAX},
	{SVEBlobYMaxAtXMin /*M_Y_MAX_AT_X_MIN*/, SVBlobFeatureGuids[SvOi::SV_YMAXAT_XMIN], IDS_OBJECTNAME_YMAX_AT_XMIN},
	{SVEBlobElongation /*M_ELONGATION*/, SVBlobFeatureGuids[SvOi::SV_ELONGATION], IDS_OBJECTNAME_BLOB_ELONGATION},
	{SVEBlobIntercept /*M_INTERCEPT_0*/, SVBlobFeatureGuids[SvOi::SV_INTERCEPT_0], IDS_OBJECTNAME_BLOB_INTERCEPT0},
	{SVEBlobIntercept45 /*M_INTERCEPT_45*/, SVBlobFeatureGuids[SvOi::SV_INTERCEPT_45], IDS_OBJECTNAME_BLOB_INTERCEPT45},
	{SVEBlobIntercept90 /*M_INTERCEPT_90*/, SVBlobFeatureGuids[SvOi::SV_INTERCEPT_90], IDS_OBJECTNAME_BLOB_INTERCEPT90},
	{SVEBlobIntercept135 /*M_INTERCEPT_135*/, SVBlobFeatureGuids[SvOi::SV_INTERCEPT_135], IDS_OBJECTNAME_BLOB_INTERCEPT135},
	{SVEBlobMomentX0Y1 /*M_MOMENT_X0_Y1*/, SVBlobFeatureGuids[SvOi::SV_MOMENT_X0Y1], IDS_OBJECTNAME_BLOB_MOMENT_X0Y1},
	{SVEBlobMomentX1Y0 /*M_MOMENT_X1_Y0*/, SVBlobFeatureGuids[SvOi::SV_MOMENT_X1Y0], IDS_OBJECTNAME_BLOB_MOMENT_X1Y0},
	{SVEBlobMomentX1Y1 /*M_MOMENT_X1_Y1*/, SVBlobFeatureGuids[SvOi::SV_MOMENT_X1Y1], IDS_OBJECTNAME_BLOB_MOMENT_X1Y1},
	{SVEBlobMomentX0Y2 /*M_MOMENT_X0_Y2*/, SVBlobFeatureGuids[SvOi::SV_MOMENT_X0Y2], IDS_OBJECTNAME_BLOB_MOMENT_X0Y2},
	{SVEBlobMomentX2Y0 /*M_MOMENT_X2_Y0*/, SVBlobFeatureGuids[SvOi::SV_MOMENT_X2Y0], IDS_OBJECTNAME_BLOB_MOMENT_X2Y0},
	{SVEBlobMomentCentralX0Y2 /*M_MOMENT_CENTRAL_X0_Y2*/, SVBlobFeatureGuids[SvOi::SV_CENTRAL_X0Y2], IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X0Y2},
	{SVEBlobMomentCentralX2Y0 /*M_MOMENT_CENTRAL_X2_Y0*/, SVBlobFeatureGuids[SvOi::SV_CENTRAL_X2Y0], IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X2Y0},
	{SVEBlobMomentCentralX1Y1 /*M_MOMENT_CENTRAL_X1_Y1*/, SVBlobFeatureGuids[SvOi::SV_CENTRAL_X1Y1], IDS_OBJECTNAME_BLOB_MOMENT_CENTRAL_X1Y1},
	{SVEBlobAxisPrincipalAngle /*M_AXIS_PRINCIPAL_ANGLE*/, SVBlobFeatureGuids[SvOi::SV_AXISPRINCIPAL_ANGLE], IDS_OBJECTNAME_BLOB_AXIS_PRINCIPAL_ANGLE},
	{SVEBlobAxisSecondaryAngle /*M_AXIS_SECONDARY_ANGLE*/, SVBlobFeatureGuids[SvOi::SV_AXISSECONDARY_ANGLE], IDS_OBJECTNAME_BLOB_AXIS_SECONDARY_ANGLE},
	{SVEBlobEulerNumber /*M_EULER_NUMBER*/, SVBlobFeatureGuids[SvOi::SV_EULER_NBR], IDS_OBJECTNAME_BLOB_EULER_NUMBER},
	{SV_M_CENTER_BOUNDING_BOX_X, SVBlobFeatureGuids[SvOi::SV_CENTER_X_SOURCE], IDS_OBJECTNAME_CENTER_X_SOURCE},
	{SV_M_CENTER_BOUNDING_BOX_Y, SVBlobFeatureGuids[SvOi::SV_CENTER_Y_SOURCE], IDS_OBJECTNAME_CENTER_Y_SOURCE}
};

SV_IMPLEMENT_CLASS( SVBlobAnalyzerClass, SVBlobAnalyzerClassGuid );
#pragma endregion Declarations

SVBlobAnalyzerClass::SVBlobAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass(POwner, StringResourceID ) 
, m_pResultTable( nullptr )
{
	init();
}

void SVBlobAnalyzerClass::init()
{
	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		m_ResultTableColumnValueObjects[i] = nullptr;
	}

	m_lNumberOfBlobsFound = 0;
	m_lNumberOfBlobsToProcess = 0;
	m_pResultBlob = nullptr;

	//Indentify our output type.
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVBlobAnalyzerObjectType;

	//Register embedded objects.
	RegisterEmbeddedObject(
		&m_PersistantFeaturesEnabled, 
		SVBlobEnabledFeaturesObjectGuid,
		IDS_OBJECTNAME_ENABLEDFEATURES,
		false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(
		&m_lvoBlobSampleSize, 
		SVNbrOfBlobsObjectGuid,
		IDS_OBJECTNAME_NBROFBLOBS,
		false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_lvoMaxBlobDataArraySize, 
		SVMaxBlobDataArraySizeObjectGuid,
		IDS_OBJECTNAME_MAX_BLOB_DATA_ARRAY_SIZE,
		false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_SortFeature, 
		SVSortFeatureObjectGuid,
		IDS_OBJECTNAME_SORTFEATURE,
		false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(
		&m_lvoNumberOfBlobsFound, 
		SVNbrOfBlobsFoundObjectGuid,
		IDS_OBJECTNAME_NBROFBLOBSFOUND,
		false, SvOi::SVResetItemNone );
	m_lvoNumberOfBlobsFound.setSaveValueFlag(false);

	RegisterEmbeddedObject(
		&m_SortAscending, 
		SVSortAscendingObjectGuid,
		IDS_OBJECTNAME_SORTASCENDING,
		false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(
		&m_bExcludeFailed, 
		SVExcludeFailedObjectGuid,
		IDS_OBJECTNAME_EXCLUDEFAILED,
		false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(
		&m_bvoFillBlobs,
		SVBlobUseFillGuid,
		IDS_BLOB_USE_FILL,
		false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_colorBlobEnumValue,
		SVBlobColorGuid,
		IDS_BLACK_BLOBS,
		false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(
		&m_evoBlobFillColor,
		SVBlobFillColorGuid,
		IDS_BLOB_FILL_COLOR,
		false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(
		&m_evoBlobType,
		SVBlobFillTypeGuid,
		IDS_BLOB_FILL_TYPE,
		false, SvOi::SVResetItemNone );

	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		RegisterEmbeddedObject(
			&m_Value[i], 
			BlobFeatureConstants[i].rEmbeddedID,
			BlobFeatureConstants[i].NewStringResourceID,
			false, SvOi::SVResetItemNone );

		m_Value[i].SetDefaultValue(0, true);
		m_Value[i].setSaveValueFlag(false);
		m_Value[i].setSaveDefaultValueFlag(true);
		m_Value[i].SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);

		m_guidResults[i] = GUID_NULL;
	}

	m_SortAscending.SetDefaultValue(BOOL(false), true);
	m_bExcludeFailed.SetDefaultValue(BOOL(false), true);

	//Default Features all disabled
	std::string FeaturesEnabled;
	FeaturesEnabled.resize(SvOi::SV_NUMBER_OF_BLOB_FEATURES, _T('0'));
	m_PersistantFeaturesEnabled.SetDefaultValue (FeaturesEnabled, true);
	/*--- End of FEATURE LIST. -------------------------------------------------*/

	// Set default inputs and outputs
	addDefaultInputObjects();

	m_lvoBlobSampleSize.SetDefaultValue (SvOi::SV_MAX_NUMBER_OF_BLOBS, true);
	m_lvoMaxBlobDataArraySize.SetDefaultValue(1, true);

	m_SortFeature.SetDefaultValue (SvOi::SV_AREA, true);

	CreateArray();

	m_colorBlobEnumValue.SetEnumTypes(g_strBlobColorEnums);
	m_colorBlobEnumValue.SetDefaultValue(SV_BLOB_WHITE, true);
	//set default values for the BlobFill value objects
	m_bvoFillBlobs.SetDefaultValue(BOOL(false), true);
	m_evoBlobFillColor.SetEnumTypes(g_strBlobFillColorEnums);
	m_evoBlobFillColor.SetDefaultValue(SV_BLOB_FILL_BLACK, true);
	m_evoBlobType.SetEnumTypes(g_strBlobFillTypeEnums);
	m_evoBlobType.SetDefaultValue(SV_BLOB_FILL_EXCLUDED, true);
}

/////////////////////////////////////////////////////////////////////////////
//
//
SVBlobAnalyzerClass::~SVBlobAnalyzerClass()
{
	CloseObject ();
}

#pragma region IBlobAnalyzer
SvOi::NameValueVector SVBlobAnalyzerClass::getFeatureList(bool isSelected) const
{
	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
	assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

	SvOi::NameValueVector list;
	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{

		if ( (_T('1') == FeaturesEnabled[i] && isSelected) || (_T('0') == FeaturesEnabled[i] && !isSelected))
		{
			//
			// Skip the M_SUM_PIXEL blob feature if doing 'available' list.
			//
			if(SvOi::SV_SUM_PIXEL == i  && !isSelected)
			{
				continue;
			}
			list.push_back(SvOi::NameValuePair{ m_Value[i].GetName(), i });
		}
	}
	return list;
}

SvOi::IObjectClass* SVBlobAnalyzerClass::getResultBlob()
{ 
	return dynamic_cast<SvOi::IObjectClass*> (m_pResultBlob);
}

#pragma endregion IBlobAnalyzer

bool SVBlobAnalyzerClass::CloseObject()
{
    SVImageAnalyzerClass::CloseObject ();
	
	SVMatroxBlobInterface::DestroyResult(m_ResultBufferID);
	SVMatroxBlobInterface::DestroyContext(m_BlobContextID);

    return true;
}

DWORD SVBlobAnalyzerClass::AllocateResult(int FeatureIndex)
{
	SVClassInfoStruct       resultClassInfo;
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

	SVDoubleResultClass*    pResult(nullptr);
	DWORD LastError(0);


	//not a MIL Feature...  Just return
	if ((FeatureIndex == SvOi::SV_CENTER_X_SOURCE) || (FeatureIndex == SvOi::SV_CENTER_Y_SOURCE))
	{
		return 0;
	}

	// Setup the result

	// Declare Input Interface of Result...
	interfaceInfo.EmbeddedID = m_Value[FeatureIndex].GetEmbeddedID();
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
	std::string Title = m_Value[FeatureIndex].GetName();
	resultClassInfo.m_ClassName += _T(" ") + Title;

	// Construct the result class
	pResult = dynamic_cast<SVDoubleResultClass*> (resultClassInfo.Construct());
	m_guidResults[FeatureIndex] = pResult->GetUniqueObjectID();

	if(!pResult)
	{	
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16111, GetUniqueObjectID());
		return -SvStl::Err_16111;
	}

	Add(pResult);

	SvDef::SVObjectTypeInfoStruct info;
	info.ObjectType = SvDef::SVValueObjectType;
	info.SubType = SvDef::SVDoubleValueObjectType;
	info.EmbeddedID = SVValueObjectGuid;

	SVDoubleValueObjectClass* pValue = dynamic_cast<SVDoubleValueObjectClass*>( getFirstObject( info ) );

	if (!pValue)
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16112, GetUniqueObjectID());
		return -SvStl::Err_16112 ; 
	}

	pValue->SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );

	// Ensure this Object's inputs get connected
	pResult->ConnectAllInputs();

	// And last - Create (initialize) it

	if( !pResult->IsCreated() )
	{
		// And finally try to create the child object...
		if( !CreateChildObject(pResult) )
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10041 ); 
					
			// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
			GUID objectID = pResult->GetUniqueObjectID();
			if( GUID_NULL != objectID )
			{
				Delete( objectID );
			}
			else
			{
				delete pResult;
			}
		}
	}

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
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

//    SVDoubleResultClass*    pResult;

	DWORD LastError(0);
	
	do
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_lvoNumberOfBlobsFound.GetEmbeddedID();
		resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );
		
		resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultLongObjectType;
		resultClassInfo.m_ClassId = SVLongResultClassGuid;
		resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
		std::string Title = m_lvoNumberOfBlobsFound.GetName();
		resultClassInfo.m_ClassName += _T(" ") + Title;
		
		// Construct the result class
		m_pResultBlob = (SVLongResultClass *) resultClassInfo.Construct();
		
		if(!m_pResultBlob)
		{
			SvStl::MessageMgrStd  Ex(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Ex.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16113, GetUniqueObjectID());
			LastError =   -SvStl::Err_16113 ; 
			break;
		}
		
		Add( m_pResultBlob );
		
		SvDef::SVObjectTypeInfoStruct info;
		info.SubType = SvDef::SVLongValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		SVLongValueObjectClass* pValue = dynamic_cast<SVLongValueObjectClass*>( getFirstObject( info ) );

		if (!pValue)
		{		
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16114, GetUniqueObjectID());
			LastError = -SvStl::Err_16114 ; 
			break;
		}
		
		pValue->SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		
		// Ensure this Object's inputs get connected
		m_pResultBlob->ConnectAllInputs();
		
		// And last - Create (initialize) it
		
		if( ! m_pResultBlob->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(m_pResultBlob) )
			{
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_BlobAnalyzer_ResultCreationFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10042, GetUniqueObjectID() ); 
				
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = m_pResultBlob->GetUniqueObjectID();
				if( GUID_NULL != objectID )
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

void SVBlobAnalyzerClass::FreeResult(int FeatureIndex)
{
	SvOi::IObjectClass* pResult = getResultObject(FeatureIndex);

	if (nullptr != pResult)
	{
		m_guidResults[FeatureIndex] = GUID_NULL;
		DestroyChildObject(dynamic_cast<SVTaskObjectClass*>(pResult), SvDef::SVMFSetDefaultInputs);
		pResult = nullptr;
	}
}

SvOi::IObjectClass* SVBlobAnalyzerClass::getResultObject(int Feature)
{
	if (0 <= Feature && SvOi::SV_NUMBER_OF_BLOB_FEATURES > Feature)
	{
		return dynamic_cast<IObjectClass*> (SVObjectManagerClass::Instance().GetObject(m_guidResults[Feature]));
	}
	return nullptr;
}

void SVBlobAnalyzerClass::RebuildResultObjectArray()
{
	SvOl::SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;

	SvOl::SVInObjectInfoStruct*	pResultInputInfo;

	SVDoubleResultClass*    pResult;
	SVObjectClass*          pSVObject;
	
	SvDef::SVObjectTypeInfoStruct info;

	info.ObjectType = SvDef::SVResultObjectType;
	info.SubType = SvDef::SVResultDoubleObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		pResult = dynamic_cast< SVDoubleResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		pResult->GetPrivateInputList( resultInputList );

		pResultInputInfo = resultInputList[0];

		pSVObject = pResultInputInfo->GetInputObjectInfo().getObject();

		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if (&m_Value[i] == pSVObject)
			{
				m_guidResults[i] = pResult->GetUniqueObjectID();
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
	SvOl::SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;

	SvOl::SVInObjectInfoStruct*	pResultInputInfo;
	SVLongResultClass*    pResult = nullptr;
	SVObjectClass*          pSVObject;

	SvDef::SVObjectTypeInfoStruct info;

	info.ObjectType = SvDef::SVResultObjectType;
	info.SubType = SvDef::SVResultLongObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		pResult = dynamic_cast< SVLongResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		pResult->GetPrivateInputList( resultInputList );

		pResultInputInfo = resultInputList[0];

		pSVObject = pResultInputInfo->GetInputObjectInfo().getObject();

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
bool SVBlobAnalyzerClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result{ true };
	std::string FeaturesEnabled;

	do
	{
		if(!SVImageAnalyzerClass::CreateObject(rCreateStructure))
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16116, GetUniqueObjectID());
			Result = false;
			break;
		}

		m_pResultTable = dynamic_cast<TableObject*>(SvOi::FindObject(GetUniqueObjectID(), SvDef::SVObjectTypeInfoStruct(SvDef::TableObjectType, SvDef::SVNotSetSubObjectType)));
		if (nullptr == m_pResultTable)
		{
			m_pResultTable = new TableObject(this);
			Add(m_pResultTable);
			if (!CreateChildObject(m_pResultTable))
			{
				Result = false;
				SvStl::MessageContainer message;
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
				Msg.setMessage(message.getMessage());
				break;
			}
		}

		m_PersistantFeaturesEnabled.GetValue(FeaturesEnabled);
		if (SvOi::SV_NUMBER_OF_BLOB_FEATURES > static_cast<int> (FeaturesEnabled.size()))
		{

			FeaturesEnabled.resize(SvOi::SV_NUMBER_OF_BLOB_FEATURES, '0');
			m_PersistantFeaturesEnabled.SetValue(FeaturesEnabled);
		}
		
		m_lvoBlobSampleSize.GetValue( m_lBlobSampleSize );
		m_lvoMaxBlobDataArraySize.GetValue( m_lMaxBlobDataArraySize );
		
		//--- It is assumed, in the body of the class, that only enabled features
		//--- are registered.  All features needed to be register before now so that
		//--- the scripting would have access to them.
		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if (FeaturesEnabled[i] == _T('0'))
			{
				RemoveEmbeddedObject(&m_Value[i]);
			}
			m_Value[i].SetArraySize(m_lMaxBlobDataArraySize);	// no longer sample size (max number of blobs found)
		}
		
		HRESULT MatroxCode = SVMatroxBlobInterface::CreateResult(m_ResultBufferID);

		if (M_NULL == m_ResultBufferID)
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16117, GetUniqueObjectID());
			Result = false;
			break;
		}

#if SV_DESIRED_MIL_VERSION == 0x0900
		MatroxCode = SVMatroxBlobInterface::Set(m_ResultBufferID, SVEBlobIdentifier, static_cast<long>(SVValueBinary));

		BOOL l_bUseFillBlob;
		m_bvoFillBlobs.GetValue(l_bUseFillBlob);
		if (l_bUseFillBlob)
		{
			MatroxCode = SVMatroxBlobInterface::Set(m_ResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable));
		}
		else
		{
			MatroxCode = SVMatroxBlobInterface::Set(m_ResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable));
		}
#else
		MatroxCode = SVMatroxBlobInterface::CreateContext(m_BlobContextID);

		if (S_OK != MatroxCode || M_NULL == m_BlobContextID)
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16148);
			Result = false;
			break;
		}

		MatroxCode = SVMatroxBlobInterface::Set(m_BlobContextID, SVEBlobIdentifier, static_cast<long>(SVValueBinary));

		BOOL l_bUseFillBlob;
		m_bvoFillBlobs.GetValue(l_bUseFillBlob);
		if (l_bUseFillBlob)
		{
			MatroxCode = SVMatroxBlobInterface::Set(m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable));
		}
		else
		{
			MatroxCode = SVMatroxBlobInterface::Set(m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable));
		}
#endif

		// size to fit number of blobs MIL searches for
		m_vec2dBlobResults.resize(SvOi::SV_NUMBER_OF_BLOB_FEATURES, m_lBlobSampleSize);

		// Sri	04-12-00
		// Features Box X Max, Box X Min, Box Y Max and Box Y Min should always be enabled,
		// since we have to draw bounding rectangles to all the blobs found.
		for (int i = 0; i <= SvOi::SV_BOXY_MIN; i++)
		{
			if (_T('1') != FeaturesEnabled [i])
			{
				FeaturesEnabled[i] = _T('1');
				EnableFeature(i);
				
				SvOi::IObjectClass* pResult = getResultObject(i);
				if(nullptr == pResult)
				{
					SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16118, GetUniqueObjectID());
					Result = false;
					break; // Break out of for loop 
				}
				
				SVRangeClass *pRange = dynamic_cast<SVResultClass*>(pResult)->GetResultRange();
				if(nullptr == pRange)
				{
					SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
					MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16119, GetUniqueObjectID());
					Result = false;
					break; // Break out of for loop 
				}
				
				// Set some high values, so that it doesn't fail
				pRange->setHighValues(Range_defaults::highDef, Range_defaults::highDef);
			}
		}
		m_PersistantFeaturesEnabled.SetValue(FeaturesEnabled);

		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
		//    Restore selected features after recreation. msvFeatureListID get
		//    created (allocated) here.
		BuildFeatureListID();


		if ( !Result )
		{
			break; // If any error had occurred in the for loop, break out of while loop also.	
		}

		long lSortIndex;
		m_SortFeature.GetValue (lSortIndex);
		if(lSortIndex < 0)
		{
			m_SortFeature.SetValue(1L);
		}
		if(!m_pResultBlob && !(m_pResultBlob = GetBlobResultObject()))
		{
			if (AllocateBlobResult() & SV_ERROR_CONDITION)
			{
				break; // Some error has occurred.
			}
			
			SVRangeClass *pRange = m_pResultBlob->GetResultRange();
			if(!pRange)
			{
			
				SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16120, GetUniqueObjectID());
				Result = false;
				break; 
			}
			pRange->setLowValues(m_defaultResultNumberOfBlobsLowFail, m_defaultResultNumberOfBlobsLowWarn);
			pRange->setHighValues(m_defaultResultNumberOfBlobsHighFail, m_defaultResultNumberOfBlobsHighWarn);
		}
		// End. Sri
		double Value = m_Value[0].GetDefaultValue ();
		m_vec2dBlobResults.fill( Value );
		
	} while ( false );

	
	if ( !Result )
	{
		m_isCreated = false;
	}
	else
	{
		m_isCreated = true;
	}

	// Set Embedded defaults
	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		if( !m_Value[i].IsCreated() )
		{
			CreateChildObject( &m_Value[i] );
		}

		if (FeaturesEnabled[i] == '0')
		{
			m_Value[i].SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
		else
		{
			m_Value[i].SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::AddAttribute );
		}

		if ( SvDef::SV_NO_ATTRIBUTES != m_Value[i].ObjectAttributesAllowed() )
		{
			//! Required for older configurations
			m_Value[i].SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}

	//! Required for older configurations
	m_PersistantFeaturesEnabled.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lvoBlobSampleSize.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_SortFeature.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_SortAscending.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_bExcludeFailed.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_lvoNumberOfBlobsFound.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVBlobAnalyzerClass::UpdateBlobFeatures()
{
	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);

	//! Only update the features if they have changed
	if (m_PreviousFeaturesEnabled != FeaturesEnabled)
	{
		assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if (SvOi::SV_CENTER_X_SOURCE != i && SvOi::SV_CENTER_Y_SOURCE != i)
			{
				if ('1' == FeaturesEnabled[i])
				{
					EnableFeature(i);
				}
				else
				{
					m_Value[i].SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute);
					FreeResult(i);
					RemoveEmbeddedObject(&m_Value[i]);
				}
			}
		}

		dynamic_cast<SVInspectionProcess*>(GetInspection())->SetDefaultInputs();
		BuildFeatureListID();
		m_PreviousFeaturesEnabled = FeaturesEnabled;
	}
}

void SVBlobAnalyzerClass::EnableFeature(int Feature)
{
	assert(0 <= Feature && SvOi::SV_NUMBER_OF_BLOB_FEATURES > Feature);
	//! Check is Feature already in embedded list	
	SVObjectPtrVector::const_iterator Iter = std::find_if(m_embeddedList.begin(), m_embeddedList.end(), [Feature](const SVObjectPtrVector::value_type pEntry)->bool
	{
		return (pEntry->GetEmbeddedID() == BlobFeatureConstants[Feature].rEmbeddedID);
	}
	);
	if (m_embeddedList.end() == Iter)
	{
		m_Value[Feature].SetObjectAttributesAllowed(SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute);
		RegisterEmbeddedObject(&m_Value[Feature], BlobFeatureConstants[Feature].rEmbeddedID, BlobFeatureConstants[Feature].NewStringResourceID, false, SvOi::SVResetItemNone);
		AllocateResult(Feature);
	}
}

bool SVBlobAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = true;
	long lSortFeature;
	bool l_bCenterXSet = false;
	bool l_bCenterYSet = false;

	SVImageClass* pInputImage(nullptr);

	HRESULT MatroxCode;

	do
	{
		if(!__super::onRun(rRunStatus, pErrorMessages))
		{
			Result = false;
			break;
		}

		//
		// Mil library determines the blobs and count of blobs.
		//
		pInputImage = getInputImage(true);

		if (nullptr == pInputImage)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16135, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

			SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord);
			if (nullptr == pImageBuffer)
			{
				ASSERT(false);
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16136, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			if (pImageBuffer->isEmpty())
			{
				ASSERT(false);
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16137, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

		//
		// Analyze the image for blobs and features of blobs.
		//
		MatroxCode = SVMatroxBlobInterface::Execute( m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), m_BlobContextID );

		if( S_OK != MatroxCode )
		{
			ASSERT( false );
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16138, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		std::string FeaturesEnabled;
		m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
		assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

		// Sri 04-12-00
		// Check whether we want to exclude the blobs, which has at least one of its features 
		// out of Range
		BOOL	bExclude;
		m_bExcludeFailed.GetValue(bExclude);
		if(bExclude)
		{
			for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
			{
				if (_T('1') == FeaturesEnabled[i])
				{
					// check to see if the feature is SV_CENTER_X_SOURCE or SV_CENTER_Y_SOURCE
					// if so do not call the MIL functions.
					
					if ( (i == SvOi::SV_CENTER_X_SOURCE) || (i == SvOi::SV_CENTER_Y_SOURCE) )
					{
						continue;
					}
					
					SVResultClass* pResult = dynamic_cast<SVResultClass*>(getResultObject(i));
					if(!pResult)
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16139, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
						break;
					}
					SVRangeClass* pRange = pResult->GetResultRange();
					if(!pRange)
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16140, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
						break;
					}

					double dLow(0), dHigh(0);
					pRange->getValue(RangeEnum::ER_FailHigh, dHigh);
					pRange->getValue(RangeEnum::ER_FailLow, dLow);

					// Now that we have indirect high and low ranges it is possible that dLow is larger than dHigh.
					// This would cause the MIL function to return an error.  To avoid this exception, we set both to 0.
					// Desired behavior in this case is that ALL blobs are excluded.
					if( dLow > dHigh )
					{
						dLow = 0;
						dHigh = 0;
						// To exclude all blobs, also exclude any blobs which are in the range.
						MatroxCode = SVMatroxBlobInterface::BlobSelect( m_ResultBufferID, 
							SVEBlobExclude, 
							BlobFeatureConstants[i].MILFeatureDef, 
							SVECondInRange, 
							dLow, 
							dHigh );
					}

					// Exclude all blobs that are out of the range for this feature.
					MatroxCode = SVMatroxBlobInterface::BlobSelect(m_ResultBufferID, 
						SVEBlobExclude, 
						BlobFeatureConstants[i].MILFeatureDef, 
						SVECondOutRange, 
						dLow, 
						dHigh );

					if( S_OK != MatroxCode )
					{
						Result = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16141, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
						break;
					}
				}
			}
		}// end if(bExclude)
		// End. Sri

		//
		// Get the number blobs found by MblobCalculate()
		//
		MatroxCode = SVMatroxBlobInterface::GetNumber( m_ResultBufferID, m_lNumberOfBlobsFound );

		if( S_OK != MatroxCode )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16142, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
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
			MatroxCode = SVMatroxBlobInterface::BlobSelect( m_ResultBufferID,
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
			MatroxCode = SVMatroxBlobInterface::GetNumber( m_ResultBufferID, lSelectedNbrOfBlobs );

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
					MatroxCode = SVMatroxBlobInterface::BlobSelect( m_ResultBufferID,
						SVEBlobExcludeOnly,
						SVEBlobLabelValue,
						SVECondGreater,
						m_lBlobSampleSize + x,
						l_lValue);

					MatroxCode = SVMatroxBlobInterface::GetNumber( m_ResultBufferID, lSelectedNbrOfBlobs);
				}
				m_lNumberOfBlobsFound = lSelectedNbrOfBlobs;

			}
			else //if(lSelectedNbrOfBlobs > m_lBlobSampleSize)
			{
				// this is an error condition!  i.e. selected more blobs then we asked for !
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16143, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			if( S_OK != MatroxCode )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16144, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
				break;
			}
		}// end if (m_lNumberOfBlobsFound > m_lBlobSampleSize)

		m_lvoNumberOfBlobsFound.SetValue(m_lNumberOfBlobsFound);

		// EB 2005 01 28
		// add array capability to blob results
		m_lNumberOfBlobsToProcess = std::min(m_lNumberOfBlobsFound, m_lMaxBlobDataArraySize);	// no more results than max data array size
		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if (_T('1') == FeaturesEnabled[i])
			{
				m_Value[i].SetResultSize(m_lNumberOfBlobsToProcess);
			}
		}

		// do the following even if no blobs have been found.
		//if number of blobs to process = 0,  make sure the feature do not have values from previous runs.
		if (0 == m_lNumberOfBlobsToProcess)
		{
			for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
			{
				if (_T('1') == FeaturesEnabled[i])
				{
					//This sets the first blob value to 0
					m_Value[i].SetValue(0.0, 0);
				}
			}
		}
	
		// Get the array of features values for each feature selected.
		//
		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if ( _T('1') == FeaturesEnabled[i])
			{
				if (SvOi::SV_CENTER_X_SOURCE == i  || SvOi::SV_CENTER_Y_SOURCE == i) // not a MIL Feature, do not process
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

				vector2d<double>::row_type& row = m_vec2dBlobResults[i];
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
				if(SVEBlobSumPixel == BlobFeatureConstants[i].MILFeatureDef)
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
				if (m_lNumberOfBlobsFound != 0 && (SvOi::SV_CENTER_X_SOURCE != i || SvOi::SV_CENTER_Y_SOURCE != i) )
				{
					MatroxCode = SVMatroxBlobInterface::GetResult(m_ResultBufferID, BlobFeatureConstants[i]. MILFeatureDef, pData);
				}
			}// end if (msvszFeaturesEnabled [eFeature] == _T('1'))

		}// end for (eFeature = SV_AREA; eFeature < SV_NUMBER_OF_BLOB_FEATURES; eFeature = (SVBlobFeatureEnum) (eFeature + 1))
		
		if (m_lNumberOfBlobsFound != 0 && ( _T('1') == FeaturesEnabled[SvOi::SV_CENTER_X_SOURCE] || 
				_T('1') == FeaturesEnabled[SvOi::SV_CENTER_Y_SOURCE]))
		{
			double * pCenterXData = nullptr;
			bool l_bCenterXSet = false;
			if (_T('1') == FeaturesEnabled[SvOi::SV_CENTER_X_SOURCE])
			{
				vector2d<double>::row_type& row = m_vec2dBlobResults[SvOi::SV_CENTER_X_SOURCE];
				pCenterXData = &(row[0]);
				memset( pCenterXData, 0L, sizeof( double ) * row.size() );
				l_bCenterXSet = true;
			}

			double * pCenterYData = nullptr;
			bool l_bCenterYSet = false;
			if ( _T('1') == FeaturesEnabled [SvOi::SV_CENTER_Y_SOURCE])
			{
				vector2d<double>::row_type& row = m_vec2dBlobResults[SvOi::SV_CENTER_Y_SOURCE];
				pCenterYData = &(row[0]);
				memset( pCenterYData, 0L, sizeof( double ) * row.size() );
				l_bCenterYSet = true;
			}

			double * pBoxXMaxData = &(m_vec2dBlobResults[SvOi::SV_BOXX_MAX][0]);
			double * pBoxXMinData = &(m_vec2dBlobResults[SvOi::SV_BOXX_MIN][0]);
			double * pBoxYMaxData = &(m_vec2dBlobResults[SvOi::SV_BOXY_MAX][0]);
			double * pBoxYMinData = &(m_vec2dBlobResults[SvOi::SV_BOXY_MIN][0]);

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

				if (nullptr != pInputImage)
				{
					pTmpImage = pInputImage;
					while ( nullptr != pTmpImage->GetParentImage() )
					{
						pTmpImage = pTmpImage->GetParentImage();
					}
					pInputImage->TranslateFromOutputSpaceToImage(pTmpImage,ptSt,ptSt);

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

		m_SortFeature.GetValue( lSortFeature );
		m_SortVector.resize( m_lNumberOfBlobsFound );
		// Check for Sort Feature
		// Note Sort Feature will be -1 if no features have been selected
		// otherwise if at least one feature has been selected
		// lSortFeature will be a valid feature index
		if( lSortFeature >= SvOi::SV_AREA && lSortFeature < SvOi::SV_NUMBER_OF_BLOB_FEATURES)
		{
			SortBlobs( lSortFeature, m_SortVector.data(), static_cast<long> (m_SortVector.size()));
		}

		//create a sort Container with the size of founded blobs and from 0 to n
		ValueObjectSortContainer resultTableSortContainer;
		resultTableSortContainer.resize(m_lNumberOfBlobsToProcess);
		for (int i = 0; i < m_lNumberOfBlobsToProcess; i++)
		{
			//@TODO[MZA][7.50][02.08.2017] This version is for the case we want to go back to full size in the table tool
			//resultTableSortContainer[i] = msvlSortMap[i];
			resultTableSortContainer[i] = i;
		}
		m_pResultTable->setSortContainer(resultTableSortContainer, rRunStatus);

		for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
		{
			if (_T('1') == FeaturesEnabled[i])
			{
				if (m_lNumberOfBlobsToProcess > 0)
				{
					//@TODO[MZA][7.50][02.08.2017] This version is for the case we want to go back to full size in the table tool
					/*if (nullptr != m_ResultTableColumnValueObjects[eFeature])
					{
						m_ResultTableColumnValueObjects[eFeature]->SetArrayValues(m_vec2dBlobResults[eFeature]);
					}*/
					// add array capability to blob results
					for ( int iBlob = 0; iBlob < m_lNumberOfBlobsToProcess; iBlob++ )
					{
						m_ResultTableColumnValueObjects[i]->SetValue(m_vec2dBlobResults[i][m_SortVector[iBlob]], iBlob);
						m_Value[i].SetValue(m_vec2dBlobResults[i][m_SortVector[iBlob]], iBlob );
					}
				}
				else
				{
					//Arvid set array size to 1 even if no blobs are present so as not to cause errors in math tools in "old style" configurations
					//Arvid this will in practice, however, undo the changes of SVO-322
					m_Value[i].SetResultSize(1); 
					m_Value[i].SetValue(0.0, 0);
				}
			}
		}

		// Now fill the blobs
		BOOL FillBlob;
		m_bvoFillBlobs.GetValue( FillBlob );

		MatroxCode = SVMatroxBlobInterface::GetNumber( m_ResultBufferID, m_lNumberOfBlobsFound );
		if ( FillBlob )
		{
			long Color;
			long Type;
			m_evoBlobFillColor.GetValue(Color);
			m_evoBlobType.GetValue(Type);
			SVBlobControlEnum eCriterion = SVEBlobAll;
			switch( Type )
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
			}// end switch( l_lType )

			MatroxCode = SVMatroxBlobInterface::BlobFill( m_ResultBufferID, pImageBuffer->getHandle()->GetBuffer(), eCriterion, Color);
		}// end if
	} while ( false );

	if (!Result )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return Result;
}

void SVBlobAnalyzerClass::SortBlobs (long alSortFeature, 
	long* alSortMap,
	long p_lArraySize )
{
	for (int i = 0; i < p_lArraySize; i++)
	{
		alSortMap [i] = i;
	}

	BOOL ascending(false);
	m_SortAscending.GetValue(ascending);
	if (m_vec2dBlobResults[alSortFeature].size() > 0)
	{
		MapQuickSort(&(m_vec2dBlobResults[alSortFeature][0]), alSortMap, 0, p_lArraySize - 1, ascending ? true : false);                           // Ascending

	}
}

void SVBlobAnalyzerClass::MapQuickSort(double* aSortArray, long* alSortMap,	long alBeginning, long alEnd, bool abAscending)
{
	long    i = alEnd;
	long   	j = alBeginning;

	if (alEnd < 0)
	{
		//          Number of blobs is zero.  This is not a fatal error, but there is 
		//          nothing to sort.
		return;
	}

	double val = aSortArray[alSortMap[(alBeginning + alEnd) / 2]];

	do
	{
		if (abAscending)
		{
			while (aSortArray[alSortMap[j]] < val)
			{
				j++;
			}
			while (aSortArray[alSortMap[i]] > val)
			{
				i--;
			}
		}
		else
		{
			while (aSortArray[alSortMap[j]] > val)
			{
				j++;
			}
			while (aSortArray[alSortMap[i]] < val)
			{
				i--;
			}
		}

		if (i >= j)
		{
			if (i != j)
			{
				long lTemp = alSortMap[i];
				alSortMap[i] = alSortMap[j];
				alSortMap[j] = lTemp;
			}

			i--;
			j++;
		}

	} while (j <= i);

	if (alBeginning < i)
	{
		MapQuickSort(aSortArray, alSortMap,	alBeginning, i,	abAscending);
	}
	if (j < alEnd)
	{
		MapQuickSort(aSortArray, alSortMap,	j, alEnd, abAscending);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
DWORD SVBlobAnalyzerClass::BuildFeatureListID ()
{
	DWORD LastError(0);
#if SV_DESIRED_MIL_VERSION == 0x0900
	HRESULT MatroxCode = SVMatroxBlobInterface::DestroyContext(m_BlobContextID);

	if (S_OK != MatroxCode)
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
		MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16148);
		LastError = -SvStl::Err_16148;
		return LastError;

	}

	MatroxCode = SVMatroxBlobInterface::CreateContext(m_BlobContextID);

	if (M_NULL == m_BlobContextID)
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16149);
		LastError = - SvStl::Err_16149;
		return LastError;
	}
#else
	HRESULT MatroxCode = S_OK;
#endif


	std::set<SVBlobSelectionEnum> featureSet;

	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
	assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		if (_T('1') == FeaturesEnabled[i])
		{
			if ((i == SvOi::SV_CENTER_X_SOURCE) || (i == SvOi::SV_CENTER_Y_SOURCE))
			{
				continue;
			}

			featureSet.insert(BlobFeatureConstants[i].MILFeatureDef);
		}
	}

	MatroxCode = SVMatroxBlobInterface::BlobSelectFeature(m_BlobContextID, featureSet);
	if (S_OK != MatroxCode)
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
		MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16150);
		LastError = -SvStl::Err_16150;
		return LastError;
	}	

	return LastError;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsPtOverResult( CPoint point )
// -----------------------------------------------------------------------------
// .Description : When the user right clicks inside a tool figure, checks whether its
//              : on a blob. If it finds a blob at that point, gets the index of the  
//				: blob and returns true.
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
				double* pxMax = &(m_vec2dBlobResults[SvOi::SV_BOXX_MAX][0]);
				double* pxMin = &(m_vec2dBlobResults[SvOi::SV_BOXX_MIN][0]);
				double* pyMax = &(m_vec2dBlobResults[SvOi::SV_BOXY_MAX][0]);
				double* pyMin = &(m_vec2dBlobResults[SvOi::SV_BOXY_MIN][0]);

				int iMapSize = static_cast<int> (m_SortVector.size());
			
				for (int i = 0; i < (int)l_lCurrentNbrOfBlobs && i < iMapSize ; i++)
				{
					RECT l_oRect;

					long l = m_SortVector[i];

					l_oRect.top = static_cast<long>(pyMin[l]);
					l_oRect.left = static_cast<long>(pxMin[l]);
					l_oRect.bottom = static_cast<long>(pyMax[l]);
					l_oRect.right = static_cast<long>(pxMax[l]);
				
					SVExtentFigureStruct l_svFigure = l_oRect;
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

				if( S_OK == l_svFigure.IsPointOverFigure( rPoint ) )
					{
						m_nBlobIndex = m_SortVector[i]; 

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
	if(m_nBlobIndex < 0 || 	m_nBlobIndex >= static_cast<int> (m_SortVector.size()))
		return;

	SVBlobAnalyzerResultDlg	dlg;
	dlg.m_pBlobAnalyzer = this;
	dlg.DoModal();
}

void SVBlobAnalyzerClass::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
	assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		if (_T('1') == FeaturesEnabled[i])
		{
			std::string tmp = m_Value[i].GetCompleteName();
			if (1 < m_Value[i].getArraySize())
			{
				tmp += "[1]";
			}
			inserter = SvOi::ParameterPairForML(tmp, m_Value[i].GetUniqueObjectID());

			if (SvOi::SV_BOXX_MAX > i || SvOi::SV_BOXY_MIN < i)
			{
				auto* pResultObject = SVObjectManagerClass::Instance().GetObject(m_guidResults[i]);
				if (nullptr != pResultObject)
				{
					SVRangeClass* pRangeObject = dynamic_cast<SVRangeClass*>(pResultObject->getFirstObject(SvDef::SVObjectTypeInfoStruct(SvDef::SVObjectTypeEnum::SVRangeObjectType)));
					if (nullptr != pRangeObject)
					{
						pRangeObject->addEntriesToMonitorList(inserter);
					}
				}
			}
		}
	}
}

void SVBlobAnalyzerClass::CreateArray()
{
	m_lvoBlobSampleSize.GetValue(m_lBlobSampleSize);
	m_lvoMaxBlobDataArraySize.GetValue(m_lMaxBlobDataArraySize);

	m_SortVector.resize(m_lBlobSampleSize);

	m_vec2dBlobResults.resize(SvOi::SV_NUMBER_OF_BLOB_FEATURES, m_lBlobSampleSize );

	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
	assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

	// add array capability to blob results
	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++ )
	{
		assert(i < c_maxTableColumn);
		if (_T('1') == FeaturesEnabled[i])
		{
			m_Value[i].SetArraySize( m_lMaxBlobDataArraySize );	// no longer sample size (max number of blobs found)
			m_ResultTableColumnValueObjects[i] = m_pResultTable->updateOrCreateColumn(TableColumnValueObjectGuid[i], BlobFeatureConstants[i].NewStringResourceID, m_lMaxBlobDataArraySize);
		}
		else
		{
			m_Value[i].SetArraySize( 0 );
			if (nullptr != m_ResultTableColumnValueObjects[i])
			{
				m_pResultTable->removeColumn(TableColumnValueObjectGuid[i]);
				m_ResultTableColumnValueObjects[i] = nullptr;
			}
		}
	}
}

bool SVBlobAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	UpdateBlobFeatures();

	BOOL l_bIsFillBlob;
	m_bvoFillBlobs.GetValue( l_bIsFillBlob );

#if SV_DESIRED_MIL_VERSION == 0x0900
	if ( l_bIsFillBlob )
	{
		SVMatroxBlobInterface::Set( m_ResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable) );
	}
	else
	{
		SVMatroxBlobInterface::Set( m_ResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable) );
	}

	long colorBlobEnum;
	m_colorBlobEnumValue.GetValue( colorBlobEnum );
	SVMatroxBlobInterface::SetForeground( m_ResultBufferID, SV_BLOB_BLACK == colorBlobEnum );
#else
	if (l_bIsFillBlob)
	{
		SVMatroxBlobInterface::Set(m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable));
	}
	else
	{
		SVMatroxBlobInterface::Set(m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable));
	}

	long colorBlobEnum;
	m_colorBlobEnumValue.GetValue(colorBlobEnum);
	SVMatroxBlobInterface::SetForeground(m_BlobContextID, SV_BLOB_BLACK == colorBlobEnum);
#endif

	CreateArray();

	RebuildResultObjectArray();

	return Result;
}

HRESULT SVBlobAnalyzerClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray )
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
					RECT Rect;

					double Value( 0.0 );
					m_Value[SvOi::SV_BOXX_MIN].GetValue(Value, i);
					Rect.left = static_cast<long> (Value);
					m_Value[SvOi::SV_BOXX_MAX].GetValue(Value, i);
					Rect.right = static_cast<long> (Value);
					m_Value[SvOi::SV_BOXY_MIN].GetValue(Value, i);
					Rect.top = static_cast<long> (Value);
					m_Value[SvOi::SV_BOXY_MAX].GetValue(Value, i);
					Rect.bottom = static_cast<long> (Value);

					SVExtentFigureStruct l_svFigure = Rect;
					
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

					SVExtentMultiLineStruct l_multiLine;

					l_multiLine.m_Color = SvDef::DefaultSubFunctionColor1;
					
					l_multiLine.AssignExtentFigure( l_svFigure, SvDef::DefaultSubFunctionColor1 );

					UpdateOverlayIDs( l_multiLine );

					p_rMultiLineArray.push_back( l_multiLine );
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
					long l = m_SortVector[i];
					RECT l_oRect;

					l_oRect.top = static_cast<long> (pyMin[l]);
					l_oRect.left = static_cast<long> (pxMin[l]);
					l_oRect.bottom = static_cast<long> (pyMax[l]);
					l_oRect.right = static_cast<long> (pxMax[l]);

					SVExtentFigureStruct l_svFigure = l_oRect;
					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

					SVExtentMultiLineStruct l_multiLine;
					l_multiLine.m_Color = SvDef::DefaultSubFunctionColor1;

					l_multiLine.AssignExtentFigure( l_svFigure, SvDef::DefaultSubFunctionColor1 );

					UpdateOverlayIDs( l_multiLine );

					p_rMultiLineArray.push_back( l_multiLine );
				}
			}
		}
	}
	return S_OK;
}

void SVBlobAnalyzerClass::addDefaultInputObjects( SvOl::SVInputInfoListClass* PInputListToFill )
{
	__super::addDefaultInputObjects( PInputListToFill );

#ifdef _DEBUG
	std::string FeaturesEnabled;
	m_PersistantFeaturesEnabled.getValue(FeaturesEnabled);
	assert(SvOi::SV_NUMBER_OF_BLOB_FEATURES == FeaturesEnabled.size());

	for (int i = 0; i < SvOi::SV_NUMBER_OF_BLOB_FEATURES; i++)
	{
		UINT uiAttributes = m_Value[i].ObjectAttributesAllowed();
		bool l_bOk = ( _T('1') == FeaturesEnabled[i] &&
			( uiAttributes & SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES) != SvDef::SV_NO_ATTRIBUTES ) ||
			( _T('0') == FeaturesEnabled[i] &&
			(uiAttributes & SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES) == SvDef::SV_NO_ATTRIBUTES );

		// if this ASSERT fires, verify that the attributes are being set correctly!!!!!!! jms & eb 2006 01 20
		ASSERT( !IsCreated() || l_bOk || (i == SvOi::SV_CENTER_X_SOURCE || i == SvOi::SV_CENTER_Y_SOURCE) );
	}
#endif
}

