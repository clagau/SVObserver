//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolTask
//* .File Name       : $Workfile:   SVExternalToolTask.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.10  $
//* .Check In Date   : $Date:   30 Sep 2014 15:41:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolTask.h"

#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVObjectLibrary\SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOMFCLibrary\SVOINIClass.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVExternalTool.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVRange.h"
#include "SVOCore/SVTaskObject.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "SVStatusLibrary\GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVExternalToolTask, SVExternalToolTaskGuid )

SVExternalToolTaskData::SVExternalToolTaskData() {}

SVExternalToolTaskData::SVExternalToolTaskData(const SVExternalToolTaskData& src)
{
	*this = src;
}

SVExternalToolTaskData::~SVExternalToolTaskData()
{
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
			m_aInputObjects.resize( rhs.m_aInputObjects.size() );
		}
		std::copy( rhs.m_aInputObjects.begin(), rhs.m_aInputObjects.end(), m_aInputObjects.begin() );

		if ( m_aInputObjectNames.size() != rhs.m_aInputObjectNames.size() )	// if this is only a copy of the original, size will be 0
		{
			m_aInputObjectNames.resize( rhs.m_aInputObjectNames.size() );
		}
		std::copy( rhs.m_aInputObjectNames.begin(), rhs.m_aInputObjectNames.end(), m_aInputObjectNames.begin() );

		if ( m_aResultObjects.size() != rhs.m_aResultObjects.size() )	// if this is only a copy of the original, size will be 0
		{
			m_aResultObjects.resize( rhs.m_aResultObjects.size() );
		}
		std::copy( rhs.m_aResultObjects.begin(), rhs.m_aResultObjects.end(), m_aResultObjects.begin() );

		if ( m_aResultObjectNames.size() != rhs.m_aResultObjectNames.size() )	// if this is only a copy of the original, size will be 0
		{
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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVExternalToolTaskObjectType;

	size_t i;

	m_hrInitialized = S_FALSE;

	SvOml::SVOINIClass m_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());

	m_bUseImageCopies = m_SvimIni.GetValueInt(_T("External Tool"), _T("UseImageCopy"), TRUE) != FALSE;

	// Initialize input images array.
	m_Data.m_aInputImageInfo.resize(SVExternalToolTaskData::NUM_INPUT_IMAGES);
	for ( i=0; i < m_Data.m_aInputImageInfo.size(); i++)
	{
		SVString l_Name = SvUl_SF::Format( _T( "ExternalToolTaskImage%d" ), i );

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
		SVString l_Name = SvUl_SF::Format( _T( "ExternalToolTaskValue%d" ), i );

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

		if( nullptr != GetTool() )
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
		SvUl::SetBits( m_Data.m_aInputObjects[i].ObjectAttributesAllowedRef(),  SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SETABLE_ONLINE, bUsed );
		SvUl::SetBits( m_Data.m_aInputObjectNames[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE , false );
		SvUl::SetBits( m_Data.m_aInputObjectNames[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, bUsed );
		SvUl::SetBits( m_Data.m_aInputObjects[i].ObjectAttributesAllowedRef(), SV_SELECTABLE_FOR_EQUATION , bUsed );
	}

	for ( i = 0; i < SVExternalToolTaskData::NUM_RESULT_IMAGES; i++)
	{
		SvUl::SetBits( m_aResultImages[i].ObjectAttributesAllowedRef(), SV_HIDDEN, i >= m_Data.m_lNumResultImages );
		SvUl::SetBits( m_aResultImages[i].ObjectAttributesAllowedRef(), SV_ARCHIVABLE_IMAGE, i < m_Data.m_lNumResultImages );
	}

	for ( i = 0; i < SVExternalToolTaskData::NUM_RESULT_OBJECTS; i++)
	{
		bool bUsed = i < m_Data.m_lNumResultValues;
		SvUl::SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE, bUsed );
		SvUl::SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, false );
		SvUl::SetBits( m_Data.m_aResultObjectNames[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE | SV_PRINTABLE, false );
		SvUl::SetBits( m_Data.m_aResultObjects[i].ObjectAttributesAllowedRef(),  SV_SELECTABLE_FOR_EQUATION, bUsed );
	}

	for ( i=0; i < static_cast<long>(m_Data.m_aDllDependencies.size()); i++)
	{
		SvUl::SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_REMOTELY_SETABLE, true );
		SvUl::SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_VIEWABLE, false );
		CString sTemp;
		m_Data.m_aDllDependencies[i].GetValue( sTemp );
		bool bUsed = !sTemp.IsEmpty();
		SvUl::SetBits( m_Data.m_aDllDependencies[i].ObjectAttributesAllowedRef(),  SV_PRINTABLE, bUsed );
	}
	SvUl::SetBits( m_Data.m_voToolVersion.ObjectAttributesAllowedRef(), SV_PRINTABLE, true );
	SvUl::SetBits( m_Data.m_voToolName.ObjectAttributesAllowedRef(), SV_PRINTABLE, true );
	SvUl::SetBits( m_Data.m_voDllPath.ObjectAttributesAllowedRef(), SV_PRINTABLE | SV_REMOTELY_SETABLE, true );
}

