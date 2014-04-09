//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolTask
//* .File Name       : $Workfile:   SVExternalToolTask.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   08 Apr 2014 09:44:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVExternalToolTask.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOMFCLibrary/SVOINIClass.h"

#include "SVExternalTool.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVRange.h"
#include "SVTaskObject.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVValueObject.h"

SV_IMPLEMENT_CLASS( SVExternalToolTask, SVExternalToolTaskGuid )

SVExternalToolTaskData::SVExternalToolTaskData() {}

SVExternalToolTaskData::SVExternalToolTaskData(const SVExternalToolTaskData& src)
{
	*this = src;
}

const SVExternalToolTaskData& SVExternalToolTaskData::operator = (const SVExternalToolTaskData& rhs)
{
	if ( this != &rhs )
	{
		// EB 20040825 added these... why weren't they here before???
		m_voDllPath = rhs.m_voDllPath;
		m_aDllDependencies = rhs.m_aDllDependencies;


		m_voToolName = rhs.m_voToolName;
		m_voToolVersion = rhs.m_voToolVersion;

		m_aInputImageInfo = rhs.m_aInputImageInfo;

		m_aInputObjectInfo = rhs.m_aInputObjectInfo;
		
		//////////////////////////////////////////////////////
		// !! EB 20040820
		// These vectors contain registered objects which cannot be deallocated!
		// use std::copy which simply performs element assignment
		// instead of vector::operator =
		// note: operator = will work in VC6, but this is nonportable

		if ( m_aInputObjects.size() != rhs.m_aInputObjects.size() )	// if this is only a copy of the original, size will be 0
		{
			//ASSERT( m_aInputObjects.size() == 0 );
			m_aInputObjects.resize( rhs.m_aInputObjects.size() );
		}
		std::copy( rhs.m_aInputObjects.begin(), rhs.m_aInputObjects.end(), m_aInputObjects.begin() );

		if ( m_aInputObjectNames.size() != rhs.m_aInputObjectNames.size() )	// if this is only a copy of the original, size will be 0
		{
			//ASSERT( m_aInputObjectNames.size() == 0 );
			m_aInputObjectNames.resize( rhs.m_aInputObjectNames.size() );
		}
		std::copy( rhs.m_aInputObjectNames.begin(), rhs.m_aInputObjectNames.end(), m_aInputObjectNames.begin() );

		if ( m_aResultObjects.size() != rhs.m_aResultObjects.size() )	// if this is only a copy of the original, size will be 0
		{
			//ASSERT( m_aResultObjects.size() == 0 );
			m_aResultObjects.resize( rhs.m_aResultObjects.size() );
		}
		std::copy( rhs.m_aResultObjects.begin(), rhs.m_aResultObjects.end(), m_aResultObjects.begin() );

		if ( m_aResultObjectNames.size() != rhs.m_aResultObjectNames.size() )	// if this is only a copy of the original, size will be 0
		{
			//ASSERT( m_aResultObjectNames.size() == 0 );
			m_aResultObjectNames.resize( rhs.m_aResultObjectNames.size() );
		}
		std::copy( rhs.m_aResultObjectNames.begin(), rhs.m_aResultObjectNames.end(), m_aResultObjectNames.begin() );

		m_aResultImageDefinitions = rhs.m_aResultImageDefinitions;
		m_aResultValueDefinitions = rhs.m_aResultValueDefinitions;
		m_aInputValueDefinitions = rhs.m_aInputValueDefinitions;

		m_lNumInputImages = rhs.m_lNumInputImages;
		m_lNumInputValues = rhs.m_lNumInputValues;
		m_lNumResultImages = rhs.m_lNumResultImages;
		m_lNumResultValues = rhs.m_lNumResultValues;

		m_RangeResultData = rhs.m_RangeResultData;
		
		m_PropTreeState = rhs.m_PropTreeState;
	}

	return *this;
}


SVExternalToolTask::SVExternalToolTask( SVObjectClass* POwner, int StringResourceID )
				 :SVTaskObjectListClass(FALSE, POwner, StringResourceID ), ISVCancel() 
{

	outObjectInfo.ObjectTypeInfo.ObjectType = SVExternalToolTaskObjectType;
	
	size_t i;

	m_hrInitialized = S_FALSE;

	SVOINIClass m_SvimIni;

	m_SvimIni.SetFile(_T("C:\\SVObserver\\bin\\SVIM.INI"));

	m_bUseImageCopies = m_SvimIni.GetValueInt(_T("External Tool"), _T("UseImageCopy"), TRUE) != FALSE;
	
	// Initialize input images array.
	m_Data.m_aInputImageInfo.resize(SVExternalToolTaskData::NUM_INPUT_IMAGES);
	for ( i=0; i < m_Data.m_aInputImageInfo.size(); i++)
	{
		SVString l_Name;

		l_Name.Format( _T( "ExternalToolTaskImage%d" ), i );

		// this object will be filled in SVTaskObject::ConnectAllInputs
		// we supply the base object type (Image) and ConnectAllInputs finds the nearest match (Toolset main image)
		m_Data.m_aInputImageInfo[i].SetInputObjectType( SVImageObjectType );
		m_Data.m_aInputImageInfo[i].SetObject( GetObjectInfo() );
		RegisterInputObject( &m_Data.m_aInputImageInfo[i], l_Name );
	}


	RegisterEmbeddedObject( &m_Data.m_voDllPath, SVDllFileNameGuid, IDS_OBJECTNAME_DLL_PATH, false, SVResetItemTool  );

	// Initialize Dll Dependencies 
	m_Data.m_aDllDependencies.resize(SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES);
	for ( i=0; i < m_Data.m_aDllDependencies.size(); i++)
	{
		RegisterEmbeddedObject( &m_Data.m_aDllDependencies[i], aSVDllDependencyFileNameGuid[i], IDS_OBJECTNAME_DLL_DEP_FILE_01 + static_cast<int>(i), false, SVResetItemTool );
	}

	// init Tool Name
	RegisterEmbeddedObject( &m_Data.m_voToolName, SVDllToolNameGuid, IDS_OBJECTNAME_DLL_TOOL_NAME, false, SVResetItemNone );


	// Init Tool Version
	RegisterEmbeddedObject( &m_Data.m_voToolVersion, SVDllToolVersionGuid, IDS_OBJECTNAME_DLL_TOOL_VERSION, false, SVResetItemNone );

	// Init Input Object Info array
	m_Data.m_aInputObjectInfo.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);
	m_Data.m_aInputObjects.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);
	m_Data.m_aInputObjectNames.resize(SVExternalToolTaskData::NUM_INPUT_OBJECTS);
	for ( i=0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++)
	{
		// register objects
		SVString l_Name;

		l_Name.Format( _T( "ExternalToolTaskValue%d" ), i );

		m_Data.m_aInputObjectInfo[i].SetObject( GetObjectInfo() );
		RegisterInputObject( &m_Data.m_aInputObjectInfo[i], l_Name );

		RegisterEmbeddedObject( &m_Data.m_aInputObjects[i], aSVVariantInputObjectGuid[i], IDS_OBJECTNAME_INPUT_01 + static_cast<int>(i), false, SVResetItemNone );
		RegisterEmbeddedObject( &m_Data.m_aInputObjectNames[i], aSVVariantInputObjectNameGuid[i], IDS_OBJECTNAME_INPUT_01_NAME + static_cast<int>(i), false, SVResetItemNone );

		// set default values
		VARIANT vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = VT_EMPTY;
		m_Data.m_aInputObjects[i].SetDefaultValue(vtTemp, TRUE);
		m_Data.m_aInputObjectNames[i].SetDefaultValue(CString(), TRUE);
	}

	int l_pImageNames[] = { IDS_OBJECTNAME_IMAGE1, IDS_OBJECTNAME_IMAGE2, IDS_OBJECTNAME_IMAGE3, IDS_OBJECTNAME_IMAGE4 };

	// Set Default Image Parameters for Result Images
	for( i = 0 ; i < SVExternalToolTaskData::NUM_RESULT_IMAGES ; i++)
	{
		SVImageInfoClass imageInfo;

		RegisterEmbeddedObject( &m_aResultImages[i], aSVVariantResultImageObjectGuid[i], l_pImageNames[i] );

		if( GetTool() != NULL )
		{
			imageInfo.SetOwner( GetTool()->GetUniqueObjectID() );
		}

		imageInfo.SetImageProperty( SVImagePropertyPixelDepth, 8 );
		imageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 ); 

		imageInfo.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
		imageInfo.SetExtentProperty( SVExtentPropertyWidth, 100 );
		imageInfo.SetExtentProperty( SVExtentPropertyHeight, 100 );

		SVImageClass* pImage = &(m_aResultImages[i]);

		pImage->UpdateImage( SVImageTypePhysical, SV_GUID_NULL, imageInfo );
	}

	// Result Objects
	m_Data.m_aResultObjectNames.resize(SVExternalToolTaskData::NUM_RESULT_OBJECTS);
	m_Data.m_aResultObjects.resize(SVExternalToolTaskData::NUM_RESULT_OBJECTS);

	for ( i=0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		// Register
		RegisterEmbeddedObject( &m_Data.m_aResultObjects[i], aSVVariantResultObjectGuid[i], IDS_OBJECTNAME_RESULT_01 + static_cast<int>(i), false, SVResetItemNone );
		RegisterEmbeddedObject( &m_Data.m_aResultObjectNames[i], aSVVariantResultObjectNameGuid[i], IDS_OBJECTNAME_RESULT_01_NAME + static_cast<int>(i), false, SVResetItemNone );

		// Defaults
		VARIANT vtTemp;
		::VariantInit( &vtTemp);
		vtTemp.vt = VT_EMPTY;
		m_Data.m_aResultObjects[i].SetDefaultValue(vtTemp, TRUE);
		m_Data.m_aResultObjectNames[i].SetDefaultValue(CString(), TRUE);
	}

	addDefaultInputObjects();

	// default tool counts
	m_Data.m_lNumInputImages = 0;
	m_Data.m_lNumInputValues = 0;
	m_Data.m_lNumResultImages = 0;
	m_Data.m_lNumResultValues = 0;

	SetAllAttributes();


}

