//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HistogramAnalyzer
//* .File Name       : $Workfile:   HistogramAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "HistogramAnalyzer.h"
#include "Definitions/SVImageFormatEnum.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVResultLong.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxGraphicsInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double histogram_width = 512;
const double histogram_height = 512;
#pragma endregion Declarations

namespace 
{
	namespace mil // should really go into it's own header file?
	{
		namespace color
		{
			inline double white()		{ return SVMatroxGraphicsInterface::CreateRGB888(255,255,255); }
			inline double black()		{ return SVMatroxGraphicsInterface::CreateRGB888(  0,  0,  0); }
			inline double color(unsigned char val) { return SVMatroxGraphicsInterface::CreateRGB888(val,val,val); }
		}

		class Context // RAII wrapper around MIL graphic context
		{
		public:
			explicit Context() : m_handle()
			{
				SVMatroxGraphicsInterface::Create( m_handle );
			}

			Context(const Context& rRhs) = delete;

			~Context()
			{
				SVMatroxGraphicsInterface::Destroy( m_handle );
			}

			Context& operator=(const Context& rRhs) = delete;

			void Clear( SVMatroxBuffer& p_rBuffer )
			{
				SVMatroxGraphicsInterface::Clear( m_handle, p_rBuffer );
			}

			void rectFill( SVMatroxBuffer& p_rBuffer, const RECT& rRect )
			{
				SVMatroxGraphicsInterface::RectangleFill( m_handle, p_rBuffer, rRect);
			}

			void setForeColor( double col )
			{
				SVMatroxGraphicsInterface::Color( m_handle, col );
			}

			void setBackColor( double col )
			{
				SVMatroxGraphicsInterface::BackColor( m_handle, col );
			}

		protected:
			__int64 m_handle = M_NULL;
		};
	}

	class MILCanvas: public mil::Context
	{
	public:
		explicit MILCanvas( const SVMatroxBuffer& p_rBuffer )
		:	Context(), m_buffer( p_rBuffer ) 
		{
			setForeColor(mil::color::white());
			Clear( m_buffer );
			setBackColor(mil::color::white());
			setForeColor(mil::color::black());
		}

		void Draw(const RECT& rRect, int)
		{
			rectFill( m_buffer, rRect );
		}

	private:
		SVMatroxBuffer m_buffer;
	};
}
namespace helper
{
	template<typename ValueObject>
	typename ValueObject::value_type GetValue(const ValueObject & vo)
	{
		typename ValueObject::value_type val;
		vo.GetValue(val);
		return val;
	}
}

///////////////////////////////////////////////////////////
// JH Error Codes used by this program
///////////////////////////////////////////////////////////
#define JH_ErrorBase			14000
#define Err_StdDevCalc			(JH_ErrorBase+101)
#define Err_SetVariance			(JH_ErrorBase+102)
#define Err_NegVariance			(JH_ErrorBase+103)
#define Err_SetStdDev			(JH_ErrorBase+104)
#define Err_SetMean				(JH_ErrorBase+105)
#define Err_SetStart			(JH_ErrorBase+106)
#define Err_SetEnd				(JH_ErrorBase+107)
#define Err_SetCount			(JH_ErrorBase+108)
#define Err_SetMax				(JH_ErrorBase+109)
#define Err_SetMin				(JH_ErrorBase+110)
#define Err_SetSize				(JH_ErrorBase+111)
#define Err_NewResult			(JH_ErrorBase+112)
#define Err_ResultEmpty			(JH_ErrorBase+113)
#define Err_ArrayEmpty			(JH_ErrorBase+114)
#define Err_CreateStruct		(JH_ErrorBase+115)
#define Err_GetInputImage		(JH_ErrorBase+116)
#define Err_MilGetHandle		(JH_ErrorBase+117)
#define Err_MilHistogram		(JH_ErrorBase+118)
#define Err_MilGetHistogram		(JH_ErrorBase+119)
#define Err_SetArrayValues		(JH_ErrorBase+120)
#define Err_SetImageInfo		(JH_ErrorBase+121)
#define Err_GetImageHandle		(JH_ErrorBase+122)
#define Err_PaintHistogram		(JH_ErrorBase+123)
#define Err_CreateImage			(JH_ErrorBase+124)
#define Err_SetScale			(JH_ErrorBase+125)
#define Err_SetHighPeak			(JH_ErrorBase+126)
#define Err_SetLowPeak			(JH_ErrorBase+127)
#define Err_SetValley			(JH_ErrorBase+128)
#define Err_SetAccumulate		(JH_ErrorBase+129)
#define Err_SetDynaHeight		(JH_ErrorBase+130)
#define Err_SetDefaultPeak		(JH_ErrorBase+131)
#define Err_SetThreshold		(JH_ErrorBase+132)
#define Err_SetMinPeak			(JH_ErrorBase+133)
#define Err_SetFixedHeight		(JH_ErrorBase+134)
#define Err_SetValleyLow		(JH_ErrorBase+135)
#define Err_SetValleyHigh		(JH_ErrorBase+136)
#define Err_SetValleyDefault	(JH_ErrorBase+137)
#define Err_GetResult			(JH_ErrorBase+138)

