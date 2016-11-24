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

#include "stdafx.h"
//Moved to precompiled header: #include <boost/utility.hpp>
//Moved to precompiled header: #include <boost/fusion/container.hpp>
//Moved to precompiled header: #include <boost/fusion/algorithm.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVHistogramAnalyzer.h"

#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVOCore/SVDataBuffer.h"
#include "SVOCore/SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVResultLong.h"
#include "SVStatusLibrary/MessageManager.h"

const double histogram_width = 512;
const double histogram_height = 512;

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

			operator SVMatroxGraphics() const { return m_handle; }

			void Clear( SVMatroxBuffer& p_rBuffer )
			{
				SVMatroxGraphicsInterface::Clear( m_handle, p_rBuffer );
			}

			void rectFill( SVMatroxBuffer& p_rBuffer, const CRect & rect )
			{
				SVMatroxGraphicsInterface::RectangleFill( m_handle, p_rBuffer, rect );
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
			SVMatroxGraphics m_handle;
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

		void Draw(const CRect & rect, int color)
		{
			rectFill( m_buffer, rect );
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVHistogramAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : This class encapsulates the histogram analyzer.
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVHistogramAnalyzerClass, SVHistogramAnalyzerClassGuid );

SVHistogramAnalyzerClass::SVHistogramAnalyzerClass( LPCSTR ObjectName )
						  :SVImageAnalyzerClass( ObjectName )
{
	init();
}

SVHistogramAnalyzerClass::SVHistogramAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
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
		m_outObjectInfo.ObjectTypeInfo.SubType = SVHistogramAnalyzerObjectType;

		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvHistogramValue, 
			SVHistogramValueObjectGuid, 
			IDS_OBJECTNAME_HISTOGRAM,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMeanValue,
			SVHistogramMeanObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MEAN,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvStdDevValue,
			SVHistogramStdDevObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_STDDEV,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeStartValue,
			SVHistogramRangeStartObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGESTART,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeEndValue,
			SVHistogramRangeEndObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGEEND,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMaxPixelValue,
			SVHistogramMaxPixelObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MAXPIXEL,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPixelValue,
			SVHistogramMinPixelObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MINPIXEL,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPixelCountValue,
			SVHistogramPixelCountObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_PIXELCOUNT,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvRangeSizeValue,
			SVHistogramRangeSizeObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_RANGESIZE,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvScaleValue,
			SVHistogramScaleObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_SCALE,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvHighPeak,
			SVHistogramHighPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_HIGHPEAK,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvLowPeak,
			SVHistogramLowPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_LOWPEAK,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValley,
			SVHistogramValleyObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEY,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvPeakThreshold,
			SVHistogramPeakThresholdObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_PEAKTHRESHOLD,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDefaultPeak,
			SVHistogramDefaultPeakObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_DEFAULTPEAK,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvMinPeakHeight,
			SVHistogramMinPeakHeightObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_MINPEAKHEIGHT,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvAccumulateCounts,
			SVHistogramAccumulateObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_ACCUMULATE,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyLowerBound,
			SVHistogramValleyLowerBoundObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYLOW,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyUpperBound,
			SVHistogramValleyUpperBoundObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYHIGH,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvValleyDefault,
			SVHistogramValleyDefaultObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_VALLEYDEFAULT,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvDynamicHeight,
			SVHistogramDynamicHeightObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_DYNAHEIGHT,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&msvFixedHeightValue,
			SVHistogramFixedHeightValueObjectGuid,
			IDS_OBJECTNAME_HISTOGRAM_FIXEDHEIGHT,
			false, SVResetItemNone );

		RegisterEmbeddedObject( 
			&m_histogramImage,
			SVHistogramImageObjectGuid,
			IDS_OBJECTNAME_HISTOGRAMIMAGE
			);

		// Set Embedded defaults
		msvHistogramValue.SetDefaultValue(0, true);
		msvStdDevValue.SetDefaultValue(0.0, true);
		msvMeanValue.SetDefaultValue(0.0, true);
		msvFixedHeightValue.SetDefaultValue(10.0, true);

		msvRangeStartValue.SetDefaultValue(0, true);
		msvRangeEndValue.SetDefaultValue(255, true);
		msvMaxPixelValue.SetDefaultValue(0, true);
		msvMinPixelValue.SetDefaultValue(0, true);
		msvPixelCountValue.SetDefaultValue(0, true);
		msvRangeSizeValue.SetDefaultValue(0, true);
		msvScaleValue.SetDefaultValue(0, true);
		msvHighPeak.SetDefaultValue(0, true);
		msvLowPeak.SetDefaultValue(0, true);
		msvValley.SetDefaultValue(0, true);
		msvPeakThreshold.SetDefaultValue(10, true);
		msvDefaultPeak.SetDefaultValue(0, true);
		msvMinPeakHeight.SetDefaultValue(0.5, true);
		msvValleyLowerBound.SetDefaultValue(0, true);
		msvValleyUpperBound.SetDefaultValue(255, true);
		msvValleyDefault.SetDefaultValue(0, true);

		msvDynamicHeight.SetDefaultValue(false, true);
		msvAccumulateCounts.SetDefaultValue(false, true);

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
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16089);
			
		}
}