void SVExternalToolTask::SetAllAttributes()
{
	long i;
	for ( i = 0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++)
	{
		bool bUsed = i < m_Data.m_lNumInputValues;
		SetBits( m_Data.m_aInputObjects[i].ObjectAttributesAllowedRef(),  SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SETABLE_ONLINE, bUsed );
		SetBits( m_Data.m_aInputObjectNames[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE , false );
		SetBits( m_Data.m_aInputObjectNames[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, bUsed );
		SetBits( m_Data.m_aInputObjects[i].ObjectAttributesAllowedRef(), SV_SELECTABLE_FOR_EQUATION , bUsed );
	}

	for ( i = 0; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
	{
		SetBits( m_aResultImages[i].ObjectAttributesAllowedRef(), SV_HIDDEN, i >= m_Data.m_lNumResultImages );
		SetBits( m_aResultImages[i].ObjectAttributesAllowedRef(), SV_ARCHIVABLE_IMAGE, i < m_Data.m_lNumResultImages );
	}

	for ( i = 0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		bool bUsed = i < m_Data.m_lNumResultValues;
		SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE, bUsed );
		SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, false );
		SetBits( m_Data.m_aResultObjectNames[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE | SV_PRINTABLE, false );
		SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_SELECTABLE_FOR_EQUATION, bUsed );
	}

	for ( i=0; i < static_cast<long>(m_Data.m_aDllDependencies.size()); i++)
	{
		SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_REMOTELY_SETABLE, true );
		SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE, false );
		CString sTemp;
		m_Data.m_aDllDependencies[i].GetValue( sTemp );
		bool bUsed = !sTemp.IsEmpty();
		SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, bUsed );
	}
	SetBits( m_Data.m_voToolVersion.ObjectAttributesAllowedRef(), SV_PRINTABLE, true );
	SetBits( m_Data.m_voToolName.ObjectAttributesAllowedRef(), SV_PRINTABLE, true );
	SetBits( m_Data.m_voDllPath.ObjectAttributesAllowedRef(), SV_PRINTABLE | SV_REMOTELY_SETABLE, true );

}

SVExternalToolTask::~SVExternalToolTask()
{
	CloseObject();
	// remove embeddeds so that invalid objects won't be Close()d
	embeddedList.RemoveAll();
}

LONG_PTR SVExternalToolTask::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR dwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
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


		// handle renaming of toolset variables
	case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (DwMessageValue); // Object with new name
			LPCTSTR tstrOriginalName = ( LPCTSTR ) DwMessageContext;

			if( RenameToolSetSymbol(pObject, tstrOriginalName ) )
				dwResult = SVMR_SUCCESS;
			break;
		}

	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			SVInObjectInfoStruct* pInInfo = reinterpret_cast <SVInObjectInfoStruct*> (DwMessageValue);
			SVObjectClass* pObject = pInInfo->GetInputObjectInfo().PObject;
			if ( SVImageClass* pImage = dynamic_cast <SVImageClass*> (pObject) )
			{
				// find object
				for ( int i=0; i < SVExternalToolTaskData::NUM_INPUT_IMAGES; i++ )
				{
					SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
					if ( rInfo.GetInputObjectInfo().PObject == pImage )
					{
						// replace with tool set image
						SVToolSetClass* pToolSet = GetInspection()->GetToolSet();
						SVObjectTypeInfoStruct imageObjectInfo;
						imageObjectInfo.ObjectType = SVImageObjectType;
						SVImageClass* pToolSetImage = dynamic_cast <SVImageClass*> ((SVObjectClass*) ::SVSendMessage( pToolSet, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&imageObjectInfo) ) );

						rInfo.SetInputObject( pToolSetImage );
						LONG_PTR bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(&rInfo), NULL );
						break;
					}
				}

				dwResult = SVMR_SUCCESS;
				break;
			}
			else if ( SVValueObjectClass* pValueObject = dynamic_cast <SVValueObjectClass*> (pObject) )
			{
				// find object
				for ( int i=0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++ )
				{
					SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
					if ( rInfo.GetInputObjectInfo().PObject == pValueObject )
					{
						// set input to NULL
						rInfo.SetInputObject( NULL );
						LONG_PTR bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(&rInfo), NULL );

						// set value to default
						m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, TRUE);
						break;
					}
				}

				dwResult = SVMR_SUCCESS;
				break;
			}

			dwResult = SVMR_NO_SUCCESS;
			break;
		}

	}

	dwResult = SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
	return dwResult;
}

BOOL SVExternalToolTask::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = FALSE;

	if( SVTaskObjectListClass::CreateObject( PCreateStructure ) &&
			PCreateStructure->CheckCreateStructName( "SVToolLevelCreateStruct" ) )
	{
		if( GetInspection() != NULL && GetTool() != NULL )
		{
			HRESULT hr = S_OK;
			try
			{
				hr = Initialize();
			}
			catch (SVErrorException& e)
			{
				hr = e.info().hr;
			}

			l_bOk = TRUE;
		}
	}

	isCreated = l_bOk;

	return l_bOk;
}

