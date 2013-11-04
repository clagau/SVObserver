//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramAnalyzerClass
//* .File Name       : $Workfile:   SVHistogramAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:16  $
//******************************************************************************

#include "stdafx.h"
#include <boost/utility.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/bind.hpp>
#include "SVHistogramAnalyzer.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVDataBuffer.h"
#include "SVImageClass.h"
#include "SVInspectionProcess.h"
#include "SVImageProcessingClass.h"
#include "SVResultLong.h"

const double histogram_width = 512;
const double histogram_height = 512;

namespace 
{
	namespace mil // should really go into it's own header file?
	{
		namespace color
		{
			inline double white()		{ return SVMatroxGraphicsInterface::CreateRGB888(255,255,255); }
			inline double silver()		{ return SVMatroxGraphicsInterface::CreateRGB888(192,192,192); }
			inline double gray()			{ return SVMatroxGraphicsInterface::CreateRGB888(128,128,128); }
			inline double charcoal()	{ return SVMatroxGraphicsInterface::CreateRGB888( 64, 64, 64); }
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
//#define Err_		(JH_ErrorBase+139)

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
	
	while (1)
	{
		// Identify our output type
		outObjectInfo.ObjectTypeInfo.SubType = SVHistogramAnalyzerObjectType;
		
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
		
		//RegisterEmbeddedObject( 
		//	&msvVarianceValue,
		//	SVHistogramVarianceObjectGuid,
		//	IDS_OBJECTNAME_HISTOGRAM_VARIANCE,
		//	false, SVResetItemNone );
		//
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
		
		//RegisterEmbeddedObject( 
		//	&msvUseDefaultPeak,
		//	SVHistogramUseDefaultPeakObjectGuid,
		//	IDS_OBJECTNAME_HISTOGRAM_USEDEFAULTPEAK,
		//	false, SVResetItemNone );
		
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
		msvHistogramValue.SetDefaultValue (0, true);
		//msvVarianceValue.SetDefaultValue (0.0, true);
		msvStdDevValue.SetDefaultValue (0.0, true);
		msvMeanValue.SetDefaultValue (0.0, true);
		msvFixedHeightValue.SetDefaultValue(10.0, true);
		
		msvRangeStartValue.SetDefaultValue (0, true);
		msvRangeEndValue.SetDefaultValue (255, true);
		msvMaxPixelValue.SetDefaultValue (0, true);
		msvMinPixelValue.SetDefaultValue (0, true);
		msvPixelCountValue.SetDefaultValue (0, true);
		msvRangeSizeValue.SetDefaultValue (0, true);
		msvScaleValue.SetDefaultValue (0, true);
		msvHighPeak.SetDefaultValue (0, true);
		msvLowPeak.SetDefaultValue (0, true);
		msvValley.SetDefaultValue (0, true);
		msvPeakThreshold.SetDefaultValue (10, true);
		msvDefaultPeak.SetDefaultValue (0, true);
		msvMinPeakHeight.SetDefaultValue(0.5, true);
		msvValleyLowerBound.SetDefaultValue (0, true);
		msvValleyUpperBound.SetDefaultValue (255, true);
		msvValleyDefault.SetDefaultValue (0, true);

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
			msvError.msvlErrorCd = -Err_NewResult;
			SV_TRAP_ERROR_BRK (msvError, Err_NewResult);
		}
		break;
	}
}

void SVHistogramAnalyzerClass::AddResult(const std::pair<GUID, DWORD> & p)
{
	SVClassInfoStruct l_resultInfo;
	SVObjectTypeInfoStruct l_ifceInfo;
	SVLongResultClass * pAnalyzerResult;

	l_ifceInfo.EmbeddedID = p.first;
	l_resultInfo.DesiredInputInterface.Add( l_ifceInfo );
	l_resultInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	l_resultInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
	l_resultInfo.ClassId = SVLongResultClassGuid;
	l_resultInfo.ClassName.LoadString(p.second);

	pAnalyzerResult = dynamic_cast<SVLongResultClass *>(l_resultInfo.Construct());

	if (!pAnalyzerResult)
	{
		throw fail_to_create();
	}

	Add( pAnalyzerResult );
	//m_resultGuids[p.first] = pAnalyzerResult->GetUniqueObjectID();
}

BOOL SVHistogramAnalyzerClass::OnValidate ()
{

   msvError.ClearLastErrorCd ();   
   msvError.msvlErrorCd = 0x00000000;

   while (1)
   {
      if (!SVImageAnalyzerClass::OnValidate ())
      {
//       Error code set inside SVImageAnalyzerClass::OnValidate ()
//		 Next line commented out to remove message box.
//       SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1081);
         break;
      }

      if (msvHistResultID.empty())
      {
         msvError.msvlErrorCd = -Err_ResultEmpty;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, Err_ResultEmpty);
      }

      if (msvplHistValues.size() == 0)
      {
         msvError.msvlErrorCd = -Err_ArrayEmpty;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, Err_ArrayEmpty);
      }