SVExternalToolTask::~SVExternalToolTask()
{
	CloseObject();
	// remove embeddeds so that invalid objects won't be Close()d
	m_embeddedList.RemoveAll();
}

BOOL SVExternalToolTask::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = false;

	if( SVTaskObjectListClass::CreateObject( PCreateStructure ) &&
			PCreateStructure->CheckCreateStructName( "SVToolLevelCreateStruct" ) )
	{
		if( nullptr != GetInspection() && nullptr != GetTool() )
		{
			HRESULT hr = S_OK;
			try
			{
				hr = Initialize();
			}
			catch ( const SvStl::MessageContainer& e)
			{
				hr = static_cast<HRESULT> (e.getMessage().m_MessageCode);
			}

			l_bOk = true;
		}
	}

	m_isCreated = l_bOk;

	return l_bOk;
}

HRESULT SVExternalToolTask::Initialize(	SVDllLoadLibraryCallback fnNotify )
{

	HRESULT hr = S_FALSE;
	BSTR bstrName = nullptr;
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
	catch( const SvStl::MessageContainer& e )
	{
		m_hrInitialized = static_cast<HRESULT> (e.getMessage().m_MessageCode);
		throw;
	}

	if ( S_OK == hr )
	{
		try
		{
			///////////////////////////////////////
			//    Get DLL information
			///////////////////////////////////////

			hr = m_dll.GetToolName(&bstrName);
			if( S_OK != hr )
			{
				throw hr;
			}

			CString strName( bstrName );
			m_Data.m_voToolName.SetValue(1, strName );

			SysFreeString( bstrName );
			
			long lVersion = 0;
			hr = m_dll.GetToolVersion(&lVersion);

			if( S_OK != hr )
			{
				throw hr;
			}
			m_Data.m_voToolVersion.SetValue(1, lVersion );

			///////////////////////////////////////
			//    Initialize Inputs
			///////////////////////////////////////

			// Initialize Input Images

			hr = m_dll.GetNumberOfInputImages( &m_Data.m_lNumInputImages );
			if( S_OK != hr )
			{
				throw hr;
			}

			std::vector< SVImageDefinitionStruct> aInputImages;	// used in m_dll.InitializeRun

			m_aInspectionInputImages.resize(m_Data.m_lNumInputImages);
			m_aInspectionInputHBMImages.resize(m_Data.m_lNumInputImages);
			aInputImages.resize(m_Data.m_lNumInputImages);

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
							if ( nullptr != m_aInspectionInputHBMImages[i].hbm )
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

								if( nullptr != GetTool() )
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
				else // can't find image!
				{
					// @TODO - add error handling
				}
			}// end for( int i = 0 ; i < m_Data.m_lNumInputImages ; i++ )


			///////////////////////////////////////
			// Initialize Input Objects
			///////////////////////////////////////

			InputValueDefinitionStruct* paInputValueDefs = nullptr;
			hr = m_dll.GetInputValueDefinitions( &m_Data.m_lNumInputValues, &paInputValueDefs );
			if( S_OK != hr )
			{
				throw hr;
			}

			m_aInspectionInputValues.resize(m_Data.m_lNumInputValues);
			m_Data.m_aInputValueDefinitions.resize(m_Data.m_lNumInputValues);

			SVObjectClass* pObject = nullptr;

			for ( i = 0 ; i < m_Data.m_lNumInputValues ; i++)
			{
				SVVariantValueObjectClass& rInputValue = m_Data.m_aInputObjects[i];
				if( rInputValue.GetDefaultType() == VT_EMPTY )
				{
					rInputValue.SetDefaultValue(paInputValueDefs[i].vDefaultValue, TRUE);
				}

				HRESULT hrChangeType = ::VariantChangeType(&m_aInspectionInputValues[i], &m_aInspectionInputValues[i], 0, static_cast<VARTYPE>(paInputValueDefs[i].lVT) );

				CString strTmp ; 
				m_Data.m_aInputObjects[i].GetValue(strTmp);

				m_Data.m_aInputValueDefinitions[i] = paInputValueDefs[i];

				// get Input object name
				SVStringValueObjectClass& rvo = m_Data.m_aInputObjectNames[i];
				rvo.SetDefaultValue(CString(paInputValueDefs[i].bstrDisplayName), TRUE);	// set to all buckets
			
			}// end for ( int i = 0 ; i < m_Data.m_lNumInputValues ; i++)

			HRESULT hrConnectInputs;
			if( S_OK != (hrConnectInputs = ConnectInputs()) )
			{
				hr = hrConnectInputs;
			}

			hr = m_dll.DestroyInputValueDefinitionStructures(paInputValueDefs);
			if( S_OK != hr )
			{
				throw hr;
			}
			paInputValueDefs = nullptr;

			InspectionInputsToVariantArray();

			///////////////////////////////////////
			//    Initialize Results
			///////////////////////////////////////

			// Initialize Result Objects
			ResultValueDefinitionStruct* paResultValueDefs = nullptr;
			hr = m_dll.GetResultValueDefinitions( &m_Data.m_lNumResultValues, &paResultValueDefs);
			if( S_OK != hr )
			{
				throw hr;
			}

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
			if( S_OK != hr )
			{
				throw hr;
			}

			paResultValueDefs = nullptr;

			///////////////////////////////////////
			//    InitializeRun
			///////////////////////////////////////

			hr = m_dll.InitializeRun(guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(aInputImages[0]) : nullptr,
                                           m_Data.m_lNumInputValues, m_Data.m_lNumInputValues ? &(m_aInspectionInputValues[0]) : nullptr );
			if( S_OK != hr )
			{
				throw hr;
			}
			if( m_dll.UseMil() )
			{
				// This must be done after InitializeRun so the Dll has a guid for this tool
				hr = m_dll.SetMILInputImages( guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : nullptr );
				if( S_OK != hr )
				{
					throw hr;
				}
			}
			///////////////////////////////////////
			// Initialize Result Images
			///////////////////////////////////////

			SVImageDefinitionStruct* paResultImageDefs = nullptr;
			hr = m_dll.GetResultImageDefinitions(guid, &m_Data.m_lNumResultImages, &paResultImageDefs);
			if( S_OK != hr )
			{
				throw hr;
			}

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
					if( nullptr != GetTool() )
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
					if( nullptr != GetTool() )
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
				if( nullptr != GetTool() )
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
			if( S_OK != hr )
			{
				throw hr;
			}
			paResultImageDefs = nullptr;

			if( m_dll.UseMil() )
			{
				hr = m_dll.SetMILResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr );
				if( S_OK != hr )
				{
					throw hr;
				}
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
			hr = S_FALSE;
		}
#endif
	}// end if ( S_OK == hr )
	
	if( S_OK == hr )
	{
		SVImageClass* l_psvOutput = GetResultImage( 0 );
		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		if( nullptr != pTool && nullptr != l_psvOutput )
		{
			hr = pTool->UpdateOffsetData( l_psvOutput );
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
		if ( nullptr != it->hbm )
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

	if(    nullptr != GetInspection()
		&& nullptr != GetTool()
		&& S_OK == m_hrInitialized )
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
				if ( pInputImage )
				{
					SVImageExtentClass l_InputImageExtents(pInputImage->GetImageExtents());

					CRect rectInput;
					l_InputImageExtents.GetOutputRectangle( rectInput );

					CRect rectPrevious(m_aPreviousInputImageRect[i]);
					
					if ( rectInput != rectPrevious )
					{
						bNeedReset = true;
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
				m_aInspectionInputValues[i] = GetInputValue(i);
				HRESULT hrChangeType = ::VariantChangeType(&m_aInspectionInputValues[i], &m_aInspectionInputValues[i], 0, static_cast<VARTYPE>(m_Data.m_aInputValueDefinitions[i].lVT) );
				if ( S_OK != hrChangeType )
				{
					m_aInspectionInputValues[i].Clear();
					m_aInspectionInputValues[i].ChangeType( static_cast<VARTYPE>(m_Data.m_aInputValueDefinitions[i].lVT));
				}
			}
			// send input values to DLL
			hr = m_dll.SetInputValues( guid, m_Data.m_lNumInputValues, m_Data.m_lNumInputValues ? &(m_aInspectionInputValues[0]) : nullptr );
			if ( S_OK != hr )
			{
				throw hr;
			}

			BOOL l_bOkToRun = TRUE;

			// collect input images
			for ( i=0; i < m_Data.m_lNumInputImages; i++)
			{
				SVSmartHandlePointer l_ImageBuffer;
				SVImageClass* pInputImage = GetInputImage(i);
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
				hr = m_dll.SetMILInputImages( guid, m_Data.m_lNumInputImages, m_Data.m_lNumInputImages ? &(m_aInspectionInputImages[0]) : nullptr );
				if ( S_OK != hr )
				{
					throw hr;
				}
			}
			else
			{
				std::vector<HBITMAP> aDIBHandles(m_Data.m_lNumInputImages);
				for ( int i=0; i < m_Data.m_lNumInputImages; i++ )
					aDIBHandles[i] = m_aInspectionInputHBMImages.at(i).hbm;
				hr = m_dll.SetHBITMAPInputImages(guid, m_Data.m_lNumInputImages ? &(aDIBHandles[0]) : nullptr );
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
				hr = m_dll.SetMILResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultImages[0]) : nullptr );
				if ( S_OK != hr )
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
			if ( S_OK != hr )
			{
				throw hr;
			}

			///////////////////////////////////////
			//    Outputs
			///////////////////////////////////////

			// Result Values

			hr = m_dll.GetResultValues(guid, m_Data.m_lNumResultValues, m_Data.m_lNumResultValues ? &(m_aInspectionResultValues[0]) : nullptr );
			if ( S_OK != hr )
			{
				throw hr;
			}

			for ( i=0; i < m_Data.m_lNumResultValues; i++ )
			{
				GetResultValueObject(i)->SetValue( RRunStatus.m_lResultDataIndex, m_aInspectionResultValues[i] );

				// Clear OleVariant that was created in Dll.
				m_aInspectionResultValues[i].Clear();
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
					hr = m_dll.GetHBITMAPResultImages( guid, m_Data.m_lNumResultImages, m_Data.m_lNumResultImages ? &(m_aInspectionResultHBMImages[0]) : nullptr );
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
						if ( S_OK == hr )
						{
							if( l_Code != SVMEE_STATUS_OK )
							{
								hr = l_Code | SVMEE_MATROX_ERROR;
							}
						}
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

			l_bOk = FALSE;
		}
#ifndef _DEBUG
		catch (...)
		{
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
	SVExternalToolTaskData* pData = new SVExternalToolTaskData(m_Data);
	rpCancelData = pData;

	// set temp data
	std::vector<SVResultClass*> aRangeResults = GetResultRangeObjects();
	std::vector<SVResultClass*>::iterator iter;
	for ( iter = aRangeResults.begin(); iter != aRangeResults.end(); ++iter)
	{
		SVCancelData* pCancelData = nullptr;
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
				SVObjectManagerClass::Instance().DisconnectObjectInput(pImageInfo->GetInputObjectInfo().UniqueObjectID, pImageInfo );
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
			SVObjectManagerClass::Instance().ConnectObjectInput(pImageInfo->GetInputObjectInfo().UniqueObjectID, pImageInfo);
		}

		SVToolClass* pTool = dynamic_cast <SVToolClass*> (GetAncestor( SVToolObjectType ));
		// Reset all objects again...
		if (nullptr != pTool)
		{
			pTool->resetAllObjects(true, false);
		}
		
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
		SVObjectClass *pObject = rInfo.GetInputObjectInfo().PObject;
		if( SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pObject) )
		{
			pValueObject->GetValue( vtTemp );
		}
		else if( BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (pObject) )
		{
			pBasicValueObject->getValue( vtTemp );
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
		}
		if( S_OK == hrChangeType )
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
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumInputImages )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[iIndex];
		return static_cast <SVImageClass*> (rInfo.GetInputObjectInfo().PObject);
	}
	return nullptr;
}

