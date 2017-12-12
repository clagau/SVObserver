//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class RingBufferTool :	public SVToolClass
{
	SV_DECLARE_CLASS (RingBufferTool);

#pragma region Constructor
public:
	RingBufferTool (SVObjectClass*	pOwner = nullptr, int stringResourceID = IDS_CLASSNAME_RINGBUFFER_TOOL );

	virtual ~RingBufferTool(void);
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual SVStringValueObjectClass* GetInputImageNames() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVImageClass* getInputImage();
	SVImageClass* getOutputImage(int index); 
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//************************************
	/// Initialized the class.
	//************************************
	void LocalInitialize();

	//************************************
	/// Build the input object list for this tool.
	//************************************
	void BuildInputObjectList();

	//************************************
	/// Build the embedded object list for this tool.
	//************************************
	void BuildEmbeddedObjectList();

	//************************************
	/// Copy the image from ringbuffer to a output image.
	/// \param outputIndex [in] the index of the output image.
	/// \param imageIndex [in] the required index of the image in the ringbuffer.
	/// \param maxIndexPos [in] the maximum index which is valid in the ringbuffer. If ringbuffer full, maxIndexPos == ringBufferDepth -1
	/// \param ringBufferDepth [in] the depth of the ringbuffer.
	/// \returns int if the set bit to 1 if outputIndex is set (outputIndex == 1 => 1, outputIndex == 2 => 2, outputIndex == 3 => 4), else the return value is 0.
	//************************************
	int SetOutputImage( int outputIndex, int imageIndex, int maxIndexPos, long ringBufferDepth );

	//************************************
	/// Set the tool ROI extent to the full size of the input image.
	//************************************
	void SetToolROIExtentToFullInputImage ();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	// Source Image Name - embedded
	SVStringValueObjectClass m_SourceImageNames;
	// Output Image - embedded
	SVImageClass m_OutputImages[SvDef::cRingBufferNumberOutputImages];

	// Source Image - input
	SVInObjectInfoStruct m_InputImageObjectInfo;

	//embedded parameter
	SVLongValueObjectClass m_BufferDepth;
	LinkedValue m_ImageIndexManager[SvDef::cRingBufferNumberOutputImages];
	SVLongValueObjectClass m_FlagOfOutputImage;

	//ringbuffer
	std::vector<SVImageBufferHandlePtr> m_ringBuffer;
	bool m_isBufferFull;
	int m_nextBufferPos;
#pragma endregion Member Variables
};

