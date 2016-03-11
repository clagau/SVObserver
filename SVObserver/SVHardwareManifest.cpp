//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHardwareManifest
//* .File Name       : $Workfile:   SVHardwareManifest.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   23 Oct 2014 13:27:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVHardwareManifest.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
#include <map>
#include <set>

static const CString SVIM_TRIGGER_SOURCE_IO_BOARD_STRING   ( _T("IO_Board_1") );
static const CString SVIM_SOFTWARE_TRIGGER_SOURCE_STRING   ( _T("SoftwareTrigger_1") );
static const CString SVIM_CAMERA_TRIGGER_SOURCE_STRING   ( _T("CameraTrigger") );
static const CString SVIM_DIG_NAME_STRING       ( _T(".Dig_") );

typedef std::set<SVIMProductEnum> SVIMProductEnumSet;
typedef std::map<SVIMProductEnum, SVIMProductEnumSet> SVIMProductCompatibilityList;

const SVHardwareManifest::SVIMTypeMap SVHardwareManifest::m_SVIMTypeMap = boost::assign::map_list_of< SVIMProductEnum, SVIMTypeInfoStruct >
	// SVIMProductEnum,                               SUPPORTED, DIGITIZERS, TRIGGERS
	( SVIM_PRODUCT_FULL,          SVIMTypeInfoStruct( false,     4,			4 ) )
	( SVIM_PRODUCT_05,            SVIMTypeInfoStruct( false,     2,			2 ) )
	( SVIM_PRODUCT_RGB_MONO,      SVIMTypeInfoStruct( false,     3,			1 ) )
	( SVIM_PRODUCT_RGB_COLOR,     SVIMTypeInfoStruct( false,     1,			1 ) )
	( SVIM_PRODUCT_D1,            SVIMTypeInfoStruct( false,     3,			1 ) )
	( SVIM_PRODUCT_D2,            SVIMTypeInfoStruct( false,     6,			2 ) )
	( SVIM_PRODUCT_D3,            SVIMTypeInfoStruct( false,     9,			3 ) )
	( SVIM_PRODUCT_X1,            SVIMTypeInfoStruct( false,     3,			1 ) )
	( SVIM_PRODUCT_X2,            SVIMTypeInfoStruct( false,     6,			3 ) )
	( SVIM_PRODUCT_X3,            SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_D1_COLOR,      SVIMTypeInfoStruct( false,     3,			1 ) )
	( SVIM_PRODUCT_D2_COLOR,      SVIMTypeInfoStruct( false,     6,			2 ) )
	( SVIM_PRODUCT_D3_COLOR,      SVIMTypeInfoStruct( false,     9,			3 ) )
	( SVIM_PRODUCT_X1_COLOR,      SVIMTypeInfoStruct( false,     3,			1 ) )
	( SVIM_PRODUCT_X2_COLOR,      SVIMTypeInfoStruct( false,     6,			3 ) )
	( SVIM_PRODUCT_X3_COLOR,      SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_D1_HUB,        SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_D2_HUB,        SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_D3_HUB,        SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_X1_HUB,        SVIMTypeInfoStruct( false,     3,			3 ) )
	( SVIM_PRODUCT_X2_GD1A,       SVIMTypeInfoStruct( true,      1,			1 ) )
	( SVIM_PRODUCT_X2_GD1A_COLOR, SVIMTypeInfoStruct( false,     1,			1 ) )
	( SVIM_PRODUCT_X2_GD2A,       SVIMTypeInfoStruct( false,     2,			2 ) )
	( SVIM_PRODUCT_X2_GD2A_COLOR, SVIMTypeInfoStruct( false,     2,			2 ) )
	( SVIM_PRODUCT_X2_GD4A,       SVIMTypeInfoStruct( false,     4,			3 ) )
	( SVIM_PRODUCT_X2_GD4A_COLOR, SVIMTypeInfoStruct( false,     4,			3 ) )
	( SVIM_PRODUCT_X2_GD8A,       SVIMTypeInfoStruct( true,      8,			3 ) )
	( SVIM_PRODUCT_X2_GD8A_COLOR, SVIMTypeInfoStruct( false,     8,			3 ) )
	//	( SVIM_PRODUCT_X2_GD8A_NONIO, SVIMTypeInfoStruct( true,      8,			3 ) )
	//	( SVIM_PRODUCT_X2_GD8A_NONIO_COLOR, SVIMTypeInfoStruct( true, 8,			3 ) )
	;
	
