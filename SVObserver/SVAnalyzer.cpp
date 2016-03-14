//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzer.cpp
//* .File Name       : $Workfile:   SVAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVImageLibrary/SVDrawContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVAnalyzerClass, SVAnalyzerClassGuid );

SVAnalyzerClass::SVAnalyzerClass( LPCSTR ObjectName )
				:SVTaskObjectListClass( ObjectName ) 
{
	init();
}

SVAnalyzerClass::SVAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				:SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

// Initialization of newly Instantiated Object
void SVAnalyzerClass::init()
{
	pAnalyzerResult			= NULL;
	isDirty					= FALSE;

	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;

	// Set up the Defualt Inputs/Outputs
	addDefaultInputObjects();

}

SVAnalyzerClass::~SVAnalyzerClass()
{

}

BOOL SVAnalyzerClass::CloseObject()
{
	return SVTaskObjectListClass::CloseObject();
}

BOOL SVAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		if( GetInspection() && GetTool() )
		{
			isDirty	= FALSE;

			bOk = TRUE;
		}
	}

	isCreated = bOk;

	return bOk;
}

void SVAnalyzerClass::MakeDirty()
{
	isDirty = TRUE;
}

SVResultClass* SVAnalyzerClass::GetResultObject()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVResultObjectType;
	SVResultClass* pResult = reinterpret_cast<SVResultClass*>(SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ) );
	return pResult;
}

DWORD_PTR SVAnalyzerClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	SVAnalyzerLevelCreateStruct createStruct;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo        = this;
			createStruct.AnalyzerObjectInfo		= this;
			createStruct.ToolObjectInfo			= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

		case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( ConnectObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) != S_OK )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo        = this;
			createStruct.AnalyzerObjectInfo		= this;
			createStruct.ToolObjectInfo			= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

		case SVMSGID_CREATE_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			return CreateChildObject(pChildObject, static_cast<DWORD>(DwMessageContext));
		}

		case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
			//@warning [MZA, 21.07.15] The DWORD_PTR DwMessageValue has to be converted via reinterpret_cast to SVObjectClass*. This is dangerous.
			//After the reinterpret_cast to SVOjectClass*, to use a kind_of or dynamic_cast does not help for more safety, because it will not check anything.
			//We should cleanup this and replace processMessage with other methods (see SVO-806)
			createStruct.OwnerObjectInfo        = this;
			createStruct.AnalyzerObjectInfo		= this;
			createStruct.ToolObjectInfo			= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
		}

		case SVMSGID_DISCONNECT_IMAGE_OBJECT:
		{
			SVAnalyzerClass * l_pAnalyzer = reinterpret_cast<SVAnalyzerClass *>(DwMessageValue);
			l_pAnalyzer->DisconnectImages();
			return SVMR_SUCCESS;
		}
	}
	return( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

DWORD_PTR SVAnalyzerClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.AnalyzerObjectInfo		= this;
	createStruct.ToolObjectInfo			= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();
	
	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
}

SV_IMPLEMENT_CLASS( SVImageAnalyzerClass, SVImageAnalyzerClassGuid );

SVImageAnalyzerClass::SVImageAnalyzerClass( LPCSTR ObjectName )
					 :SVAnalyzerClass( ObjectName ) 
{
	init();
}

SVImageAnalyzerClass::SVImageAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					 :SVAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVImageAnalyzerClass::init()
{
	// Set object type info...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	// Set sub type only in derived classes!

	// Set Input requirement
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ImageAnalyzerImage" ) );

	addDefaultInputObjects();
}

SVImageAnalyzerClass::~SVImageAnalyzerClass()
{
}

BOOL SVImageAnalyzerClass::CloseObject()
{
	return SVAnalyzerClass::CloseObject();
}

BOOL SVImageAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}

SVImageClass* SVImageAnalyzerClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

unsigned long SVImageAnalyzerClass::GetInputPixelDepth ()
{
    SVImageClass *pImage;
    SVImageInfoClass ImageInfo; 

    unsigned long   ulPixelDepth;

//  Calling function must clear last error.

    while (1)
    {
        pImage = getInputImage ();
        if (!pImage)
        {
            msvError.msvlErrorCd = -1101;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1101);
        }

        ulPixelDepth = pImage->getPixelDepth();
        break;
    }

    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
        return msvError.GetLastErrorCd ();

    return ulPixelDepth;
}

BOOL SVImageAnalyzerClass::OnValidate()
{
	BOOL l_bOk = TRUE;
		

	l_bOk = l_bOk && inputImageObjectInfo.IsConnected();
	l_bOk = l_bOk && inputImageObjectInfo.GetInputObjectInfo().PObject != NULL;
	l_bOk = l_bOk && inputImageObjectInfo.GetInputObjectInfo().PObject->IsValid();

  return l_bOk;
}