_variant_t SVExternalToolTask::GetInputValue(int iIndex)
{
	_variant_t value;
	value.Clear();
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumInputValues )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[iIndex];
		SVObjectClass *pObject = rInfo.GetInputObjectInfo().PObject;
		if(  SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (pObject) )
		{
			pValueObject->GetValue( value );
		}
		else if( BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (pObject) )
		{
			pBasicValueObject->getValue( value );
		}
		else
		{
			(m_Data.m_aInputObjects[iIndex]).GetValue( value );
		}
	}
	return value;
}

SVImageClass* SVExternalToolTask::GetResultImage(int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumResultImages )
	{
		return &(m_aResultImages[iIndex]);
	}
	return nullptr;
}

SVVariantValueObjectClass* SVExternalToolTask::GetResultValueObject(int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_Data.m_lNumResultValues )
	{
		return &(m_Data.m_aResultObjects[iIndex]);
	}
	return nullptr;
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
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultVariantObjectType;
		resultClassInfo.m_ClassId = SVVariantResultClassGuid;
		resultClassInfo.m_ClassName = _T("Range");
		strTitle = m_Data.m_aResultObjects [iIndex].GetName(); 
		resultClassInfo.m_ClassName += SV_TSTR_SPACE + strTitle;
		
		// Construct the result class
		pResult = dynamic_cast< SVVariantResultClass *> (resultClassInfo.Construct());
		
		if(!pResult)
		{
			throw  -12346;
		}
		
		Add( pResult );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVVariantValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		SVVariantValueObjectClass* pValue = dynamic_cast<SVVariantValueObjectClass*>(pResult->getFirstObject(info));
		
		if (!pValue)
		{
			throw -12347;
		}
		
		pValue->ObjectAttributesAllowedRef() = pValue->ObjectAttributesAllowed() & ~SV_DEFAULT_ATTRIBUTES;
		
		// Ensure this Object's inputs get connected
		pResult->ConnectAllInputs();

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(pResult, SVMFResetObject ) )
			{
				// Remove it from the TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( SV_GUID_NULL != objectID )
				{
					Delete( objectID );
				}
				else
				{
					delete pResult;
				}
				
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
	SVVariantResultClass*   pResult = nullptr;
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
			pResult = nullptr;
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

HRESULT SVExternalToolTask::ClearData()
{
	HRESULT hr = S_OK;
	int i = 0;
	
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
		if( S_OK != hrTmp )
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
		m_Data.m_aInputObjectInfo[i].SetInputObject( nullptr );
		m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, TRUE);
		m_Data.m_aInputObjectNames[i].SetDefaultValue(_T(""), TRUE);
	}

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

	// Data Definition List stuff
	CollectInputImageNames();

	try
	{
		hr = Initialize( SVDllLoadLibraryCallbackDefault() );
	}
	catch ( const SvStl::MessageContainer& e)
	{
		hr = static_cast<HRESULT> (e.getMessage().m_MessageCode);
	}

	return hr;
}