      break;
   }

   if( (msvError.GetLastErrorCd () & SV_ERROR_CONDITION) ||
	   (msvError.msvlErrorCd       & SV_ERROR_CONDITION) )
   {
       SetInvalid ();
       return FALSE;
   }

   isObjectValid.SetValue (1, TRUE);
   return TRUE;

}

SVHistogramAnalyzerClass::~SVHistogramAnalyzerClass()
{
   CloseObject ();
}


BOOL SVHistogramAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
    SVImageClass *pSVImage;
    msvError.ClearLastErrorCd ();

    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
         msvError.msvlErrorCd = -Err_CreateStruct;
         SV_TRAP_ERROR_BRK (msvError, Err_CreateStruct);
        }

        pSVImage = getInputImage ();
        if (!pSVImage)
        {
         msvError.msvlErrorCd = -Err_GetInputImage;
         SV_TRAP_ERROR_BRK (msvError, Err_GetInputImage);
        }

        msvlHistValueArraySize = 1 << (pSVImage->getPixelDepth() & SVBufferSize);
        msvplHistValues.resize( msvlHistValueArraySize );

        if (msvplHistValues.size() == 0)
        {
         msvError.msvlErrorCd = -Err_ArrayEmpty;
         SV_TRAP_ERROR_BRK (msvError, Err_ArrayEmpty);
        }

        for( int i = 0; i < msvlHistValueArraySize; i++ )
           msvplHistValues [i] = 0L;

		SVDataBufferInfoClass svData;

		svData.Length = msvlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = msvHistResultID;
		if ( SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) == S_OK )
		{
			msvHistResultID = svData.HBuffer.milResult;
		}

		if (msvHistResultID.empty())
		{
			msvError.msvlErrorCd = -Err_ResultEmpty;
			SV_TRAP_ERROR_BRK (msvError, Err_ResultEmpty);
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
			msvError.msvlErrorCd = -Err_CreateImage;
			SV_TRAP_ERROR_BRK (msvError, Err_CreateImage);
		}

		break;
    }

    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
        isCreated = FALSE;
    }
    else
        isCreated = TRUE;


		msvHistogramValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		//msvVarianceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
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

    return isCreated;
}

void SVHistogramAnalyzerClass::DisconnectImages()
{
   GetInspection()->RemoveImage(&m_histogramImage);
}

BOOL SVHistogramAnalyzerClass::CloseObject()
{
   msvError.ClearLastErrorCd ();

   while (1)
   {
	   m_histogramImage.CloseObject();
      msvplHistValues.clear();

	   SVMatroxImageInterface l_lIntf;

      l_lIntf.Destroy( msvHistResultID );

      SVImageAnalyzerClass::CloseObject ();
      break;
   }

   if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
   {
      return FALSE;
   }

   return TRUE;
}


SVResultClass* SVHistogramAnalyzerClass::GetResultObject(const GUID & guid)
{
	for(int i = 0; i < GetSize(); ++i)
	{
		SVLongResultClass * l_result = dynamic_cast<SVLongResultClass *>(GetAt(i));
		if (l_result && l_result->GetInputEmbeddedID() == guid)
			return l_result;
	}
	return NULL;
}


