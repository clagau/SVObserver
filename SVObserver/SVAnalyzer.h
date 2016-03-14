//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerClass
//* .File Name       : $Workfile:   SVAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 10:09:12  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVANALYZER_H
#define SVANALYZER_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************
#include "SVTaskObjectList.h"

//******************************************************************************
//* PRE-DECLARATION(S):
//******************************************************************************

class SVImageClass;
class SVRefExtentListClass;
class SVResultClass;
class SVToolClass;
class SVIPDoc;

//******************************************************************************
//* CONSTANT(S):
//******************************************************************************

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Base Class of module SVAnalyzer
// -----------------------------------------------------------------------------
// .Description : 
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
//	:19.01.1999 RO			Supplements for 3.0, now derived from 
//							SVTaskObjectClass instead of SVObjectClass
////////////////////////////////////////////////////////////////////////////////
class SVAnalyzerClass : public SVTaskObjectListClass
{
	SV_DECLARE_CLASS( SVAnalyzerClass );

protected:

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL, 
		int StringResourceID = IDS_CLASSNAME_SVANALYZER 
	);

private:
	void init();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVAnalyzerClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual void DisconnectImages() {};

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
public:
	void MakeDirty();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	virtual SVResultClass* GetResultObject();
	
// Sri. 04-12-00
// used in pattern and Blob Analyzers.
	virtual BOOL IsPtOverResult( CPoint point ){ return FALSE;};
	virtual	void DisplayAnalyzerResult( void){};
// End. Sri

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR createAllObjectsFromChild( SVObjectClass* pChildObject ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	BOOL	isDirty;
	SVResultClass*				pAnalyzerResult;

};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Base Class for all image analyzers
// -----------------------------------------------------------------------------
// .Description : 
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
//  :02.02.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVImageAnalyzerClass : public SVAnalyzerClass
{
	SV_DECLARE_CLASS( SVImageAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVImageAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVImageAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVIMAGEANALYZER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVImageAnalyzerClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

private:
	void init();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
/*- GetInputPixelDepth () --------------------------------------------------*/
/*- If successful, this function will return the pixel depth of the         */
/*- SVImageAnalyzerClass input image.  If unsuccessful an error condition   */
/*- will be returned. ------------------------------------------------------*/
    unsigned long       GetInputPixelDepth ();

	SVImageClass*		getInputImage();
	
//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
public:

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	SVInObjectInfoStruct		inputImageObjectInfo;

};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVANALYZER_H