SV_IMPLEMENT_CLASS( HistogramAnalyzer, SvPb::HistogramAnalyzerClassId);

HistogramAnalyzer::HistogramAnalyzer( LPCSTR ObjectName )
						  :ImageAnalyzer( ObjectName )
{
	init();
}

HistogramAnalyzer::HistogramAnalyzer( SVObjectClass* POwner, int StringResourceID )
		:ImageAnalyzer( POwner, StringResourceID ) 
{
	init();
}

struct fail_to_create: public std::runtime_error
{
	fail_to_create(): std::runtime_error("failed to create result") {}
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class HistogramAnalyzer
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void HistogramAnalyzer::init()
{
	// init () should instantiate any dynamic children and 
	//   add them to the task object list.  
	// Identify output type.
	// Identify input type needs.
	// Register any embedded objects -- all embedded objects are
	//   outputs by default.
	// Set embedded defaults.
	// Set local defaults.
	// 

	// Default taskObjectList items:

	
		// Identify our output type
		m_ObjectTypeInfo.m_SubType = SvPb::HistogramAnalyzerObjectType;

		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvHistogramValue, 
			SvPb::HistogramValueEId,
			IDS_OBJECTNAME_HISTOGRAM,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMeanValue,
			SvPb::HistogramMeanEId,
			IDS_OBJECTNAME_HISTOGRAM_MEAN,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvStdDevValue,
			SvPb::HistogramStdDevEId,
			IDS_OBJECTNAME_HISTOGRAM_STDDEV,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeStartValue,
			SvPb::HistogramRangeStartEId,
			IDS_OBJECTNAME_HISTOGRAM_RANGESTART,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeEndValue,
			SvPb::HistogramRangeEndEId,
			IDS_OBJECTNAME_HISTOGRAM_RANGEEND,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMaxPixelValue,
			SvPb::HistogramMaxPixelEId,
			IDS_OBJECTNAME_HISTOGRAM_MAXPIXEL,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPixelValue,
			SvPb::HistogramMinPixelEId,
			IDS_OBJECTNAME_HISTOGRAM_MINPIXEL,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPixelCountValue,
			SvPb::HistogramPixelCountEId,
			IDS_OBJECTNAME_HISTOGRAM_PIXELCOUNT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeSizeValue,
			SvPb::HistogramRangeSizeEId,
			IDS_OBJECTNAME_HISTOGRAM_RANGESIZE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvScaleValue,
			SvPb::HistogramScaleEId,
			IDS_OBJECTNAME_HISTOGRAM_SCALE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvHighPeak,
			SvPb::HistogramHighPeakEId,
			IDS_OBJECTNAME_HISTOGRAM_HIGHPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvLowPeak,
			SvPb::HistogramLowPeakEId,
			IDS_OBJECTNAME_HISTOGRAM_LOWPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValley,
			SvPb::HistogramValleyEId,
			IDS_OBJECTNAME_HISTOGRAM_VALLEY,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPeakThreshold,
			SvPb::HistogramPeakThresholdEId,
			IDS_OBJECTNAME_HISTOGRAM_PEAKTHRESHOLD,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDefaultPeak,
			SvPb::HistogramDefaultPeakEId,
			IDS_OBJECTNAME_HISTOGRAM_DEFAULTPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPeakHeight,
			SvPb::HistogramMinPeakHeightEId,
			IDS_OBJECTNAME_HISTOGRAM_MINPEAKHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvAccumulateCounts,
			SvPb::HistogramAccumulateEId,
			IDS_OBJECTNAME_HISTOGRAM_ACCUMULATE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyLowerBound,
			SvPb::HistogramValleyLowerBoundEId,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYLOW,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyUpperBound,
			SvPb::HistogramValleyUpperBoundEId,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYHIGH,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyDefault,
			SvPb::HistogramValleyDefaultEId,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYDEFAULT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDynamicHeight,
			SvPb::HistogramDynamicHeightEId,
			IDS_OBJECTNAME_HISTOGRAM_DYNAHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvFixedHeightValue,
			SvPb::HistogramFixedHeightValueEId,
			IDS_OBJECTNAME_HISTOGRAM_FIXEDHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedImage(
			&m_histogramImage,
			SvPb::HistogramImageEId,
			IDS_OBJECTNAME_HISTOGRAMIMAGE
			);

		// Set Embedded defaults
		msvHistogramValue.SetDefaultValue(0, true);
		msvHistogramValue.setSaveValueFlag(false);
		msvStdDevValue.SetDefaultValue(0.0, true);
		msvStdDevValue.setSaveValueFlag(false);
		msvMeanValue.SetDefaultValue(0.0, true);
		msvMeanValue.setSaveValueFlag(false);
		msvFixedHeightValue.SetDefaultValue(10.0, true);
		msvRangeStartValue.SetDefaultValue(0, true);
		msvRangeEndValue.SetDefaultValue(255, true);

		msvMaxPixelValue.SetDefaultValue(0, true);
		msvMaxPixelValue.setSaveValueFlag(false);
		msvMinPixelValue.SetDefaultValue(0, true);
		msvMinPixelValue.setSaveValueFlag(false);
		msvPixelCountValue.SetDefaultValue(0, true);
		msvPixelCountValue.setSaveValueFlag(false);
		msvRangeSizeValue.SetDefaultValue(0, true);
		msvRangeSizeValue.setSaveValueFlag(false);
		msvScaleValue.SetDefaultValue(0, true);
		msvHighPeak.SetDefaultValue(0, true);
		msvHighPeak.setSaveValueFlag(false);
		msvLowPeak.SetDefaultValue(0, true);
		msvLowPeak.setSaveValueFlag(false);
		msvValley.SetDefaultValue(0, true);
		msvValley.setSaveValueFlag(false);
		msvPeakThreshold.SetDefaultValue(10, true);
		msvDefaultPeak.SetDefaultValue(0, true);
		msvMinPeakHeight.SetDefaultValue(0.5, true);
		msvValleyLowerBound.SetDefaultValue(0, true);
		msvValleyUpperBound.SetDefaultValue(255, true);
		msvValleyDefault.SetDefaultValue(0, true);

		msvDynamicHeight.SetDefaultValue( BOOL(false), true);
		msvAccumulateCounts.SetDefaultValue( BOOL(false), true);

		m_histogramImage.InitializeImage(SvPb::SVImageTypeEnum::SVImageTypeIndependent);

		m_rangeStart = 0;
		m_rangeEnd = 255;
		m_scale = 0;

		typedef std::pair<SvPb::EmbeddedIdEnum, DWORD> Param;
		try
		{
			constexpr std::array<Param, 3> cResultParameters
			{
				std::make_pair(SvPb::HistogramHighPeakEId, IDS_CLASSNAME_HISTOGRAMANALYZER_FIRSTPEAKRESULT),
				std::make_pair(SvPb::HistogramLowPeakEId, IDS_CLASSNAME_HISTOGRAMANALYZER_SECONDPEAKRESULT),
				std::make_pair(SvPb::HistogramValleyEId, IDS_CLASSNAME_HISTOGRAMANALYZER_VALLEYRESULT)
			};

			for (const auto& rResultParam : cResultParameters)
			{
				AddResult(rResultParam);
			}
		}
		catch (const fail_to_create &)
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			
		}
}

