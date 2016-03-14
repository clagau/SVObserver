//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzerResult
//* .File Name       : $Workfile:   SVOCVAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:30  $
//******************************************************************************

#ifndef SVOCVANALYZERESULTCLASS_H
#define SVOCVANALYZERESULTCLASS_H

#include "SVSystemLibrary/SVLockableClass.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVOCVBlobRecord.h"
#include "SVResult.h"

#define  OCV_MAX_ENTREES       10000  // Max number of lines allowed in the match file

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCVAnalyzeResultClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVOCVAnalyzeResultClass : public SVResultClass
{
protected:
	SV_DECLARE_CLASS( SVOCVAnalyzeResultClass )

public:
	SVOCVAnalyzeResultClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZERESULT );

	~SVOCVAnalyzeResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual CRect Draw( HDC DC, CRect R );
	virtual BOOL OnValidate();

	void GetOCVResultString( CString & csResult );
	void WriteToArchive(CFile & fileArchive);
	
	BOOL GenerateFontModel();
	HRESULT LoadMatchString();
	void HideResults();

	BOOL BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	int CheckStringInTable(CString MatchString);
	BOOL UpdateResultFigure( long p_lIndex );

	SVImageClass* getInputImage();

	virtual HRESULT ResetObject();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray );

private:
	void clearAll();

//******************************************************************************
// Attribute(s):
//******************************************************************************
public:
	// Results
	SVOCVCharacterResultArray arrayOCVCharacterResults;

protected:

	long     m_lTotalBytes;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	int      m_nTotalCount;
	char     *m_pDataArr[OCV_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;

	SvSyl::SVLockableClass		lock;

	// Input: Image
	SVInObjectInfoStruct inputImageObjectInfo;
	long m_lCurrentFoundStringLength;

public:
	// *** // ***
	// New value objects instead of parameterized string value object
	SVBoolValueObjectClass		m_bvoUseMatchFile;
	SVFileNameValueObjectClass	m_fnvoFontFileName;
	SVFileNameValueObjectClass	m_fnvoConstraintsFileName;
	SVFileNameValueObjectClass	m_fnvoControlsFileName;
	SVFileNameValueObjectClass	m_fnvoMatchStringFileName;
	SVBoolValueObjectClass		m_bvoPerformOCR;
	// *** // ***

	// Output
	SVStringValueObjectClass	m_svoMatchString;
	SVStringValueObjectClass	m_svoFoundString;
	SVLongValueObjectClass		m_lvoMatchLineNumber;

	// Exposing OCV Match Scores
	SVDoubleValueObjectClass	m_dvoHighestMatchScore;
	SVDoubleValueObjectClass	m_dvoLowestMatchScore;
	SVDoubleValueObjectClass	m_dvoAverageMatchScore;

	//	MIL object ids used in the OCR/OCV operation
	SVMatroxOcr m_milFontID;
	SVMatroxOcrResult m_milResultID;
	long m_lFontStringLength;
	long m_lFontStringLengthMax;
	long m_lMatchStringLength;

private:
	bool m_bHasLicenseError;
};

#endif //#ifndef SVOCVANALYZERESULTCLASS_H