HRESULT SVExternalToolTask::Initialize(	SVDllLoadLibraryCallback fnNotify )// throw (SVErrorException)
{

	HRESULT hr = S_FALSE;
	BSTR bstrName = NULL;
	int i = 0; // used in for loops

	GUID guid = GetUniqueObjectID();


	HRESULT hrUninitializeRun = m_dll.UninitializeRun(guid);
	HRESULT hrClose = m_dll.Close();
	HRESULT hrUninitialize = Uninitialize();

	m_hrInitialized = S_FALSE;

	m_aInspectionInputImages.clear();
	m_aInspectionInputValues.clear();
	m_aInspectionResultValues.clear();
	m_aInspectionResultImages.clear();

	m_Data.m_aInputValueDefinitions.clear();
	m_Data.m_aResultValueDefinitions.clear();
	m_Data.m_aResultImageDefinitions.clear();


	// try to connect to DLL
	CString strDllPath;
	m_Data.m_voDllPath.GetValue(m_Data.m_voDllPath.GetLastSetIndex(),strDllPath);

	try
	{
		hr = m_dll.Open(strDllPath, fnNotify);
	}
	catch (SVErrorException& e)
	{
		m_hrInitialized = e.info().hr;
		throw;
	}

	if ( hr == S_OK )
	{
		try
		{
			///////////////////////////////////////
			//    Get DLL information
			///////////////////////////////////////

			hr = m_dll.GetToolName(&bstrName);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;
			CString strName( bstrName );
			m_Data.m_voToolName.SetValue(1, strName );

			SysFreeString( bstrName );
			
			long lVersion=0;
			hr = m_dll.GetToolVersion(&lVersion);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;
			m_Data.m_voToolVersion.SetValue(1, lVersion );

			///////////////////////////////////////
			//    Initialize Inputs
			///////////////////////////////////////

			// Initialize Input Images

			hr = m_dll.GetNumberOfInputImages( &m_Data.m_lNumInputImages );
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;


			std::vector< SVImageDefinitionStruct> aInputImages;	// used in m_dll.InitializeRun

			m_aInspectionInputImages.resize(m_Data.m_lNumInputImages);
			m_aInspectionInputHBMImages.resize(m_Data.m_lNumInputImages);
			aInputImages.resize(m_Data.m_lNumInputImages);

			//ConnectInputImages(); not necessary when using SVImageInfoToNewHBitmap (see below)

			m_aPreviousInputImageRect.clear();
			for( i = 0 ; i < m_Data.m_lNumInputImages ; i++ )
			{
				SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_Data.m_aInputImageInfo[i].GetInputObjectInfo().PObject);
				if( pImage )
				{
					SVImageInfoClass imageInfo = pImage->GetImageInfo();
					GetImageDefinitionStruct( aInputImages[i] , imageInfo );

					CRect rect;
					imageInfo.GetOutputRectangle( rect );
					m_aPreviousInputImageRect.push_back( rect );

					// remember MIL handle
					SVSmartHandlePointer l_ImageBuffer;
					pImage->GetImageHandle(l_ImageBuffer);

					SVImageBufferHandleImage l_MilHandle;

					if( !( l_ImageBuffer.empty() ) )
					{
						l_ImageBuffer->GetData( l_MilHandle );

						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionInputImages[i] = static_cast<long>(l_MilHandle.GetBuffer().GetIdentifier());

						if( !m_dll.UseMil() )
						{
							if ( m_aInspectionInputHBMImages[i].hbm != NULL )
							{
								::DeleteObject(m_aInspectionInputHBMImages[i].hbm);
								m_aInspectionInputHBMImages[i].FreeBitmapInfo();
								m_aInspectionInputHBMImages[i].Clear();
							}
							HRESULT hrInitialCopy = SVIHBitmapUtilities::SVImageInfoToNewDIB( imageInfo, m_aInspectionInputHBMImages[i] );
						}
						else
						{
							if( m_bUseImageCopies )
							{
								SVImageInfoClass imageInfoCopy;
								GetImageInfo(&(aInputImages[i]), imageInfoCopy);	// get the physical info from our definition struct

								if( GetTool() != NULL )
								{
									imageInfoCopy.SetOwner( GetTool()->GetUniqueObjectID() );
								}
								else
								{
									imageInfoCopy.SetOwner( SV_GUID_NULL );
								}

								// create buffer
								SVImageClass* pImageCopy = &(m_aInputImagesCopy[i]);
								pImageCopy->UpdateImage( SVImageTypePhysical, SV_GUID_NULL, imageInfoCopy );
								pImageCopy->SetImageDepth( 1 );
							}
						}
					}
				}// end if( pImage )
				else
				{// can't find image!
					//ASSERT(FALSE);
				}

			}// end for( int i = 0 ; i < m_Data.m_lNumInputImages ; i++ )


			///////////////////////////////////////
			// Initialize Input Objects
			///////////////////////////////////////

			InputValueDefinitionStruct* paInputValueDefs = NULL;
			hr = m_dll.GetInputValueDefinitions( &m_Data.m_lNumInputValues, &paInputValueDefs );
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;

			m_aInspectionInputValues.resize(m_Data.m_lNumInputValues);
			m_Data.m_aInputValueDefinitions.resize(m_Data.m_lNumInputValues);

			SVObjectClass* pObject = NULL;
			SVInspectionProcess* pInspection = GetInspection();

			for ( i = 0 ; i < m_Data.m_lNumInputValues ; i++)
			{
				SVVariantValueObjectClass& rInputValue = m_Data.m_aInputObjects[i];
				if( rInputValue.GetDefaultType() == VT_EMPTY )
				{
					rInputValue.SetDefaultValue(paInputValueDefs[i].vDefaultValue, TRUE);
				}

				HRESULT hrChangeType = ::VariantChangeType(&m_aInspectionInputValues[i], &m_aInspectionInputValues[i], 0, static_cast<VARTYPE>(paInputValueDefs[i].lVT) );
				//m_aInspectionInputValues[i].ChangeType( paInputValueDefs[i].lVT );

				CString strTmp ; 
				m_Data.m_aInputObjects[i].GetValue(strTmp);

				m_Data.m_aInputValueDefinitions[i] = paInputValueDefs[i];

				// get Input object name
				SVStringValueObjectClass& rvo = m_Data.m_aInputObjectNames[i];
				rvo.SetDefaultValue(CString(paInputValueDefs[i].bstrDisplayName), TRUE);	// set to all buckets
			
			}// end for ( int i = 0 ; i < m_Data.m_lNumInputValues ; i++)

			HRESULT hrConnectInputs;
			if( (hrConnectInputs = ConnectInputs()) != S_OK )
			{
				hr = hrConnectInputs;
			}

			hr = m_dll.DestroyInputValueDefinitionStructures(paInputValueDefs);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;
			paInputValueDefs = NULL;

			InspectionInputsToVariantArray();

			///////////////////////////////////////
			//    Initialize Results
			///////////////////////////////////////

			// Initialize Result Objects
			ResultValueDefinitionStruct* paResultValueDefs = NULL;
			hr = m_dll.GetResultValueDefinitions( &m_Data.m_lNumResultValues, &paResultValueDefs);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;

			m_aInspectionResultValues.resize(m_Data.m_lNumResultValues);
			m_Data.m_aResultValueDefinitions.resize(m_Data.m_lNumResultValues);
			
			for ( i = 0 ; i < m_Data.m_lNumResultValues ; i++)
			{
				ResultValueDefinitionStruct& rDef = paResultValueDefs[i];
				m_Data.m_aResultValueDefinitions[i] = rDef;

				if(rDef.lVT != VT_BSTR && !GetResultRangeObject(i) )	// Do not allocate result if already exists....
				{
					AllocateResult(i);
				}
			}

			hr = m_dll.DestroyResultValueDefinitionStructures(paResultValueDefs);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;
			paResultValueDefs = NULL;


			///////////////////////////////////////
			//    InitializeRun
			///////////////////////////////////////

			hr = m_dll.InitializeRun(guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(aInputImages[0]) : NULL,
                                           m_Data.m_lNumInputValues, m_Data.m_lNumInputValues ? &(m_aInspectionInputValues[0]) : NULL );
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;

			if( m_dll.UseMil() )
			{
				// This must be done after InitializeRun so the Dll has a guid for this tool
				hr = m_dll.SetMILInputImages( guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : NULL );
				//ASSERT( hr == S_OK );
				if( hr != S_OK )
					throw hr;
			}
			///////////////////////////////////////
			// Initialize Result Images
			///////////////////////////////////////

			SVImageDefinitionStruct* paResultImageDefs = NULL;
			hr = m_dll.GetResultImageDefinitions(guid, &m_Data.m_lNumResultImages, &paResultImageDefs);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;

			m_aInspectionResultImages.resize(m_Data.m_lNumResultImages);
			m_aInspectionResultHBMImages.resize(m_Data.m_lNumResultImages);
			m_Data.m_aResultImageDefinitions.resize(m_Data.m_lNumResultImages);

			for ( i = 0 ; i < m_Data.m_lNumResultImages; i++)
			{
				m_Data.m_aResultImageDefinitions[i] = paResultImageDefs[i];

				{// begin block

					// get image info
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if( GetTool() != NULL )
					{
						imageInfo.SetOwner( GetTool()->GetUniqueObjectID() );
					}
					else
					{
						imageInfo.SetOwner( SV_GUID_NULL );
					}

					// create buffer
					SVImageClass* pImage = &(m_aResultImages[i]);

					pImage->UpdateImage( SVImageTypePhysical, SV_GUID_NULL, imageInfo );
				}// end block
				if ( m_bUseImageCopies )
				{
					SVImageInfoClass imageInfo;
					GetImageInfo(&paResultImageDefs[i], imageInfo);
					if( GetTool() != NULL )
					{
						imageInfo.SetOwner( GetTool()->GetUniqueObjectID() );
					}
					else
					{
						imageInfo.SetOwner( SV_GUID_NULL );
					}

					// create buffer
					SVImageClass* pImage = &(m_aResultImagesCopy[i]);

					pImage->UpdateImage( SVImageTypePhysical, SV_GUID_NULL, imageInfo );
					pImage->SetImageDepth( 1 );
				}// end if ( m_bUseImageCopies )

			}// end for ( int i = 0 ; i < m_Data.m_lNumResultImages; i++)

			for ( i = m_Data.m_lNumResultImages ; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
			{
				// get image info
				SVImageInfoClass imageInfo;
				if( GetTool() != NULL )
				{
					imageInfo.SetOwner( GetTool()->GetUniqueObjectID() );
				}
				imageInfo.SetImageProperty( SVImagePropertyPixelDepth, 8 );
				imageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 ); 

				imageInfo.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
				imageInfo.SetExtentProperty( SVExtentPropertyWidth, 100 );
				imageInfo.SetExtentProperty( SVExtentPropertyHeight, 100 );

				// create buffer
				SVImageClass* pImage = &(m_aResultImages[i]);

				pImage->UpdateImage( SVImageTypePhysical, SV_GUID_NULL, imageInfo );
			}

			hr = m_dll.DestroyImageDefinitionStructure(paResultImageDefs);
			//ASSERT( hr == S_OK );
			if( hr != S_OK )
				throw hr;
			paResultImageDefs = NULL;

			if( m_dll.UseMil() )
			{
				hr = m_dll.SetMILResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : NULL );
				//ASSERT( hr == S_OK );
				if( hr != S_OK )
					throw hr;
			}

			SetAllAttributes();
		}// end try
		catch ( HRESULT l_hr )
		{
			l_hr;	// remove compiler warning
			// logging ???
		}
		catch ( int l_iErrorCode )
		{
			l_iErrorCode;	// remove compiler warning
			// logging ???
			hr = S_FALSE;// Error Initializing Tool
		}

#ifndef _DEBUG
		catch (...)
		{
			//ASSERT( FALSE );
			hr = S_FALSE;
		}
#endif
	}// end if ( hr == S_OK )
	
	if( hr == S_OK )
	{
		SVImageClass* l_psvOutput = GetResultImage( 0 );

		if( GetTool() != NULL && l_psvOutput != NULL )
		{
			hr = GetTool()->UpdateOffsetData( l_psvOutput );
		}
	}

	m_hrInitialized = hr;

	return hr;
}