void HistogramAnalyzer::AddResult(const std::pair<SvPb::EmbeddedIdEnum, DWORD>& p)
{
	SvIe::SVClassInfoStruct l_resultInfo;
	SvDef::SVObjectTypeInfoStruct l_ifceInfo;

	l_ifceInfo.m_EmbeddedID = p.first;
	l_resultInfo.m_DesiredInputVector.push_back( l_ifceInfo );
	l_resultInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	l_resultInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVResultLongObjectType;
	l_resultInfo.m_ClassId = SvPb::LongResultClassId;
	l_resultInfo.m_ClassName = SvUl::LoadStdString(p.second);

	SvOp::SVLongResult* pAnalyzerResult = dynamic_cast<SvOp::SVLongResult*> (l_resultInfo.Construct());

	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
}

bool HistogramAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (M_NULL == m_HistResultID)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	if (0 == msvplHistValues.size())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

HistogramAnalyzer::~HistogramAnalyzer()
{
	HistogramAnalyzer::CloseObject();
}

bool HistogramAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	int LastError{0};

    while (1)
    {
        if (!ImageAnalyzer::CreateObject(rCreateStructure) )
        {
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			LastError = E_FAIL;
			break;
        }

		SvIe::SVImageClass* pImage = getInputImage();
        if (!pImage)
        {
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			LastError = E_FAIL;
			break;
        }

        msvlHistValueArraySize = 1 << (pImage->getPixelDepth() & SVBufferSize);
        msvplHistValues.resize( msvlHistValueArraySize );

        if (msvplHistValues.size() == 0)
        {
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			LastError = E_FAIL;
			break;
        }

        for( int i = 0; i < msvlHistValueArraySize; i++ )
           msvplHistValues [i] = 0L;

		SvIe::SVDataBufferInfoClass svData;

		svData.Length = msvlHistValueArraySize;
		svData.Type = SvIe::SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_HistResultID;
		if ( S_OK == SvIe::SVImageProcessingClass::CreateDataBuffer( &svData )  )
		{
			m_HistResultID = svData.HBuffer.milResult;
		}

		if (M_NULL == m_HistResultID)
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			LastError = E_FAIL;
			break;
		}

		msvRangeStartValue.GetValue(m_rangeStart);
		msvRangeEndValue.GetValue(m_rangeEnd);
		msvScaleValue.GetValue(m_scale);
		msvPeakThreshold.GetValue(m_threshold);
		msvDefaultPeak.GetValue(m_default);
		msvMinPeakHeight.GetValue(m_minHeight);
		msvAccumulateCounts.GetValue(m_accumulate);
		msvValleyLowerBound.GetValue(m_lowValley);
		msvValleyUpperBound.GetValue(m_highValley);
		msvValleyDefault.GetValue(m_valleyDefault);
		msvDynamicHeight.GetValue(m_dynamicHeight);
		msvFixedHeightValue.GetValue(m_fixedHeight);

		if (S_OK != createHistogramImage())
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams));
			LastError = E_FAIL;
			break;
		}

		break;
    }

    if( 0 !=LastError)
    {
        m_isCreated = false;
    }
    else
	{
        m_isCreated = true;
	}

	const UINT cAttributes = SvPb::audittrail | SvPb::remotelySetable | SvPb::setableOnline;
	msvHistogramValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvStdDevValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvMeanValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvRangeStartValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvRangeEndValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvMaxPixelValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvMinPixelValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvPixelCountValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvRangeSizeValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvScaleValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvHighPeak.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvLowPeak.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvValley.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	msvPeakThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvDefaultPeak.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvAccumulateCounts.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvMinPeakHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvValleyLowerBound.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvValleyUpperBound.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvValleyDefault.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvDynamicHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvFixedHeightValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

    return m_isCreated;
}

