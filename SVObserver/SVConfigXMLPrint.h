//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigXMLPrint
//* .File Name       : $Workfile:   SVConfigXMLPrint.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   03 Dec 2013 13:54:14  $
//******************************************************************************

#ifndef SVCONFIGXMLPRINT_H
#define SVCONFIGXMLPRINT_H

#include <string>
#include <sstream>
#include "xmllite.h"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time.hpp>

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVParamListDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVObjectLibrary/SVObjectClass.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "SVEdge.h"
#include "SVEquation.h"
#include "SVImageClass.h"
#include "SVLine.h"	// Sri 2/21/00
#include "SVLineAnalyzer.h"	// Sri 2/21/00
#include "SVObserver.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVInputObjectList.h"
#include "SVDigitalInputObject1.h"
#include "SVDigitalOutputObject1.h"
#include "SVOCRParamStruct.h"
#include "SVOCRGrayParamStruct.h"
#include "SVArchiveTool.h"
#include "SVStatTool.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVAcquisitionClass.h"
#include "SVFileNameArrayClass.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVIODoc.h"
#include "SVTriggerClass.h"
#include "SVIPDoc.h"
#include "SVShapeMaskHelperClass.h"
#include "SVIOController.h"
#include "SVObserver.h"
#include "SVUserMaskOperatorClass.h"

using namespace SVStringConversions;

typedef CComPtr<IXmlWriter> Writer;

namespace sv_xml
{
	class SVConfigXMLPrint
	{
	public:
		const std::string Print() const;

	private:
		void PrintXMLDoc(Writer  writer) const;
		void WriteTriggers(Writer  writer) const;
		void WriteTrigger(Writer  writer, SVTriggerObject * t) const;
		void WriteCameras(Writer  writer) const;
		void WriteCamera(Writer  writer, SVVirtualCamera * cam) const;
		void WriteHardwareAcq(Writer  writer, SVVirtualCamera * cam) const;
		void WriteFileAcq(Writer  writer, SVVirtualCamera * cam) const;
		void WritePPQs(Writer  writer) const;
		void WritePPQCameras(Writer  writer, SVPPQObject * pPPQ) const;
		void WritePPQInspections(Writer  writer, SVPPQObject * pPPQ) const;
		void WriteInspections(Writer  writer) const;
		void WriteToolSets(Writer writer) const;
		void WriteToolSet(Writer writer, SVInspectionProcess * insp) const;
		void WriteTool(Writer writer, SVToolClass * ts) const;
		void WriteArchiveTool(Writer writer, SVArchiveTool * ar) const;
		void WriteModuleIO(Writer writer) const;
		void WriteResultIO(Writer writer) const;
		void WriteIOSection(Writer writer) const;
		void WritePPQBar(Writer writer) const;
		void WriteObject( Writer writer, SVObjectClass* pObj ) const;
		void WriteValueObject( Writer writer, SVValueObjectClass* pObj ) const;
		void WriteChildren( Writer writer, SVObjectClass* pObj ) const;
		void WriteFriends( Writer writer, SVObjectClass* pObj ) const;
		void WriteInputOutputList( Writer writer, SVObjectClass* pObj ) const;
		void WriteOCRParameters(Writer writer, const std::string & strParameters) const;
		void WriteOCRGrayScaleParameters(Writer writer, const std::string & strParameters) const;
		void WriteValueObject(Writer writer, const std::wstring  tag, const std::wstring  lpszName, const std::wstring  lpszValue) const;
		void WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const;

		mutable SVConfigurationObject * m_cfo;
		mutable int nToolNumber;
	};

	class HG2String
	{
		HGLOBAL hg;
	public:
		HG2String(HGLOBAL h): hg(h)
		{
		}
		~HG2String()
		{
			::GlobalUnlock(hg);
		}

		const std::string operator()() { return reinterpret_cast<const char *>(::GlobalLock(hg)); }
	};

	const std::wstring now()
	{
		const std::wstring sep = L" ";
		typedef boost::posix_time::ptime ptime;
		typedef boost::posix_time::microsec_clock clock;
		ptime pt = clock::local_time();
		std::wstringstream ss;
		ss << pt.date() << sep << pt.time_of_day() << std::ends;
		return ss.str();
	}

	class SVDeviceParamConfigXMLHelper :
		public Loki::BaseVisitor,
		public Loki::Visitor<SVDeviceParam>,
		public Loki::Visitor<SVLongValueDeviceParam>,
		public Loki::Visitor<SVi64ValueDeviceParam>,
		public Loki::Visitor<SVBoolValueDeviceParam>,
		public Loki::Visitor<SVStringValueDeviceParam>,
		public Loki::Visitor<SVParamListDeviceParam>,
		public Loki::Visitor<SVLutDeviceParam>,
		public Loki::Visitor<SVLightReferenceDeviceParam>,
		public Loki::Visitor<SVCameraFormatsDeviceParam>,
		public Loki::Visitor<SVCustomDeviceParam>
	{
	public:
		SVDeviceParamConfigXMLHelper(Writer writer, SVDeviceParamCollection& rCamFileParams);
		HRESULT Visit(SVDeviceParam&);
		HRESULT Visit(SVLongValueDeviceParam&);
		HRESULT Visit(SVi64ValueDeviceParam&);
		HRESULT Visit(SVBoolValueDeviceParam&);
		HRESULT Visit(SVStringValueDeviceParam&);
		HRESULT Visit(SVParamListDeviceParam&);
		HRESULT Visit(SVLutDeviceParam&);
		HRESULT Visit(SVLightReferenceDeviceParam&);
		HRESULT Visit(SVCameraFormatsDeviceParam&);
		HRESULT Visit(SVCustomDeviceParam&);

	private:
		Writer m_writer;
		SVDeviceParamCollection & m_rCamFileParams;
	};
}

#include "SVConfigXMLPrint.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVConfigXMLPrint.h_v  $
 * 
 *    Rev 1.3   03 Dec 2013 13:54:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  876
 * SCR Title:  Add user mask image to print configuration
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include.
 * Removed empty lines and dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2013 09:48:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVShapeMaskHelperClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2013 07:32:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jun 2013 11:33:00   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Fixed nodes out of order. Fixed strange characters at the end of report.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2013 19:22:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2013 03:16:10   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  841
 * SCR Title:  Change configuration print report format to xml
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
