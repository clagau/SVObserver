//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLine
//* .File Name       : $Workfile:   SVLine.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:12:46  $
//******************************************************************************

#pragma once

#include "SVDataBuffer.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVGlobal.h"

class SVAnalyzerClass;
class SVImageClass;
class SVToolClass;

class SVDataArrayClass
{
public:
	SVDataArrayClass( DWORD DataType = SV_BYTE );

	~SVDataArrayClass()	{ FreeExtra(); };

	virtual BOOL SetSize( int NewSize = -1, int NewGrow = 20, DWORD NewDataType = 0 );
	virtual void RemoveAll() { SetSize( 0 ); };
	virtual void FreeExtra() { SetSize( -1 ); };

	virtual BOOL SetAt( int Index, void* PNewData );

public:
	virtual DWORD	GetData();
	virtual int		GetDataSize( DWORD NewDataType = 0L );
	virtual int		GetSize();

	virtual DWORD	GetAt( int Index, void* PReturnedData = NULL );

protected:

	BYTE*	pData;
	int		size;
	int		oldSize;
	int		grow;
	DWORD	type;
};


class SVLineClass : public SVObjectAppClass
{
	SV_DECLARE_CLASS( SVLineClass );

public:
	typedef std::vector< unsigned long > SVResultVector;

	SVLineClass();   
	virtual ~SVLineClass();

	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL Destroy();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual void *GetDataBuffer();

	virtual BOOL ReadLine();
	virtual BOOL ReadData( double dProfileHeight = 1.0 );

	virtual HRESULT GetHeadPoint( POINT &p_oPoint );
	virtual HRESULT GetTailPoint( POINT &p_oPoint );

	virtual void Persist(SVObjectWriter& rWriter);

	SVImageExtentClass m_svLineExtent;

	long mlLinePixelLength;

	long mlLineArrayLength;

	SVDataArrayClass mLineArray;
	SVDataArrayClass mNormalizedArray;

	SVResultVector mResultArray;

	BOOL m_bDisplayGraph;

	DWORD m_dwLineArrayDataType;

	long m_lPixelDepth;
	
	//*
	DWORD m_dwMaxThreshold;
	DWORD m_dwMinThreshold;
	DWORD m_dwColorNumber;
	DWORD m_dwDisplayedMaxThresholdBarValue;
	DWORD m_dwDisplayedMinThresholdBarValue;

	SVValueBaseNormalizerClass	m_svNormalizer;
	//*/

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

};