bool HistogramAnalyzer::CloseObject()
{
	m_histogramImage.CloseObject();
	msvplHistValues.clear();
	SVMatroxImageInterface l_lIntf;
	l_lIntf.Destroy( m_HistResultID );
	ImageAnalyzer::CloseObject();

   return true;
}

SvOi::IObjectClass* HistogramAnalyzer::GetResultObject(SvPb::EmbeddedIdEnum embeddedID)
{
	for(int i = 0; i < GetSize(); ++i)
	{
		SvOp::SVLongResult* pResult = dynamic_cast<SvOp::SVLongResult*> (getTaskObject(i));
		if (nullptr != pResult && embeddedID == pResult->GetInputEmbeddedID())
		{
			return dynamic_cast<SvOi::IObjectClass*> (pResult);
		}
	}
	return nullptr;
}

bool HistogramAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = true;
	SvIe::SVImageClass *pInputImage{nullptr};

	while (1)
	{
		///////////////////////////////////////////////////
		// Clear values to be calculated
		///////////////////////////////////////////////////
		msvStdDevValue.SetValue(0.0);
		msvMeanValue.SetValue(0.0);

		msvMaxPixelValue.SetValue(0L);
		msvMinPixelValue.SetValue(0L);
		msvPixelCountValue.SetValue(0L);
		msvHighPeak.SetValue(0L);
		msvLowPeak.SetValue(0L);
		msvValley.SetValue(0L);

		Result = __super::onRun( rRunStatus, pErrorMessages );
		if (!Result)
		{
			break;
		}

		pInputImage = getInputImage(true);

		if(nullptr == pInputImage)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SvOi::ITRCImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());

		if( nullptr == pImageBuffer)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVMatroxImageInterface l_lImageIntf;

		if(pImageBuffer->isEmpty() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		HRESULT MatroxCode = l_lImageIntf.Histogram(m_HistResultID, pImageBuffer->getHandle()->GetBuffer() );
		if (S_OK != MatroxCode)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MatroxCode = l_lImageIntf.GetResult(m_HistResultID,  msvplHistValues );

		if (S_OK != MatroxCode)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		m_histogram.SetPixelCounts(msvplHistValues.begin(), msvplHistValues.end());
		m_histogram.SetScale(m_scale?histogram::logarithmic:histogram::linear);
		m_histogram.SetPeakDefault(m_default);
		m_histogram.SetPeakThreshold(m_threshold);
		m_histogram.SetValleyDefault(m_valleyDefault);
		m_histogram.SetValleyLow(m_lowValley);
		m_histogram.SetValleyHigh(m_highValley);
		m_histogram.SetFixedHeight(m_fixedHeight);
		m_histogram.SetHeightOption(m_dynamicHeight?histogram::dynamic:histogram::fixed);
		m_histogram.Clip(m_rangeStart, m_rangeEnd);
		m_histogram.CalcStats();

		msvHistogramValue.SetArraySize(static_cast<int>(msvplHistValues.size()));	
		if ( S_OK != msvHistogramValue.SetArrayValues( msvplHistValues) )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range start
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeStartValue.SetValue(m_rangeStart))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range end
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeEndValue.SetValue(m_rangeEnd))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range size
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeSizeValue.SetValue(m_rangeEnd - m_rangeStart + 1L))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save scale
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvScaleValue.SetValue(m_scale))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save peak threshold
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvPeakThreshold.SetValue(m_threshold))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save default peak
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDefaultPeak.SetValue(m_default))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save min peak height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvMinPeakHeight.SetValue(m_minHeight))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save "use dynamic height" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDynamicHeight.SetValue(static_cast<BOOL> (m_histogram.IsDynaHeight())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save "accumulate counts" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvAccumulateCounts.SetValue(m_accumulate))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;;
		}

		///////////////////////////////////////////////////////////////////
		// Save fixed height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvFixedHeightValue.SetValue(m_histogram.GetFixedHeight()))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley lower bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyLowerBound.SetValue(static_cast<long> (m_histogram.GetValleyLow())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley upper bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyUpperBound.SetValue(static_cast<long> (m_histogram.GetValleyHigh())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley default
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyDefault.SetValue(static_cast<long> (m_histogram.GetValleyDefault())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if( m_histogram.pixel_num() > 1 )
		{
			///////////////////////////////////////////////////////////////////
			// Save Mean Value
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMeanValue.SetValue(m_histogram.mean()))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save Standard Deviation
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvStdDevValue.SetValue(m_histogram.stdDev()))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save pixel count
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvPixelCountValue.SetValue(static_cast<long> (m_histogram.pixel_num())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save min pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMinPixelValue.SetValue(static_cast<long> (m_histogram.min_pixel())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save max pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMaxPixelValue.SetValue(static_cast<long> (m_histogram.max_pixel())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save high peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvHighPeak.SetValue(static_cast<long> (m_histogram.high_peak())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save low peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvLowPeak.SetValue(static_cast<long> (m_histogram.low_peak())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save valley
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvValley.SetValue(static_cast<long> (m_histogram.valley())))
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					pErrorMessages->push_back(Msg);
				}
				break;
			}
		}

		if (S_OK != paintHistogramImage(rRunStatus.m_triggerRecord))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				pErrorMessages->push_back(Msg);
			}
			break;
		}
		break;
	}

	
	if(!Result)	
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}