// compare cancel data (original) with current data.
HRESULT SVExternalToolTask::FindInvalidatedObjects(SVObjectVector& rList, const SVCancelData* pCancelData, FindEnum eWhich )
{
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

void SVExternalToolTask::GetDLLMessageString(HRESULT hr, BSTR* bstrMessage) const
{
	m_dll.GetMessageString(hr, bstrMessage);
}

bool SVExternalToolTask::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	bool Result(false);
	if (nullptr != pObjectInInfo)
	{
		SVObjectClass* pObject = pObjectInInfo->GetInputObjectInfo().PObject;
		if ( SVImageClass* pImage = dynamic_cast <SVImageClass*> (pObject) )
		{
			// find object
			for ( int i=0; i < SVExternalToolTaskData::NUM_INPUT_IMAGES; i++ )
			{
				SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
				if ( rInfo.GetInputObjectInfo().PObject == pImage )
				{
					// replace with tool set image
					SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (GetInspection());
					SVToolSetClass* pToolSet = (nullptr != pInspection) ? pInspection->GetToolSet() : nullptr;
					SVObjectTypeInfoStruct imageObjectInfo;
					imageObjectInfo.ObjectType = SVImageObjectType;
					SVImageClass* pToolSetImage = (nullptr != pToolSet) ? dynamic_cast <SVImageClass*> (pToolSet->getFirstObject(imageObjectInfo)) : nullptr;

					rInfo.SetInputObject( pToolSetImage );
					rInfo.GetInputObjectInfo().PObject->ConnectObjectInput(&rInfo);
					break;
				}
			}

			Result = true;
		}
		else if ( SVValueObjectClass* pValueObject = dynamic_cast <SVValueObjectClass*> (pObject) )
		{
			// find object
			for ( int i=0; i < SVExternalToolTaskData::NUM_INPUT_OBJECTS; i++ )
			{
				SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
				if ( rInfo.GetInputObjectInfo().PObject == pValueObject )
				{
					rInfo.SetInputObject( nullptr );
					rInfo.GetInputObjectInfo().PObject->ConnectObjectInput(&rInfo);

					// set value to default
					m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, true);
					break;
				}
			}

			Result = true;
		}
	}
	return __super::DisconnectObjectInput(pObjectInInfo) && Result;
}