HRESULT SVExternalToolTask::Uninitialize()
{
	HRESULT hr = S_OK;



	for( std::vector<SVDIBITMAPINFO>::iterator it = m_aInspectionInputHBMImages.begin(); it != m_aInspectionInputHBMImages.end(); ++it)
	{
		if ( it->hbm != NULL )
		{
			::DeleteObject(it->hbm);
			it->FreeBitmapInfo();
			it->Clear();
		}
	}
	m_aInspectionInputHBMImages.clear();
	return hr;
}


BOOL SVExternalToolTask::CloseObject()
{
	Uninitialize();
	if ( IsCreated() )
	{
		HRESULT hr = m_dll.UninitializeRun(GetUniqueObjectID());
		hr = m_dll.Close();
		if( SVTaskObjectClass::CloseObject() )
		{
			BOOL bRetVal = TRUE; // m_svOutputImageObject.CloseObject();

			return bRetVal;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL SVExternalToolTask::OnValidate()
{
	BOOL bRetVal = FALSE;

	if(    GetInspection() != NULL
		&& GetTool() != NULL
		&& m_hrInitialized == S_OK )
	{
		bRetVal = SVTaskObjectListClass::OnValidate();
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

BOOL SVExternalToolTask::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = TRUE;

	if ( m_dll.IsHandleNull() )
	{
		RRunStatus.SetInvalid();
		SetInvalid();
		return FALSE;
	}
	// All inputs and outputs must be validated first
	if( SVTaskObjectListClass::onRun( RRunStatus ) )
	{
		try
		{
			// See Section 3.5.3.  
			// SetInputValues()
			// SetMILInputImages()
			// SetMILResultImages()
			// RunTool()
			// GetResultValues()

			
			SVMatroxBufferInterface::SVStatusCode l_Code;
			
			GUID guid = GetUniqueObjectID();
			int i;
			HRESULT hr = S_OK;

			// Data Definition List stuff
			CollectInputImageNames(RRunStatus);

			// EB 20051121
			// check input images to see if they have resized
			// If so, do a ResetObject here first.
			// collect input images
			bool bNeedReset = false;

			if ( m_aPreviousInputImageRect.size() == 0 )
			{
				for ( i=0; i < m_Data.m_lNumInputImages; i++)
					m_aPreviousInputImageRect.push_back( CRect() );
			}

			for ( i=0; i < m_Data.m_lNumInputImages; i++)
			{
				SVImageClass* pInputImage = GetInputImage(i);
				//ASSERT( pInputImage );
				if ( pInputImage )
				{
					SVImageExtentClass l_InputImageExtents(pInputImage->GetImageExtents());

					CRect rectInput;
					l_InputImageExtents.GetOutputRectangle( rectInput );

					CRect rectPrevious(m_aPreviousInputImageRect[i]);
					
					if ( rectInput != rectPrevious )
					{
						bNeedReset = true;
						//m_aPreviousInputImageRect[i] = rectInput;	//   done in Initialize, not needed here
						break;
					}
				}
			}

			if ( bNeedReset )
				ResetObject();





			/////////////////////////////////////////////////////
			//   Inputs
			/////////////////////////////////////////////////////

			// collect input values
			for ( i=0; i < m_Data.m_lNumInputValues; i++)
			{
				SVValueObjectClass* pValueObject = GetInputValueObject(i);
				//ASSERT( pValueObject );
				//m_aInspectionInputValues[i].Clear();
				pValueObject->GetValue( m_aInspectionInputValues[i] );
				HRESULT hrChangeType = ::VariantChangeType(&m_aInspectionInputValues[i], &m_aInspectionInputValues[i], 0, static_cast<VARTYPE>(m_Data.m_aInputValueDefinitions[i].lVT) );
				if ( hrChangeType != S_OK )
				{
					m_aInspectionInputValues[i].Clear();
					m_aInspectionInputValues[i].ChangeType( static_cast<VARTYPE>(m_Data.m_aInputValueDefinitions[i].lVT));
				}

			}
			// send input values to DLL
			hr = m_dll.SetInputValues( guid, m_Data.m_lNumInputValues, m_Data.m_lNumInputValues ? &(m_aInspectionInputValues[0]) : NULL );
			if ( hr != S_OK )
			{
				throw hr;
			}

			BOOL l_bOkToRun = TRUE;

			// collect input images
			for ( i=0; i < m_Data.m_lNumInputImages; i++)
			{
				SVSmartHandlePointer l_ImageBuffer;
				SVImageClass* pInputImage = GetInputImage(i);
				//ASSERT( pInputImage );
				if ( pInputImage )
				{
					pInputImage->GetImageHandle(l_ImageBuffer);

					SVImageBufferHandleImage l_MilHandle;

					if( !( l_ImageBuffer.empty() ) )
					{
						l_ImageBuffer->GetData( l_MilHandle );

						if( m_dll.UseMil() )
						{
							if( !m_bUseImageCopies )
							{
								// This cast assumes that a mil handle will never get larger than a 32 bit long.
								m_aInspectionInputImages[i] = static_cast<long>(l_MilHandle.GetBuffer().GetIdentifier());	// assign to internal buffer handle
								if( m_aInspectionInputImages[i] == 0 )
								{
									l_bOkToRun = FALSE;
								}
							}
							else
							{
								SVSmartHandlePointer l_ImageBufferCopy;
								m_aInputImagesCopy[i].GetImageHandle(l_ImageBufferCopy);
								
								SVImageBufferHandleImage l_CopyMilHandle;
								if( !( l_ImageBufferCopy.empty() ) )
								{
									l_ImageBufferCopy->GetData( l_CopyMilHandle );
								}

								if( ! l_CopyMilHandle.empty() && ! l_MilHandle.empty() )
								{
									l_Code = SVMatroxBufferInterface::CopyBuffer( l_CopyMilHandle.GetBuffer(), l_MilHandle.GetBuffer());
									
									// This cast assumes that a Mil Id will never get larger than 32 bits in size.
									m_aInspectionInputImages[i] = static_cast<long>(l_CopyMilHandle.GetBuffer().GetIdentifier());	// assign to copy handle
									if( m_aInspectionInputImages[i] == 0 )
									{
										l_bOkToRun = FALSE;
									}
								}
								else
								{
									l_bOkToRun = FALSE;
								}
							}
						}// if( m_dll.UseMil() )
						else
						{
							// EB 20050224
							// Don't recreate the bitmaps every inspection
							// only once on Initialize
							/*
							::DeleteObject(m_aInspectionInputHBMImages[i]);
							m_aInspectionInputHBMImages[i] = 0;
							//*/

							if( l_MilHandle.empty() )
							{
								l_bOkToRun = FALSE;
							}
							l_Code = SVMatroxBufferInterface::CopyBuffer( m_aInspectionInputHBMImages[i].hbm, l_MilHandle.GetBuffer() );
						}
					}
				}
			}

			if( m_dll.UseMil() )
			{
				// send input images to DLL
				hr = m_dll.SetMILInputImages( guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : NULL );
				if ( hr != S_OK )
				{
					throw hr;
				}
			}
			else
			{
				std::vector<HBITMAP> aDIBHandles(m_Data.m_lNumInputImages);
				for ( int i=0; i < m_Data.m_lNumInputImages; i++ )
					aDIBHandles[i] = m_aInspectionInputHBMImages.at(i).hbm;
				hr = m_dll.SetHBITMAPInputImages(guid, m_Data.m_lNumInputImages ? &(aDIBHandles[0]) : NULL );
			}

			if( m_dll.UseMil() )
			{
				if ( !m_bUseImageCopies )
				{
					// collect result images
					for ( i=0; i < m_Data.m_lNumResultImages; i++)
					{
						GetResultImage(i)->SetImageHandleIndex( RRunStatus.Images );

						SVSmartHandlePointer l_ImageBuffer;
						GetResultImage(i)->GetImageHandle(l_ImageBuffer);

						SVImageBufferHandleImage l_MilHandle;

						if( !( l_ImageBuffer.empty() ) )
						{
							l_ImageBuffer->GetData( l_MilHandle );
						}

						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionResultImages[i] = static_cast<long>(l_MilHandle.GetBuffer().GetIdentifier());
						if( m_aInspectionResultImages[i] == 0 )
						{
							l_bOkToRun = FALSE;
						}
					}
				}// if ( !m_bUseImageCopies )
				else
				{
					// collect result image copies
					for ( i=0; i < m_Data.m_lNumResultImages; i++)
					{
						// Always use bucket 0 for copy
						SVSmartHandlePointer l_ImageBuffer;
						m_aResultImagesCopy[i].GetImageHandle(l_ImageBuffer);

						SVImageBufferHandleImage l_MilHandle;

						if( !( l_ImageBuffer.empty() ) )
						{
							l_ImageBuffer->GetData( l_MilHandle );
						}

						// this cast assumes that a mil handle will never be larger 32 bits.
						m_aInspectionResultImages[i] = static_cast<long>(l_MilHandle.GetBuffer().GetIdentifier());
						if( m_aInspectionResultImages[i] == 0 )
						{
							l_bOkToRun = FALSE;
						}
					}
				}

				// give result images to dll
				hr = m_dll.SetMILResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : NULL );
				if ( hr != S_OK )
				{
					throw hr;
				}
			}// if( m_dll.UseMil() )
			else
			{ // HBitmap Images
			}

			
			///////////////////////////////////////
			//    Run
			///////////////////////////////////////
			long lToolStatus = 0;
			if( l_bOkToRun )
			{
				hr = m_dll.RunTool(guid, &lToolStatus);
			}
			if ( hr != S_OK )
				throw hr;

			


			///////////////////////////////////////
			//    Outputs
			///////////////////////////////////////

			// Result Values

			hr = m_dll.GetResultValues(guid, m_Data.m_lNumResultValues, m_Data.m_lNumResultValues ? &(m_aInspectionResultValues[0]) : NULL );
			if ( hr != S_OK )
				throw hr;

			for ( i=0; i < m_Data.m_lNumResultValues; i++ )
			{
				
				//ASSERT( m_Data.m_aResultValueDefinitions[i].lVT == m_aInspectionResultValues[i].vt);

				GetResultValueObject(i)->SetValue( RRunStatus.m_lResultDataIndex, m_aInspectionResultValues[i] );
				
				// Clear OleVariant that was created in Dll.
				m_aInspectionResultValues[i].Clear();
				//m_aInspectionResultValues[i].vt = m_Data.m_aResultValueDefinitions[i].lVT;
			}

 

			// Result Images

			if ( m_Data.m_lNumResultImages > 0 )
			{
				if( m_dll.UseMil() )
				{
					if ( m_bUseImageCopies )
					{
						// collect result images
						for ( i=0; i < m_Data.m_lNumResultImages; i++)
						{
							GetResultImage(i)->SetImageHandleIndex( RRunStatus.Images );

							SVSmartHandlePointer l_ImageBuffer;
							GetResultImage(i)->GetImageHandle(l_ImageBuffer);

							SVSmartHandlePointer l_ImageBufferCopy;
							m_aResultImagesCopy[i].GetImageHandle(l_ImageBufferCopy);

							SVImageBufferHandleImage l_MilHandle;
							SVImageBufferHandleImage l_CopyMilHandle;

							if( ! l_ImageBuffer.empty() && ! l_ImageBufferCopy.empty() )
							{
								l_ImageBuffer->GetData( l_MilHandle );
								l_ImageBufferCopy->GetData( l_CopyMilHandle );
							}

							if( ! l_MilHandle.empty() && ! l_CopyMilHandle.empty() )
							{
								// Ask Joe: what if different sizes??
								SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), l_CopyMilHandle.GetBuffer());
							}
						}
					}// end if ( m_bUseImageCopies )
				}// if( m_dll.UseMil() )
				else
				{ // Use HBitmaps 
					hr = m_dll.GetHBITMAPResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultHBMImages[0]) : NULL );
					for( int i = 0 ; i < m_Data.m_lNumResultImages ; i++ )
					{
						// Create new Mil handle from HBitmap
						SVSmartHandlePointer l_ImageBuffer;
						GetResultImage(i)->GetImageHandle(l_ImageBuffer);

						SVImageBufferHandleImage l_MilHandle;

						if( !( l_ImageBuffer.empty() ) )
						{
							l_ImageBuffer->GetData( l_MilHandle );
						}

						l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), m_aInspectionResultHBMImages[i] );
						if ( hr == S_OK )
						{
							if( l_Code != SVMEE_STATUS_OK )
							{
								hr = l_Code | SVMEE_MATROX_ERROR;
							}
						}
						// EB 20050224
						// Don't delete the HBITMAPS!
						/*
						if( ::DeleteObject(m_aInspectionResultHBMImages[i]) == 0 )
						{
							//ASSERT(FALSE);
						}
						else
						//*/
						m_aInspectionResultHBMImages[i] = 0;
						
					}// end for( int i = 0 ; i < m_Data.m_lNumResultImages ; i++ )
				}// end else use HBITMAP
			}// end if ( m_Data.m_lNumResultImages > 0 )

			const int iBitmapInfoHeader = sizeof( BITMAPINFOHEADER );
			const int iDibSize = sizeof( DIBSECTION );
				
			if( l_bOkToRun )
			{
				RRunStatus.SetPassed();
			}
			else
			{
				l_bOk = FALSE;
			}


		}	// end try
		catch (HRESULT hr)
		{
			hr;	// remove compiler warning
			// log hresult??
			//ASSERT( FALSE );

			l_bOk = FALSE;
		}
