//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionSource
//* .File Name       : $Workfile:   SVAcquisitionSource.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:36  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVFileNameArrayClass.h"

enum { AcNone, AcCamera, AcFile };

class SVAcquisitionSourceClass
{
public:
	SVAcquisitionSourceClass();
	SVAcquisitionSourceClass( const SVAcquisitionSourceClass& svASC );
	SVAcquisitionSourceClass(   int iAcqType, 
	                            int iAcqTrigType, 
	                            const SVFileNameArrayClass& svFileNameArray, 
	                            const SVImageInfoClass& svImageInfo, 
	                            const SVLightReference& svLightRefArray );
	SVAcquisitionSourceClass( int iAcqType, 
	                          int iAcqTrigType, 
	                          LPCTSTR szFileNameList, 
	                          const SVImageInfoClass& svImageInfo, 
	                          const SVLightReference& svLightRefArray );

	~SVAcquisitionSourceClass();

	const SVAcquisitionSourceClass& operator=(const SVAcquisitionSourceClass& svASC);

	SVFileNameArrayClass GetFileNameArray();
	
	BOOL SetFileNameArray( LPCTSTR szFileNameList );
	BOOL SetFileNameArray( const SVFileNameArrayClass& svFileNameArray );

public:
	int AcquisitionType;
	int AcquisitionTriggerType;
	SVImageInfoClass AcquisitionImageInfo;
	SVLightReference AcquisitionLightReferenceArray;

private:
	SVFileNameArrayClass msvFileNameArray;

};

class SVAcquisitionSourceListClass : public SVVector< SVAcquisitionSourceClass* >
{
public:
	SVAcquisitionSourceListClass();
	SVAcquisitionSourceListClass(const SVAcquisitionSourceListClass& svASLC );

	~SVAcquisitionSourceListClass();

	const SVAcquisitionSourceListClass& operator=(const SVAcquisitionSourceListClass& svASLC);

	void DeleteAll();

};


