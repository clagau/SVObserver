// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Feb 2014 16:11:30  $
// ******************************************************************************

#ifndef SVOCMARCHIVE_INL
#define SVOCMARCHIVE_INL

#include "SVOCMArchive.h"
#include "SVXmlStream.h"
#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVConfigurationTags.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVLibrary/SVBStr.h"							// SVBStr
#include "SVXMLLibrary/SVXML2TreeConverter.h"
#include "SVObserverEnums.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVObserverApp( unsigned long ulSVOVersion, 
																					 unsigned long &ulSECVersion, 
																					 CArchive &rArchive, 
																					 SVTreeType& p_rTree,
																					 CString &rIODocName,
																					 typename SVTreeType::SVBranchHandle p_pParent)
{
	SVTreeType::SVBranchHandle l_pBranch( NULL );

	CString csName, csValue;
	HRESULT hrOk = S_FALSE;

	bool bIsColorMode = FALSE;

	int iNumOfAcqs;

	_variant_t *psvData = NULL;

	// [CString] - Description
	hrOk = AddItem( CTAG_DESCRIPTION, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
	HandleErrorCondition(12002, hrOk);

	// [CString][CString] - Version Number
	rArchive >> csName;	// Version #
	hrOk = AddItem( CTAG_VERSION_NUMBER, p_pParent, SVUnsignedLongType, p_rTree, rArchive, &ulSECVersion );
	HandleErrorCondition(12003, hrOk);

	// Add configutation type default to SVIM_PRODUCT_FULL TRB.
	_variant_t l_Variant(SVIM_PRODUCT_FULL);
	hrOk = AddItem( CTAG_CONFIGURATION_TYPE, p_pParent, p_rTree, l_Variant );
	HandleErrorCondition(12004, hrOk);

	// JMS - Section Begin - Get and Check Color SVIM Mode Active Flag
	if ( ulSECVersion >= 0x00031401 )	// Color SVIM - Version 3.20 Beta 1
	{
		rArchive >> csName;	// Color SVIM Mode Active
		hrOk = AddItem( CTAG_COLOR_SVIM_MODE_ACTIVE, p_pParent, SVBoolType, p_rTree, rArchive, &bIsColorMode );
		HandleErrorCondition(12004, hrOk);
	}
	// JMS - Section End - Get and Check Color SVIM Mode Active Flag

	if ( ulSECVersion >= 0x00000002 )
	{
		// Read History Block
		hrOk = AddItem( CTAG_HISTORY_LEVEL, p_pParent, SVUnsignedLongType, p_rTree, rArchive );
		HandleErrorCondition(12005, hrOk);

		// Creation Date 
		rArchive >> csName;	// Creation Date #
		hrOk = AddItem( CTAG_CREATION_DATE, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12006, hrOk);

		if ( ulSECVersion >= 0x00010000 )
		{
			// Last Update
			rArchive >> csName;	// Last Update #
			hrOk = AddItem( CTAG_LAST_UPDATE, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
			HandleErrorCondition(12007, hrOk);
		}

		// Creation Author / Update Author
		rArchive >> csName;	// Author #
		hrOk = AddItem( CTAG_AUTHOR, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12008, hrOk);

		if (ulSECVersion < 0x00030001)
		{
			// Serialize module trigger source...
			// Out of use..
			hrOk = AddItem( CTAG_MODULE_TRIGGER_SOURCE, p_pParent, SVIntType, p_rTree, rArchive );
			HandleErrorCondition(12009, hrOk);

			if (ulSECVersion >= 0x00010000)
			{
				// Serialize intern trigger rate...
				// New in file version 1.0
				// Out of use in version 2.42 Beta 1
				hrOk = AddItem( CTAG_INTERNAL_TRIGGER_SOURCE, p_pParent, SVDoubleType, p_rTree, rArchive );
				HandleErrorCondition(12010, hrOk);
			}

			// Serialize PPQ settings...
			// Out of use. Only for older versions with only one main PPQ...
			hrOk = AddItem( CTAG_PPQ_SETTINGS, p_pParent, SVIntType, p_rTree, rArchive );
			HandleErrorCondition(12011, hrOk);

			hrOk = AddItem( CTAG_PPQ_SETTINGS_ACTIVATED, p_pParent, SVBoolType, p_rTree, rArchive );
			HandleErrorCondition(12012, hrOk);

			if (ulSECVersion >= 0x00020303)
			{
				hrOk = AddItem( CTAG_PPQ_MODE, p_pParent, SVIntType, p_rTree, rArchive );
				HandleErrorCondition(12013, hrOk);
				hrOk = AddItem( CTAG_OUTPUT_DELAY_TIME, p_pParent, SVLongType, p_rTree, rArchive );
				HandleErrorCondition(12014, hrOk);
			}
		}

		// Serialize DDE Server settings...
		hrOk = AddItem( CTAG_CURRENT_DDE_SERVER, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12015, hrOk);

		hrOk = AddItem( CTAG_CURRENT_DDE_TOPIC, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12016, hrOk);

		hrOk = AddItem( CTAG_IS_DDE_SERVER_CONNECTED, p_pParent, SVBoolType, p_rTree, rArchive );
		HandleErrorCondition(12017, hrOk);


		hrOk = GetIOBranch( p_rTree, l_pBranch );
		HandleErrorCondition(12018, hrOk);

		// Serialize I/O Doc
		hrOk = AddItem( CTAG_IS_IO_LOADED, l_pBranch, SVBoolType, p_rTree, rArchive );
		HandleErrorCondition(12019, hrOk);

		hrOk = AddItem( CTAG_IO_FILE_NAME, l_pBranch, SVBStrType, p_rTree, rArchive, &rIODocName );
		HandleErrorCondition(12020, hrOk);

		// Serialize System
		hrOk = AddItem( CTAG_NUMBER_OF_ACQ_DEVICES, p_pParent, SVIntType, p_rTree, rArchive, &iNumOfAcqs );
		HandleErrorCondition(12021, hrOk);

		for ( int i = 0; i < iNumOfAcqs; i++ )
		{
			SVTreeType::SVBranchHandle l_pAcq( NULL );
			int iNumberOfIPs = 0;

			csName.Format( CTAGF_ACQ_BOARD_X, i );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pAcq );
			HandleErrorCondition(12022, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pAcq != NULL )
				{
					hrOk = ArchiveSVSystemClass( ulSVOVersion, 
						ulSECVersion, 
						bIsColorMode,
						rArchive, 
						p_rTree,
						l_pAcq,
						iNumberOfIPs );
					HandleErrorCondition(12023, hrOk);
					if ( hrOk == S_OK )
					{
						for ( int j = 0; j < iNumberOfIPs; j++ )
						{
							long lValue;

							rArchive >> lValue;
							//								csName.Format( "Number of Image Views for IP%d", j );

							//								hrOk = AddItem( (LPCTSTR) csName, p_pParent, SVLongType, p_rTree, rArchive );
						}
					}
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVSystemClass(unsigned long ulSVOVersion, 
																					 unsigned long ulSECVersion, 
																					 bool bIsColorMode,
																					 CArchive &rArchive, 
																					 SVTreeType& p_rTree,
																					 typename SVTreeType::SVBranchHandle p_pParent,
																					 int &riIPDocSize )
{
	CString	csName;
	HRESULT hrOk = S_FALSE;

	CString csBoardName;
	short sValue;
	unsigned char ucValue;
	unsigned long ulValue;

	rArchive >> ulValue;	// Microsoft class run-time serialize version information
	rArchive >> sValue;	// Microsoft class run-time serialize name size
	for ( int i = 0; i < sValue; i++ )
	{
		rArchive >> ucValue; // Microsoft class run-time serialize name
	}

	if (ulSECVersion >= 0x00020002)
	{
		int i( 0 );

		SVTreeType::SVBranchHandle l_pBranch( NULL );
		SVTreeType::SVBranchHandle l_pNext( NULL );

		hrOk = AddItem( CTAG_UNIQUE_OBJECT_ID, p_pParent, SVClsIdType, p_rTree, rArchive );
		HandleErrorCondition(12024, hrOk);

		hrOk = AddItem( CTAG_NAME_OF_BOARD_TYPE, p_pParent, SVBStrType,
			p_rTree, rArchive, &csName );
		HandleErrorCondition(12025, hrOk);


		if ( csName.CompareNoCase( "VIPER QUAD" ) == 0 || csName.CompareNoCase( "VIPER DUAL" ) == 0 )
		{
			csBoardName = "Viper_Quad_1";
		}
		else if ( csName.CompareNoCase( "VIPER RGB" ) == 0 )
		{
			csBoardName = "Viper_RGB_1";
		}
		else if ( csName.CompareNoCase( "VGA" ) == 0 )
		{
			csBoardName = "M_System_Vga";
		}

		hrOk = AddItem( CTAG_NAME_OF_BOARD_DRIVER, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csName );
		HandleErrorCondition(12026, hrOk);

		hrOk = AddItem( CTAG_MAXIMIM_DEVICE_NUMBER, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12027, hrOk);

		hrOk = AddBranch( p_rTree, CTAG_DIGITIZER_ARRAY_CLASS, p_pParent, l_pBranch );
		HandleErrorCondition(12028, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVDigitizerArrayClass( ulSVOVersion, 
					ulSECVersion, 
					bIsColorMode,
					csBoardName,
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12029, hrOk);
			}
		}

		hrOk = AddItem( CTAG_SIZE_OF_IPDOC_INFO_LIST, p_pParent, SVIntType, 
			p_rTree, rArchive, &riIPDocSize );
		HandleErrorCondition(12030, hrOk);

		for ( i = 0; i < riIPDocSize; i++ )
		{
			hrOk = GetInspectionBranch( i+1, p_rTree, l_pNext );
			HandleErrorCondition(12031, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pNext != NULL )
				{
					_bstr_t l_Temp;

					p_rTree.GetBranchName( l_pNext, l_Temp.GetBSTR() );

					CString csInspection = static_cast< LPCTSTR >( l_Temp );

					hrOk = ArchiveSVIPDocInfoClass( ulSVOVersion, 
						ulSECVersion,
						csBoardName,
						csInspection,
						rArchive, 
						p_rTree,
						l_pNext );
					HandleErrorCondition(12032, hrOk);
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDigitizerArrayClass(unsigned long ulSVOVersion, 
																									 unsigned long ulSECVersion, 
																									 bool bIsColorMode,
																									 CString &rcsBoardName,
																									 CArchive &rArchive, 
																									 SVTreeType& p_rTree,
																									 typename SVTreeType::SVBranchHandle p_pParent )
{
	HRESULT hrOk = S_FALSE;

	int iSize = 1;

	if (ulSECVersion >= 0x00022a01)
	{
		CString csName;

		SVTreeType::SVBranchHandle l_pBranch( NULL );

		hrOk = AddItem( CTAG_SIZE_OF_DIGITIZER_ARRAY, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12033, hrOk);

		for ( int i = 0; i < iSize; i++ )
		{
			hrOk = GetAcquisitionDeviceBranch( rcsBoardName, i, p_rTree, l_pBranch );
			HandleErrorCondition(12034, hrOk);

			hrOk = GetCameraBranch( i+1, p_rTree, l_pBranch );
			HandleErrorCondition(12035, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVDigitizerObjectClass( ulSVOVersion, 
						ulSECVersion,
						bIsColorMode, 
						rcsBoardName, 
						i,
						rArchive,
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12036, hrOk);
				}
			}
		}

		if ( bIsColorMode )
		{
			hrOk = AddItem( CTAG_RED_DIGITIZER, p_pParent, SVIntType, 
				p_rTree, rArchive );
			HandleErrorCondition(12037, hrOk);

			hrOk = AddItem( CTAG_GREEN_DIGITIZER, p_pParent, SVIntType, 
				p_rTree, rArchive );
			HandleErrorCondition(12038, hrOk);

			hrOk = AddItem( CTAG_BLUE_DIGITIZER, p_pParent, SVIntType, 
				p_rTree, rArchive );
			HandleErrorCondition(12039, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDigitizerObjectClass(unsigned long ulSVOVersion, 
																										unsigned long ulSECVersion, 
																										bool bIsColorMode,
																										CString &rcsBoardName, 
																										long lIndex, 
																										CArchive &rArchive, 
																										SVTreeType& p_rTree,
																										typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pPPQCameraBranch( NULL );

	bool bIsAcq = FALSE;
	bool bIsPQ = FALSE;

	if (ulSECVersion >= 0x00022a01)
	{
		hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
			p_rTree, rArchive );
		HandleErrorCondition(12040, hrOk);

		hrOk = AddItem( CTAG_OBJECT_NAME, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csValue );
		HandleErrorCondition(12041, hrOk);
	}

	SVTreeType::SVBranchHandle l_pPPQ( NULL );

	hrOk = GetPPQBranch( lIndex+1, p_rTree, l_pPPQ );
	HandleErrorCondition(12042, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pPPQ != NULL )
		{
			SVTreeType::SVBranchHandle l_pNext( NULL );

			if ( GetBranch( p_rTree, CTAG_TRIGGER, l_pPPQ, l_pNext ) != S_OK )
			{
				hrOk = AddBranch( p_rTree, CTAG_TRIGGER, l_pPPQ, l_pNext );
				HandleErrorCondition(12043, hrOk);
			}

			if ( hrOk == S_OK )
			{
				CString csName;

				SVTreeType::SVBranchHandle l_pNext_x( NULL );

				csName.Format( CTAGF_TRIGGER_X, lIndex+1 );

				if ( GetBranch( p_rTree, csName, l_pNext, l_pNext_x ) != S_OK )
				{
					_variant_t svVariant;

					svVariant.SetString( csName );

					hrOk = AddBranch( p_rTree, csName, l_pNext, l_pNext_x );
					HandleErrorCondition(12044, hrOk);
				}
			}

			if ( GetBranch( p_rTree, CTAG_CAMERA, l_pPPQ, l_pPPQCameraBranch ) != S_OK )
			{
				hrOk = AddBranch( p_rTree, CTAG_CAMERA, l_pPPQ, l_pPPQCameraBranch );
				HandleErrorCondition(12045, hrOk);
			}
		}
	}

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_DIGITIZER_INFO_STRUCT, p_pParent, l_pBranch );
	HandleErrorCondition(12047, hrOk);

	long lCount = 0;    // number of inputs for this Digitizer

	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitizerInfoStruct( ulSVOVersion, 
				ulSECVersion, 
				bIsColorMode, 
				rcsBoardName, 
				lIndex,
				l_pPPQCameraBranch,
				rArchive, 
				p_rTree,
				l_pBranch,
				lCount);
			HandleErrorCondition(12048, hrOk);
		}
	}

	hrOk = AddItem( CTAG_IS_ACQUISITION_TRIGGER_INFORMATION_STORED, p_pParent, SVBoolType, 
		p_rTree, rArchive, &bIsAcq );
	HandleErrorCondition(12049, hrOk);

	if ( bIsAcq )
	{
		hrOk = GetTriggerBranch( lIndex+1, p_rTree, l_pBranch );
		HandleErrorCondition(12050, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				SVTreeType::SVLeafHandle htiNext;

				if ( GetItem( p_rTree, CTAG_TRIGGER_DEVICE, l_pBranch, htiNext ) != S_OK )
				{
					CString csName;

					_variant_t svVariant;

					csName.Format( "%s.Dig_%d", rcsBoardName, lIndex );

					svVariant.SetString( csName );

					hrOk = AddItem( CTAG_TRIGGER_DEVICE, l_pBranch, p_rTree, svVariant );
					HandleErrorCondition(12051, hrOk);
				}

				hrOk = ArchiveSVAcquisitionTriggerEventServerClass( ulSVOVersion, 
					ulSECVersion, 
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12052, hrOk);
			}
		}
	}

	hrOk = AddItem( CTAG_IS_PQ_INFORMATION_STORED, p_pParent, SVBoolType, 
		p_rTree, rArchive, &bIsPQ );
	HandleErrorCondition(12053, hrOk);

	if ( bIsPQ )
	{
		hrOk = AddBranch( p_rTree, CTAG_PQ_OBJECT_CLASS, p_pParent, l_pBranch );
		HandleErrorCondition(12054, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVPQObjectClass( ulSVOVersion, 
					ulSECVersion,
					lIndex, 
					rArchive, 
					p_rTree,
					l_pBranch,
					lCount);
				HandleErrorCondition(12055, hrOk);
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDigitizerInfoStruct(unsigned long ulSVOVersion, 
																									 unsigned long ulSECVersion, 
																									 bool bIsColorMode,
																									 CString &rcsBoardName, 
																									 long lIndex, 
																									 typename SVTreeType::SVBranchHandle p_pPPQCameraBranch,
																									 CArchive &rArchive, 
																									 SVTreeType& p_rTree,
																									 typename SVTreeType::SVBranchHandle p_pParent,
																									 long& rlCount)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	if (ulSECVersion >= 0x00000002)
	{
		int iSelected = 0;

		hrOk = AddItem( CTAG_ACTIVATE_DIGITIZER, p_pParent, SVBoolType, 
			p_rTree, rArchive );
		HandleErrorCondition(12056, hrOk);

		hrOk = AddItem( CTAG_SELECTED_SOURCE, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSelected );
		HandleErrorCondition(12057, hrOk);

		hrOk = AddBranch( p_rTree, CTAG_ACQUISITION_SOURCE_LIST, p_pParent, l_pBranch );
		HandleErrorCondition(12058, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVAcquisitionSourceListClass( ulSVOVersion, 
					ulSECVersion, 
					bIsColorMode, 
					rcsBoardName, 
					lIndex,
					iSelected,
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12059, hrOk);
			}
		}

		hrOk = AddItem( CTAG_MAXIMUM_GRAB_CHANNEL, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12060, hrOk);

		hrOk = AddItem( CTAG_MAXIMIM_SIMULTANEOUS_GRAB_CHANNEL, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12061, hrOk);

		hrOk = AddItem( CTAG_MAXIMUM_MONOCHROME_SOURCE, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12062, hrOk);

		hrOk = AddItem( CTAG_MAXIMUM_RGB_SOURCE, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12063, hrOk);

		hrOk = AddItem( CTAG_MAXIMUM_BAND_NUMBER_TYPES, p_pParent, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12064, hrOk);

		short sSize;

		hrOk = AddItem( CTAG_SIZE_OF_BAND_NUMBER_TYPE_LONG_ARRAY, p_pParent, SVShortType, 
			p_rTree, rArchive, &sSize );
		HandleErrorCondition(12065, hrOk);

		hrOk = AddArray( CTAG_BAND_NUMBER_TYPE_LONG_ARRAY, p_pParent, SVUnsignedLongArrayType, 
			sSize, p_rTree, rArchive );
		HandleErrorCondition(12066, hrOk);
	}

	if (ulSECVersion >= 0x00020303)
	{
		short sSize;

		hrOk = AddItem( CTAG_SIZE_OF_PPQ_INDEX_HANDLE_OWNER_ARRAY, p_pParent, SVShortType, 
			p_rTree, rArchive, &sSize );
		HandleErrorCondition(12067, hrOk);

		for ( short s = 0; s < sSize; s++ )
		{
			CString csName;

			csName.Format( CTAGF_PPQ_INDEX_HANDLE_OWNER_X, s );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
			HandleErrorCondition(12068, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					SVTreeType::SVBranchHandle l_pCamera_x( NULL );

					CString csName;

					if ( 1 < sSize )
					{
						csName.Format( CTAGF_CAMERA_X, s+1 );
					}
					else
					{
						csName.Format( CTAGF_CAMERA_X, lIndex+1 );
					}

					if ( GetBranch( p_rTree, csName, p_pPPQCameraBranch, l_pCamera_x ) != S_OK )
					{
						_variant_t svVariant;

						svVariant.SetString( csName );

						hrOk = AddBranch( p_rTree, csName, p_pPPQCameraBranch, l_pCamera_x );
						HandleErrorCondition(12046, hrOk);
					}

					hrOk = ArchiveSVPPQIndexHandleOwnerStruct( ulSVOVersion, 
						ulSECVersion,
						l_pCamera_x, 
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12069, hrOk);
				}
			}
		}// end for ( short s = 0; s < sSize; s++ )

		hrOk = AddBranch( p_rTree, CTAG_DDE_INPUT_ENTRY_LIST, p_pParent, l_pBranch );
		HandleErrorCondition(12070, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVDDEInputEntryListClass( ulSVOVersion, 
					ulSECVersion, 
					lIndex, 
					rArchive, 
					p_rTree,
					l_pBranch,
					rlCount);
				HandleErrorCondition(12071, hrOk);
			}
		}

		hrOk = AddBranch( p_rTree, CTAG_PQ_DIGITAL_INPUT_INFO_LIST, p_pParent, l_pBranch );
		HandleErrorCondition(12072, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVPQDigitialInputInfoListClass( ulSVOVersion, 
					ulSECVersion, 
					lIndex, 
					rArchive, 
					p_rTree,
					l_pBranch,
					rlCount);
				HandleErrorCondition(12073, hrOk);
			}
		}
	}// end if (ulSECVersion >= 0x00020303)

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVAcquisitionSourceListClass(unsigned long ulSVOVersion, 
																													unsigned long ulSECVersion, 
																													bool bIsColorMode,
																													CString &rcsBoardName, 
																													long lIndex, 
																													long lSelected, 
																													CArchive &rArchive, 
																													SVTreeType& p_rTree,
																													typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	int iSize;

	if (ulSECVersion >= 0x00020002)
	{
		hrOk = AddItem( CTAG_SIZE_OF_ACQUISITION_SOURCE_LIST, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12074, hrOk);

		for ( int i = 0; i < iSize; i++ )
		{
			CString csName;

			SVTreeType::SVBranchHandle l_pBranch( NULL );

			csName.Format( CTAGF_ACQUISITION_SOURCE_X, i );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
			HandleErrorCondition(12075, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVAcquisitionSourceClass( ulSVOVersion, 
						ulSECVersion, 
						bIsColorMode, 
						rcsBoardName, 
						lIndex,
						lSelected == i,
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12076, hrOk);
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVAcquisitionSourceClass(unsigned long ulSVOVersion, 
																											unsigned long ulSECVersion, 
																											bool bIsColorMode,
																											CString &rcsBoardName, 
																											long lIndex, 
																											bool bIsSelected, 
																											CArchive &rArchive, 
																											SVTreeType& p_rTree,
																											typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	if (ulSECVersion >= 0x00000002)
	{
		unsigned long ulReserved;

		SVTreeType::SVBranchHandle l_pBranch( NULL );
		SVTreeType::SVBranchHandle l_pLight( NULL );

		hrOk = AddBranch( p_rTree, CTAG_IMAGE_INFO_CLASS, p_pParent, l_pBranch );
		HandleErrorCondition(12077, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVImageInfoClass( ulSVOVersion, 
					ulSECVersion, 
					bIsColorMode, 
					rcsBoardName,
					lIndex,
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12078, hrOk);
			}
		}

		if ( bIsSelected )
		{
			hrOk = GetAcquisitionDeviceBranch( rcsBoardName, lIndex, p_rTree, l_pBranch );
			HandleErrorCondition(12079, hrOk);
			if ( hrOk == S_OK )
			{
				hrOk = AddBranch( p_rTree, CTAG_LIGHT_REFERENCE_ARRAY, l_pBranch, l_pLight );
				HandleErrorCondition(12080, hrOk);
			}
		}
		else
		{
			hrOk = AddBranch( p_rTree, CTAG_LIGHT_REFERENCE_ARRAY, p_pParent, l_pLight );
			HandleErrorCondition(12081, hrOk);
		}

		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVLightReferenceArrayClass( ulSVOVersion, 
				ulSECVersion, 
				bIsColorMode,
				rArchive, 
				p_rTree,
				l_pLight );
			HandleErrorCondition(12082, hrOk);
		}

		hrOk = AddItem( CTAG_ACQUISITION_TYPE, p_pParent, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12083, hrOk);

		hrOk = AddItem( CTAG_ACQUISITION_TRIGGER_TYPE, p_pParent, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12084, hrOk);

		hrOk = AddItem( CTAG_ACQUISITION_SOURCE_NAME, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csValue );
		HandleErrorCondition(12085, hrOk);

		if ( bIsSelected )
		{
			hrOk = GetAcquisitionDeviceBranch( rcsBoardName, lIndex, p_rTree, l_pBranch );
			HandleErrorCondition(12086, hrOk);
			if ( hrOk == S_OK )
			{
				_variant_t svVariant;

				hrOk = AddItem( CTAG_ACQUISITION_DEVICE_FILE_NAME, l_pBranch, SVBStrType, 
					p_rTree, rArchive, &csValue );
				HandleErrorCondition(12087, hrOk);

				svVariant.SetString( csValue );

				hrOk = AddItem( CTAG_ACQUISITION_DEVICE_FILE_NAME, p_pParent, p_rTree, svVariant );
				HandleErrorCondition(12088, hrOk);
			}
		}
		else
		{
			hrOk = AddItem( CTAG_ACQUISITION_DEVICE_FILE_NAME, p_pParent, SVBStrType, 
				p_rTree, rArchive, &csValue );
			HandleErrorCondition(12089, hrOk);
		}

		rArchive >> ulReserved;
		rArchive >> ulReserved;
		rArchive >> ulReserved;
		rArchive >> ulReserved;
		rArchive >> ulReserved;
		rArchive >> ulReserved;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVImageInfoClass( unsigned long ulSVOVersion, 
																							unsigned long ulSECVersion, 
																							bool bIsColorMode,
																							CString &rcsBoardName, 
																							long lIndex, 
																							CArchive &rArchive, 
																							SVTreeType& p_rTree,
																							typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	long lBandLink = 0;
	long lBandNumber = 0;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_EXTENT_INFORMATION, p_pParent, l_pBranch );
	HandleErrorCondition(12090, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVExtentClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12091, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_ABSOLUTE_EXTENT_INFORMATION, p_pParent, l_pBranch );
	HandleErrorCondition(12092, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVExtentClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12093, hrOk);
		}
	}

	hrOk = AddItem( CTAG_TYPE, p_pParent, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12094, hrOk);

	hrOk = AddItem( CTAG_SAVE_LOAD_INDEX, p_pParent, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12095, hrOk);

	hrOk = AddItem( CTAG_BAND_NUMBER, p_pParent, SVLongType, 
		p_rTree, rArchive, &lBandNumber );
	HandleErrorCondition(12096, hrOk);

	hrOk = AddItem( CTAG_BAND_LINK, p_pParent, SVLongType, 
		p_rTree, rArchive, &lBandLink );
	HandleErrorCondition(12097, hrOk);

	if ( -1 < lBandLink )
	{
		hrOk = GetCameraBranch( lIndex+1, p_rTree, l_pBranch );
		HandleErrorCondition(12098, hrOk);
		if ( hrOk == S_OK )
		{
			SVTreeType::SVLeafHandle htiNext;

			if ( GetItem( p_rTree, CTAG_ACQUISITION_DEVICE, l_pBranch, htiNext ) != S_OK )
			{
				CString csValue;

				_variant_t svVariant;

				if ( bIsColorMode )
				{
					csValue.Format( "%s.Dig_%d.Ch_All", rcsBoardName, lIndex );
				}
				else
				{
					csValue.Format( "%s.Dig_%d.Ch_%d", rcsBoardName, lIndex, lBandLink );
				}

				svVariant.SetString( csValue );

				hrOk = AddItem( CTAG_ACQUISITION_DEVICE, l_pBranch, p_rTree, svVariant );
				HandleErrorCondition(12099, hrOk);
			}
		}
	}

	hrOk = AddItem( CTAG_PIXEL_DEPTH, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12100, hrOk);

	hrOk = AddItem( CTAG_ROTATION_ANGLE, p_pParent, SVDoubleType, 
		p_rTree, rArchive );
	HandleErrorCondition(12101, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVLightReferenceArrayClass( unsigned long ulSVOVersion, 
																												unsigned long ulSECVersion, 
																												bool bIsColorMode,
																												CArchive &rArchive, 
																												SVTreeType& p_rTree,
																												typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	int iSize;

	if (ulSECVersion >= 0x00000002)
	{
		hrOk = AddItem( CTAG_SIZE, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12102, hrOk);

		for ( int i = 0; i < iSize; i++ )
		{
			CString csName;

			SVTreeType::SVBranchHandle l_pBranch( NULL );

			csName.Format( CTAGF_BAND_X, i );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
			HandleErrorCondition(12103, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVLightReferenceClass( ulSVOVersion, 
						ulSECVersion, 
						bIsColorMode,
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12104, hrOk);
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVLightReferenceClass( unsigned long ulSVOVersion, 
																									 unsigned long ulSECVersion, 
																									 bool bIsColorMode,
																									 CArchive &rArchive, 
																									 SVTreeType& p_rTree,
																									 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	int iSize;
	unsigned long ulType;
	unsigned long ulValue;
	_variant_t svVariant;

	if (ulSECVersion >= 0x00000002)
	{
		hrOk = AddItem( CTAG_SIZE, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12105, hrOk);

		for ( int i = 0; i < iSize; i++ )
		{
			CString csName, csValue;

			SVTreeType::SVBranchHandle l_pBranch( NULL );

			csName.Format( CTAGF_LIGHTREFERENCE_X, i );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
			HandleErrorCondition(12106, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = AddItem( CTAG_NAME, l_pBranch, SVBStrType, 
						p_rTree, rArchive, &csValue );
					HandleErrorCondition(12107, hrOk);

					hrOk = AddItem( CTAG_RESOURCE_ID, l_pBranch, SVIntType, 
						p_rTree, rArchive );
					HandleErrorCondition(12108, hrOk);

					rArchive >> ulType;
					rArchive >> ulValue;

					_variant_t l_UseAndClear = ulType;
					hrOk = AddItem( CTAG_TYPE, l_pBranch, p_rTree, l_UseAndClear );
					l_UseAndClear.Clear();
					HandleErrorCondition(12109, hrOk);

					if( ulValue == (unsigned long) 100 )	// used as a default value for contrast
					{
						if( ulType  == 0x08070004 ) //CORACQ_PRM_CONTRAST
						{
							ulValue = -999999999L; // default - read from cam file
						}
						else if (  ((ulType == 0x08080004)    // contrast RGB band 0
							|| (ulType == 0x08090004)    // contrast RGB band 1
							|| (ulType == 0x080A0004)) ) // contrast RGB band 2
						{
							if (bIsColorMode)
							{
								ulValue = -999999999L; // default - read from cam file
							}
							else
							{
								ulValue = -999999999L; // default - read from cam file
							}
						}
					}

					l_UseAndClear = ulValue;
					hrOk = AddItem( CTAG_VALUE, l_pBranch, p_rTree, l_UseAndClear );
					l_UseAndClear.Clear();
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVExtentClass( unsigned long ulSVOVersion, 
																					 unsigned long ulSECVersion, 
																					 CArchive &rArchive, 
																					 SVTreeType& p_rTree,
																					 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	hrOk = AddItem( CTAG_OBJECT_LEFT_POSITION, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12111, hrOk);

	hrOk = AddItem( CTAG_OBJECT_TOP_POSITION, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12112, hrOk);

	hrOk = AddItem( CTAG_OBJECT_WIDTH, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12113, hrOk);

	hrOk = AddItem( CTAG_OBJECT_HEIGHT, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12114, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPPQIndexHandleOwnerStruct( unsigned long ulSVOVersion, 
																												 unsigned long ulSECVersion, 
																												 typename SVTreeType::SVBranchHandle p_pPPQCameraBranch,
																												 CArchive &rArchive, 
																												 SVTreeType& p_rTree,
																												 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	if ( p_pPPQCameraBranch != NULL )
	{
		hrOk = AddItem( CTAG_POSITION, p_pPPQCameraBranch, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12115, hrOk);
	}
	else
	{
		hrOk = AddItem( CTAG_POSITION, p_pParent, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12116, hrOk);
	}

	hrOk = AddItem( CTAG_INDEX_HANDLE, p_pParent, SVUnsignedIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12117, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDDEEntryStruct( unsigned long ulSVOVersion, 
																							unsigned long ulSECVersion,
																							CArchive &rArchive, 
																							SVTreeType& p_rTree,
																							typename SVTreeType::SVBranchHandle p_pParent,
																							long& rlCount,
																							LPCTSTR tszNameFormat )
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	if (ulSECVersion >= 0x00000002)
	{
		_variant_t l_ValueUseAndClear;

		CString csName;

		SVTreeType::SVBranchHandle l_pBranch( NULL );

		int iPPQPosition = 0;
		int iDataSourceType = 0;

		rArchive >> iPPQPosition;
		rArchive >> iDataSourceType;
		rArchive >> csName;

		//??XML // PPQ DDE inputs / outputs
		// EB 2002 09 06    Changed the inputs & outputs to use generated names with the actual name as a child item
		//hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
		rlCount++;
		CString sTemp;
		sTemp.Format(tszNameFormat, rlCount);
		hrOk = AddBranch( p_rTree, sTemp, p_pParent, l_pBranch );
		HandleErrorCondition(12118, hrOk);

		l_ValueUseAndClear.SetString( _T("DDE") );

		hrOk = AddItem( CTAG_IO_TYPE, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12331, hrOk);			

		l_ValueUseAndClear.SetString( csName );

		hrOk = AddItem( CTAG_ITEM_NAME, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12327, hrOk);

		l_ValueUseAndClear = iPPQPosition;

		hrOk = AddItem( CTAG_PPQ_POSITION, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12120, hrOk);

		l_ValueUseAndClear = iDataSourceType;

		hrOk = AddItem( CTAG_DATA_SOURCE_TYPE, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12122, hrOk);

		hrOk = AddItem( CTAG_INITIAL_SIZE, l_pBranch, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12123, hrOk);

		hrOk = AddItem( CTAG_TYPE, l_pBranch, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12124, hrOk);

		// Version 3.12 Release
		if (ulSECVersion >= 0x00030cff)
		{
			hrOk = AddItem( CTAG_DDE_ATTRIBUTES, l_pBranch, SVUnsignedLongType, 
				p_rTree, rArchive );
			HandleErrorCondition(12125, hrOk);
		}

		hrOk = ArchiveSVPQDataLinkInfoStruct( ulSVOVersion, 
			ulSECVersion, 
			rArchive, 
			p_rTree,
			l_pBranch);
		HandleErrorCondition(12126, hrOk);
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDDEInputEntryListClass( unsigned long ulSVOVersion, 
																											unsigned long ulSECVersion,
																											long lIndex, 
																											CArchive &rArchive, 
																											SVTreeType& p_rTree,
																											typename SVTreeType::SVBranchHandle p_pParent,
																											long& rlCount)
{
	HRESULT hrOk = S_FALSE;

	int iSize;

	if (ulSECVersion >= 0x00000002)
	{
		SVTreeType::SVBranchHandle l_pPPQ( NULL );
		SVTreeType::SVBranchHandle l_pInput( NULL );

		hrOk = GetPPQBranch( lIndex+1, p_rTree, l_pPPQ );
		HandleErrorCondition(12127, hrOk);

		if ( GetBranch( p_rTree, CTAG_INPUT, l_pPPQ, l_pInput ) != S_OK )
		{
			hrOk = AddBranch( p_rTree, CTAG_INPUT, l_pPPQ, l_pInput );
			HandleErrorCondition(12128, hrOk);
		}

		hrOk = AddItem( CTAG_SIZE_OF_DDE_INPUT_ENTRY_LIST, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12129, hrOk);

		for ( int i = 0; i < iSize; i++ )
		{
			hrOk = ArchiveSVDDEEntryStruct( ulSVOVersion, 
				ulSECVersion,
				rArchive, 
				p_rTree,
				l_pInput,
				rlCount,
				CTAGF_INPUT_X);
			HandleErrorCondition(12130, hrOk);
		}// end for ( int i = 0; i < iSize; i++ )
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQDataLinkInfoStruct( unsigned long ulSVOVersion, 
																										unsigned long ulSECVersion,
																										CArchive &rArchive, 
																										SVTreeType& p_rTree,
																										typename SVTreeType::SVBranchHandle p_pParent,
																										typename SVTreeType::SVBranchHandle& p_rpBranch,
																										long* plCount,
																										LPCTSTR tszNameFormat)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	CString csName;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	(*plCount)++;
	rArchive >> csName;
	//??XML // PPQ inputs (DDE & digital)
	// EB 2002 09 06    Changed the inputs & outputs to use generated names with the actual name as a child item
	CString sTemp;
	sTemp.Format(tszNameFormat, *plCount);
	hrOk = AddBranch( p_rTree, (LPCTSTR)sTemp, p_pParent, p_rpBranch );
	//hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, p_rBranchIter );
	HandleErrorCondition(12131, hrOk);

	l_pBranch = p_rpBranch;

	if (hrOk == S_OK)
	{
		_variant_t l_ValueUseAndClear;

		l_ValueUseAndClear.SetString( _T("Digital") );
		hrOk = AddItem( CTAG_IO_TYPE, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12333, hrOk);			

		l_ValueUseAndClear.SetString( csName );
		hrOk = AddItem( CTAG_NAME, l_pBranch, p_rTree, l_ValueUseAndClear );
		l_ValueUseAndClear.Clear();
		HandleErrorCondition(12329, hrOk);
	}

	hrOk = ArchiveAddSVPQDataLinkInfoStruct( ulSVOVersion, ulSECVersion, rArchive, p_rTree, p_pParent, l_pBranch );

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQDataLinkInfoStruct( unsigned long ulSVOVersion, 
																										unsigned long ulSECVersion,
																										CArchive &rArchive, 
																										SVTreeType& p_rTree,
																										typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	CString csName;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	l_pBranch = p_pParent;
	rArchive >> csName;

	CString csTemp;

	csTemp = _T( "VIPER DUAL" );
	if( 0 == csTemp.CompareNoCase( csName.Left( 10 ) ) )
		csName = _T( "VIPER DUAL" ) + csName.Mid( 10 );

	csTemp = _T( "VIPER QUAD" );
	if( 0 == csTemp.CompareNoCase( csName.Left( 10 ) ) )
		csName = _T( "VIPER QUAD" ) + csName.Mid( 10 );

	csTemp = _T( "VIPER RGB" );
	if( 0 == csTemp.CompareNoCase( csName.Left( 9 ) ) )
		csName = _T( "VIPER RGB" ) + csName.Mid( 9 );

	if ( csName.Find("VIPER DUAL.Dig_0.", 0 ) != -1 )
	{
		csName.Replace("VIPER DUAL.Dig_0.", "" );
	}
	else if ( csName.Find("VIPER DUAL.Dig_1.", 0 ) != -1 )
	{
		csName.Replace("VIPER DUAL.Dig_1.", "" );
	}
	else if ( csName.Find("VIPER QUAD.Dig_0.", 0 ) != -1 )
	{
		csName.Replace("VIPER QUAD.Dig_0.", "" );
	}
	else if ( csName.Find("VIPER QUAD.Dig_1.", 0 ) != -1 )
	{
		csName.Replace("VIPER QUAD.Dig_1.", "" );
	}
	else if ( csName.Find("VIPER QUAD.Dig_2.", 0 ) != -1 )
	{
		csName.Replace("VIPER QUAD.Dig_2.", "" );
	}
	else if ( csName.Find("VIPER QUAD.Dig_3.", 0 ) != -1 )
	{
		csName.Replace("VIPER QUAD.Dig_3.", "" );
	}
	else if ( csName.Find("VIPER RGB.Dig_0.", 0 ) != -1 )
	{
		csName.Replace("VIPER RGB.Dig_0.", "" );
	}

	csName.Replace(". ", "" );

	_variant_t svVariant;

	svVariant.SetString( (LPCTSTR)csName );

	hrOk = AddItem( CTAG_NAME, l_pBranch, p_rTree, svVariant );
	HandleErrorCondition(12132, hrOk);

	hrOk = ArchiveAddSVPQDataLinkInfoStruct( ulSVOVersion, ulSECVersion, rArchive, p_rTree, p_pParent, l_pBranch );

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveAddSVPQDataLinkInfoStruct( unsigned long ulSVOVersion, 
																										    unsigned long ulSECVersion,
																										    CArchive &rArchive, 
																										    SVTreeType& p_rTree,
																										    typename SVTreeType::SVBranchHandle p_pParent,
																												typename SVTreeType::SVBranchHandle p_pBranch )
{
	HRESULT hrOk = S_FALSE;

	unsigned long ulReserved;

	SVTreeType::SVBranchHandle l_pData( NULL );

	hrOk = AddItem( CTAG_ATTRIBUTE, p_pBranch, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12133, hrOk);

	hrOk = AddItem( CTAG_DATA_TYPE, p_pBranch, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12134, hrOk);

	hrOk = AddItem( CTAG_MAXIMUM_DATA_SIZE, p_pBranch, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12135, hrOk);

	hrOk = AddItem( CTAG_ENABLE_DIGITAL_OUTPUT, p_pBranch, SVBoolType, 
		p_rTree, rArchive );
	HandleErrorCondition(12136, hrOk);

	hrOk = AddItem( CTAG_USE_FOR_MASTER_RESULT, p_pBranch, SVBoolType, 
		p_rTree, rArchive );
	HandleErrorCondition(12137, hrOk);

	hrOk = AddItem( CTAG_WAIT_FOR_DDE_INPUT_COMPLETION, p_pBranch, SVBoolType, 
		p_rTree, rArchive );
	HandleErrorCondition(12138, hrOk);

	rArchive >> ulReserved;
	rArchive >> ulReserved;
	rArchive >> ulReserved;
	rArchive >> ulReserved;
	rArchive >> ulReserved;
	rArchive >> ulReserved;
	rArchive >> ulReserved;

	hrOk = AddItem( CTAG_PRODUCER_UNIQUE_REFERENCE_ID, p_pBranch, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12139, hrOk);

	hrOk = AddItem( CTAG_DATA_LINK_UNIQUE_REFERENCE_ID, p_pBranch, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12140, hrOk);

	hrOk = AddBranch( p_rTree, CTAG_DEFAULT_DATA, p_pBranch, l_pData );
	HandleErrorCondition(12141, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pData != NULL )
		{
			hrOk = ArchiveSVByteArrayClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pData );
			HandleErrorCondition(12142, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_CURRENT_DATA, p_pBranch, l_pData );
	HandleErrorCondition(12143, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pData != NULL )
		{
			hrOk = ArchiveSVByteArrayClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pData );
			HandleErrorCondition(12144, hrOk);
		}
	}
		
	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVByteArrayClass( unsigned long ulSVOVersion, 
																							unsigned long ulSECVersion, 
																							CArchive &rArchive, 
																							SVTreeType& p_rTree,
																							typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	unsigned long dwCount = 0;
	unsigned short wCount = 0;

	rArchive >> wCount;
	if (wCount != 0xFFFF)
	{
		dwCount = wCount;
	}
	else
	{
		rArchive >> dwCount;
	}

	hrOk = AddArray( CTAG_ARRAY_DATA, p_pParent, SVUnsignedCharArrayType, 
		dwCount, p_rTree, rArchive );
	HandleErrorCondition(12145, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQDigitialInputInfoListClass( unsigned long ulSVOVersion, 
																														unsigned long ulSECVersion,
																														long lIndex, 
																														CArchive &rArchive, 
																														SVTreeType& p_rTree,
																														typename SVTreeType::SVBranchHandle p_pParent,
																														long& rlCount)
{
	HRESULT hrOk = S_FALSE;

	int iSize;

	hrOk = AddItem( CTAG_SIZE_OF_PQ_DIGITAL_INPUT_ENTRY_LIST, 
		p_pParent, SVIntType, p_rTree, rArchive, &iSize );
	HandleErrorCondition(12146, hrOk);

	for ( int i = 0; i < iSize; i++ )
	{
		CString csName;

		SVTreeType::SVBranchHandle l_pBranch( NULL );

		csName.Format( CTAGF_PQ_DIGITAL_INPUT_ENTRY_X, i );

		hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
		HandleErrorCondition(12147, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVPQDigitialInputInfoStruct( ulSVOVersion, 
					ulSECVersion,
					lIndex, 
					rArchive, 
					p_rTree,
					l_pBranch,
					rlCount);
				HandleErrorCondition(12148, hrOk);
			}
		}
	}// end for ( int i = 0; i < iSize; i++ )

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQDigitialInputInfoStruct( unsigned long ulSVOVersion, 
																												 unsigned long ulSECVersion, 
																												 long lIndex,
																												 CArchive &rArchive, 
																												 SVTreeType& p_rTree,
																												 typename SVTreeType::SVBranchHandle p_pParent,
																												 long& rlCount)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pPPQ( NULL );
	SVTreeType::SVBranchHandle l_pInput( NULL );
	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = GetPPQBranch( lIndex+1, p_rTree, l_pPPQ );
	HandleErrorCondition(12149, hrOk);

	if ( GetBranch( p_rTree, CTAG_INPUT, l_pPPQ, l_pInput ) != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_INPUT, l_pPPQ, l_pInput );
		HandleErrorCondition(12150, hrOk);
	}

	hrOk = ArchiveSVPQDataLinkInfoStruct( ulSVOVersion, 
		ulSECVersion,
		rArchive, 
		p_rTree,
		l_pInput,
		l_pBranch,
		&rlCount,
		"DIO.Input%d");
//		CTAGF_INPUT_X);
	HandleErrorCondition(12151, hrOk);

	hrOk = AddItem( CTAG_PPQ_POSITION, l_pBranch, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12152, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVAcquisitionTriggerEventServerClass( unsigned long ulSVOVersion, 
																																	unsigned long ulSECVersion, 
																																	CArchive &rArchive, 
																																	SVTreeType& p_rTree,
																																	typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_DEFINITION_OF_EVENT_SERVER, p_pParent, l_pBranch );
	HandleErrorCondition(12153, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVEventServerClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12154, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_TOGGLE_FLAG, p_pParent, l_pBranch );
	HandleErrorCondition(12155, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12156, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVEventServerClass( unsigned long ulSVOVersion, 
																								unsigned long ulSECVersion, 
																								CArchive &rArchive, 
																								SVTreeType& p_rTree,
																								typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12157, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDigitalOutputObjectClass( unsigned long ulSVOVersion, 
																												unsigned long ulSECVersion, 
																												CArchive &rArchive, 
																												SVTreeType& p_rTree,
																												typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12158, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQObjectClass( unsigned long ulSVOVersion, 
																						 unsigned long ulSECVersion,
																						 long lIndex, 
																						 CArchive &rArchive, 
																						 SVTreeType& p_rTree,
																						 typename SVTreeType::SVBranchHandle p_pParent,
																						 long& rlCount)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12159, hrOk);

	hrOk = AddBranch( p_rTree, CTAG_PQ_CHECKER_INFORMATION, p_pParent, l_pBranch );
	HandleErrorCondition(12160, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVPQCheckerObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12161, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_PPQ_DEFINITION_INFORMATION, p_pParent, l_pBranch );
	HandleErrorCondition(12162, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVPPQInfoStruct( ulSVOVersion, 
				ulSECVersion,
				lIndex, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12163, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_DDE_CONTROL_DEFINITION_INFORMATION, p_pParent, l_pBranch );
	HandleErrorCondition(12164, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDDEControlClass( ulSVOVersion, 
				ulSECVersion,
				lIndex, 
				rArchive, 
				p_rTree,
				l_pBranch);
			HandleErrorCondition(12165, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPQCheckerObjectClass( unsigned long ulSVOVersion, 
																										unsigned long ulSECVersion, 
																										CArchive &rArchive, 
																										SVTreeType& p_rTree,
																										typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12166, hrOk);

	hrOk = AddBranch( p_rTree, CTAG_DATA_VALID_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12167, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12168, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_NOT_INSPECTED_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12169, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12170, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_NAK_MISTRIGGER_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12171, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12172, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_ACK_COMPLEMENT_OF_NAK_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12173, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12174, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_MF_MASTER_FAULT_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12175, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12176, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_MW_MASTER_WARNING_OUTPUT_OBJECT, p_pParent, l_pBranch );
	HandleErrorCondition(12177, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulSECVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12178, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVPPQInfoStruct( unsigned long ulSVOVersion, 
																						 unsigned long ulSECVersion, 
																						 long lIndex,
																						 CArchive &rArchive, 
																						 SVTreeType& p_rTree,
																						 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pPPQ( NULL );

	hrOk = GetPPQBranch( lIndex+1, p_rTree, l_pPPQ );
	HandleErrorCondition(12179, hrOk);

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, l_pPPQ, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12180, hrOk);

	hrOk = AddItem( CTAG_PPQ_LENGTH, l_pPPQ, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12181, hrOk);

	hrOk = AddItem( CTAG_PPQ_OUTPUT_RESET_DELAY_TIME, l_pPPQ, SVUnsignedLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12182, hrOk);

	hrOk = AddItem( CTAG_PPQ_MODE, l_pPPQ, SVIntType, 
		p_rTree, rArchive );
	HandleErrorCondition(12183, hrOk);

	hrOk = AddItem( CTAG_PPQ_OUTPUT_DELAY_TIME, l_pPPQ, SVUnsignedLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12184, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDDEControlClass( unsigned long ulSVOVersion, 
																							 unsigned long ulSECVersion, 
																							 long lIndex,
																							 CArchive &rArchive, 
																							 SVTreeType& p_rTree,
																							 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_DDE_OUTPUT_ENTRY_LIST, p_pParent, l_pBranch );
	HandleErrorCondition(12185, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDDEOutputEntryListClass( ulSVOVersion, 
				ulSECVersion,
				lIndex, 
				rArchive, 
				p_rTree,
				l_pBranch);
			HandleErrorCondition(12186, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDDEOutputEntryListClass( unsigned long ulSVOVersion, 
																											 unsigned long ulSECVersion, 
																											 long lIndex,
																											 CArchive &rArchive, 
																											 SVTreeType& p_rTree,
																											 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	int iSize;

	SVTreeType::SVBranchHandle l_pPPQ( NULL );
	SVTreeType::SVBranchHandle l_pOutput( NULL );

	hrOk = GetPPQBranch( lIndex+1, p_rTree, l_pPPQ );
	HandleErrorCondition(12187, hrOk);

	if ( GetBranch( p_rTree, CTAG_OUTPUT, l_pPPQ, l_pOutput ) != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_OUTPUT, l_pPPQ, l_pOutput );
		HandleErrorCondition(12188, hrOk);
	}

	hrOk = AddItem( CTAG_SIZE_OF_DDE_OUTPUT_ENTRY_LIST, p_pParent, SVIntType, 
		p_rTree, rArchive, &iSize );
	HandleErrorCondition(12189, hrOk);

	long lCount = 0;    // output count
	for ( int i = 0; i < iSize; i++ )
	{
		hrOk = ArchiveSVDDEEntryStruct( ulSVOVersion, 
			ulSECVersion, 
			rArchive, 
			p_rTree,
			l_pOutput,
			lCount,
			CTAGF_OUTPUT_X);
		HandleErrorCondition(12190, hrOk);
	}// end for ( int i = 0; i < iSize; i++ )

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIPDocInfoClass( unsigned long ulSVOVersion, 
																							unsigned long ulSECVersion,
																							CString &rcsBoardName,
																							CString &rcsInspection, 
																							CArchive &rArchive, 
																							SVTreeType& p_rTree,
																							typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );
	SVTreeType::SVBranchHandle l_pEnviro( NULL );

	hrOk = AddItem( CTAG_NUMBER_OF_VIEWS, p_pParent, SVLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12191, hrOk);

	hrOk = AddItem( CTAG_INSPECTION_FILE_NAME, p_pParent, SVBStrType, 
		p_rTree, rArchive);
	HandleErrorCondition(12192, hrOk);

	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12193, hrOk);

	hrOk = GetEnvironmentBranch( p_rTree, l_pEnviro );
	HandleErrorCondition(12194, hrOk);

	hrOk = AddBranch( p_rTree, CTAG_IPDOC_REQUIREMENT_INFO, l_pEnviro, l_pBranch );
	HandleErrorCondition(12195, hrOk);

	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVIPDocRequirementInfoClass( ulSVOVersion, 
				ulSECVersion,
				p_pParent,
				rcsBoardName,
				rcsInspection,
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12196, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIPDocRequirementInfoClass( unsigned long ulSVOVersion, 
																												 unsigned long ulSECVersion,
																												 typename SVTreeType::SVBranchHandle p_pInspection,
																												 CString &rcsBoardName,
																												 CString &rcsInspection, 
																												 CArchive &rArchive, 
																												 SVTreeType& p_rTree,
																												 typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csName, csValue;
	HRESULT hrOk = S_FALSE;

	int iSize;

	hrOk = AddItem( CTAG_DEFAULT_SEC_PATH, p_pParent, SVBStrType, 
		p_rTree, rArchive, &csValue );
	HandleErrorCondition(12197, hrOk);

	hrOk = AddItem( CTAG_DEFAULT_SEC_TITLE, p_pParent, SVBStrType, 
		p_rTree, rArchive, &csValue );
	HandleErrorCondition(12198, hrOk);

	hrOk = AddItem( CTAG_SIZE_OF_CAMERA_INFO_ARRAY, p_pParent, SVIntType, 
		p_rTree, rArchive, &iSize );
	HandleErrorCondition(12199, hrOk);

	for ( int i = 0; i < iSize; i++ )
	{
		SVTreeType::SVBranchHandle l_pBranch( NULL );

		csName.Format( CTAGF_CAMERA_INFO_X, i );

		hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
		HandleErrorCondition(12200, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				hrOk = ArchiveSVCameraInfoStruct( ulSVOVersion, 
					ulSECVersion,
					p_pInspection,
					rcsBoardName,
					rcsInspection, 
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12201, hrOk);
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVCameraInfoStruct( unsigned long ulSVOVersion, 
																								unsigned long ulSECVersion,
																								typename SVTreeType::SVBranchHandle p_pInspection,
																								CString &rcsBoardName,
																								CString &rcsInspection, 
																								CArchive &rArchive, 
																								SVTreeType& p_rTree,
																								typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	int iIndex = 0;
	unsigned long ulBandNumber = 1;
	unsigned long ulBandLink = 0;

	SVTreeType::SVBranchHandle l_pPPQ( NULL );
	SVTreeType::SVBranchHandle l_pInspect( NULL );
	SVTreeType::SVBranchHandle l_pData( NULL );
	SVTreeType::SVBranchHandle l_pVInputs( NULL );

	hrOk = AddItem( CTAG_DIGITIZER_NUMBER, p_pParent, SVIntType, 
		p_rTree, rArchive, &iIndex );
	HandleErrorCondition(12202, hrOk);

	hrOk = AddItem( CTAG_BAND_NUMBER, p_pParent, SVUnsignedLongType, 
		p_rTree, rArchive, &ulBandNumber );
	HandleErrorCondition(12203, hrOk);

	hrOk = AddItem( CTAG_BAND_LINK, p_pParent, SVUnsignedLongType, 
		p_rTree, rArchive, &ulBandLink );
	HandleErrorCondition(12204, hrOk);

	hrOk = GetPPQBranch( iIndex+1, p_rTree, l_pPPQ );
	HandleErrorCondition(12205, hrOk);

	if ( GetBranch( p_rTree, CTAG_INSPECTION, l_pPPQ, l_pInspect ) != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_INSPECTION, l_pPPQ, l_pInspect );
		HandleErrorCondition(12206, hrOk);
	}

	if ( GetBranch( p_rTree, (LPCTSTR)rcsInspection, l_pInspect, l_pData ) != S_OK )
	{
		hrOk = AddBranch( p_rTree, (LPCTSTR)rcsInspection, l_pInspect, l_pData );
		HandleErrorCondition(12207, hrOk);
	}

	if ( p_pInspection != NULL )
	{
		CString csValue;

		if ( iIndex == 0 )
		{
			csValue.Format( CTAGF_CAMERA_X, ulBandLink+1 );
		}
		else
		{
			csValue.Format( CTAGF_CAMERA_X, iIndex+1 );
		}

		_variant_t svVariant;

		svVariant.SetString( csValue );

		hrOk = AddItem( CTAG_INSPECTION_TOOLSET_IMAGE, p_pInspection, p_rTree, svVariant );
		HandleErrorCondition(12099, hrOk);
	}// end if
	else
	{
		// Code to go find all the PPQ inputs that were selected for viewing
		SVTreeType::SVBranchHandle l_pInputs( NULL );
		SVTreeType::SVBranchHandle l_pIO( NULL );
		SVTreeType::SVBranchHandle l_pItem( NULL );
		SVTreeType::SVBranchHandle l_pNew( NULL );
		_variant_t vtName;
		_variant_t vtSize;
		_variant_t vtObj;
		_variant_t vtAttrib;
		CString strName;
		CString strObj;
		CString strItem;
		CString strTemp;
		long lSize;
		long l;
		DWORD dwAttrib;

		l = 1;
		hrOk = GetInspectionBranch( l++, p_rTree, l_pInspect );
		HandleErrorCondition(12099, hrOk);
		while( hrOk == S_OK )
		{
			_bstr_t l_Name;

			p_rTree.GetBranchName( l_pInspect, l_Name.GetBSTR() );

			if( rcsInspection == static_cast< LPCTSTR >( l_Name ) )
				break;

			hrOk = GetInspectionBranch( l++, p_rTree, l_pInspect );
			HandleErrorCondition(12099, hrOk);
		}// end while

		hrOk = AddBranch( p_rTree, CTAG_VIEWED_INPUTS, l_pInspect, l_pVInputs );
		HandleErrorCondition(12099, hrOk);

		hrOk = GetBranch( p_rTree, CTAG_INPUT, l_pPPQ, l_pInputs );
		HandleErrorCondition(12099, hrOk);

		if( p_rTree.IsValidBranch(l_pInputs) == S_OK )
		{
			SVTreeType::SVBranchHandle l_ChildIter = NULL;
				
			p_rTree.GetFirstBranch( l_pInputs, l_ChildIter );

			while( p_rTree.IsValidBranch( l_ChildIter ) == S_OK )
			{
				SVTreeType::SVLeafHandle l_NameIter;
				SVTreeType::SVLeafHandle l_SizeIter;

				// Get the next input item from the PPQ
				hrOk = GetItem( p_rTree, CTAG_ITEM_NAME, l_ChildIter, l_NameIter );
				HandleErrorCondition(12099, hrOk);
				if( hrOk == S_OK )
				{
					p_rTree.GetLeafData( l_NameIter, vtName.GetVARIANT() );
					strName = vtName;
				}
				// Get the IO list and its size
				hrOk = GetIOBranch( p_rTree, l_pIO );
				HandleErrorCondition(12099, hrOk);

				hrOk = GetItem( p_rTree, CTAG_NUMBER_OF_INPUT_ENTRIES, l_pIO, l_SizeIter );
				HandleErrorCondition(12099, hrOk);

				p_rTree.GetLeafData( l_SizeIter, vtSize.GetVARIANT() );
				lSize = vtSize;

				for( long l = 0; l < lSize; l++ )
				{
					// Get the next input item from the IO
					strItem.Format( CTAGF_INPUT_ENTRY_X, l );

					hrOk = GetBranch( p_rTree, strItem, l_pIO, l_pItem );
					HandleErrorCondition(12099, hrOk);

					SVTreeType::SVLeafHandle htiObjName;

					hrOk = GetItem( p_rTree, CTAG_OBJECT_NAME, l_pItem, htiObjName );
					HandleErrorCondition(12099, hrOk);

					p_rTree.GetLeafData( htiObjName, vtObj.GetVARIANT() );
					strObj = vtObj;
					strTemp.Format( "DIO.%s", strObj );
					if( strTemp == strName )
					{
						SVTreeType::SVLeafHandle htiAttrib;
						hrOk = GetItem( p_rTree, CTAG_OBJECT_ATTRIBUTE_SET, l_pItem, htiAttrib );
						HandleErrorCondition(12099, hrOk);

						p_rTree.GetLeafData( htiAttrib, vtAttrib.GetVARIANT() );
						dwAttrib = vtAttrib;

	#define SV_VIEWABLE 0x00000001
						if( SV_VIEWABLE & dwAttrib )
						{
							hrOk = AddBranch( p_rTree, strName, l_pVInputs, l_pNew );							
							HandleErrorCondition(12099, hrOk);
						}// end if

						break;
					}// end if

				}// end for

				p_rTree.GetNextBranch( l_pInputs, l_ChildIter );

			}// end while
		}

	}// end else
	if ( ! rcsBoardName.IsEmpty() )
	{
		SVTreeType::SVBranchHandle l_pCamera( NULL );
		SVTreeType::SVLeafHandle htiAcqDev;

		if ( iIndex == 0 )
		{
			hrOk = GetCameraBranch( ulBandLink+1, p_rTree, l_pCamera );
		}
		else
		{
			hrOk = GetCameraBranch( iIndex+1, p_rTree, l_pCamera );
		}

		hrOk = GetItem( p_rTree, CTAG_ACQUISITION_DEVICE, l_pCamera, htiAcqDev );
		if ( hrOk != S_OK )
		{
			CString csValue;

			csValue.Format( "%s.Dig_%d.Ch_%d", rcsBoardName, iIndex, ulBandLink );

			_variant_t svVariant;

			svVariant.SetString( csValue );

			hrOk = AddItem( CTAG_ACQUISITION_DEVICE, l_pCamera, p_rTree, svVariant );
			HandleErrorCondition(12099, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveIOD(unsigned long ulSVOVersion, 
																 unsigned long &ulIODVersion, 
																 CArchive &rArchive, 
																 SVTreeType& p_rTree)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = GetIOBranch( p_rTree, l_pBranch );
	HandleErrorCondition(12208, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVIODoc( ulSVOVersion, 
				ulIODVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12209, hrOk);

			if ( hrOk != S_OK )
			{
				p_rTree.Clear();
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIODoc( unsigned long ulSVOVersion, 
																		 unsigned long &ulIODVersion, 
																		 CArchive &rArchive, 
																		 SVTreeType& p_rTree,
																		 typename SVTreeType::SVBranchHandle p_pParent)
{
	CString csName, csValue;
	HRESULT hrOk = S_FALSE;

	int iSize;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddItem( CTAG_DESCRIPTION, p_pParent, SVBStrType, 
		p_rTree, rArchive, &csValue );
	HandleErrorCondition(12210, hrOk);

	rArchive >> csName;	// Version #
	hrOk = AddItem( CTAG_VERSION_NUMBER, p_pParent, SVUnsignedLongType, 
		p_rTree, rArchive, &ulIODVersion );
	HandleErrorCondition(12211, hrOk);

	if (ulIODVersion >= 0x00000002)
	{
		int i( 0 );

		// Read History Block
		hrOk = AddItem( CTAG_HISTORY_LEVEL, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12212, hrOk);

		// Creation Date 
		rArchive >> csName;	// Creation Date #
		hrOk = AddItem( CTAG_CREATION_DATE, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csValue );
		HandleErrorCondition(12213, hrOk);

		// Creation Author / Update Author
		rArchive >> csName;	// Author #
		hrOk = AddItem( CTAG_AUTHOR, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csValue );
		HandleErrorCondition(12214, hrOk);


		GUID l_tmpGUID; // Read the obsolete GUID for the IODoc
				// Read it so the CArchive pointer moves ahead but do not use.
		hrOk = SerializeGUID( rArchive, l_tmpGUID ); // Read Unique Reference Id
		HandleErrorCondition(12215, hrOk);

		if (ulIODVersion >= 0x00022a01)	// 2.42
		{
			hrOk = AddBranch( p_rTree, CTAG_EXTERNAL_TRIGGER_SERVER, p_pParent, l_pBranch );
			HandleErrorCondition(12216, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVTriggerEventServerClass( ulSVOVersion, 
						ulIODVersion, 
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12217, hrOk);
				}
			}

			hrOk = AddBranch( p_rTree, CTAG_TIMER_TRIGGER_SERVER, p_pParent, l_pBranch );
			HandleErrorCondition(12218, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVTimerTriggerEventServerClass( ulSVOVersion, 
						ulIODVersion, 
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12219, hrOk);
				}
			}
		}

		hrOk = AddItem( CTAG_IO_ENTRY_IDENTIFIER, p_pParent, SVBStrType, 
			p_rTree, rArchive, &csValue );
		HandleErrorCondition(12220, hrOk);

		hrOk = AddItem( CTAG_NUMBER_OF_IO_ENTRIES, p_pParent, SVIntType, 
			p_rTree, rArchive, &iSize );
		HandleErrorCondition(12221, hrOk);

		for ( i = 0; i < iSize; i++ )
		{
			CString csName;

			SVTreeType::SVBranchHandle l_pBranch( NULL );

			csName.Format( CTAGF_IO_ENTRY_X, i );

			hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pBranch );
			HandleErrorCondition(12222, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVIOEntryClass( ulSVOVersion, 
						ulIODVersion, 
						rArchive, 
						p_rTree,
						l_pBranch );
					HandleErrorCondition(12223, hrOk);
				}
			}
		}

		if (ulIODVersion >= 0x00022a01)
		{
			hrOk = AddItem( CTAG_NUMBER_OF_INPUT_ENTRIES, p_pParent, SVIntType, 
				p_rTree, rArchive, &iSize );
			HandleErrorCondition(12224, hrOk);

			for ( i = 0; i < iSize; i++ )
			{
				SVTreeType::SVBranchHandle l_pInput( NULL );

				csName.Format( CTAGF_INPUT_ENTRY_X, i );

				hrOk = AddBranch( p_rTree, (LPCTSTR)csName, p_pParent, l_pInput );
				HandleErrorCondition(12225, hrOk);
				if ( hrOk == S_OK )
				{
					if ( l_pInput != NULL )
					{
						hrOk = ArchiveSVDigitalInputObjectClass( ulSVOVersion, 
							ulIODVersion, 
							rArchive, 
							p_rTree,
							l_pInput );
						HandleErrorCondition(12226, hrOk);
					}
				}
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVTriggerEventServerClass( unsigned long ulSVOVersion, 
																											 unsigned long ulIODVersion, 
																											 CArchive &rArchive, 
																											 SVTreeType& p_rTree,
																											 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_DEFINITION_OF_EVENT_SERVER, p_pParent, l_pBranch );
	HandleErrorCondition(12227, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVEventServerClass( ulSVOVersion, 
				ulIODVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12228, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_TOGGLE_FLAG, p_pParent, l_pBranch );
	HandleErrorCondition(12229, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVDigitalOutputObjectClass( ulSVOVersion, 
				ulIODVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12230, hrOk);
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVTimerTriggerEventServerClass( unsigned long ulSVOVersion, 
																														unsigned long ulIODVersion, 
																														CArchive &rArchive, 
																														SVTreeType& p_rTree,
																														typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_TRIGGER_EVENT_HANDLING, p_pParent, l_pBranch );
	HandleErrorCondition(12231, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVTriggerEventServerClass( ulSVOVersion, 
				ulIODVersion, 
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12232, hrOk);
		}
	}

	hrOk = AddItem( CTAG_DELAY_TIME, p_pParent, SVUnsignedLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12233, hrOk);

	hrOk = AddItem( CTAG_PERIODIC_TIME, p_pParent, SVUnsignedLongType, 
		p_rTree, rArchive );
	HandleErrorCondition(12234, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIOEntryClass( unsigned long ulSVOVersion, 
																						unsigned long ulIODVersion, 
																						CArchive &rArchive, 
																						SVTreeType& p_rTree,
																						typename SVTreeType::SVBranchHandle p_pParent)
{
	CString	csValue;
	HRESULT hrOk = S_FALSE;

	if (ulIODVersion >= 0x00000002)
	{
		_variant_t svVariant;
		CString csImageName;
		CString csTemp;

		rArchive >> csImageName;

		csTemp = _T( "VIPER DUAL" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
			csImageName = _T( "VIPER DUAL" ) + csImageName.Mid( 10 );

		csTemp = _T( "VIPER QUAD" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
			csImageName = _T( "VIPER QUAD" ) + csImageName.Mid( 10 );

		csTemp = _T( "VIPER RGB" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 9 ) ) )
			csImageName = _T( "VIPER RGB" ) + csImageName.Mid( 9 );

		if ( csImageName.Find("VIPER DUAL.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_0.PQ.Out", "Dig_0.PPQ_1" );
			csImageName.Replace("Dig_0.AcqTrigger.Toggle Flag", "Dig_0.PPQ_1.Trigger Toggle" );
			csImageName.Replace("VIPER DUAL.Dig_0.", "" );
		}
		else if ( csImageName.Find("VIPER DUAL.Dig_1.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_1.PQ.Out", "Dig_1.PPQ_2" );
			csImageName.Replace("Dig_1.AcqTrigger.Toggle Flag", "Dig_1.PPQ_2.Trigger Toggle" );
			csImageName.Replace("VIPER DUAL.Dig_1.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_0.PQ.Out", "Dig_0.PPQ_1" );
			csImageName.Replace("Dig_0.AcqTrigger.Toggle Flag", "Dig_0.PPQ_1.Trigger Toggle" );
			csImageName.Replace("VIPER QUAD.Dig_0.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_1.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_1.PQ.Out", "Dig_1.PPQ_2" );
			csImageName.Replace("Dig_1.AcqTrigger.Toggle Flag", "Dig_1.PPQ_2.Trigger Toggle" );
			csImageName.Replace("VIPER QUAD.Dig_1.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_2.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_2.PQ.Out", "Dig_2.PPQ_3" );
			csImageName.Replace("Dig_2.AcqTrigger.Toggle Flag", "Dig_2.PPQ_3.Trigger Toggle" );
			csImageName.Replace("VIPER QUAD.Dig_2.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_3.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_3.PQ.Out", "Dig_3.PPQ_4" );
			csImageName.Replace("Dig_3.AcqTrigger.Toggle Flag", "Dig_3.PPQ_4.Trigger Toggle" );
			csImageName.Replace("VIPER QUAD.Dig_3.", "" );
		}
		else if ( csImageName.Find("VIPER RGB.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("Dig_0.PQ.Out", "Dig_0.PPQ_1" );
			csImageName.Replace("Dig_0.AcqTrigger.Toggle Flag", "Dig_0.PPQ_1.Trigger Toggle" );
			csImageName.Replace("VIPER RGB.Dig_0.", "" );
		}

		csImageName.Replace(". ", "" );

		svVariant.SetString( (LPCTSTR)csImageName );

		// [CString] - Image Name
		hrOk = AddItem( CTAG_IO_ENTRY_NAME, p_pParent, p_rTree, svVariant );
		HandleErrorCondition(12236, hrOk);

		hrOk = AddItem( CTAG_IS_OUTPUT, p_pParent, SVBoolType, 
			p_rTree, rArchive );
		HandleErrorCondition(12237, hrOk);

		hrOk = AddItem( CTAG_CHANNEL, p_pParent, SVUnsignedLongType, 
			p_rTree, rArchive );
		HandleErrorCondition(12238, hrOk);

		if (ulIODVersion >= 0x00020302)
		{
			hrOk = AddItem( CTAG_IS_MODULE_IO_LIST_ENTRY, p_pParent, SVBoolType, 
				p_rTree, rArchive );
			HandleErrorCondition(12239, hrOk);

			hrOk = AddItem( CTAG_IS_FORCED, p_pParent, SVBoolType, 
				p_rTree, rArchive );
			HandleErrorCondition(12240, hrOk);

			hrOk = AddItem( CTAG_FORCED_VALUE, p_pParent, SVBoolType, 
				p_rTree, rArchive );
			HandleErrorCondition(12241, hrOk);

			hrOk = AddItem( CTAG_IS_INVERTED, p_pParent, SVBoolType, 
				p_rTree, rArchive );
			HandleErrorCondition(12242, hrOk);

			hrOk = AddItem( CTAG_DEFAULT_VALUE, p_pParent, SVBoolType, 
				p_rTree, rArchive );
			HandleErrorCondition(12243, hrOk);

			if (ulIODVersion >= 0x00043000)
			{
				hrOk = AddItem( CTAG_IS_COMBINED, p_pParent, SVBoolType, 
					p_rTree, rArchive );
				HandleErrorCondition(12991, hrOk);

				hrOk = AddItem( CTAG_IS_COMBINED_ACK, p_pParent, SVBoolType, 
					p_rTree, rArchive );
				HandleErrorCondition(12992, hrOk);
			}// end if

		}

		hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
			p_rTree, rArchive );
		HandleErrorCondition(12244, hrOk);

		hrOk = AddItem( CTAG_IO_OBJECT_ATTRIBUTE, p_pParent, SVIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12245, hrOk);
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVDigitalInputObjectClass( unsigned long ulSVOVersion, 
																											 unsigned long ulIODVersion, 
																											 CArchive &rArchive, 
																											 SVTreeType& p_rTree,
																											 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	CString	strVal;
	// [GUID] - Unique Reference ID
	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, 
		p_rTree, rArchive );
	HandleErrorCondition(12246, hrOk);

	// [CString] - Object Name
	hrOk = AddItem( CTAG_OBJECT_NAME, p_pParent, SVBStrType, 
		p_rTree, rArchive, &strVal );
	HandleErrorCondition(12247, hrOk);

	// if Greater than 3.12 Release
	if (ulIODVersion > 0x00030CFF)
	{
		// [UINT] - Object Attributes Set
		hrOk = AddItem( CTAG_OBJECT_ATTRIBUTE_SET, p_pParent, SVUnsignedIntType, 
			p_rTree, rArchive );
		HandleErrorCondition(12248, hrOk);
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveIPD(unsigned long ulSVOVersion, 
																 unsigned long &ulIPDVersion, 
																 CArchive &rArchive, 
																 SVTreeType& p_rTree,
																 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	if( p_pParent == NULL )
	{
		return E_FAIL;
	}

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	_bstr_t l_Name;

	p_rTree.GetBranchName( p_pParent, l_Name.GetBSTR() );

	CString csInspection = static_cast< LPCTSTR>( l_Name );

	hrOk = AddBranch( p_rTree, CTAG_SVIPDOC, p_pParent, l_pBranch );
	HandleErrorCondition(12249, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			hrOk = ArchiveSVIPDoc( ulSVOVersion, 
				ulIPDVersion,
				csInspection,
				rArchive, 
				p_rTree,
				l_pBranch );
			HandleErrorCondition(12250, hrOk);
			if ( hrOk != S_OK )
			{
				p_rTree.Clear();
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIPDoc( unsigned long ulSVOVersion, 
																		 unsigned long &ulIPDVersion, 
																		 CString &rcsInspection,
																		 CArchive &rArchive, 
																		 SVTreeType& p_rTree,
																		 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	CString csName, csValue;

	SVTreeType::SVBranchHandle l_pBranch( NULL );

	// [int] - Patch Value
	hrOk = AddItem( CTAG_PATCH_VALUE, p_pParent, SVIntType, p_rTree, rArchive );
	HandleErrorCondition(12251, hrOk);

	// [CString] - SVObserver IPD Identification
	hrOk = AddItem( CTAG_DESCRIPTION, p_pParent, SVBStrType, p_rTree, rArchive, &csName );
	HandleErrorCondition(12252, hrOk);

	// [CString][DWORD] - Version Number
	rArchive >> csName;	// "Version #"
	hrOk = AddItem( CTAG_VERSION_NUMBER, p_pParent, SVUnsignedLongType, p_rTree, rArchive, &ulIPDVersion );
	HandleErrorCondition(12253, hrOk);

	if (ulIPDVersion >= 0x00030001)
	{
		// [DWORD] - History Block
		hrOk = AddItem( CTAG_HISTORY_LEVEL, p_pParent, SVUnsignedLongType, p_rTree, rArchive );
		HandleErrorCondition(12254, hrOk);

		// [CString][CString] - Creation Date 
		rArchive >> csName;	// "Creation Date #"
		hrOk = AddItem( CTAG_CREATION_DATE, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12255, hrOk);

		// [CString][CString] - Name of Author
		rArchive >> csName;	// "Author #"
		hrOk = AddItem( CTAG_AUTHOR, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12256, hrOk);

		// [GUID] - Unique Reference ID
		hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive );
		HandleErrorCondition(12257, hrOk);

		if (ulIPDVersion >= 0x00030a02)
		{
			// Serialize View Data
			// ...
			SVTreeType::SVBranchHandle l_pBranch( NULL );

			hrOk = AddBranch( p_rTree, CTAG_IPDOC_VIEWS, p_pParent, l_pBranch );
			HandleErrorCondition(12258, hrOk);
			if ( hrOk == S_OK )
			{
				if ( l_pBranch != NULL )
				{
					hrOk = ArchiveSVIPDocViewData(ulSVOVersion, 
						ulIPDVersion, 
						rArchive, 
						p_rTree,
						l_pBranch);
					HandleErrorCondition(12259, hrOk);
				}
			}
		}

		// [int] - Height Result View
		hrOk = AddItem( CTAG_HEIGHT_RESULT_VIEW, p_pParent, SVIntType, p_rTree, rArchive);
		HandleErrorCondition(12260, hrOk);

		// [int] - Width Toolset View
		hrOk = AddItem( CTAG_WIDTH_TOOLSET_VIEW, p_pParent, SVIntType, p_rTree, rArchive);
		HandleErrorCondition(12261, hrOk);

		SVTreeType::SVBranchHandle l_pBranch( NULL );

		hrOk = AddBranch( p_rTree, CTAG_WINDOW_PLACEMENT, p_pParent, l_pBranch );
		HandleErrorCondition(12262, hrOk);

		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				// [WINDOWPLACEMENT] - Window Placement Definition
				hrOk = ArchiveWindowPlacementData(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
				HandleErrorCondition(12263, hrOk);
			}
		}

		hrOk = AddBranch( p_rTree, CTAG_IPDOC_REQUIREMENT_INFO, p_pParent, l_pBranch );
		HandleErrorCondition(12264, hrOk);

		if ( hrOk == S_OK )
		{
			if ( l_pBranch != NULL )
			{
				CString csBoardName;
				// [SVIPDocRequirementInfoClass] - Detail Document Requirement Information
				hrOk = ArchiveSVIPDocRequirementInfoClass( ulSVOVersion, 
					ulIPDVersion,
					NULL,
					csBoardName,
					rcsInspection,
					rArchive, 
					p_rTree,
					l_pBranch );
				HandleErrorCondition(12265, hrOk);
			}
		}

		// [GUID] - Unique Reference ID
		hrOk = AddItem( CTAG_OWNER_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive );
		HandleErrorCondition(12266, hrOk);

		// [CString] - Toolset Configuration Script
		hrOk = AddItem( CTAG_TOOLSET_SCRIPT, p_pParent, SVBStrType, p_rTree, rArchive, &csValue );
		HandleErrorCondition(12267, hrOk);
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVIPDocViewData( unsigned long ulSVOVersion, 
																						 unsigned long &ulIPDVersion, 
																						 CArchive &rArchive, 
																						 SVTreeType& p_rTree,
																						 typename SVTreeType::SVBranchHandle p_pParent)
{
	CString csName;
	HRESULT hrOk = S_FALSE;

	// [CString] - SVObserver View Identification
	hrOk = AddItem( CTAG_VIEW_IDENTIFICATION, p_pParent, SVBStrType, p_rTree, rArchive, &csName );
	HandleErrorCondition(12268, hrOk);

	// [CString][DWORD] - Version Number
	rArchive >> csName;	// Version #
	hrOk = AddItem( CTAG_VERSION_NUMBER, p_pParent, SVUnsignedLongType, p_rTree, rArchive, &ulIPDVersion );
	HandleErrorCondition(12269, hrOk);

	if (ulIPDVersion >= 0x00030a02)	// Version 3.10 Beta 2
	{
		SVTreeType::SVBranchHandle l_pBranch( NULL );
		SVTreeType::SVBranchHandle l_pViews( NULL );

		// [DWORD] - History Block
		hrOk = AddItem( CTAG_HISTORY_BLOCK, p_pParent, SVUnsignedLongType, p_rTree, rArchive );
		HandleErrorCondition(12270, hrOk);

		// Creation Date 
		rArchive >> csName;	// Creation Date #
		hrOk = AddItem( CTAG_CREATION_DATE, p_pParent, SVBStrType, p_rTree, rArchive );
		HandleErrorCondition(12271, hrOk);

		// Creation Author / Update Author
		rArchive >> csName;	// Author #
		hrOk = AddItem( CTAG_AUTHOR, p_pParent, SVBStrType, p_rTree, rArchive );
		HandleErrorCondition(12272, hrOk);

		// [GUID] - Unique Reference ID
		hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive );
		HandleErrorCondition(12273, hrOk);

		hrOk = AddBranch( p_rTree, CTAG_VIEWS, p_pParent, l_pViews );
		HandleErrorCondition(12274, hrOk);
		if ( hrOk == S_OK )
		{
			if ( l_pViews != NULL )
			{
				// { BEGIN LOOP - View Data - Loop until End-of-Views }
				while (1)
				{
					// [CString] - Class Name
					rArchive >> csName;

					if (csName.Compare(_T("End of Views")) == 0)
						break;

					// [int] - View Number
					hrOk = AddBranch( p_rTree, csName, l_pViews, l_pBranch );
					HandleErrorCondition(12275, hrOk);
					if ( hrOk == S_OK )
					{
						if ( l_pBranch != NULL )
						{
							hrOk = AddItem(CTAG_VIEW_NUMBER, l_pBranch, SVIntType, p_rTree, rArchive);
							HandleErrorCondition(12276, hrOk);

							if (csName == CTAG_SVIMAGE_VIEW_SCROLL)
							{
								// [SVImageViewScroll] - Scrolling Information
								hrOk = ArchiveSVImageViewScroll(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
								HandleErrorCondition(12277, hrOk);
							}
							else if (csName == CTAG_SVIMAGE_VIEW_CLASS)
							{
								// [SVImageViewClass] - Image Information
								hrOk = ArchiveSVImageViewClass(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
								HandleErrorCondition(12278, hrOk);
							}
							else if (csName == CTAG_SVTOOLSET_TAB_VIEW_CLASS)
							{
								// [SVToolSetTabViewClass] - Tool Set Tab Information
								hrOk = ArchiveSVToolSetTabViewClass(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
								HandleErrorCondition(12279, hrOk);
							}
							else if (csName == CTAG_SVRESULTVIEW_CLASS)
							{
								// [SVResultViewClass] - Result Information
								hrOk = ArchiveSVResultViewClass(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
								HandleErrorCondition(12280, hrOk);
							}
						}
					}
				}
				// { END LOOP - View Data }
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVImageViewScroll(	unsigned long ulSVOVersion, 
																							 unsigned long &ulIPDVersion, 
																							 CArchive &rArchive, 
																							 SVTreeType& p_rTree,
																							 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;
	SVTreeType::SVBranchHandle l_pBranch( NULL );

	hrOk = AddBranch( p_rTree, CTAG_IMAGE_VIEW, p_pParent, l_pBranch );
	HandleErrorCondition(12281, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [SVImageViewClass] - Image View Information
			hrOk = ArchiveSVImageViewClassEmbedded(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12282, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_SCROLL_SIZE, p_pParent, l_pBranch );
	HandleErrorCondition(12283, hrOk);

	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [CSize] - Scroll Size
			hrOk = ArchiveSizeData(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12284, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_SCROLL_POINT, p_pParent, l_pBranch );
	HandleErrorCondition(12285, hrOk);

	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [CPoint] - Scroll Point
			hrOk = ArchivePointData(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12286, hrOk);
		}
	}

	// [int] - Cell Height
	hrOk = AddItem( CTAG_CELL_HEIGHT, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12287, hrOk);

	// [int] - Cell Width
	hrOk = AddItem( CTAG_CELL_WIDTH, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12288, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVImageViewClass(unsigned long ulSVOVersion, 
																							unsigned long &ulIPDVersion, 
																							CArchive &rArchive, 
																							SVTreeType& p_rTree,
																							typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	// [long] - Image Zoom Factor
	hrOk = AddItem( CTAG_IMAGE_ZOOM_FACTOR, p_pParent, SVLongType, p_rTree, rArchive);
	HandleErrorCondition(12289, hrOk);

	// [BOOL] - Image Zoom Factor
	BOOL bViewInitialized = FALSE;
	hrOk = AddItem( CTAG_VIEW_INITIALIZED, p_pParent, SVBoolType, p_rTree, rArchive, &bViewInitialized);
	HandleErrorCondition(12290, hrOk);

	if (bViewInitialized)
	{
		_variant_t svVariant;
		CString csImageName;
		CString csTemp;

		rArchive >> csImageName;

		csTemp = _T( "VIPER DUAL" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
			csImageName = _T( "VIPER DUAL" ) + csImageName.Mid( 10 );

		csTemp = _T( "VIPER QUAD" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
			csImageName = _T( "VIPER QUAD" ) + csImageName.Mid( 10 );

		csTemp = _T( "VIPER RGB" );
		if( 0 == csTemp.CompareNoCase( csImageName.Left( 9 ) ) )
			csImageName = _T( "VIPER RGB" ) + csImageName.Mid( 9 );

		if ( csImageName.Find("VIPER DUAL.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER DUAL.Dig_0.", "" );
		}
		else if ( csImageName.Find("VIPER DUAL.Dig_1.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER DUAL.Dig_1.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER QUAD.Dig_0.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_1.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER QUAD.Dig_1.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_2.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER QUAD.Dig_2.", "" );
		}
		else if ( csImageName.Find("VIPER QUAD.Dig_3.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER QUAD.Dig_3.", "" );
		}
		else if ( csImageName.Find("VIPER RGB.Dig_0.", 0 ) != -1 )
		{
			csImageName.Replace("VIPER RGB.Dig_0.", "" );
		}

		svVariant.SetString( (LPCTSTR)csImageName );

		// [CString] - Image Name
		hrOk = AddItem( CTAG_IMAGE_NAME, p_pParent, p_rTree, svVariant );
		HandleErrorCondition(12292, hrOk);
		// [CString] - Image Name
		//		hrOk = AddItem( CTAG_IMAGE_NAME, p_pParent, SVBStrType, p_rTree, rArchive);
	}


	// [GUID] - Unique Reference ID
	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive);
	HandleErrorCondition(12293, hrOk);

	// [CString] - Freeze Script
	hrOk = AddItem( CTAG_FREEZE_SCRIPT, p_pParent, SVBStrType, p_rTree, rArchive);
	HandleErrorCondition(12294, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVImageViewClassEmbedded(unsigned long ulSVOVersion, 
																											unsigned long &ulIPDVersion, 
																											CArchive &rArchive, 
																											SVTreeType& p_rTree,
																											typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	if (ulIPDVersion >= 0x00030a01)
	{
		// [long] - Image Zoom Factor
		hrOk = AddItem( CTAG_IMAGE_ZOOM_FACTOR, p_pParent, SVLongType, p_rTree, rArchive);
		HandleErrorCondition(12295, hrOk);

		// [BOOL] - Image Zoom Factor
		BOOL bViewInitialized = FALSE;
		hrOk = AddItem( CTAG_VIEW_INITIALIZED, p_pParent, SVBoolType, p_rTree, rArchive, &bViewInitialized);
		HandleErrorCondition(12296, hrOk);

		if (bViewInitialized)
		{
			_variant_t svVariant;
			CString csImageName;
			CString csTemp;

			rArchive >> csImageName;

			csTemp = _T( "VIPER DUAL" );
			if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
				csImageName = _T( "VIPER DUAL" ) + csImageName.Mid( 10 );

			csTemp = _T( "VIPER QUAD" );
			if( 0 == csTemp.CompareNoCase( csImageName.Left( 10 ) ) )
				csImageName = _T( "VIPER QUAD" ) + csImageName.Mid( 10 );

			csTemp = _T( "VIPER RGB" );
			if( 0 == csTemp.CompareNoCase( csImageName.Left( 9 ) ) )
				csImageName = _T( "VIPER RGB" ) + csImageName.Mid( 9 );

			if ( csImageName.Find("VIPER DUAL.Dig_0.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER DUAL.Dig_0.", "" );
			}
			else if ( csImageName.Find("VIPER DUAL.Dig_1.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER DUAL.Dig_1.", "" );
			}
			else if ( csImageName.Find("VIPER QUAD.Dig_0.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER QUAD.Dig_0.", "" );
			}
			else if ( csImageName.Find("VIPER QUAD.Dig_1.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER QUAD.Dig_1.", "" );
			}
			else if ( csImageName.Find("VIPER QUAD.Dig_2.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER QUAD.Dig_2.", "" );
			}
			else if ( csImageName.Find("VIPER QUAD.Dig_3.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER QUAD.Dig_3.", "" );
			}
			else if ( csImageName.Find("VIPER RGB.Dig_0.", 0 ) != -1 )
			{
				csImageName.Replace("VIPER RGB.Dig_0.", "" );
			}

			svVariant.SetString( (LPCTSTR)csImageName );

			// [CString] - Image Name
			hrOk = AddItem( CTAG_IMAGE_NAME, p_pParent, p_rTree, svVariant );
			HandleErrorCondition(12298, hrOk);
			// [CString] - Image Name
			//			hrOk = AddItem( CTAG_IMAGE_NAME, p_pParent, SVBStrType, p_rTree, rArchive);
		}
	}

	if (ulIPDVersion >= 0x00030a02)
	{
		// [GUID] - Unique Reference ID
		hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive);
		HandleErrorCondition(12299, hrOk);

		// [CString] - Freeze Script
		hrOk = AddItem( CTAG_FREEZE_SCRIPT, p_pParent, SVBStrType, p_rTree, rArchive);
		HandleErrorCondition(12300, hrOk);
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVToolSetTabViewClass(	unsigned long ulSVOVersion, 
																									 unsigned long &ulIPDVersion, 
																									 CArchive &rArchive, 
																									 SVTreeType& p_rTree,
																									 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	// [int] - Cell Height
	hrOk = AddItem( CTAG_CELL_HEIGHT, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12301, hrOk);

	// [int] - Cell Width
	hrOk = AddItem( CTAG_CELL_WIDTH, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12302, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSVResultViewClass(	unsigned long ulSVOVersion, 
																							 unsigned long &ulIPDVersion, 
																							 CArchive &rArchive, 
																							 SVTreeType& p_rTree,
																							 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	// [int] - Cell Height
	hrOk = AddItem( CTAG_CELL_HEIGHT, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12303, hrOk);

	// [int] - Cell Width
	hrOk = AddItem( CTAG_CELL_WIDTH, p_pParent, SVIntType, p_rTree, rArchive);
	HandleErrorCondition(12304, hrOk);

	// [GUID] - Unique Reference ID
	hrOk = AddItem( CTAG_UNIQUE_REFERENCE_ID, p_pParent, SVClsIdType, p_rTree, rArchive);
	HandleErrorCondition(12305, hrOk);

	// [CString] - Freeze Script
	hrOk = AddItem( CTAG_FREEZE_SCRIPT, p_pParent, SVBStrType, p_rTree, rArchive);
	HandleErrorCondition(12306, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveWindowPlacementData( unsigned long ulSVOVersion, 
																								 unsigned long &ulIPDVersion, 
																								 CArchive &rArchive, 
																								 SVTreeType& p_rTree,
																								 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;
	SVTreeType::SVBranchHandle l_pBranch( NULL );

	// [UINT] - Length
	hrOk = AddItem( CTAG_LENGTH, p_pParent, SVUnsignedIntType, p_rTree, rArchive);
	HandleErrorCondition(12307, hrOk);

	// [UINT] - Flags
	hrOk = AddItem( CTAG_FLAGS, p_pParent, SVUnsignedIntType, p_rTree, rArchive);
	HandleErrorCondition(12308, hrOk);

	// [UINT] - Show Command
	hrOk = AddItem( CTAG_SHOW_COMMAND, p_pParent, SVUnsignedIntType, p_rTree, rArchive);
	HandleErrorCondition(12309, hrOk);

	hrOk = AddBranch( p_rTree, CTAG_MINIMUM_POSITION, p_pParent, l_pBranch );
	HandleErrorCondition(12310, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [POINT] - Minimum Position
			hrOk = ArchivePointData(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12311, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_MAXIMUM_POSITION, p_pParent, l_pBranch );
	HandleErrorCondition(12312, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [POINT] - Maximum Position
			hrOk = ArchivePointData(ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12313, hrOk);
		}
	}

	hrOk = AddBranch( p_rTree, CTAG_NORMAL_POSITION, p_pParent, l_pBranch );
	HandleErrorCondition(12314, hrOk);
	if ( hrOk == S_OK )
	{
		if ( l_pBranch != NULL )
		{
			// [RECT] - Normal Position
			hrOk = ArchiveRectData( ulSVOVersion, ulIPDVersion, rArchive, p_rTree, l_pBranch);
			HandleErrorCondition(12315, hrOk);
		}
	}

	return hrOk;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primitives section
// the HandleErrorCondition call is not made within primitives so that more meaningful
// error placement and reporting can be made
// a primitive for this purpose is defined as a basic function which is called from more than
// one place in SVOCMArchive
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




template< typename SVTreeType >
HRESULT SVOCMArchive::ArchivePointData( unsigned long ulSVOVersion, 
																			 unsigned long &ulIPDVersion, 
																			 CArchive &rArchive, 
																			 SVTreeType& p_rTree,
																			 typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	// [long] - X
	hrOk = AddItem( CTAG_X, p_pParent, SVLongType, p_rTree, rArchive);

	// [long] - Y
	hrOk = AddItem( CTAG_Y, p_pParent, SVLongType, p_rTree, rArchive);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveRectData(unsigned long ulSVOVersion, 
																			unsigned long &ulIPDVersion, 
																			CArchive &rArchive, 
																			SVTreeType& p_rTree,
																			typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;
	SVTreeType::SVBranchHandle l_pBranch( NULL );

	// [long] - Left
	hrOk = AddItem( CTAG_LEFT, p_pParent, SVLongType, p_rTree, rArchive);
	HandleErrorCondition(12316, hrOk);

	// [long] - Top
	hrOk = AddItem( CTAG_TOP, p_pParent, SVLongType, p_rTree, rArchive);
	HandleErrorCondition(12317, hrOk);

	// [long] - Right
	hrOk = AddItem( CTAG_RIGHT, p_pParent, SVLongType, p_rTree, rArchive);
	HandleErrorCondition(12318, hrOk);

	// [long] - Bottom
	hrOk = AddItem( CTAG_BOTTOM, p_pParent, SVLongType, p_rTree, rArchive);
	HandleErrorCondition(12319, hrOk);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::ArchiveSizeData(unsigned long ulSVOVersion, 
																			unsigned long &ulIPDVersion, 
																			CArchive &rArchive, 
																			SVTreeType& p_rTree,
																			typename SVTreeType::SVBranchHandle p_pParent)
{
	HRESULT hrOk = S_FALSE;

	// [long] - CX
	hrOk = AddItem( CTAG_CX, p_pParent, SVLongType, p_rTree, rArchive);

	// [long] - CY
	hrOk = AddItem( CTAG_CY, p_pParent, SVLongType, p_rTree, rArchive);

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetEnvironmentBranch( SVTreeType& p_rTree, 
																					 typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pEnviro( NULL );
	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	p_rpBranch = NULL;

	hrOk = GetBranch( p_rTree, CTAG_ENVIRONMENT, l_Root, l_pEnviro );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_ENVIRONMENT, l_Root, l_pEnviro );
	}

	if ( hrOk == S_OK )
	{
		p_rpBranch = l_pEnviro;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetAcquisitionDeviceBranch( LPCTSTR lpszBoardName,
																								 long lDigNumber,
																								 SVTreeType& p_rTree, 
																								 typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pCamera( NULL );
	SVTreeType::SVBranchHandle l_pBoard( NULL );
	SVTreeType::SVBranchHandle l_pDig_x( NULL );

	p_rpBranch = NULL;

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_ACQUISITION_DEVICE, l_Root, l_pCamera );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_ACQUISITION_DEVICE, l_Root, l_pCamera );
	}

	if ( hrOk == S_OK )
	{
		hrOk = GetBranch( p_rTree, lpszBoardName, l_pCamera, l_pBoard );
		if ( hrOk != S_OK )
		{
			hrOk = AddBranch( p_rTree, lpszBoardName, l_pCamera, l_pBoard );
		}

		if ( hrOk == S_OK )
		{
			CString csName;

			csName.Format( CTAGF_DIG_X, lDigNumber );

			hrOk = GetBranch( p_rTree, csName, l_pBoard, l_pDig_x );
			if ( hrOk != S_OK )
			{
				hrOk = AddBranch( p_rTree, csName, l_pBoard, l_pDig_x );
			}

			if ( hrOk == S_OK )
			{
				p_rpBranch = l_pDig_x;
			}
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetCameraBranch( long lNumber,
																			SVTreeType& p_rTree, 
																			typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pCamera( NULL );
	SVTreeType::SVBranchHandle l_pCamera_x( NULL );

	p_rpBranch = NULL;

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_CAMERA, l_Root, l_pCamera );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_CAMERA, l_Root, l_pCamera );
	}

	if ( hrOk == S_OK )
	{
		CString csName;

		csName.Format( CTAGF_CAMERA_X, lNumber );

		hrOk = GetBranch( p_rTree, csName, l_pCamera, l_pCamera_x );
		if ( hrOk != S_OK )
		{
			hrOk = AddBranch( p_rTree, csName, l_pCamera, l_pCamera_x );
		}

		if ( hrOk == S_OK )
		{
			p_rpBranch = l_pCamera_x;
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetTriggerBranch( long lNumber,
																			 SVTreeType& p_rTree, 
																			 typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pTrigger( NULL );
	SVTreeType::SVBranchHandle l_pTrigger_x( NULL );

	p_rpBranch = NULL;

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_TRIGGER, l_Root, l_pTrigger );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_TRIGGER, l_Root, l_pTrigger );
		HandleErrorCondition(12320, hrOk);
	}

	if ( hrOk == S_OK )
	{
		CString csName;

		csName.Format( CTAGF_TRIGGER_X, lNumber );

		hrOk = GetBranch( p_rTree, csName, l_pTrigger, l_pTrigger_x );
		if ( hrOk != S_OK )
		{
			hrOk = AddBranch( p_rTree, csName, l_pTrigger, l_pTrigger_x );
			HandleErrorCondition(12321, hrOk);
		}

		if ( hrOk == S_OK )
		{
			p_rpBranch = l_pTrigger_x;
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetInspectionBranch( long lNumber,
																					SVTreeType& p_rTree, 
																					typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pInspection( NULL );
	SVTreeType::SVBranchHandle l_pInspection_x( NULL );

	p_rpBranch = NULL;

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_INSPECTION, l_Root, l_pInspection );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_INSPECTION, l_Root, l_pInspection );
		HandleErrorCondition(12322, hrOk);
	}

	if ( hrOk == S_OK )
	{
		CString csName;

		csName.Format( CTAGF_INSPECTION_X, lNumber );

		hrOk = GetBranch( p_rTree, csName, l_pInspection, l_pInspection_x );
		if ( hrOk != S_OK )
		{
			hrOk = AddBranch( p_rTree, csName, l_pInspection, l_pInspection_x );
			HandleErrorCondition(12323, hrOk);
		}

		if ( hrOk == S_OK )
		{
			p_rpBranch = l_pInspection_x;
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetPPQBranch( long lNumber,
																	 SVTreeType& p_rTree, 
																	 typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pPPQ( NULL );
	SVTreeType::SVBranchHandle l_pPPQ_x( NULL );

	p_rpBranch = NULL;

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_PPQ, l_Root, l_pPPQ );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_PPQ, l_Root, l_pPPQ );
	}

	if ( hrOk == S_OK )
	{
		CString csName;

		csName.Format( CTAGF_PPQ_X, lNumber );

		hrOk = GetBranch( p_rTree, csName, l_pPPQ, l_pPPQ_x );
		if ( hrOk != S_OK )
		{
			hrOk = AddBranch( p_rTree, csName, l_pPPQ, l_pPPQ_x );
		}

		if ( hrOk == S_OK )
		{
			p_rpBranch = l_pPPQ_x;
		}
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetIOBranch( SVTreeType& p_rTree, 
																	typename SVTreeType::SVBranchHandle& p_rpBranch )
{
	HRESULT hrOk = S_FALSE;

	SVTreeType::SVBranchHandle l_pIO( NULL );

	SVTreeType::SVBranchHandle l_Root( NULL );

	p_rTree.GetRoot( l_Root );

	hrOk = GetBranch( p_rTree, CTAG_IO, l_Root, l_pIO );
	if ( hrOk != S_OK )
	{
		hrOk = AddBranch( p_rTree, CTAG_IO, l_Root, l_pIO );
	}

	if ( hrOk == S_OK )
	{
		p_rpBranch = l_pIO;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetBranch( SVTreeType& p_rTree, 
																LPCTSTR lpszName, 
																typename SVTreeType::SVBranchHandle p_pParent, 
																typename SVTreeType::SVBranchHandle& p_rpItem )
{
	HRESULT hrOk = S_OK;

	if( p_pParent != NULL )
	{
		hrOk = p_rTree.FindBranch( p_pParent, _bstr_t( lpszName ), p_rpItem );
	}
	else
	{
		p_rpItem = NULL;

		hrOk = E_FAIL;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::GetItem( SVTreeType& p_rTree, 
															LPCTSTR lpszName, 
															typename SVTreeType::SVBranchHandle p_pParent, 
															typename SVTreeType::SVLeafHandle& p_rLeaf )
{
	HRESULT hrOk = S_OK;

	if( p_pParent != NULL )
	{
		hrOk = p_rTree.FindLeaf( p_pParent, _bstr_t( lpszName ), p_rLeaf );
	}
	else
	{
		hrOk = E_FAIL;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::AddItem(  LPCTSTR lpszName, 
															typename SVTreeType::SVBranchHandle& p_rParent,
															SVTreeType& p_rTree, 
															_variant_t &rVariant )
{
	HRESULT hrOk = S_OK;

	hrOk = p_rTree.CreateLeaf( p_rParent, _bstr_t( lpszName ), rVariant );

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::AddItem(  LPCTSTR lpszName, 
															typename SVTreeType::SVBranchHandle& p_rParent,
															int iType,
															SVTreeType &rTree, 
															CArchive &rArchive,
															void *pvValue )
{
	HRESULT hrOk = S_OK;

	_variant_t svVariant;

	switch ( iType )
	{
	case SVBStrType:
		{
			CString Value;

			unsigned __int64 lPos = rArchive.GetFile()->Seek( 0, CFile::current );

			rArchive >> Value;

			lPos = rArchive.GetFile()->Seek( 0, CFile::current );

			svVariant.SetString( (LPCTSTR)Value );

			if ( pvValue )
			{
				*((CString *)pvValue) = Value;
			}

			break;
		}
	case SVBoolType:
		{
			BOOL bValue;

			rArchive >> bValue;

			bool Value = bValue == TRUE;

			svVariant = Value;

			if ( pvValue )
			{
				*((bool *)pvValue) = Value;
			}

			break;
		}
	case SVFloatType:
		{
			float Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((float *)pvValue) = Value;
			}

			break;
		}
	case SVDoubleType:
		{
			double Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((double *)pvValue) = Value;
			}

			break;
		}
	case SVCharType:
		{
			char Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((char *)pvValue) = Value;
			}

			break;
		}
	case SVShortType:
		{
			short Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((short *)pvValue) = Value;
			}

			break;
		}
	case SVIntType:
		{
			int Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((int *)pvValue) = Value;
			}

			break;
		}
	case SVLongType:
		{
			long Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((long *)pvValue) = Value;
			}

			break;
		}
	case SVUnsignedCharType:
		{
			unsigned char Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((unsigned char *)pvValue) = Value;
			}

			break;
		}
	case SVUnsignedShortType:
		{
			unsigned short Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((unsigned short *)pvValue) = Value;
			}

			break;
		}
	case SVUnsignedIntType:
		{
			unsigned int Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((unsigned int *)pvValue) = Value;
			}

			break;
		}
	case SVUnsignedLongType:
		{
			unsigned long Value;

			rArchive >> Value;

			svVariant = Value;

			if ( pvValue )
			{
				*((unsigned long *)pvValue) = Value;
			}

			break;
		}
	case SVClsIdType:
		{
			GUID Value;

			hrOk = SerializeGUID( rArchive, Value ); // Unique Reference Id

			if ( hrOk == S_OK )
			{
				svVariant = SVGUID( Value ).ToVARIANT();
			}

			if ( pvValue )
			{
				*((GUID *)pvValue) = Value;
			}

			break;
		}
	default:
		{
			hrOk = E_FAIL;

			break;
		}
	}

	if ( hrOk == S_OK )
	{
		hrOk = AddItem( lpszName, p_rParent, rTree, svVariant );
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::AddArray( LPCTSTR lpszName, 
															 typename SVTreeType::SVBranchHandle& p_rParent,
															 int iType,
															 unsigned long ulSize,
															 SVTreeType &rTree, 
															 CArchive &rArchive )
{
	HRESULT hrOk = S_FALSE;

	SVSAFEARRAY svArray;

	switch ( iType )
	{
	case SVBStrArrayType:
		{
			CString Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				_variant_t l_Variant;

				rArchive >> Value;

				l_Variant.SetString( Value );

				hrOk = svArray.Add( l_Variant );
			}

			break;
		}
	case SVBoolArrayType:
		{
			BOOL bValue;
			bool Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> bValue;

				Value = bValue == TRUE;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVFloatArrayType:
		{
			float Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVDoubleArrayType:
		{
			double Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVCharArrayType:
		{
			char Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVShortArrayType:
		{
			short Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVIntArrayType:
		{
			int Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVLongArrayType:
		{
			long Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVUnsignedCharArrayType:
		{
			unsigned char Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVUnsignedShortArrayType:
		{
			unsigned short Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVUnsignedIntArrayType:
		{
			unsigned int Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVUnsignedLongArrayType:
		{
			unsigned long Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				rArchive >> Value;

				hrOk = svArray.Add( _variant_t( Value ) );
			}

			break;
		}
	case SVClsIdArrayType:
		{
			GUID Value;

			for ( unsigned long l = 0; l < ulSize; l++ )
			{
				hrOk = SerializeGUID( rArchive, Value ); // Unique Reference Id
				if ( hrOk == S_OK )
				{
					hrOk = svArray.Add( SVGUID( Value ).ToVARIANT() );
				}
			}

			break;
		}
	default:
		{
			break;
		}
	}

	if ( hrOk == S_OK )
	{
		_variant_t l_Temp( svArray );

		hrOk = AddItem( lpszName, p_rParent, rTree, l_Temp );
	}

	if (ulSize == 0)    // EB 2002 08 09
		hrOk = S_OK;

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::AddBranch(SVTreeType& p_rTree, 
																LPCTSTR lpszName, 
																typename SVTreeType::SVBranchHandle p_pParent, 
																typename SVTreeType::SVBranchHandle& p_rpBranch)
{
	HRESULT hrOk = S_OK;

	if( p_pParent != NULL )
	{
		hrOk = p_rTree.CreateBranch( p_pParent, _bstr_t( lpszName ), &p_rpBranch );
	}
	else
	{
		p_rpBranch = NULL;

		hrOk = E_FAIL;
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateTreeFromConfigurationFile (unsigned long  ulSVOVersion, 
																											 unsigned long  ulConfigVersion, 
																											 BSTR			bstrFilename,
																											 SVTreeType& p_rTree)
{
	USES_CONVERSION;


	HRESULT		hr( S_OK );

	SVString l_FileName( bstrFilename );

	CString		cstrMessage;
	BSTR			bstrRevisionHistory;

	hr = 0;
	bstrRevisionHistory = NULL;

	while (1)
	{
		if( ulConfigVersion < 0x00040000 )
		{
			//-		Configuration files prior to version 4.00 should not be able to get
			//-		here.
			hr = -1663;
			break;
		}
		else if( 0x00040000 <= ulConfigVersion && ulConfigVersion < 0x00043200 )
		{
			SVXmlStream< SVTreeType > xml( p_rTree );

			if( !xml.Load_XML_Document( l_FileName.c_str() ) )
			{
				hr = E_FAIL;
			}
		}
		else if( 0x00043200 <= ulConfigVersion )
		{
			BSTR bstrChangedNode( NULL );

			SVTreeType l_XMLTree( p_rTree );
			SVXMLClass l_XMLTreeBuilder;

			hr = l_XMLTreeBuilder.Initialize();

			if( SV_SUCCEEDED( hr ) )
			{
				hr = l_XMLTreeBuilder.PreserveWhitespace( true );
			}

			if( SV_SUCCEEDED( hr ) )
			{
				//hr = l_XMLTreeBuilder.CopyXMLFileToTree( l_XMLTree, ulSVOVersion, bstrFilename, &bstrRevisionHistory, &bstrChangedNode );

				hr = l_XMLTreeBuilder.CopyXMLFileToDOM( bstrFilename, &bstrRevisionHistory );

				if( SV_SUCCEEDED( hr ) )
				{
					hr = SVXML2TreeConverter::CopyToTree( l_XMLTreeBuilder, l_XMLTree, L"Base", false );
				}
			}

			/*
			if( !SV_SUCCEEDED( hr ) )
			{
				cstrMessage.Format("WARNING: Configuration has been changed.  \
													 The last changed node is (other nodes may also have been changed) - %s. \
													 Configuration loading will continue.",
													 W2T (bstrChangedNode));

				AfxMessageBox(cstrMessage);
			}
			*/
		}
		else
		{
			hr = E_FAIL;
		}

		break;
	}

	SysFreeString (bstrRevisionHistory);
	bstrRevisionHistory = NULL;

	return hr;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateConfigurationFileFromTree(unsigned long ulSVOVersion, BSTR bstrRevisionHistory, unsigned long ulSECVersion, LPCTSTR pstrFilename, SVTreeType& p_rTree)
{
	HRESULT hr = S_OK;

	SVTreeType l_XMLTree( p_rTree );
	SVXMLClass l_XMLTreeBuilder;
	
	hr = l_XMLTreeBuilder.Initialize( 0,		// do not use checksums
		1, 						// use program version/data revision info
		0);						// do not use encryption

	if( SV_SUCCEEDED( hr ) )
	{
		hr = l_XMLTreeBuilder.PreserveWhitespace( true );
	}

	if( SV_SUCCEEDED( hr ) )
	{
		SVBStr bstrFilename;
		SVBStr bstrFormatName;

		bstrFilename = pstrFilename;

		hr = bstrFormatName.CopyFromWChar (L"SVObserver");

		if( SV_SUCCEEDED( hr ) )
		{
			hr = l_XMLTreeBuilder.CopyTreeToXMLFile( l_XMLTree, ulSVOVersion, bstrFormatName, bstrRevisionHistory, bstrFilename );
		}
	}
	return hr;
}

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVConfigurationLibrary\SVOCMArchive.inl_v  $
 * 
 *    Rev 1.2   03 Feb 2014 16:11:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed method ArchiveSEC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jun 2013 16:44:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 May 2013 10:33:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums to SVConfigurationLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 May 2013 08:55:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix loading problems with SEC files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:39:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Oct 2012 10:11:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to reflect fixing issues with XML functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.11   22 Jun 2007 10:02:22   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.10   20 Jun 2007 13:44:58   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.9   10 Oct 2005 09:27:42   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Removed Test Code for encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.8   22 Sep 2005 16:01:16   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Add choice to Save to prevent encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.7   16 Sep 2005 16:16:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Encryption was generically implemented through the SVXMLClass.  Because the configurations use this class, their interface to the SVXMLClass needed modified.  
* 
* Also, the configuration handling was modified to handle actual excrypted configurations because this was believed the best way to test the XML enryption.  Getting the version number needed enhanced since the part getting the version number has no encryption knowledge.
* 
* The following functions were modified:
*  SVOMArchive::CreateConfigurationFileFromTree ()
*  SVOMArchive::CreateTreeFromConfigurationFile ()
*  SVOMArchive::GetSVOSavedVersionNumberFromConfigurationFile (), and
*  SVOMArchive::FindVersion440_ ()
* 
* The following functions were added:
*  SVOMArchive::FindVersion440_Standard (), and
*  SVOMArchive::FindVersion440_RevisionHistory () 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:44:12   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:43:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   22 Oct 2004 07:27:28   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   In order to implement the new XML parser classes, load routines where re-written and version detection methods where implemented so that old configuration files would load correctly.
* 
* Added headers for referencing:
*   USES_CONVERSION,
*   SVXMLClass,
*   SVCTreeControlClass,
*   SVBStr.
* 
* Rewrote and exchanged...
*   CreateTreeFromConfigurationFile () 
*   for
*   BuildTreeFromXMLDoc (),
* and
*   CreateConfigurationFileFromTree ()
*   for
*   CreateXMLDocFromTree ().
* 
* Added member functions:
*   GetSVOSavedVersionNumberFromConfigurationFile (),
*   FindVersion401_430 (), and
*   FindVersion440_ ().
* 
* 
* 
* 
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   02 Oct 2003 10:43:02   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  386
* SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Assigned old light reference values that had never been set to a new flag value for processing in the loading process.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   29 Sep 2003 14:47:40   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Fixed archive code to only conditionally read new combined settings.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   25 Sep 2003 14:22:38   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Added new configuration flags to support combined digital outputs.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   09 Jan 2003 16:05:56   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  317
* SCR Title:  Load SEC improperly imports Light Reference data for RGB Color
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Modified the ArchiveSVLightReferenceArrayClass and ArchiveSVLightReferenceClass functions to take as a parameter whether the system is a RGB Color or not (bIsColorMode).
* In ArchiveSVLightReferenceClass, if setting a default value for RGB contrast, check this flag before assigning the value. If color mode, default value=10000 otherwise default is 8100.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.0   18 Nov 2002 09:56:16   ryoho
* Project:  SVObserver
* Change Request (SCR) nbr:  226
* SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   first version of the file
* 
* /////////////////////////////////////////////////////////////////////////////////////
*/
