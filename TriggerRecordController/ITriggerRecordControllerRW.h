//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordControllerRW.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecordController in read/write-modus.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#include "ITriggerRecordControllerR.h"
#pragma endregion Includes


#pragma region Declarations
struct MatroxBufferChildDataStruct;
struct SVMatroxBufferCreateChildStruct;
struct SVMatroxBufferCreateStruct;

namespace SvTrc
{
class ITriggerRecordR;
class ITriggerRecordRW;
class IImage;
typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
typedef std::shared_ptr< ITriggerRecordRW > ITriggerRecordRWPtr;
typedef std::shared_ptr<IImage> IImagePtr;
}
#pragma endregion Declarations

namespace SvTrc
{
	class ITriggerRecordControllerRW : public ITriggerRecordControllerR
	{
	public:
		virtual ~ITriggerRecordControllerRW() = default;

		/// Clear all and free all image buffer and set this object to start state.
		virtual void clearAll() = 0;

		/// Set the list of GUID of the available inspections. ATTENTION: This command reset the whole controller. A new reset of all inspection and all images have to be done.
		/// \param rInspectionList [in] inspection list.
		/// \returns bool True if successfully. If false, no set and reset is done.
		virtual bool setInspections(SvPb::InspectionList&& rInspectionList) = 0;

		/// Resize the number of records for inspections. ATTENTION: All old Tr-instances of all IPs have to be deleted before.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param inspectionPosVec [in] A vector of position (in the inspection list) of the inspections.
		/// \param newSizeTr [in] The new number of records.
		/// \param newSizeTrofIntereset [in] The new number of records of interest.
		virtual void resizeIPNumberOfRecords(const std::vector<int>& inspectionPosVec, long newSizeTr, long newSizeTrOfIntereset = 0) = 0;

		/// Create a new trigger record in a free slot and block this slot for this instance in write-modus.
		/// \param inspectionPos [in] Position (in the inspection list) of the inspection.
		/// \returns SvTrc::ITriggerRecordRWPtr
		virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) = 0;

		/// Close a triggerRecord instance (it mus be the last one) and open a reader version.
		/// \param pTriggerRecord [in] The writer instance
		/// \returns SvTrc::ITriggerRecordRPtr The reader instance
		virtual ITriggerRecordRPtr closeWriteAndOpenReadTriggerRecordObject(ITriggerRecordRWPtr& pTriggerRecord) = 0;


		/// Close the writer instance and avoid to update the last trId.
		/// \param pTriggerRecord [in] The writer instance
		virtual void closeWriteObjectWithoutUpdateLastTrId(ITriggerRecordRWPtr& pTriggerRecord) = 0;

		/// Return a handle-instance to a new locked buffer. It will be automatically unlocked if the handle-instance will be deleted.
		/// \param bufferStruct [in] Structure of the required buffer.
		/// \param createBufferExternIfNecessary [in] If can not get a buffer from ImageController (e.g. because resetting is running) then if this parameter true it will create a buffer. ATTENTION: This buffer can not be used e.g. with setImage)
		/// \returns SvTRC::IImagePtr
		virtual IImagePtr getImageBuffer(const SVMatroxBufferCreateStruct& bufferStruct, bool createBufferExternIfNecessary = false) const = 0;

		/// Delete intern the memory and start the process to create the new memory structure. ATTENTION: All old TR-instances of all IPs have to be deleted before.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param inspectionPos [in] Position (in the inspection list) of the inspection, if only reset of additional buffers set m_cResetStartedAddBuffer.
		virtual void startResetTriggerRecordStructure(int inspectionPos = m_cResetStartedAddBuffer) = 0;

		/// Finished the reset of the trigger record structure and create the new memory structure.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		virtual void finishResetTriggerRecordStructure() = 0;

		/// Set the TRC to global initialization, that means with changing of the TRC-structure the images (and its memory) will not be changed, but first with finishGlobalInit. 
		/// This increase the performance, if it will be initialized more IPs.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		virtual void setGlobalInit() = 0;