#ifndef _DEBUG
		catch (...)
		{
			//ASSERT( FALSE );
			l_bOk = FALSE;
		}
#endif
	}// end if( SVTaskObjectListClass::onRun( RRunStatus ) )

	if ( ! l_bOk )
	{
		RRunStatus.SetInvalid();
		SetInvalid();
	}

	return l_bOk;
}


HRESULT SVExternalToolTask::SetPathName(const CString& strPath)
{
	HRESULT hr=S_OK;
	m_Data.m_voDllPath.SetValue(1, strPath);
	return hr;
}

HRESULT SVExternalToolTask::SetDependencies(const SVDependenyNames astrPath)
{
	HRESULT hr = S_FALSE;
	int iSize = astrPath.GetSize();
	if(iSize <= SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES )
	{
		int i( 0 );

		for( i = 0 ; i < iSize ; i++)
		{
			m_Data.m_aDllDependencies[i].SetValue(1, astrPath.GetAt(i));
		}
		for( i = iSize ; i < SVExternalToolTaskData::NUM_TOOL_DEPENDENCIES ; i++)
		{
			m_Data.m_aDllDependencies[i].SetValue(1, _T(""));
		}

		hr = S_OK;
	}
	return hr;
}

HRESULT SVExternalToolTask::GetResultImageDefinitions( SVImageDefinitionStructArray& raResultImageDefinitions )
{
	raResultImageDefinitions = m_Data.m_aResultImageDefinitions;
	return S_OK;
}

HRESULT SVExternalToolTask::GetResultValueDefinitions ( ResultValueDefinitionStructArray& raResultValueDefinitions )
{
	raResultValueDefinitions = m_Data.m_aResultValueDefinitions;
	return S_OK;
}

HRESULT SVExternalToolTask::GetInputValueDefinitions( InputValueDefinitionStructArray& raInputValueDefinitions )
{
	raInputValueDefinitions = m_Data.m_aInputValueDefinitions;
	return S_OK;
}

// ISVCancel interface function
bool SVExternalToolTask::CanCancel()
{
	return true;
}

// ISVCancel interface function
HRESULT SVExternalToolTask::GetCancelData(SVCancelData*& rpCancelData)
{
	//ASSERT(rpCancelData == NULL);
	SVExternalToolTaskData* pData = new SVExternalToolTaskData(m_Data);
	rpCancelData = pData;

	// set temp data
	std::vector<SVResultClass*> aRangeResults = GetResultRangeObjects();
	std::vector<SVResultClass*>::iterator iter;
	for ( iter = aRangeResults.begin(); iter != aRangeResults.end(); ++iter)
	{
		SVCancelData* pCancelData = NULL;
		(*iter)->GetCancelData( pCancelData );
		pData->m_RangeResultData.mapData[*iter] = pCancelData;
	}

	return S_OK;
}

// ISVCancel interface function
HRESULT SVExternalToolTask::SetCancelData(SVCancelData* pCancelData)
{
	SVExternalToolTaskData* pData = dynamic_cast<SVExternalToolTaskData*> (pCancelData);
	if ( pData )
	{
		size_t i( 0 );

		// disconnect changed objects
		
		for ( i = 0 ; i < m_Data.m_aInputImageInfo.size() ; i++)
		{

			SVInObjectInfoStruct* pImageInfo = &m_Data.m_aInputImageInfo[i];
		
			// Disconnect input info of input object...
			if( pImageInfo->IsConnected() )
			{
				// Send to the Object we are using
				::SVSendMessage( pImageInfo->GetInputObjectInfo().UniqueObjectID, 
								 SVM_DISCONNECT_OBJECT_INPUT, 
								 reinterpret_cast<LONG_PTR>(pImageInfo), 
								 NULL );
			}

		}



		m_Data = *pData;

		// Set Range Result data
		SVMultiCancelData::MapType::iterator iter;
		for ( iter = pData->m_RangeResultData.mapData.begin(); iter != pData->m_RangeResultData.mapData.end(); ++iter)
		{
			iter->first->SetCancelData( iter->second );
		}
		m_Data.m_RangeResultData.mapData.clear();	// don't remember this temp data;


		for ( i = 0 ; i < m_Data.m_aInputImageInfo.size() ; i++)
		{
			SVInObjectInfoStruct* pImageInfo = &m_Data.m_aInputImageInfo[i];
			// reconnect changed objects
			// Connect input info to new input object...
			LONG_PTR dwConnectResult = ::SVSendMessage( pImageInfo->GetInputObjectInfo().UniqueObjectID, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(pImageInfo), NULL );
		}

		SVToolClass* pTool = NULL;
		pTool = dynamic_cast <SVToolClass*> (GetAncestor( SVToolObjectType ));
		//ASSERT( pTool );
		// Tell Tool image source has changed
		::SVSendMessage( pTool, SVM_IMAGE_SOURCE_CHANGED, NULL, NULL );

		// Reset all objects again...
		SVToolSetClass* pToolSet = NULL;
		pToolSet = dynamic_cast<SVToolSetClass*> (GetAncestor( SVToolSetObjectType ));
		//ASSERT( pToolSet );
		::SVSendMessage( pTool, SVM_RESET_ALL_OBJECTS, NULL, NULL );

		return S_OK;
	}// end if ( pData )
	else
	{
		return S_FALSE;
	}
}

