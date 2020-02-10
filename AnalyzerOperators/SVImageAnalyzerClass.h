//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
#pragma once
#pragma region Includes
#include "SVAnalyzer.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}

namespace SvAo
{

class SVImageAnalyzerClass : public SVAnalyzerClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVImageAnalyzerClass );

public:
	explicit SVImageAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVImageAnalyzerClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEANALYZER );
	virtual ~SVImageAnalyzerClass();
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

/*- GetInputPixelDepth () --------------------------------------------------*/
/*- If successful, this function will return the pixel depth of the         */
/*- SVImageAnalyzerClass input image.  If unsuccessful an error condition   */
/*- will be returned. ------------------------------------------------------*/
    unsigned long       GetInputPixelDepth ();


protected:
	SvIe::SVImageClass* getInputImage(bool bRunMode = false);
	SvOl::SVInObjectInfoStruct m_inputImageObjectInfo;

private:
	void init();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
};

} //namespace SvAo