void SVHistogramAnalyzerClass::AddResult(const std::pair<GUID, DWORD> & p)
{
	SVClassInfoStruct l_resultInfo;
	SVObjectTypeInfoStruct l_ifceInfo;
	SVLongResultClass * pAnalyzerResult;

	l_ifceInfo.EmbeddedID = p.first;
	l_resultInfo.m_DesiredInputInterface.Add( l_ifceInfo );
	l_resultInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	l_resultInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
	l_resultInfo.m_ClassId = SVLongResultClassGuid;
	l_resultInfo.m_ClassName = SvUl_SF::LoadString(p.second);

	pAnalyzerResult = dynamic_cast<SVLongResultClass *>(l_resultInfo.Construct());

	if (!pAnalyzerResult)
	{
		throw fail_to_create();
	}

	Add( pAnalyzerResult );
}

BOOL SVHistogramAnalyzerClass::OnValidate()
{

   DWORD LastError(0);

   while (1)
   {
      if (!SVImageAnalyzerClass::OnValidate())
      {
		  SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		  MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16150);
		  LastError = - SvOi::Err_16150;
		  break;
      }

      if (msvHistResultID.empty())
      {
		  SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		  MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16151);
		  LastError = - SvOi::Err_16151;
		  break;
      }

      if (0 == msvplHistValues.size())
      {
		  SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		  MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16152);
		  LastError = - SvOi::Err_16152;
		  break;
      }

      break;
   }

   if( 0 != LastError )
   {
       SetInvalid();
       return false;
   }

   m_isObjectValid.SetValue(1, true);
   return true;
}

SVHistogramAnalyzerClass::~SVHistogramAnalyzerClass()
{
   CloseObject();
}

BOOL SVHistogramAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
    SVImageClass *pSVImage(nullptr);
   DWORD LastError(0);

    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16153);
			LastError = - SvOi::Err_16153;
			break;
        }

        pSVImage = getInputImage();
        if (!pSVImage)
        {
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16154);
			LastError = - SvOi::Err_16154;
			break;
        }

        msvlHistValueArraySize = 1 << (pSVImage->getPixelDepth() & SVBufferSize);
        msvplHistValues.resize( msvlHistValueArraySize );

        if (msvplHistValues.size() == 0)
        {
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16155);
			LastError = - SvOi::Err_16155;
			break;
        }

        for( int i = 0; i < msvlHistValueArraySize; i++ )
           msvplHistValues [i] = 0L;

		SVDataBufferInfoClass svData;

		svData.Length = msvlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = msvHistResultID;
		if ( S_OK == SVImageProcessingClass::CreateDataBuffer( &svData )  )
		{
			msvHistResultID = svData.HBuffer.milResult;
		}

		if (msvHistResultID.empty())
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16156);
			LastError = - SvOi::Err_16156;
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
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16157);
			LastError = - SvOi::Err_16157;
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

	msvHistogramValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvStdDevValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvMeanValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvRangeStartValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvRangeEndValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvMaxPixelValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvMinPixelValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvPixelCountValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvRangeSizeValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvScaleValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvHighPeak.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvLowPeak.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvValley.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvPeakThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvDefaultPeak.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvAccumulateCounts.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvMinPeakHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvValleyLowerBound.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvValleyUpperBound.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvValleyDefault.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvDynamicHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	msvFixedHeightValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;

    return m_isCreated;
}