void SVExternalToolTask::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	SVString newPrefix;
	SVString oldPrefix;

	if( const SVInspectionProcess* pInspection = dynamic_cast<const SVInspectionProcess*> (&rRenamedObject) )
	{
		newPrefix = pInspection->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType ) + _T( "." );
	}// end if
	else if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = pBasicValueObject->GetCompleteObjectNameToObjectType( nullptr, SVRootObjectType );
	}
	else if( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (&rRenamedObject) )
	{
		newPrefix = pValueObject->GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );
	}
	else
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	// loop through all inputs & rename
	// input objects
	for( long i=0; i < m_Data.m_lNumInputValues; i++ )
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		SVObjectClass* pInputObject = rInfo.GetInputObjectInfo().PObject;
		if( nullptr != pInputObject )
		{
			//Either it is the object itself changing name or the toolset
			if ( &rRenamedObject == pInputObject ||
				(&rRenamedObject == pInputObject->GetAncestor(rRenamedObject.GetObjectInfo().ObjectTypeInfo.ObjectType)) )
			{
				CString sValue;
				m_Data.m_aInputObjects[i].GetValue(sValue);
				sValue.Replace( oldPrefix.c_str(), newPrefix.c_str() );
				m_Data.m_aInputObjects[i].SetValue(1, sValue);
			}
		}
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