HRESULT HistogramAnalyzer::createHistogramImage()
{
	HRESULT hr = S_OK;

	SVImageInfoClass ImageInfo;

	double l_dWidth  = histogram_width;
	double l_dHeight = histogram_height;

	if( nullptr != GetParent() )
	{
		ImageInfo.SetOwner( GetParent()->getObjectId() );
	}

	ImageInfo.SetOwnerImage( m_histogramImage.getObjectId() );

	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

	ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));
	ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyWidth, l_dWidth );
	ImageInfo.SetExtentProperty( SvPb::SVExtentPropertyHeight, l_dHeight );
	ImageInfo.SetTranslation(SvPb::SVExtentTranslationNone );

	// Try to create image object...
	if( S_OK != m_histogramImage.UpdateImageSetParentAndImageInfo(SvDef::InvalidObjectId, ImageInfo ) )
	{
		return Err_SetImageInfo;
	}
	return hr;
}

HRESULT HistogramAnalyzer::paintHistogramImage(const SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	HRESULT hr = S_OK;
	SvOi::ITRCImagePtr pImageBuffer = m_histogramImage.getImageToWrite(pTriggerRecord);
	if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
	{
		return Err_GetImageHandle;
	}

	RECT l_client;
	m_histogramImage.GetImageExtents().GetRectangle(l_client);
	
	m_histogram.SetClient(l_client);
	MILCanvas canvas(pImageBuffer->getHandle()->GetBuffer());
	//canvas can be captured as reference due to function pointer called directly (no callback function!)
	SvUl::SVHistogramBase::drawFunctor drawFunction = [&canvas](const RECT& rRect, int param) {canvas.Draw(rRect, param); };
	m_histogram.DrawHistogram(drawFunction);

	return hr;
}

} //namespace SvAo
