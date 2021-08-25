//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
#pragma once
#pragma region Includes
#include "Analyzer.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes


namespace SvIe
{
class SVImageClass;
}

namespace SvAo
{

class ImageAnalyzer : public Analyzer
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	explicit ImageAnalyzer( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	ImageAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGEANALYZER );
	virtual ~ImageAnalyzer();
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

/*- GetInputPixelDepth () --------------------------------------------------*/
/*- If successful, this function will return the pixel depth of the         */
/*- ImageAnalyzer input image.  If unsuccessful an error condition   */
/*- will be returned. ------------------------------------------------------*/
    unsigned long       GetInputPixelDepth ();
	SvIe::SVImageClass* getInputImage(bool bRunMode = false);


protected:
	SvOl::InputObject m_inputImage;

private:
	void init();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const;
};

} //namespace SvAo