SVHardwareManifest& SVHardwareManifest::Instance()
{
	static SVHardwareManifest l_Object;
	return l_Object;
}

SVHardwareManifest::SVHardwareManifest()
{
}

SVHardwareManifest::~SVHardwareManifest()
{
	Shutdown();
}

void SVHardwareManifest::Startup()
{
	m_TriggerDeviceParams = boost::assign::list_of< SVTriggerDeviceParams >
		( SVTriggerDeviceParams( _T( "Viper_Quad_1.Dig_0" ), 0 ) )
		( SVTriggerDeviceParams( _T( "Viper_Quad_1.Dig_1" ), 1 ) )
		( SVTriggerDeviceParams( _T( "Viper_Quad_1.Dig_2" ), 2 ) )
		( SVTriggerDeviceParams( _T( "Viper_Quad_1.Dig_3" ), 3 ) )
		( SVTriggerDeviceParams( _T( "Viper_RGB_1.Dig_0" ), 0 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_0" ), 0 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_1" ), 1 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_2" ), 2 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_3" ), 3 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_4" ), 4 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_5" ), 5 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_6" ), 6 ) )
		( SVTriggerDeviceParams( _T( "IO_Board_1.Dig_7" ), 7 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_0" ), 0 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_1" ), 1 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_2" ), 2 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_3" ), 3 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_4" ), 4 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_5" ), 5 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_6" ), 6 ) )
		( SVTriggerDeviceParams( _T( "SoftwareTrigger_1.Dig_7" ), 7 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_0" ), 0 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_1" ), 1 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_2" ), 2 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_3" ), 3 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_4" ), 4 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_5" ), 5 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_6" ), 6 ) )
		( SVTriggerDeviceParams( _T( "CameraTrigger.Dig_7" ), 7 ) )
	;

	m_AcquisitionDeviceParams = boost::assign::list_of< SVAcquisitionConstructParams >
		( SVAcquisitionConstructParams( _T( "Viper_Quad_1.Dig_0.Ch_0" ),  _T( "Viper_Quad_1.Dig_0" ), 0, 0, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Viper_Quad_1.Dig_1.Ch_0" ),  _T( "Viper_Quad_1.Dig_1" ), 0, 1, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Viper_Quad_1.Dig_2.Ch_0" ),  _T( "Viper_Quad_1.Dig_2" ), 0, 2, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Viper_Quad_1.Dig_3.Ch_0" ),  _T( "Viper_Quad_1.Dig_3" ), 0, 3, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Viper_RGB_1.Dig_0.Ch_All" ), _T( "Viper_RGB_1.Dig_0" ),  0, 0, 3, 3, SVLutInfo( 3, 1024, 1023, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_0.Ch_0" ),  _T( "File.Dig_0" ),  0, 0,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_1.Ch_0" ),  _T( "File.Dig_1" ),  0, 1,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_2.Ch_0" ),  _T( "File.Dig_2" ),  0, 2,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_3.Ch_0" ),  _T( "File.Dig_3" ),  0, 3,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_4.Ch_0" ),  _T( "File.Dig_4" ),  0, 4,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_5.Ch_0" ),  _T( "File.Dig_5" ),  0, 5,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_6.Ch_0" ),  _T( "File.Dig_6" ),  0, 6,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_7.Ch_0" ),  _T( "File.Dig_7" ),  0, 7,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_8.Ch_0" ),  _T( "File.Dig_8" ),  0, 8,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_9.Ch_0" ),  _T( "File.Dig_9" ),  0, 9,  1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_10.Ch_0" ), _T( "File.Dig_10" ), 0, 10, 1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_11.Ch_0" ), _T( "File.Dig_11" ), 0, 11, 1, 1 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_0.Ch_All" ),  _T( "File.Dig_0" ),  0, 0,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_1.Ch_All" ),  _T( "File.Dig_1" ),  0, 1,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_2.Ch_All" ),  _T( "File.Dig_2" ),  0, 2,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_3.Ch_All" ),  _T( "File.Dig_3" ),  0, 3,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_4.Ch_All" ),  _T( "File.Dig_4" ),  0, 4,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_5.Ch_All" ),  _T( "File.Dig_5" ),  0, 5,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_6.Ch_All" ),  _T( "File.Dig_6" ),  0, 6,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_7.Ch_All" ),  _T( "File.Dig_7" ),  0, 7,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_8.Ch_All" ),  _T( "File.Dig_8" ),  0, 8,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_9.Ch_All" ),  _T( "File.Dig_9" ),  0, 9,  3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_10.Ch_All" ), _T( "File.Dig_10" ), 0, 10, 3, 3 ) )
		( SVAcquisitionConstructParams( _T( "File.Dig_11.Ch_All" ), _T( "File.Dig_11" ), 0, 11, 3, 3 ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_0.Ch_0" ),  _T( "Matrox_1394.Dig_0" ),  0, 0,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_1.Ch_0" ),  _T( "Matrox_1394.Dig_1" ),  0, 1,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_2.Ch_0" ),  _T( "Matrox_1394.Dig_2" ),  0, 2,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_3.Ch_0" ),  _T( "Matrox_1394.Dig_3" ),  0, 3,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_4.Ch_0" ),  _T( "Matrox_1394.Dig_4" ),  0, 4,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_5.Ch_0" ),  _T( "Matrox_1394.Dig_5" ),  0, 5,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_6.Ch_0" ),  _T( "Matrox_1394.Dig_6" ),  0, 6,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_7.Ch_0" ),  _T( "Matrox_1394.Dig_7" ),  0, 7,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_8.Ch_0" ),  _T( "Matrox_1394.Dig_8" ),  0, 8,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_9.Ch_0" ),  _T( "Matrox_1394.Dig_9" ),  0, 9,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_10.Ch_0" ), _T( "Matrox_1394.Dig_10" ), 0, 10, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_11.Ch_0" ), _T( "Matrox_1394.Dig_11" ), 0, 11, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_0.Ch_All" ),  _T( "Matrox_1394.Dig_0" ),  0, 0,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_1.Ch_All" ),  _T( "Matrox_1394.Dig_1" ),  0, 1,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_2.Ch_All" ),  _T( "Matrox_1394.Dig_2" ),  0, 2,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_3.Ch_All" ),  _T( "Matrox_1394.Dig_3" ),  0, 3,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_4.Ch_All" ),  _T( "Matrox_1394.Dig_4" ),  0, 4,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_5.Ch_All" ),  _T( "Matrox_1394.Dig_5" ),  0, 5,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_6.Ch_All" ),  _T( "Matrox_1394.Dig_6" ),  0, 6,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_7.Ch_All" ),  _T( "Matrox_1394.Dig_7" ),  0, 7,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_8.Ch_All" ),  _T( "Matrox_1394.Dig_8" ),  0, 8,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_9.Ch_All" ),  _T( "Matrox_1394.Dig_9" ),  0, 9,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_10.Ch_All" ), _T( "Matrox_1394.Dig_10" ), 0, 10, 3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_1394.Dig_11.Ch_All" ), _T( "Matrox_1394.Dig_11" ), 0, 11, 3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_0.Ch_0" ),  _T( "Matrox_GIGE.Dig_0" ),  0, 0,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_1.Ch_0" ),  _T( "Matrox_GIGE.Dig_1" ),  0, 1,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_2.Ch_0" ),  _T( "Matrox_GIGE.Dig_2" ),  0, 2,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_3.Ch_0" ),  _T( "Matrox_GIGE.Dig_3" ),  0, 3,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_4.Ch_0" ),  _T( "Matrox_GIGE.Dig_4" ),  0, 4,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_5.Ch_0" ),  _T( "Matrox_GIGE.Dig_5" ),  0, 5,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_6.Ch_0" ),  _T( "Matrox_GIGE.Dig_6" ),  0, 6,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_7.Ch_0" ),  _T( "Matrox_GIGE.Dig_7" ),  0, 7,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_8.Ch_0" ),  _T( "Matrox_GIGE.Dig_8" ),  0, 8,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_9.Ch_0" ),  _T( "Matrox_GIGE.Dig_9" ),  0, 9,  1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_10.Ch_0" ), _T( "Matrox_GIGE.Dig_10" ), 0, 10, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_11.Ch_0" ), _T( "Matrox_GIGE.Dig_11" ), 0, 11, 1, 1, SVLutInfo( 1, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 1, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_0.Ch_All" ),  _T( "Matrox_GIGE.Dig_0" ),  0, 0,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_1.Ch_All" ),  _T( "Matrox_GIGE.Dig_1" ),  0, 1,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_2.Ch_All" ),  _T( "Matrox_GIGE.Dig_2" ),  0, 2,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_3.Ch_All" ),  _T( "Matrox_GIGE.Dig_3" ),  0, 3,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_4.Ch_All" ),  _T( "Matrox_GIGE.Dig_4" ),  0, 4,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_5.Ch_All" ),  _T( "Matrox_GIGE.Dig_5" ),  0, 5,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_6.Ch_All" ),  _T( "Matrox_GIGE.Dig_6" ),  0, 6,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_7.Ch_All" ),  _T( "Matrox_GIGE.Dig_7" ),  0, 7,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_8.Ch_All" ),  _T( "Matrox_GIGE.Dig_8" ),  0, 8,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_9.Ch_All" ),  _T( "Matrox_GIGE.Dig_9" ),  0, 9,  3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_10.Ch_All" ), _T( "Matrox_GIGE.Dig_10" ), 0, 10, 3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
		( SVAcquisitionConstructParams( _T( "Matrox_GIGE.Dig_11.Ch_All" ), _T( "Matrox_GIGE.Dig_11" ), 0, 11, 3, 3, SVLutInfo( 3, 255, 254, SVDefaultLutTransform( SVLutTransformOperationNormal() ) ), 3, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF ) )
	;
}

