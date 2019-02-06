//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramAnalyzerClass
//* .File Name       : $Workfile:   SVHistogramAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/utility.hpp>
//Moved to precompiled header: #include <boost/fusion/container.hpp>
//Moved to precompiled header: #include <boost/fusion/algorithm.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
//Moved to precompiled header: #include <functional>
#include "SVHistogramAnalyzer.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "InspectionEngine/SVDataBuffer.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVResultLong.h"
#include "SVStatusLibrary/MessageManager.h"
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

		class Context: // RAII wrapper around MIL graphic context
			public boost::noncopyable
		{
		public:
			explicit Context()
			: m_handle()
			{
				SVMatroxGraphicsInterface::Create( m_handle );
			}

			~Context()
			{
				SVMatroxGraphicsInterface::Destroy( m_handle );
			}

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
			SVMatroxIdentifier m_handle = M_NULL;
		};
	}

	class MILCanvas: public mil::Context
	{
	public:
		MILCanvas( const SVMatroxBuffer& p_rBuffer )
		:	Context(), m_buffer( p_rBuffer ) 
		{
			setForeColor(mil::color::white());
			Clear( m_buffer );
			setBackColor(mil::color::white());
			setForeColor(mil::color::black());
		}

		void Draw(const RECT& rRect, int color)
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

SV_IMPLEMENT_CLASS( SVHistogramAnalyzerClass, SVHistogramAnalyzerClassGuid );

SVHistogramAnalyzerClass::SVHistogramAnalyzerClass( LPCSTR ObjectName )
						  :SVImageAnalyzerClass( ObjectName )
{
	init();
}

SVHistogramAnalyzerClass::SVHistogramAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
		:SVImageAnalyzerClass( POwner, StringResourceID ) 
{
	init();
}

namespace fusion = boost::fusion;

struct fail_to_create: public std::runtime_error
{
	fail_to_create(): std::runtime_error("failed to create result") {}
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVHistogramAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVHistogramAnalyzerClass::init()
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
		m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVHistogramAnalyzerObjectType;

		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvHistogramValue, 
			SVHistogramValueObjectGuid, 
			IDS_OBJECTNAME_HISTOGRAM,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMeanValue,
			SVHistogramMeanObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MEAN,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvStdDevValue,
			SVHistogramStdDevObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_STDDEV,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeStartValue,
			SVHistogramRangeStartObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGESTART,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeEndValue,
			SVHistogramRangeEndObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGEEND,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMaxPixelValue,
			SVHistogramMaxPixelObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MAXPIXEL,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPixelValue,
			SVHistogramMinPixelObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MINPIXEL,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPixelCountValue,
			SVHistogramPixelCountObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_PIXELCOUNT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeSizeValue,
			SVHistogramRangeSizeObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGESIZE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvScaleValue,
			SVHistogramScaleObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_SCALE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvHighPeak,
			SVHistogramHighPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_HIGHPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvLowPeak,
			SVHistogramLowPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_LOWPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValley,
			SVHistogramValleyObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEY,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPeakThreshold,
			SVHistogramPeakThresholdObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_PEAKTHRESHOLD,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDefaultPeak,
			SVHistogramDefaultPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_DEFAULTPEAK,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPeakHeight,
			SVHistogramMinPeakHeightObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MINPEAKHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvAccumulateCounts,
			SVHistogramAccumulateObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_ACCUMULATE,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyLowerBound,
			SVHistogramValleyLowerBoundObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYLOW,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyUpperBound,
			SVHistogramValleyUpperBoundObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYHIGH,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyDefault,
			SVHistogramValleyDefaultObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYDEFAULT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDynamicHeight,
			SVHistogramDynamicHeightObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_DYNAHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvFixedHeightValue,
			SVHistogramFixedHeightValueObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_FIXEDHEIGHT,
			false, SvOi::SVResetItemNone );

		RegisterEmbeddedObject( 
			&m_histogramImage,
			SVHistogramImageObjectGuid,
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

		m_histogramImage.InitializeImage(SvDef::SVImageTypeEnum::SVImageTypeIndependent);
		// Set default inputs and outputs
		addDefaultInputObjects();

		m_rangeStart = 0;
		m_rangeEnd = 255;
		m_scale = 0;

		typedef std::pair<GUID, DWORD> Param;
		try
		{
			// Add results for first/second peak and valley
			fusion::for_each(
				fusion::vector<Param, Param, Param>(
					std::make_pair(msvHighPeak.GetEmbeddedID(),
						IDS_CLASSNAME_HISTOGRAMANALYZER_FIRSTPEAKRESULT),
					std::make_pair(msvLowPeak.GetEmbeddedID(),  
						IDS_CLASSNAME_HISTOGRAMANALYZER_SECONDPEAKRESULT),
					std::make_pair(msvValley.GetEmbeddedID(),   
						IDS_CLASSNAME_HISTOGRAMANALYZER_VALLEYRESULT)
				),
				boost::bind(&SVHistogramAnalyzerClass::AddResult, this, _1)
			);
		}
		catch (const fail_to_create &)
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16089);
			
		}
}

