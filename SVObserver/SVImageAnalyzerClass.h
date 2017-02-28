//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
#pragma once
#pragma region Includes
#include "SVAnalyzer.h"
#pragma endregion Includes

class SVImageClass;

class SVImageAnalyzerClass : public SVAnalyzerClass
{
	SV_DECLARE_CLASS( SVImageAnalyzerClass );

public:
	SVImageAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVImageAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEANALYZER );
	virtual ~SVImageAnalyzerClass();
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

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
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
};