//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVTool.h"  // SVToolClass
#include "ObjectInterfaces\IRingBufferTool.h"
#include "LinkedValue.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class RingBufferTool :
	public SVToolClass
	, virtual public SvOi::IRingBufferTool
{
	SV_DECLARE_CLASS (RingBufferTool);

#pragma region Constructor
public:
	RingBufferTool (	BOOL	bCreateDefaultTaskList = FALSE, 
							SVObjectClass*	pOwner = nullptr, 
							int	stringResourceID = IDS_CLASSNAME_RINGBUFFER_TOOL );

	virtual ~RingBufferTool(void);
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL	CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	virtual HRESULT ResetObject() override;
	virtual BOOL OnValidate() override;

	virtual void ResetPrivateInputInterface() override;

	SVImageClass* getInputImage();
	SVImageClass* getOutputImage(int index); 

#pragma region virtual methods (IRingBufferTool) 
	virtual HRESULT setRingDepth(SVString value) override;
	virtual SVString getRingBufferDepthString() const override;
	virtual HRESULT setImageIndex(int indexNumber, SVString value) override;
	virtual SVString getImageIndex(int indexNumber) const override;
#pragma endregion virtual methods (IRingBufferTool) 
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;
	
	virtual BOOL	onRun( SVRunStatusClass& RRunStatus ) override;

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext ) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//************************************
	/// Initialized the class.
	//************************************
	void	LocalInitialize();

	//************************************
	/// Build the input object list for this tool.
	//************************************
	void	BuildInputObjectList();

	//************************************
	/// Build the embedded object list for this tool.
	//************************************
	void	BuildEmbeddedObjectList();

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
	static const int m_numberOfOutputImages = 2;
	static const int m_defaultRingBufferDepth = 8;
	static const int m_defaultIndex1Value = 1;
	static const int m_defaultIndex2Value = 2;
	static const VARTYPE m_varType_imageIndex = VT_I4;

	// Source Image Name - embedded
	SVStaticStringValueObjectClass m_svSourceImageName;
	// Output Image - embedded
	SVImageClass	m_OutputImages[m_numberOfOutputImages];

	// Source Image - input
	SVInObjectInfoStruct	m_InputImageObjectInfo;

	//embedded parameter
	SVLongValueObjectClass m_BufferDepth; 
	LinkedValue m_ImageIndexManagers[m_numberOfOutputImages];
	SVLongValueObjectClass	m_FlagOfOutputImage;

	//ringbuffer
	std::vector<SVSmartHandlePointer> m_ringBuffer;
	bool m_isBufferFull;
	int m_nextBufferPos;
#pragma endregion Member Variables
};

