//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedImageStore.h 
/// All Rights Reserved 
//*****************************************************************************

///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************
#pragma once

#include "SharedImageContainer.h"
#include "SVSharedPPQReader.h"
#include "SVMonitorListReader.h"

namespace SvSml
{

	class SharedMemReader
	{
	public:
		SharedMemReader(void);
		~SharedMemReader(void);
		
		///Function call to m_MLContainer
		bool IsActiveMonitorList(const SVString& Monitorlistname ) const;
		DWORD GetVersion() const; 

		/**Reload MonitorMap, PPQReader, Open Imagestores and create the imageBuffer   */
		void Reload( DWORD version);
	
		void Clear();

		///returns an unique ptr the ppqreader  
		upSharedPPQReader&  GetPPQReaderPtr(const SVString& ppqname); 

	//private:
		typedef std::map<SVString, upSharedPPQReader>  PPQReaderMap; ///map SVString upSharedPPQReader

		SVMonitorListReader m_monitorListReader;
		PPQReaderMap m_PPQSReaderMap; 
		SharedImageContainer m_ImageContainer;
		MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
	

	};
} //namespace SvSml