HRESULT SVExternalToolTask::InspectionInputsToVariantArray()
{
	for( int i = 0 ; i < m_Data.m_lNumInputValues ; i++ )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		_variant_t vtTemp;
		if( rInfo.GetInputObjectInfo().PObject )
		{
			SVValueObjectClass* pValueObject = static_cast < SVValueObjectClass* >(rInfo.GetInputObjectInfo().PObject);
			pValueObject->GetValue( vtTemp );
		}
		else
		{
			m_Data.m_aInputObjects[i].GetValue( vtTemp );
		}

		_variant_t& rVT = m_aInspectionInputValues[i];
		HRESULT hrChangeType = S_OK;
		if( rVT.vt != vtTemp.vt )
		{
			hrChangeType = ::VariantChangeType(&vtTemp, &vtTemp, 0, rVT.vt);
			//vtTemp.ChangeType(rVT.vt );
		}
		if( hrChangeType == S_OK )
		{
			rVT = vtTemp;
		}
		else
		{
			VARTYPE vt = rVT.vt;
			rVT.Clear();
			rVT.vt = vt;
		}
	}
	return S_OK;
}

SVImageClass* SVExternalToolTask::GetInputImage(int iIndex)
{
	//ASSERT( iIndex >= 0 );
	//ASSERT( iIndex < m_Data.m_lNumInputImages );
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumInputImages )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[iIndex];
		return static_cast <SVImageClass*> (rInfo.GetInputObjectInfo().PObject);
	}
	else
	{
		return NULL;
	}
}

SVValueObjectClass* SVExternalToolTask::GetInputValueObject(int iIndex)
{
	//ASSERT( iIndex >= 0 );
	//ASSERT( iIndex < m_Data.m_lNumInputValues );
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumInputValues )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[iIndex];
		if ( rInfo.GetInputObjectInfo().PObject )
		{
			return static_cast <SVValueObjectClass*> (rInfo.GetInputObjectInfo().PObject);
		}
		else
		{
			return &(m_Data.m_aInputObjects[iIndex]);
		}
	}
	else
	{
		return NULL;
	}
}

SVImageClass* SVExternalToolTask::GetResultImage(int iIndex)
{
	//ASSERT( iIndex < m_Data.m_lNumResultImages );
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumResultImages )
		return &(m_aResultImages[iIndex]);
	else
		return NULL;
}

SVVariantValueObjectClass* SVExternalToolTask::GetResultValueObject(int iIndex)
{
	//ASSERT( iIndex >= 0 );
	//ASSERT( iIndex < m_Data.m_lNumResultValues );
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumResultValues )
		return &(m_Data.m_aResultObjects[iIndex]);
	else
		return NULL;
}



HRESULT SVExternalToolTask::GetImageInfo(const SVImageDefinitionStruct* pDefinitionStruct, SVImageInfoClass& rInfo)
{
	HRESULT hr = S_OK;

	rInfo.SetImageProperty( SVImagePropertyPixelDepth, 8 ); 
	rInfo.SetImageProperty( SVImagePropertyFormat, pDefinitionStruct->eImageFormat ); 

	rInfo.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
	rInfo.SetExtentProperty( SVExtentPropertyWidth, pDefinitionStruct->lWidth );
	rInfo.SetExtentProperty( SVExtentPropertyHeight, pDefinitionStruct->lHeight );

	switch (pDefinitionStruct->eImageFormat)
	{
		case SVImageFormatRGB888:
		case SVImageFormatRGB8888:
			rInfo.SetImageProperty( SVImagePropertyBandNumber, 3 ); 
			break;
		default:
			rInfo.SetImageProperty( SVImagePropertyBandNumber, 1 ); 
			break;
	}

	return hr;
}

HRESULT SVExternalToolTask::GetImageDefinitionStruct( SVImageDefinitionStruct& rImageDef, const SVImageInfoClass& rInfo)
{
	long l_lBandNumber = 1;
	long l_lFormat = SVImageFormatUnknown;

	rInfo.GetExtentProperty( SVExtentPropertyWidth, rImageDef.lWidth );
	rInfo.GetExtentProperty( SVExtentPropertyHeight, rImageDef.lHeight );

	rInfo.GetImageProperty( SVImagePropertyFormat, l_lFormat ); 
	rInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber ); 

	if( ( rImageDef.eImageFormat == SVImageFormatRGB888 ||
		rImageDef.eImageFormat == SVImageFormatRGB8888 ) &&
		l_lBandNumber == 1 )
	{
		rImageDef.eImageFormat = SVImageFormatMono8;
	}
	else
	{
		rImageDef.eImageFormat = (SVImageFormatEnum)l_lFormat;
	}

	return S_OK;
}

HRESULT SVExternalToolTask::AllocateResult (int iIndex)
{
	SVClassInfoStruct       resultClassInfo;
	CString                 strTitle;
	SVObjectTypeInfoStruct  interfaceInfo;
	
	SVVariantResultClass*    pResult;
	
	HRESULT hr = S_OK;
	
	try
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_Data.m_aResultObjects[iIndex].GetEmbeddedID();
		resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.ObjectTypeInfo.SubType	= SVResultVariantObjectType;
		resultClassInfo.ClassId = SVVariantResultClassGuid;
		//resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
		resultClassInfo.ClassName = _T("Range");
		strTitle = m_Data.m_aResultObjects [iIndex].GetName(); 
		resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
		
		// Construct the result class
		pResult = dynamic_cast< SVVariantResultClass *> (resultClassInfo.Construct());
		
		//		SetBits( pResult->uObjectAttributesAllowed,  SV_PRINTABLE, false );
		
		if(!pResult)
		{
			//ASSERT(FALSE);
			throw  -12346;
		}
		
		Add( pResult );
		//		AddFriend( pResult->GetEmbeddedID() );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVVariantValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		SVVariantValueObjectClass* pValue = 
			dynamic_cast<SVVariantValueObjectClass*>(( SVObjectClass* )SVSendMessage(pResult,
			SVM_GETFIRST_OBJECT, 
			NULL, 
			reinterpret_cast<LONG_PTR>(&info)));
		
		if (!pValue)
		{
			throw -12347;
		}
		
		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_ATTRIBUTES;
		
		// Ensure this Object's inputs get connected
		::SVSendMessage( pResult, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<LONG_PTR>(pResult), SVMFResetObject ) != SVMR_SUCCESS )
			{
				//ASSERT(FALSE);					
				// Remove it from the TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( objectID != SVInvalidGUID )
//					RemoveFriend( objectID );
					Delete( objectID );
				else
					delete pResult;
				
				throw -12348;
			}
		}
		
	}// end try
	catch( int iErrorCode)
	{
		hr = iErrorCode;
	}
#ifndef _DEBUG
	catch(...)
	{
		hr = S_FALSE;
	}
#endif
	return hr;
}

SVResultClass* SVExternalToolTask::GetResultRangeObject(int iIndex)
{
	SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;
	
	SVInObjectInfoStruct*	pResultInputInfo;
	
	SVObjectTypeInfoStruct  info;
	SVVariantResultClass*   pResult = NULL;
	SVObjectClass*          pSVObject;
	
	long                    lDone;
	
	lDone = FALSE;
	
	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultVariantObjectType;
	
	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end() && !lDone; ++l_Iter )
	{
		pResult = dynamic_cast< SVVariantResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		pResult->GetPrivateInputList( resultInputList );
		
		pResultInputInfo = resultInputList.GetAt( 0 );
		
		pSVObject = pResultInputInfo->GetInputObjectInfo().PObject;
		
		if( &m_Data.m_aResultObjects[iIndex] == pSVObject )
		{
			lDone = TRUE;
		}
		else
		{
			pResult = NULL;
		}
	}

	return pResult;
}

std::vector<SVResultClass*> SVExternalToolTask::GetResultRangeObjects()
{
	std::vector<SVResultClass*> aObjects;
	
	SVObjectTypeInfoStruct  info;
	SVVariantResultClass*   pResult;
	
	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultVariantObjectType;
	
	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		pResult = dynamic_cast< SVVariantResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		aObjects.push_back( pResult );
	}

	return aObjects;
}

BOOL SVExternalToolTask::RenameToolSetSymbol(SVObjectClass* pObject, LPCTSTR tstrOriginalName)
{
	CString sNewPrefix;
	CString sOldPrefix;

	if( SVInspectionProcess* l_pInspection = dynamic_cast< SVInspectionProcess* >( pObject ) )
	{
		sNewPrefix = _T( "." ) + l_pInspection->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType ) + _T( "." );
		sOldPrefix = sNewPrefix;
		sOldPrefix.Replace( l_pInspection->GetName(), tstrOriginalName );
	}// end if
	else
	{
		sNewPrefix = _T( "." ) + pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		sOldPrefix = sNewPrefix;
		sOldPrefix.Replace( pObject->GetName(), tstrOriginalName );
	}// end else

	// loop through all inputs & rename
	// input objects
	long i;
	for ( i=0; i < m_Data.m_lNumInputValues; i++)
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		if ( rInfo.GetInputObjectInfo().PObject &&
			(pObject == rInfo.GetInputObjectInfo().PObject->GetAncestor(pObject->GetObjectInfo().ObjectTypeInfo.ObjectType)) )
		{
			CString sValue;
			m_Data.m_aInputObjects[i].GetValue(sValue);
			sValue.Replace( sOldPrefix, sNewPrefix );
			m_Data.m_aInputObjects[i].SetValue(1, sValue);
		}
	}

	return TRUE;
}

