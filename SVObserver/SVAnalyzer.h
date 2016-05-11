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

#pragma once
#pragma region Includes
#include "SVTaskObjectList.h"
#pragma endregion Includes

class SVImageClass;
class SVResultClass;
class SVToolClass;
class SVIPDoc;

class SVAnalyzerClass : public SVTaskObjectListClass
{
	SV_DECLARE_CLASS( SVAnalyzerClass );
public:
	SVAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVAnalyzerClass( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = nullptr, 
		int StringResourceID = IDS_CLASSNAME_SVANALYZER 
	);
	virtual ~SVAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual void DisconnectImages() {};

	void MakeDirty();

	virtual SVResultClass* GetResultObject();
	
// Sri. 04-12-00
// used in pattern and Blob Analyzers.
	virtual BOOL IsPtOverResult( CPoint point ){ return false;}
	virtual	void DisplayAnalyzerResult(){};
// End. Sri

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR createAllObjectsFromChild( SVObjectClass* pChildObject ) override;

protected:
	BOOL isDirty;
	SVResultClass* pAnalyzerResult;

private:
	void init();
};

class SVImageAnalyzerClass : public SVAnalyzerClass
{
	SV_DECLARE_CLASS( SVImageAnalyzerClass );

public:
	SVImageAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVImageAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEANALYZER );
	virtual ~SVImageAnalyzerClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

/*- GetInputPixelDepth () --------------------------------------------------*/
/*- If successful, this function will return the pixel depth of the         */
/*- SVImageAnalyzerClass input image.  If unsuccessful an error condition   */
/*- will be returned. ------------------------------------------------------*/
    unsigned long       GetInputPixelDepth ();

	SVImageClass*		getInputImage();
	

protected:
	SVInObjectInfoStruct		inputImageObjectInfo;

private:
	void init();
};