BOOL SVHistogramAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVImageClass     *pInputImage;
	

	SVMatroxImageInterface::SVStatusCode l_Code;

	pInputImage = NULL;
	msvError.ClearLastErrorCd ();
	
	while (1)
	{
		///////////////////////////////////////////////////
		// Clear values to be calculated
		///////////////////////////////////////////////////
		//msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, 0.0 );
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
			break;
		}
		
		pInputImage = getInputImage ();
		
		if( ! pInputImage )
		{
			SetInvalid ();            
			msvError.msvlErrorCd = -Err_GetInputImage;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_GetInputImage)
		}
		
		SVSmartHandlePointer ImageHandle;
		
		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			msvError.msvlErrorCd = -Err_MilGetHandle;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_MilGetHandle)
		}
				
		SVMatroxImageInterface l_lImageIntf;

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		if( l_MilHandle.empty() )
		{
			msvError.msvlErrorCd = -Err_MilGetHandle;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_MilGetHandle)
		}

		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilHandle.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_MilHistogram)
		}
		
		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );

		
		if( l_Code != SVMEE_STATUS_OK )
		{
				msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_MilGetHistogram)
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

		msvHistogramValue.SetArraySize(msvplHistValues.size());	
		if ( S_OK != msvHistogramValue.SetArrayValues( RRunStatus.m_lResultDataIndex,  msvplHistValues) )
		{
				msvError.msvlErrorCd = -Err_SetArrayValues;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetArrayValues)
		}
		
		///////////////////////////////////////////////////////////////////
		// Save range start
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeStartValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeStart ))
		{
			msvError.msvlErrorCd = -Err_SetStart;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetStart);
		}
		
		///////////////////////////////////////////////////////////////////
		// Save range end
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeEndValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeEnd ))
		{
			msvError.msvlErrorCd = -Err_SetEnd;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetEnd);
		}

		///////////////////////////////////////////////////////////////////
		// Save range size
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvRangeSizeValue.SetValue( RRunStatus.m_lResultDataIndex, m_rangeEnd - m_rangeStart + 1 ))
		{
			msvError.msvlErrorCd = -Err_SetSize;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetSize);
		}

		///////////////////////////////////////////////////////////////////
		// Save scale
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvScaleValue.SetValue( RRunStatus.m_lResultDataIndex, m_scale ))
		{
			msvError.msvlErrorCd = -Err_SetScale;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetScale);
		}

		///////////////////////////////////////////////////////////////////
		// Save peak threshold
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvPeakThreshold.SetValue( RRunStatus.m_lResultDataIndex, m_threshold ))
		{
			msvError.msvlErrorCd = -Err_SetThreshold;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetThreshold);
		}

		///////////////////////////////////////////////////////////////////
		// Save default peak
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDefaultPeak.SetValue( RRunStatus.m_lResultDataIndex, m_default ))
		{
			msvError.msvlErrorCd = -Err_SetDefaultPeak;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetDefaultPeak);
		}

		///////////////////////////////////////////////////////////////////
		// Save min peak height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvMinPeakHeight.SetValue( RRunStatus.m_lResultDataIndex, m_minHeight ))
		{
			msvError.msvlErrorCd = -Err_SetMinPeak;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetMinPeak);
		}

		///////////////////////////////////////////////////////////////////
		// Save "use dynamic height" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvDynamicHeight.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.IsDynaHeight() ))
		{
			msvError.msvlErrorCd = -Err_SetDynaHeight;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetDynaHeight);
		}

		///////////////////////////////////////////////////////////////////
		// Save "accumulate counts" flag
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvAccumulateCounts.SetValue( RRunStatus.m_lResultDataIndex, m_accumulate ))
		{
			msvError.msvlErrorCd = -Err_SetAccumulate;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetAccumulate);
		}

		///////////////////////////////////////////////////////////////////
		// Save fixed height
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvFixedHeightValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetFixedHeight() ))
		{
			msvError.msvlErrorCd = -Err_SetFixedHeight;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetFixedHeight);
		}

		///////////////////////////////////////////////////////////////////
		// Save valley lower bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyLowerBound.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyLow() ))
		{
			msvError.msvlErrorCd = -Err_SetValleyLow;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetValleyLow);
		}

		///////////////////////////////////////////////////////////////////
		// Save valley upper bound
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyUpperBound.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyHigh() ))
		{
			msvError.msvlErrorCd = -Err_SetValleyHigh;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetValleyHigh);
		}

		///////////////////////////////////////////////////////////////////
		// Save valley default
		///////////////////////////////////////////////////////////////////
		if (S_OK != msvValleyDefault.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.GetValleyDefault() ))
		{
			msvError.msvlErrorCd = -Err_SetValleyDefault;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetValleyDefault);
		}

		if( m_histogram.pixel_num() > 1 )
		{
			///////////////////////////////////////////////////////////////////
			// Save Mean Value
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMeanValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.mean() ))
			{
				msvError.msvlErrorCd = -Err_SetMean;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetMean);
			}
			
			/////////////////////////////////////////////////////////////////////
			//// Save Variance
			/////////////////////////////////////////////////////////////////////
			//if (S_OK != msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.variance() ))
			//{
			//	msvError.msvlErrorCd = -Err_SetVariance;
			//	SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetVariance);
			//}
			//
			///////////////////////////////////////////////////////////////////
			// Save Standard Deviation
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.stdDev() ))
			{
				msvError.msvlErrorCd = -Err_SetStdDev;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetStdDev);
			}

			///////////////////////////////////////////////////////////////////
			// Save pixel count
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvPixelCountValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.pixel_num() ))
			{
				msvError.msvlErrorCd = -Err_SetCount;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetCount);
			}
			
			///////////////////////////////////////////////////////////////////
			// Save min pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMinPixelValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.min_pixel() ))
			{
				msvError.msvlErrorCd = -Err_SetMin;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetMin);
			}

			///////////////////////////////////////////////////////////////////
			// Save max pixel
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvMaxPixelValue.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.max_pixel() ))
			{
				msvError.msvlErrorCd = -Err_SetMax;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetMax);
			}

			///////////////////////////////////////////////////////////////////
			// Save high peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvHighPeak.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.high_peak() ))
			{
				msvError.msvlErrorCd = -Err_SetHighPeak;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetHighPeak);
			}

			///////////////////////////////////////////////////////////////////
			// Save low peak
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvLowPeak.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.low_peak() ))
			{
				msvError.msvlErrorCd = -Err_SetLowPeak;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetLowPeak);
			}

			///////////////////////////////////////////////////////////////////
			// Save valley
			///////////////////////////////////////////////////////////////////
			if (S_OK != msvValley.SetValue( RRunStatus.m_lResultDataIndex, m_histogram.valley() ))
			{
				msvError.msvlErrorCd = -Err_SetValley;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_SetValley);
			}

		}
		if (S_OK != paintHistogramImage())
		{
			msvError.msvlErrorCd = -Err_PaintHistogram;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_PaintHistogram);
		}
		break;
  }
	
  if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
  {
		SetInvalid ();
		RRunStatus.SetInvalid();
		return FALSE;
  }
	
  return TRUE;
}