HRESULT SVExternalToolTask::ClearData()
{
	HRESULT hr = S_OK;
	int i = 0;
	for ( i=0; i < m_Data.m_lNumInputImages; i++)
	{
		//m_Data.m_aInputImageInfo[i].InputObjectInfo.PObject = NULL;
	}


	for ( i=0; i < m_Data.m_lNumResultValues; i++)
	{
		_variant_t vt;
		m_Data.m_aResultObjects[i].SetDefaultValue(vt, TRUE);
		m_Data.m_aResultObjectNames[i].SetDefaultValue(_T(""), TRUE);
	}

	std::vector< SVResultClass*> apResults = GetResultRangeObjects();
	for( std::vector<SVResultClass*>::iterator iter = apResults.begin() ; iter != apResults.end() ; ++iter )
	{
		HRESULT hrTmp = RemoveChild( *iter );
		if( hrTmp != S_OK )
			hr = hrTmp;
	}


	m_Data.m_lNumInputValues = 0;
	m_Data.m_lNumResultValues = 0;
	m_Data.m_lNumResultImages = 0;
	m_Data.m_lNumInputImages = 0;

	return hr;
}

HRESULT SVExternalToolTask::SetDefaultValues()
{
	HRESULT hr = S_OK;

	for ( int i=0; i < m_Data.m_lNumInputValues; i++)
	{
		m_Data.m_aInputObjectInfo[i].SetInputObject( NULL );
		m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, TRUE);
		m_Data.m_aInputObjectNames[i].SetDefaultValue(_T(""), TRUE);
	}

	return hr;
}

HRESULT SVExternalToolTask::SetInput( const CString& rstrInputName, const CString& rstrValue, int iBucket)
{
	// SetInput will set either a Value into the appropriate input or it will set a value object name into 
	// the appropriate input.  And if it is a valid value object name it will set the address of the value object
	// into the SVInObjectInfoStruct for that input and it will send a message to Connect Object Input.
	HRESULT hr = S_OK;

	int iIndex = 0;
	// if it is an input object, get its input index from its name (Input 01 = index 0 )
	int iPos = 0;
	if( (iPos = rstrInputName.Find(_T("Input "))) >= 0)
	{
		SVObjectClass* pObject;
		//MZA: change function to find object from inspection child object to anz dotted name
		//GetChildObjectByName(rstrValue, &pObject);
		SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strName ), pObject );

		bool bSameInspection = true;
		if( pObject )
		{
			SVObjectClass* pIPObject = GetAncestor( SVInspectionObjectType );
			SVObjectClass* pIPInputObject = pObject->GetAncestor( SVInspectionObjectType );
			bSameInspection = pIPObject->GetCompleteObjectName() == pIPInputObject->GetCompleteObjectName();
		}

		if( bSameInspection )
		{

			iIndex = atol( rstrInputName.Mid( iPos + 6) ) -1;
			if( iIndex > 0 && iIndex < 51 )
			{
			}
			else
			{
				pObject = NULL;
			}
			SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[iIndex];
			SVVariantValueObjectClass& rValue = m_Data.m_aInputObjects[iIndex];


			if ( rInfo.GetInputObjectInfo().PObject )
			{	// disconnect existing connection
				LONG_PTR bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(&rInfo), NULL );
				rInfo.SetInputObject( NULL );
			}

			if ( pObject )
			{
				rValue.SetType( VT_BSTR );
				rValue.SetValue(1, rstrValue);
				rValue.SetValue(iBucket, rstrValue);

				rInfo.SetInputObject( pObject );
				LONG_PTR bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(&rInfo), NULL );
				//ASSERT( bSuccess );
			}
			else
			{
				rValue.SetType( m_Data.m_aInputValueDefinitions[iIndex].lVT);
				if( S_OK != rValue.SetValueKeepType(1, rstrValue) )
				{
					hr = -12380;
				}
				if( S_OK != rValue.SetValueKeepType(iBucket, rstrValue) )
				{
					hr = -12381;
				}
			}
		}// end if( bSameInspection )
		else
		{
			hr = -12379;  // Error From Different Inspections
		}
	}// end if( (iPos = rsTmp.Find(_T("Input "))) >= 0)

	return hr;
}

HRESULT SVExternalToolTask::ResetObject()
{
	SVTaskObjectListClass::ResetObject();

	///////////////////////////////////////
	// Initialize Input Objects
	///////////////////////////////////////
	HRESULT hr = S_OK;

	hr = ConnectInputs();

	if( m_bUseImageCopies )
	{
		for ( int i = 0; i < m_Data.m_lNumResultImages; i++ )
		{
			m_aResultImagesCopy[i].ResetObject();
		}
	}

	InspectionInputsToVariantArray();

	try
	{
		hr = Initialize( SVDllLoadLibraryCallbackDefault() );
	}
	catch (SVErrorException& e)
	{
		hr = e.info().hr;
	}

	return hr;
}

// compare cancel data (original) with current data.
HRESULT SVExternalToolTask::FindInvalidatedObjects(SVObjectVector& rList, const SVCancelData* pCancelData, FindEnum eWhich )
{
	//ASSERT( rList.size() == 0 );	// should be empty when called
	const SVExternalToolTaskData* pOriginalData = dynamic_cast <const SVExternalToolTaskData*> (pCancelData);
	SVExternalToolTaskData* pNewData = &m_Data;

	if ( eWhich & FIND_VALUES )
	{
		// inputs
		if ( pOriginalData->m_lNumInputValues > pNewData->m_lNumInputValues )
		{
			for ( int i = pNewData->m_lNumInputValues ; i < pOriginalData->m_lNumInputValues; i++ )
			{
				rList.push_back( &(m_Data.m_aInputObjects[i]) );
			}
		}

		// outputs
		if ( pOriginalData->m_lNumResultValues > pNewData->m_lNumResultValues )
		{
			for ( int i = pNewData->m_lNumResultValues ; i < pOriginalData->m_lNumResultValues; i++ )
			{
				rList.push_back( &(m_Data.m_aResultObjects[i]) );
			}
		}
	}// end if ( eWhich & FIND_VALUES )
	
	if ( eWhich & FIND_IMAGES )
	{
		// input images
		if ( pOriginalData->m_lNumInputImages > pNewData->m_lNumInputImages )
		{
			for ( int i = pNewData->m_lNumInputImages ; i < pOriginalData->m_lNumInputImages; i++ )
			{
				const SVInObjectInfoStruct* pStruct = &(pOriginalData->m_aInputImageInfo[i]);
				rList.push_back( pStruct->GetInputObjectInfo().PObject );
			}
		}

		// result images
		if ( pOriginalData->m_lNumResultImages > pNewData->m_lNumResultImages )
		{
			for ( int i = pNewData->m_lNumResultImages ; i < pOriginalData->m_lNumResultImages; i++ )
			{
				rList.push_back( &(m_aResultImages[i]) );
			}
		}
	}// end if ( eWhich & FIND_IMAGES )

	return S_OK;
}

HRESULT SVExternalToolTask::DisconnectInputsOutputs(SVObjectVector& rListOfObjects)
{
	return SVTaskObjectListClass::DisconnectInputsOutputs(rListOfObjects);
}

HRESULT SVExternalToolTask::HideInputsOutputs(SVObjectVector& rListOfObjects)
{
	//SVObjectVector::iterator iter;
	//for ( iter = rListOfObjects.begin(); iter != rListOfObjects.end(); iter++ )
	//{
	//	(*iter)->CloseObject();
	//}
	//return S_OK;
	return SVTaskObjectListClass::HideInputsOutputs(rListOfObjects);
}

BOOL SVExternalToolTask::ConnectAllInputs()
{
	BOOL l_bRunConnect = TRUE;
	int i( 0 );

	// Check if input info is Ok for Input Images..
	for( i = 0 ; i < m_Data.m_lNumInputImages ; i++ )
	{
		l_bRunConnect = FALSE;
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
		if( rInfo.GetInputObjectInfo().UniqueObjectID != SVInvalidGUID )
		{
			l_bRunConnect = TRUE;
			break;
		}
	}

	// check if input info is ok for input objects.
	for( i = 0 ; i < m_Data.m_lNumInputValues ; i++ )
	{
		l_bRunConnect = FALSE;
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		if( rInfo.GetInputObjectInfo().UniqueObjectID != SVInvalidGUID )
		{
			l_bRunConnect = TRUE;
			break;
		}
	}

	if( l_bRunConnect )
	{
		return SVTaskObjectListClass::ConnectAllInputs();
	}
	return TRUE;
}