		/// It finished the global initialization, that means it will change the images and its memory. 
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		virtual void finishGlobalInit() = 0;

		/// Change the bufferStruct of the buffer, if for this image no entry available it will be added. It must not be in the reset state (called startResetTriggerRecordStructure before.)
		/// ATTENTION: All old Tr-instances of all IPs have to be deleted before.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param rImageId [in] Guid of the image.
		/// \param rBufferStruct [in] structure of the buffer of this image.
		/// \returns int Return the position in the list. May use it for getImage or createNewImageHandle.
		virtual int addOrChangeImage(const GUID& rImageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos = -1 ) = 0;
		
		/// Change the bufferStruct of the buffer, if for this image no entry available it will be added. It must not be in the reset state (called startResetTriggerRecordStructure before.)
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param rImageId [in] Guid of the image.
		/// \param rParentId [in] Guid of the parent image.
		/// \param rBufferStruct [in] structure of the buffer of this image.
		/// \returns int Return the position in the child list. May use it for getChildImage.
		virtual int addOrChangeChildImage(const GUID& rImageId, const GUID& rParentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos = -1) = 0;

		/// Add additional image buffer independent of inspections.  
		/// ATTENTION: All old Tr-instances of all IPs have to be deleted before.
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param ownerID [in] Guid of the owner of this buffers.
		/// \param bufferStruct [in] structure of the buffer.
		/// \param numberOfBuffers [in] Number of buffers to be added.
		/// \param clearBuffer [in] If true, it clear all buffer of this owner before it add the new one
		virtual void addImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers, bool clearBuffer = false) = 0;

		/// Remove additional image buffer of an owner (independent of inspections). The buffers numbers won't be fit immediately but with the next finishResetTriggerRecordStructure (for any inspection).  
		/// \param ownerID [in] Guid of the owner of this buffers.
		/// \param bufferStruct [in] structure of the buffer.
		/// \returns bool
		virtual bool removeImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct) = 0;

		/// Remove all additional image buffer of an owner (independent of inspections). The buffers numbers won't be fit immediately but with the next finishResetTriggerRecordStructure (for any inspection).  
		/// \param ownerID [in] Guid of the owner of this buffers.
		/// \returns bool
		virtual bool removeAllImageBuffer(const GUID& ownerID) = 0;

		/// Remove all additional image buffer of all owners (independent of inspections). The buffers numbers won't be fit immediately but with the next finishResetTriggerRecordStructure (for any inspection).  
		/// \returns bool
		virtual bool removeAllImageBuffer() = 0;

		/// Change of inspection data definition list
		/// ATTENTION: In error case the method throw an exception of the type SvStl::MessageContainer.
		/// \param rDataDefList [in] Reference to the data definition list (Move semantics!)
		/// \param valueObjectMemSize [in] Value object list memory size
		/// \param inspectionPos [in] Position (in the inspection list) of the inspection
		virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, long valueObjectMemSize, int inspectionPos = -1) = 0;

		/// Locked all reset-Methods in this controller. (e.g. In Run-Mode there should no reset possible). Is not possible if reset already started.
		/// \returns bool Return the state if reset locked. Normally it returns true, only if reset is already started the lock is not possible and it returns false.
		virtual bool lockReset() = 0;

		/// Unlocked all reset-Methods in this controller.
		virtual void unlockReset() = 0;

		/// Return if reset-Methods in this controller is locked.
		/// \returns bool
		virtual bool isResetLocked() const = 0;

		/// Return if Reset is started.
		/// \returns bool
		virtual bool isResetStarted() const = 0;

		static const int m_cResetStartedAddBuffer = -2;

		static const int cMaxTriggerRecords = 400;

		static const int cMaxTriggerRecordsOfInterest = 50;
	};

	/// Get an instance of the TR controller in read/write modus.
	/// \returns ITriggerRecordControllerRW&
	ITriggerRecordControllerRW& getTriggerRecordControllerRWInstance();
} //namespace SvTrc