void SVHardwareManifest::Shutdown()
{
	if( !( m_TriggerDeviceParams.empty() ) )
	{
		m_TriggerDeviceParams.clear();
	}

	if( !( m_AcquisitionDeviceParams.empty() ) )
	{
		m_AcquisitionDeviceParams.clear();
	}
}

const SVTriggerDeviceParamsVector& SVHardwareManifest::GetTriggerDeviceParams() const
{
	return m_TriggerDeviceParams;
}

const SVAcquisitionConstructParamsVector& SVHardwareManifest::GetAcquisitionDeviceParams() const
{
	return m_AcquisitionDeviceParams;
}

SVIMTypeInfoStruct SVHardwareManifest::GetSVIMTypeInfo( SVIMProductEnum p_ID )
{
	SVIMTypeMap::const_iterator it = m_SVIMTypeMap.find(p_ID);
	if (it != m_SVIMTypeMap.end())
	{
		return it->second;
	}
	return SVIMTypeInfoStruct(false, 0, 0);
}

SVString SVHardwareManifest::BuildSoftwareTriggerDeviceName(int iDig)
{
	return SvUl_SF::Format("%s%s%d", SVIM_SOFTWARE_TRIGGER_SOURCE_STRING, SVIM_DIG_NAME_STRING, iDig);
}