HRESULT SVExternalToolTask::ConnectInputImages()
{
	HRESULT hr = S_OK;

	std::vector< SVImageDefinitionStruct> aInputImages;	// used in m_dll.InitializeRun
	aInputImages.resize(m_Data.m_lNumInputImages);

	// Check Image Handles...
	for ( int i = 0 ; i < m_Data.m_lNumInputImages ; i++)
	{
		SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_Data.m_aInputImageInfo[i].GetInputObjectInfo().PObject);
		if( pImage )
		{
			SVImageInfoClass imageInfo = pImage->GetImageInfo();

			GetImageDefinitionStruct( aInputImages[i] , imageInfo );

			// remember MIL handle
			SVSmartHandlePointer l_ImageBuffer;
			pImage->GetImageHandle(l_ImageBuffer);

			SVImageBufferHandleImage l_MilHandle;

			if( !( l_ImageBuffer.empty() ) )
			{
				l_ImageBuffer->GetData( l_MilHandle );
			}

			// this cast assumes that a mil handle will never be larger 32 bits.
			m_aInspectionInputImages[i] = static_cast<long>(l_MilHandle.GetBuffer().GetIdentifier());

			if( m_bUseImageCopies )
			{
				SVImageInfoClass imageInfoCopy;
				GetImageInfo(&(aInputImages[i]), imageInfoCopy);	// get the physical info from our definition struct
				if( GetTool() != NULL )
				{
					imageInfoCopy.SetOwner( GetTool()->GetUniqueObjectID() );
				}
				else
				{
					imageInfoCopy.SetOwner( SV_GUID_NULL );
				}

				m_aInputImagesCopy[i].ResetObject();

			}// end if( m_bUseImageCopies )

		}// end if( pImage )
		else
		{// can't find image!
			//ASSERT(FALSE);
			hr = S_FALSE;
		}
	}

	return hr;
}

HRESULT SVExternalToolTask::ConnectInputs()
{
	HRESULT hr = S_OK;

	// ******* Connect Inputs *************
	// 
	//
	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*> ( GetTool()->GetAncestor( SVToolSetObjectType ) );

	SVObjectClass* pObject = NULL;
	SVInspectionProcess* pInspection = pToolSet->GetInspection();

	for (int i = 0 ; i < m_Data.m_lNumInputValues ; i++)
	{
		CString strObjectName ; 
		CString strCompleteObjectName = GetInspection()->GetCompleteObjectName();
		m_Data.m_aInputObjects[i].GetValue(strObjectName);
		strCompleteObjectName += "." + strObjectName;

		//MZA: change function to find object from inspection child object to anz dotted name
		SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strCompleteObjectName ), pObject );
		//
		// Inputs that are Variables must be found in the Inspection or the PPQ
		// otherwise the inputs are just variant inputs.
		//
		if ( pObject )	// input is another VO
		{
			SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
			rInfo.SetInputObject( pObject );
			if ( !rInfo.IsConnected() )
			{
				LONG_PTR bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<LONG_PTR>(&rInfo), NULL );
				//ASSERT( bSuccess );
				if( !bSuccess )
				{
					hr = S_FALSE;
				}
			}
		}
		else	// plain data
		{
			m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, FALSE);
			m_Data.m_aInputObjects[i].SetType(m_Data.m_aInputValueDefinitions[i].lVT);
			if( ::SVSendMessage( &(m_Data.m_aInputObjects[i]), SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS	)
			{
				hr = S_FALSE;
			}
		}

		// get Inupt object name
		SVStringValueObjectClass& rvo = m_Data.m_aInputObjectNames[i];
		rvo.SetDefaultValue(CString(m_Data.m_aInputValueDefinitions[i].bstrDisplayName), TRUE);	// set to all buckets
	
	}// end for ( int i = 0 ; i < m_Data.m_lNumInputValues ; i++)

	HRESULT hrConnectInputImages = ConnectInputImages();
	if ( hr == S_OK )
	{
		hr = hrConnectInputImages;
	}

	return hr;
}


HRESULT SVExternalToolTask::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	SVExternalTool* l_pTool = dynamic_cast<SVExternalTool*>(GetTool());
	if( l_pTool )
	{
		l_pTool->m_svSourceImageNames.SetArraySize( m_Data.m_lNumInputImages );
		for( int i = 0 ; i < m_Data.m_lNumInputImages ; i++ )
		{
			SVImageClass* l_pImage = GetInputImage(i);
			if( l_pImage )
			{
				CString l_strName = l_pImage->GetCompleteObjectName();
				
				l_pTool->m_svSourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, i, l_strName); 
			}
		}
	}
	return S_OK;
}



//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolTask.cpp_v  $
 * 
 *    Rev 1.7   08 Apr 2014 09:44:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  863
 * SCR Title:  Fix Bug in External Tool Inputs with inspection name.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Connect Inputs to also use the Inspection name along with external tools input object name to find the object by dotted name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Feb 2014 09:10:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Free HBitmap images in UnInitialize.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2014 10:42:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Oct 2013 13:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 12:05:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 2013 12:36:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  863
 * SCR Title:  Fix Bug in External Tool Inputs with inspection name.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Change SetInputs to use GetObjectByDottedName(...
 * - Change ConnectInputs to use GetObjectByDottedName(...
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   02 May 2013 11:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:33:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   04 Apr 2013 14:50:10   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  825
 * SCR Title:  Fix External Tool Range indexing error
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed GetResultRangeObject returning the wrong object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   04 Oct 2012 10:45:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   04 Sep 2012 14:54:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class Id definitions to make the image definition the same as the value definition for the external tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   30 Jul 2012 12:57:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   27 Jul 2012 08:14:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   12 Jul 2012 14:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   02 Jul 2012 16:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   19 Jun 2012 12:43:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Mar 2011 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   25 Feb 2011 12:08:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   10 Feb 2011 14:39:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   27 Jan 2011 11:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   08 Dec 2010 08:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   09 Nov 2010 16:09:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   05 Nov 2010 11:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   04 Nov 2010 13:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   22 Oct 2010 08:32:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove asserts to improve runtime debugging functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   15 Oct 2010 15:25:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code assoicated with casting issues and HBITMAP changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   01 Jun 2010 14:19:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Dec 2009 10:18:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   30 Jul 2009 11:35:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   09 Jun 2008 13:32:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   fixed problem with the invalid box not showing up when an external tool is first added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   09 Apr 2008 07:58:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator and Set Parent operator with Set Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   07 Mar 2008 11:55:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added support for Transfer Image Definition List Source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   30 Oct 2007 15:17:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Replaced VariantClear with VariantInit in constructor.
 * - Added SysFreeString to free resource in Initialize.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   24 Jul 2007 13:06:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   21 Jun 2007 12:46:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27.1.0.1.0   07 Apr 2008 08:05:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27.1.0.2.0   14 Jul 2009 13:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27.1.0   30 Oct 2007 09:38:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SysFreeString in initialize.
 * Added Jim's -1840 to -1844 for some hr codes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   04 Aug 2006 16:27:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Removed an ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   04 Aug 2006 10:16:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   removed ResetObject option on Initialize
 * changed code to properly handle 0 input/result images and values
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   27 Jan 2006 07:49:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Backed out change to set default range. Use SCR 542.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   26 Jan 2006 16:27:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed default range for tool result values
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   27 Jan 2006 03:34:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed extent issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   26 Jan 2006 23:49:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Initialize method to create connection to the first input image to handle aux extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   20 Jan 2006 15:07:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
 * added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
 * added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   09 Dec 2005 09:58:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  528
 * SCR Title:  Fix External Tool problem with resizing input images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed logic for detecting input image size change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   06 Dec 2005 15:38:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  528
 * SCR Title:  Fix External Tool problem with resizing input images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   ResetObject in onRun if input images change
 * In Initialize, call ResetObject on created result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   08 Nov 2005 15:51:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Nov 2005 07:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Oct 2005 14:02:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary setting of the IsConnected flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   20 Oct 2005 15:31:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  517
 * SCR Title:  Fix External Tool dependency issues
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed dependency problems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Oct 2005 10:15:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed potential crash caused by C-style casts
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Jul 2005 12:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:07:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Mar 2005 13:55:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   put try catch block around Initialize in ResetObject.  When DLL is not loaded the Initialize will throw an exception.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Mar 2005 15:19:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized HBITMAP <-> MIL conversion
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Mar 2005 15:21:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed initialization bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Feb 2005 10:08:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized HBITMAP usage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 14:07:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Feb 2005 14:17:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Oct 2004 12:52:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to allow correct functioning when choosing a different DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Aug 2004 16:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed GetImageDefinitionStruct to switch on BandNumber if the image format is RGB8888 to fix a bug in the color system where the external tool outputs do not show up in window tool inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jul 2004 15:25:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to use default value from paInputValueDefs[i] if the default value is VT_EMPTY. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jul 2004 08:58:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added case to processMessage for SVMSGID_RESIZING to resize external tool output images when input images change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jul 2004 09:04:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed ObjectTypeInfo.SubType to ObjectTypeInfo.ObjectType
 * set m_aResultImages SV_ARCHIVABLE_IMAGE attribute in SetAllAttributes
 * call pImage->CloseObject before pImage->Create in Initialize
 * removed compiler warnings
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:22:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