void SVHistogramAnalyzerClass::DisconnectImages()
{
   dynamic_cast<SVInspectionProcess*>(GetInspection())->RemoveImage(&m_histogramImage);
}

BOOL SVHistogramAnalyzerClass::CloseObject()
{
	DWORD LastError(0);
	
	m_histogramImage.CloseObject();
	msvplHistValues.clear();
	SVMatroxImageInterface l_lIntf;
	l_lIntf.Destroy( msvHistResultID );
	SVImageAnalyzerClass::CloseObject();


   if( 0 !=LastError)
   {
      return FALSE;
   }
   else
   {
	   return TRUE;
   }
   
}

SVResultClass* SVHistogramAnalyzerClass::GetResultObject(const GUID & guid)
{
	for(int i = 0; i < GetSize(); ++i)
	{
		SVLongResultClass * l_result = dynamic_cast<SVLongResultClass *>(GetAt(i));
		if (l_result && l_result->GetInputEmbeddedID() == guid)
			return l_result;
	}
	return nullptr;
}

BOOL SVHistogramAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVImageClass     *pInputImage;

	SVMatroxImageInterface::SVStatusCode l_Code;

	pInputImage = nullptr;
	DWORD LastError(0);
	
	while (1)
	{
		///////////////////////////////////////////////////
		// Clear values to be calculated
		///////////////////////////////////////////////////
		msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, 0.0 );
		msvMeanValue.SetValue( RRunStatus.m_lResultDataIndex, 0.0 );

		msvMaxPixelValue.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		msvMinPixelValue.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		msvPixelCountValue.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		msvHighPeak.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		msvLowPeak.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		msvValley.SetValue( RRunStatus.m_lResultDataIndex, 0 );

		if( !SVImageAnalyzerClass::onRun( RRunStatus ) )
		{
			
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16158);
			LastError = - SvOi::Err_16158;
			break;
		}

		pInputImage = getInputImage();

		if( ! pInputImage )
		{
			SetInvalid();
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16159);
			LastError = - SvOi::Err_16159;
			break;
		}

		SVSmartHandlePointer ImageHandle;

		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16160);
			LastError = - SvOi::Err_16160;
			break;
		}

		SVMatroxImageInterface l_lImageIntf;

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		if( l_MilHandle.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16161);
			LastError = - SvOi::Err_16161;
			break;
		}

		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilHandle.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16162);
			LastError = - SvOi::Err_16162;
			break;
		}

		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );

		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16163);
			LastError = - SvOi::Err_16163;
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
		if ( S_OK != msvHistogramValue.SetArrayValues( RRunStatus.m_lResultDataIndex,  msvplHistValues) )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16164);
			LastError = - SvOi::Err_16164;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range start
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeStartValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeStart ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16165);
			LastError = - SvOi::Err_16165;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range end
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeEndValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeEnd ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16171);
			LastError = - SvOi::Err_16171;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save range size
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeSizeValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeEnd - m_rangeStart + 1 ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16166);
			LastError = - SvOi::Err_16166;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save scale
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvScaleValue.SetValue( RRunStatus.m_lResultDataIndex, m_scale ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16167);
			LastError = - SvOi::Err_16167;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save peak threshold
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvPeakThreshold.SetValue( RRunStatus.m_lResultDataIndex, m_threshold ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16168);
			LastError = - SvOi::Err_16168;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save default peak
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDefaultPeak.SetValue( RRunStatus.m_lResultDataIndex, m_default ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16169);
			LastError = - SvOi::Err_16169;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save min peak height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvMinPeakHeight.SetValue( RRunStatus.m_lResultDataIndex, m_minHeight ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16170);
			LastError = - SvOi::Err_16170;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save "use dynamic height" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDynamicHeight.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.IsDynaHeight() ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16172);
			LastError = - SvOi::Err_16172;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save "accumulate counts" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvAccumulateCounts.SetValue( RRunStatus.m_lResultDataIndex, m_accumulate ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16173);
			LastError = - SvOi::Err_16173;
			break;;
		}

		///////////////////////////////////////////////////////////////////
		// Save fixed height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvFixedHeightValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetFixedHeight() ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16174);
			LastError = - SvOi::Err_16174;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley lower bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyLowerBound.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyLow() ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16175);
			LastError = - SvOi::Err_16175;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley upper bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyUpperBound.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyHigh() ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16176);
			LastError = - SvOi::Err_16176;
			break;
		}

		///////////////////////////////////////////////////////////////////
		// Save valley default
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyDefault.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyDefault() ))
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16177);
			LastError = - SvOi::Err_16177;
			break;
		}

		if( m_histogram.pixel_num() > 1 )
		{
			///////////////////////////////////////////////////////////////////
			// Save Mean Value
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMeanValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.mean() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16178);
				LastError = - SvOi::Err_16178;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save Standard Deviation
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.stdDev() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16179);
				LastError = - SvOi::Err_16179;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save pixel count
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvPixelCountValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.pixel_num() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16180);
				LastError = - SvOi::Err_16180;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save min pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMinPixelValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.min_pixel() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16181);
				LastError = - SvOi::Err_16181;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save max pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMaxPixelValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.max_pixel() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16182);
				LastError = - SvOi::Err_16182;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save high peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvHighPeak.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.high_peak() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16183);
				LastError = - SvOi::Err_16183;
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save low peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvLowPeak.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.low_peak() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16184);
				LastError = - SvOi::Err_16184; 
				break;
			}

			///////////////////////////////////////////////////////////////////
			// Save valley
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvValley.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.valley() ))
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16185);
				LastError = - SvOi::Err_16185;
				break;
			}
		}

		if (S_OK != paintHistogramImage())
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16186);
			LastError = - SvOi::Err_16186;
			break;
		}
		break;
	}

	
	if(0 != LastError)	
	{
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	return TRUE;
}