SVString SVHardwareManifest::BuildAcquisitionTriggerDeviceName(int iDig)
{
	return SvUl_SF::Format("%s%s%d", SVIM_CAMERA_TRIGGER_SOURCE_STRING, SVIM_DIG_NAME_STRING, iDig);
}

SVString SVHardwareManifest::BuildIOBoardTriggerDeviceName(int iDig)
{
	return SvUl_SF::Format("%s%s%d", SVIM_TRIGGER_SOURCE_IO_BOARD_STRING, SVIM_DIG_NAME_STRING, iDig);
}

bool SVHardwareManifest::IsCompatible( SVIMProductEnum ConfigType, SVIMProductEnum ProductType )
{
	static SVIMProductCompatibilityList l_compatibleList = boost::assign::map_list_of<>
	(SVIM_PRODUCT_05, boost::assign::list_of(SVIM_PRODUCT_FULL))
	(SVIM_PRODUCT_RGB_MONO, boost::assign::list_of(SVIM_PRODUCT_RGB_COLOR))

	(SVIM_PRODUCT_D1, boost::assign::list_of(SVIM_PRODUCT_X2))
	(SVIM_PRODUCT_D2, boost::assign::list_of(SVIM_PRODUCT_X2))
	(SVIM_PRODUCT_D3, boost::assign::list_of(SVIM_PRODUCT_X2))
	(SVIM_PRODUCT_X1, boost::assign::list_of(SVIM_PRODUCT_X2))
	(SVIM_PRODUCT_X3, boost::assign::list_of(SVIM_PRODUCT_X2))

	(SVIM_PRODUCT_X2_GD1A_COLOR, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	(SVIM_PRODUCT_X2_GD2A_COLOR, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	(SVIM_PRODUCT_X2_GD4A_COLOR, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	(SVIM_PRODUCT_X2_GD8A_COLOR, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))

	(SVIM_PRODUCT_X2_GD1A, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	(SVIM_PRODUCT_X2_GD2A, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	(SVIM_PRODUCT_X2_GD4A, boost::assign::list_of(SVIM_PRODUCT_X2_GD8A))
	;

	bool l_bRet = false;

	if ( ProductType == ConfigType )
	{
		l_bRet = true;
	}
	else
	{
		SVIMProductCompatibilityList::const_iterator dstIt = l_compatibleList.find(ConfigType);
		if (dstIt != l_compatibleList.end())
		{
			const SVIMProductEnumSet& l_productSet = dstIt->second;
			SVIMProductEnumSet::const_iterator it = l_productSet.find(ProductType);
			l_bRet = (it != l_productSet.end());
		}
	}
	return l_bRet;
}

bool SVHardwareManifest::IsColorSystem(SVIMProductEnum p_ProductType)
{
	static SVIMProductEnumSet l_ColorList = boost::assign::list_of
		(SVIM_PRODUCT_RGB_COLOR)
		(SVIM_PRODUCT_D1_COLOR)
		(SVIM_PRODUCT_D2_COLOR)
		(SVIM_PRODUCT_D3_COLOR)
		(SVIM_PRODUCT_X1_COLOR)
		(SVIM_PRODUCT_X2_COLOR)
		(SVIM_PRODUCT_X3_COLOR)
		(SVIM_PRODUCT_X2_GD1A_COLOR)
		(SVIM_PRODUCT_X2_GD2A_COLOR)
		(SVIM_PRODUCT_X2_GD4A_COLOR)
		(SVIM_PRODUCT_X2_GD8A_COLOR)
		(SVIM_PRODUCT_X2_GD8A_NONIO_COLOR)
		;
	SVIMProductEnumSet::const_iterator it = l_ColorList.find(p_ProductType);
	return (it != l_ColorList.end());
}

bool SVHardwareManifest::IsMatroxGige(SVIMProductEnum p_ProductType)
{
	static SVIMProductEnumSet l_GigeList = boost::assign::list_of
	(SVIM_PRODUCT_X2_GD1A)
	(SVIM_PRODUCT_X2_GD1A_COLOR)
	(SVIM_PRODUCT_X2_GD2A)
	(SVIM_PRODUCT_X2_GD2A_COLOR)
	(SVIM_PRODUCT_X2_GD4A)
	(SVIM_PRODUCT_X2_GD4A_COLOR)
	(SVIM_PRODUCT_X2_GD8A)
	(SVIM_PRODUCT_X2_GD8A_COLOR)
	(SVIM_PRODUCT_X2_GD8A_NONIO)
	(SVIM_PRODUCT_X2_GD8A_NONIO_COLOR)
	;
	SVIMProductEnumSet::const_iterator it = l_GigeList.find(p_ProductType);
	return (it != l_GigeList.end());
}

bool SVHardwareManifest::IsDigitalSVIM(SVIMProductEnum p_ProductType)
{
	static SVIMProductEnumSet l_DigitalList = boost::assign::list_of
	(SVIM_PRODUCT_D1)
	(SVIM_PRODUCT_D1_COLOR)
	(SVIM_PRODUCT_D1_HUB)
	(SVIM_PRODUCT_D2)
	(SVIM_PRODUCT_D2_COLOR)
	(SVIM_PRODUCT_D2_HUB)
	(SVIM_PRODUCT_D3)
	(SVIM_PRODUCT_D3_COLOR)
	(SVIM_PRODUCT_D3_HUB)
	(SVIM_PRODUCT_X1)
	(SVIM_PRODUCT_X2)
	(SVIM_PRODUCT_X1_COLOR)
	(SVIM_PRODUCT_X1_HUB)
	(SVIM_PRODUCT_X2_COLOR)
	(SVIM_PRODUCT_X3)
	(SVIM_PRODUCT_X3_COLOR)
	(SVIM_PRODUCT_X2_GD1A)
	(SVIM_PRODUCT_X2_GD1A_COLOR)
	(SVIM_PRODUCT_X2_GD2A)
	(SVIM_PRODUCT_X2_GD2A_COLOR)
	(SVIM_PRODUCT_X2_GD4A)
	(SVIM_PRODUCT_X2_GD4A_COLOR)
	(SVIM_PRODUCT_X2_GD8A)
	(SVIM_PRODUCT_X2_GD8A_COLOR)
	(SVIM_PRODUCT_X2_GD8A_NONIO)
	(SVIM_PRODUCT_X2_GD8A_NONIO_COLOR)
	;
	SVIMProductEnumSet::const_iterator it = l_DigitalList.find(p_ProductType);
	return (it != l_DigitalList.end());
}

bool SVHardwareManifest::IsProductTypeRAID(SVIMProductEnum p_ProductType)
{
	static SVIMProductEnumSet l_RaidList = boost::assign::list_of
	(SVIM_PRODUCT_X1)
	(SVIM_PRODUCT_X1_HUB)
	(SVIM_PRODUCT_X1_COLOR)
	(SVIM_PRODUCT_X3)
	(SVIM_PRODUCT_X3_COLOR)
	(SVIM_PRODUCT_X2)
	(SVIM_PRODUCT_X2_COLOR)
	(SVIM_PRODUCT_X2_GD1A)
	(SVIM_PRODUCT_X2_GD1A_COLOR)
	(SVIM_PRODUCT_X2_GD2A)
	(SVIM_PRODUCT_X2_GD2A_COLOR)
	(SVIM_PRODUCT_X2_GD8A)
	(SVIM_PRODUCT_X2_GD8A_COLOR)
	(SVIM_PRODUCT_X2_GD8A_NONIO)
	(SVIM_PRODUCT_X2_GD8A_NONIO_COLOR)
	;
	SVIMProductEnumSet::const_iterator it = l_RaidList.find(p_ProductType);
	return (it != l_RaidList.end());
}

bool SVHardwareManifest::IsNonIOSVIM(SVIMProductEnum p_ProductType)
{
	static SVIMProductEnumSet l_NonIOList = boost::assign::list_of
	(SVIM_PRODUCT_X2_GD8A_NONIO)
	(SVIM_PRODUCT_X2_GD8A_NONIO_COLOR)
	;
	SVIMProductEnumSet::const_iterator it = l_NonIOList.find(p_ProductType);
	return (it != l_NonIOList.end());
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVHardwareManifest.cpp_v  $
 * 
 *    Rev 1.2   23 Oct 2014 13:27:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  958
 * SCR Title:  Remove Legacy SVIM Types from Available Systems List
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   set the analog/digital models to be not supported
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Apr 2014 10:45:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  899
 * SCR Title:  Remove "SVIM Non I/O" selection from the configuration assistant
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed the System types SVIM_PRODUCT_X2_GD8A_NONIO and 
 * SVIM_PRODUCT_X2_GD8A_NONIO_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:45:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2013 10:46:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added BuildAcquisitionTriggerDeviceName method
 * Added  IsNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2013 16:04:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Moved hardware tables and look-up fucntionality to the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2011 15:55:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with extra consistancy checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Mar 2011 07:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2010 08:05:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2010 14:13:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
