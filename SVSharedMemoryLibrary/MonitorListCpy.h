#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "SVProductFilterEnum.h"
#include "SVSharedMonitorList.h"
#pragma endregion Includes


namespace Seidenader { namespace SVSharedMemoryLibrary
{
	class SVMonitorListReader;
	class SVMonitorListWriter;
	///struct holds data for one Monitorlist.
	struct MonitorListCpy
	{
	public:	
		MonitorListCpy(void);
		MonitorListCpy(const SVSharedMonitorList& sharedMonitorList);
		size_t CalcMemorySize();
		
		MonitorEntryVector  prodItems;
		MonitorEntryVector  rejctCond;
		MonitorEntryVector  failStats;

		SVString m_name;
		SVString m_ppq;
		int m_rejectDepth;
		bool m_IsActive;
		SVProductFilterEnum m_ProductFilter;
	};
	
typedef std::map<SVString, MonitorListCpy> MonitorListCpyMap;

///Helperfunction for Monitorlist 
class MonitorListCpyHelper
{
public:
	//! Load MonitorLists from Shared  Memory
	//! \param mlReader [in]
	//! \param monitorMap [out]
	//! \returns void
	static void ReloadMonitorMap(SVMonitorListReader& rmlReader, MonitorListCpyMap& rmonitorMap);
	
	
	//! Writes a monitor list to shared memory
	//! \param rWriter [in,out]
	//! \param rmonitorList [in,out]
	//! \returns void
	static void WriteMonitorList(SVMonitorListWriter& rWriter,  const MonitorListCpy& rmonitorList);

	
	//!Helper function converts ShMoListEntryVector to MoListEntryVector
	static void  Assign2MoListEntryVector (MonitorEntryVector &rcpy, const ShMoListEntryVector  &rsev);
};
 

 } /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