bool SVExternalToolTask::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );

	return __super::resetAllObjects( shouldNotifyFriends, silentReset ) && Result;
}

HRESULT SVExternalToolTask::DisconnectInputsOutputs(SVObjectVector& rListOfObjects)
{
	return __super::DisconnectInputsOutputs(rListOfObjects);
}

HRESULT SVExternalToolTask::HideInputsOutputs(SVObjectVector& rListOfObjects)
{
	return __super::HideInputsOutputs(rListOfObjects);
}

bool SVExternalToolTask::ConnectAllInputs()
{
	bool l_bRunConnect = true;
	int i( 0 );

	// Check if input info is Ok for Input Images..
	for( i = 0 ; i < m_Data.m_lNumInputImages ; i++ )
	{
		l_bRunConnect = false;
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputImageInfo[i];
		if( SV_GUID_NULL != rInfo.GetInputObjectInfo().UniqueObjectID )
		{
			l_bRunConnect = true;
			break;
		}
	}

	// check if input info is ok for input objects.
	for( i = 0 ; i < m_Data.m_lNumInputValues ; i++ )
	{
		l_bRunConnect = false;
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		if( SV_GUID_NULL != rInfo.GetInputObjectInfo().UniqueObjectID )
		{
			l_bRunConnect = true;
			break;
		}
	}

	if( l_bRunConnect )
	{
		return SVTaskObjectListClass::ConnectAllInputs();
	}
	return true;
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
				if( nullptr != GetTool() )
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
	for (int i = 0 ; i < m_Data.m_lNumInputValues ; i++)
	{
		SVInObjectInfoStruct& rInfo = m_Data.m_aInputObjectInfo[i];
		if ( nullptr != rInfo.GetInputObjectInfo().PObject )	// input is another VO
		{
			if ( !rInfo.IsConnected() )
			{
				if( !rInfo.GetInputObjectInfo().PObject->ConnectObjectInput(&rInfo) )
				{
					hr = S_FALSE;
				}
			}
		}
		else	// plain data
		{
			m_Data.m_aInputObjects[i].SetDefaultValue(m_Data.m_aInputValueDefinitions[i].vDefaultValue, FALSE);
			m_Data.m_aInputObjects[i].SetType(m_Data.m_aInputValueDefinitions[i].lVT);
			if( m_Data.m_aInputObjects[i].resetAllObjects(true, false) )
			{
				hr = S_FALSE;
			}
		}

		// get Input object name
		SVStringValueObjectClass& rvo = m_Data.m_aInputObjectNames[i];
		rvo.SetDefaultValue(CString(m_Data.m_aInputValueDefinitions[i].bstrDisplayName), TRUE);	// set to all buckets
	
	}// end for ( int i = 0 ; i < m_Data.m_lNumInputValues ; i++)

	HRESULT hrConnectInputImages = ConnectInputImages();
	if ( S_OK == hr )
	{
		hr = hrConnectInputImages;
	}

	return hr;
}

HRESULT SVExternalToolTask::CollectInputImageNames( )
{
	SVExternalTool* l_pTool = dynamic_cast<SVExternalTool*>(GetTool());
	if( l_pTool )
	{
		SVStaticStringValueObjectClass* imageNames = l_pTool->GetInputImageNames();
		if (nullptr != imageNames)
		{
			imageNames->SetArraySize( m_Data.m_lNumInputImages );
			for( int i = 0 ; i < m_Data.m_lNumInputImages ; i++ )
			{
				SVImageClass* l_pImage = GetInputImage(i);
				if( l_pImage )
				{
					CString l_strName = l_pImage->GetCompleteObjectName();

					imageNames->SetValue( 0/*Static value, this parameter will not used*/, i, l_strName); 
				}
			}
		}
	}
	return S_OK;
}