/*
void SVHistogramAnalyzerClass::setHistogramOptions(SVHistogramAnalyzerSetupClass & dlg) const
{
	dlg.m_pAnalyzer = const_cast<SVHistogramAnalyzerClass *>(this);
	dlg.m_histogram.SetScale(static_cast<histogram::scale>(m_scale));
	dlg.m_histogram.SetPeakThreshold(m_threshold);
	dlg.m_histogram.SetPeakDefault(m_default);
	dlg.m_histogram.SetDisplay(m_accumulate?histogram::accumulated:histogram::normal);
	dlg.m_histogram.SetMinHeight(m_minHeight);
	dlg.m_histogram.SetValleyDefault(m_valleyDefault);
	dlg.m_histogram.SetValleyLow(m_lowValley);
	dlg.m_histogram.SetValleyHigh(m_highValley);
	dlg.m_histogram.SetFixedHeight(m_fixedHeight);
	dlg.m_histogram.SetHeightOption(m_dynamicHeight?histogram::dynamic:histogram::fixed);
	dlg.m_histogram.Clip(m_rangeStart, m_rangeEnd);
}

void SVHistogramAnalyzerClass::getHistogramOptions(const SVHistogramAnalyzerSetupClass & dlg)
{
	m_rangeStart = dlg.m_histogram.GetLowClip();
	m_rangeEnd = dlg.m_histogram.GetHighClip();
	m_scale = dlg.m_histogram.GetScale();
	m_dynamicHeight = dlg.m_histogram.GetHeightOption() == histogram::dynamic;
	m_histogram.SetHeightOption(dlg.m_histogram.GetHeightOption());
	m_histogram.SetScale(m_scale?histogram::logarithmic:histogram::linear);
	m_default = dlg.m_histogram.GetPeakDefault();
	m_histogram.SetPeakDefault(m_default);
	m_threshold = dlg.m_histogram.GetPeakThreshold();
	m_histogram.SetPeakThreshold(m_threshold);
	m_minHeight = dlg.m_histogram.GetMinHeight();
	m_histogram.SetMinHeight(m_minHeight);
	m_valleyDefault = dlg.m_histogram.GetValleyDefault();
	m_histogram.SetValleyDefault(m_valleyDefault);
	m_lowValley = dlg.m_histogram.GetValleyLow();
	m_histogram.SetValleyLow(m_lowValley);
	m_highValley = dlg.m_histogram.GetValleyHigh();
	m_histogram.SetValleyHigh(m_highValley);
	m_fixedHeight = dlg.m_histogram.GetFixedHeight();
	m_histogram.SetFixedHeight(m_fixedHeight);
	m_accumulate = dlg.m_histogram.GetDisplay() == histogram::accumulated;
	m_histogram.SetDisplay(m_accumulate?histogram::accumulated:histogram::normal);
	m_histogram.Clip(m_rangeStart, m_rangeEnd);
}
*/