void SVHistogramAnalyzerClass::AddResult(const std::pair<GUID, DWORD> & p)
{
	SvIe::SVClassInfoStruct l_resultInfo;
	SvDef::SVObjectTypeInfoStruct l_ifceInfo;

	l_ifceInfo.EmbeddedID = p.first;
	l_resultInfo.m_DesiredInputVector.push_back( l_ifceInfo );
	l_resultInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	l_resultInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultLongObjectType;
	l_resultInfo.m_ClassId = SVLongResultClassGuid;
	l_resultInfo.m_ClassName = SvUl::LoadStdString(p.second);

	SvOp::SVLongResultClass* pAnalyzerResult = dynamic_cast<SvOp::SVLongResultClass*> (l_resultInfo.Construct());

	if (nullptr != pAnalyzerResult)
	{
		Add(pAnalyzerResult);
	}
}

bool SVHistogramAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (M_NULL == m_HistResultID)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16151, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	if (0 == msvplHistValues.size())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16152, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

SVHistogramAnalyzerClass::~SVHistogramAnalyzerClass()
{
   CloseObject();
}

bool SVHistogramAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	SvIe::SVImageClass *pImage(nullptr);
   DWORD LastError(0);

    while (1)
    {
        if (!SVImageAnalyzerClass::CreateObject(rCreateStructure) )
        {
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16153);
			LastError = - SvStl::Err_16153;
			break;
        }

        pImage = getInputImage();
        if (!pImage)
        {
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16154);
			LastError = - SvStl::Err_16154;
			break;
        }

        msvlHistValueArraySize = 1 << (pImage->getPixelDepth() & SVBufferSize);
        msvplHistValues.resize( msvlHistValueArraySize );

        if (msvplHistValues.size() == 0)
        {
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16155);
			LastError = - SvStl::Err_16155;
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
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16156);
			LastError = - SvStl::Err_16156;
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
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16157);
			LastError = - SvStl::Err_16157;
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

	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable | SvPb::setableOnline;
	msvHistogramValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvStdDevValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvMeanValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvRangeStartValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvRangeEndValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvMaxPixelValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvMinPixelValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvPixelCountValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvRangeSizeValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvScaleValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msvHighPeak.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvLowPeak.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msvValley.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
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

bool SVHistogramAnalyzerClass::CloseObject()
{
	DWORD LastError(0);
	
	m_histogramImage.CloseObject();
	msvplHistValues.clear();
	SVMatroxImageInterface l_lIntf;
	l_lIntf.Destroy( m_HistResultID );
	SVImageAnalyzerClass::CloseObject();


   if( 0 !=LastError)
   {
      return false;
   }
   else
   {
	   return true;
   }
   
}

SvOi::IObjectClass* SVHistogramAnalyzerClass::GetResultObject(const GUID & guid)
{
	for(int i = 0; i < GetSize(); ++i)
	{
		SvOp::SVLongResultClass* pResult = dynamic_cast<SvOp::SVLongResultClass*> (GetAt(i));
		if (nullptr != pResult && guid == pResult->GetInputEmbeddedID())
		{
			return dynamic_cast<SvOi::IObjectClass*> (pResult);
		}
	}
	return nullptr;
}

bool SVHistogramAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = true;
	SvIe::SVImageClass *pInputImage{nullptr};

	HRESULT MatroxCode;

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
			SetInvalid();
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16159, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SvTrc::IImagePtr pImageBuffer = pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord);

		if( nullptr == pImageBuffer)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16160, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16161, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MatroxCode = l_lImageIntf.Histogram(m_HistResultID, pImageBuffer->getHandle()->GetBuffer() );
		if (S_OK != MatroxCode)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16162, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16163, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16164, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16165, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16171, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16166, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16167, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16168, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16169, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16170, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16172, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16173, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16174, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16175, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16176, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16177, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16178, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16179, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16180, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16181, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16182, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16183, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16184, GetUniqueObjectID() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16185, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16186, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}
		break;
	}

	
	if(!Result)	
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return Result;
}

HRESULT SVHistogramAnalyzerClass::createHistogramImage()
{
	HRESULT hr = S_OK;

	SVImageInfoClass ImageInfo;

	double l_dWidth  = histogram_width;
	double l_dHeight = histogram_height;

	if( nullptr != GetParent() )
	{
		ImageInfo.SetOwner( GetParent()->GetUniqueObjectID() );
	}

	ImageInfo.SetOwnerImage( m_histogramImage.GetUniqueObjectID() );

	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
	ImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

	ImageInfo.SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));
	ImageInfo.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_dWidth );
	ImageInfo.SetExtentProperty( SvDef::SVExtentPropertyHeight, l_dHeight );
	ImageInfo.SetTranslation( SvDef::SVExtentTranslationNone );

	// Try to create image object...
	if( S_OK != m_histogramImage.UpdateImage( GUID_NULL, ImageInfo ) )
	{
		return Err_SetImageInfo;
	}
	return hr;
}

HRESULT SVHistogramAnalyzerClass::paintHistogramImage(const SvTrc::ITriggerRecordRWPtr& pTriggerRecord)
{
	HRESULT hr = S_OK;
	SvTrc::IImagePtr pImageBuffer = m_histogramImage.getImageToWrite(pTriggerRecord);
	if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
	{
		return Err_GetImageHandle;
	}

	RECT l_client;
	m_histogramImage.GetImageExtents().GetRectangle(l_client);
	
	m_histogram.SetClient(l_client);
	MILCanvas canvas(pImageBuffer->getHandle()->GetBuffer());
	m_histogram.DrawHistogram(std::bind(&MILCanvas::Draw, &canvas, std::placeholders::_1, std::placeholders::_2));

	return hr;
}

} //namespace SvAo