bool SVHistogramAnalyzerClass::calcHistogram()
{
	bool LastError(false);
	SVImageClass     *pInputImage(nullptr);
	SVMatroxImageInterface::SVStatusCode l_Code(0);

	pInputImage = getInputImage();
	if(nullptr== pInputImage)
	{
		LastError = true;
	}

	SVSmartHandlePointer ImageHandle;
	if(!LastError)
	{
		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16201);
			LastError = true;

		}
	}
	SVMatroxImageInterface l_lImageIntf;
	SVImageBufferHandleImage l_MilHandle;

	if(!LastError)
	{
		ImageHandle->GetData( l_MilHandle );
		if( l_MilHandle.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16202);
			LastError = true;
		}
	}

	if(!LastError)
	{
		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilHandle.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16203);
			LastError = true;
		}
	}

	if(!LastError)
	{
		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );
		if( l_Code != SVMEE_STATUS_OK )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16204);
			LastError = true;
		}
	}
	return (false == LastError);
}

HRESULT SVHistogramAnalyzerClass::createHistogramImage()
{
	HRESULT hr = S_OK;

	SVImageInfoClass ImageInfo;

	double l_dWidth  = histogram_width;
	double l_dHeight = histogram_height;

	if( nullptr != GetOwner() )
	{
		ImageInfo.SetOwner( GetOwner()->GetUniqueObjectID() );
	}

	ImageInfo.SetOwnerImage( m_histogramImage.GetUniqueObjectID() );

	ImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
	ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

	ImageInfo.SetExtentProperty( SVExtentPropertyPositionPoint, 0 );
	ImageInfo.SetExtentProperty( SVExtentPropertyWidth, l_dWidth );
	ImageInfo.SetExtentProperty( SVExtentPropertyHeight, l_dHeight );
	ImageInfo.SetTranslation( SVExtentTranslationNone );

	// Try to create image object...
	if( S_OK != m_histogramImage.UpdateImage( SVImageTypeFixed, ImageInfo ) )
	{
		return Err_SetImageInfo;
	}
	return hr;
}

HRESULT SVHistogramAnalyzerClass::paintHistogramImage()
{
	HRESULT hr = S_OK;
	SVSmartHandlePointer l_handle;
	if (!m_histogramImage.GetImageHandle(l_handle) || l_handle.empty())
	{
		return Err_GetImageHandle;
	}
	SVImageBufferHandleImage l_MilHandle;
	l_handle->GetData( l_MilHandle );

	CRect l_client;
	m_histogramImage.GetImageExtents().GetRectangle(l_client);
	
	m_histogram.SetClient(l_client);
	MILCanvas canvas(l_MilHandle.GetBuffer());
	m_histogram.DrawHistogram(canvas);

	return hr;
}