bool SVHistogramAnalyzerClass::calcHistogram()
{
	SVImageClass     *pInputImage;
	SVMatroxImageInterface::SVStatusCode l_Code;

	pInputImage = getInputImage ();
	if (pInputImage)
	{
		SVSmartHandlePointer ImageHandle;
		
		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			msvError.msvlErrorCd = -Err_MilGetHandle;
			SV_TRAP_ERROR(msvError, Err_MilGetHandle);
		}
				
		SVMatroxImageInterface l_lImageIntf;

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		if( l_MilHandle.empty() )
		{
			msvError.msvlErrorCd = -Err_MilGetHandle;
			SV_TRAP_ERROR(msvError, Err_MilGetHandle);
		}

		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilHandle.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
			SV_TRAP_ERROR(msvError, Err_MilHistogram);
		}
		
		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );
		
		if( l_Code != SVMEE_STATUS_OK )
		{
				msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
				SV_TRAP_ERROR(msvError, Err_MilGetHistogram);
		}
		return true;
	}
	return false;
}

HRESULT SVHistogramAnalyzerClass::createHistogramImage()
{
	HRESULT hr = S_OK;

	SVImageInfoClass ImageInfo;

	double l_dWidth  = histogram_width;
	double l_dHeight = histogram_height;

	if( GetOwner() != NULL )
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
	if( m_histogramImage.UpdateImage( SVImageTypeFixed, ImageInfo ) != S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogramAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:46:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Aug 2012 10:48:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with error number definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Jul 2012 13:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 Jul 2012 16:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   13 Apr 2011 19:12:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated analyzer to use new fixed image type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   05 Apr 2011 09:02:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new image type for unattached images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Mar 2011 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   25 Feb 2011 12:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Feb 2011 14:39:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   27 Jan 2011 11:12:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 08:05:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Nov 2010 13:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Jun 2010 14:24:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 10:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jul 2009 11:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
