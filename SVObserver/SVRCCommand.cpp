#include "stdafx.h"
#include "SVRCCommand.h"
#include "SVVisionProcessorHelper.h"
#include "SVStatusLibrary\SVSVIMStateClass.h"
#include "SVProtoBuf\envelope.pb.h"


SVRCCommand::SVRCCommand()
{
}
SVRCCommand::~SVRCCommand()
{
}

 

void SVRCCommand::GetDeviceMode(const SvPb::GetDeviceModeRequest& request , SvRpc::Task<SvPb::GetDeviceModeResponse> task)
{

	SvPenv::Error err;
	SvPb::GetDeviceModeResponse resp;

	if (GetDeviceMode(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}
void SVRCCommand::SetDeviceMode(const SvPb::SetDeviceModeRequest& request, SvRpc::Task<SvPb::SetDeviceModeResponse> task)
{
	SvPenv::Error err;
	SvPb::SetDeviceModeResponse resp;

	if (SetDeviceMode(request, resp, err))
	{
		task.finish(std::move(resp));
	}
	else
	{
		task.error(err);
	}
}

bool SVRCCommand::GetDeviceMode(const SvPb::GetDeviceModeRequest& request, SvPb::GetDeviceModeResponse& resp, SvPenv::Error& err)
{
	bool res {true};
	
	unsigned long Mode = SVIM_MODE_UNKNOWN;

	 SVVisionProcessorHelper::Instance().GetConfigurationMode(Mode);
	 switch (Mode)
	 {
		 
		 case SVIM_MODE_ONLINE:
			 resp.set_mode(SvPb::DeviceModeType::RunMode);
			 break;
		 case SVIM_MODE_OFFLINE:
			 resp.set_mode(SvPb::DeviceModeType::StopMode);
			 break;
		 case SVIM_MODE_REGRESSION:
			 resp.set_mode(SvPb::DeviceModeType::RegressionMode);
			 break;
		 case SVIM_MODE_TEST:
			 resp.set_mode(SvPb::DeviceModeType::TestMode);
			 break;

		 case SVIM_MODE_EDIT:
			 resp.set_mode(SvPb::DeviceModeType::EditMode);
			 break;

		 case SVIM_MODE_CHANGING:
			 resp.set_mode(SvPb::DeviceModeType::ModeChanging);
			 break;
		 
		///or set err and return False???
		 case SVIM_MODE_UNKNOWN:
		 default:
			 resp.set_mode(SvPb::DeviceModeType::Available);
			 break;

	 }

	return res;

}

bool SVRCCommand::SetDeviceMode(const SvPb::SetDeviceModeRequest& request, SvPb::SetDeviceModeResponse& response, SvPenv::Error& err)
{
	
	bool result(false);
	SvPb::DeviceModeType mode = request.mode();

	unsigned long DesiredMode = SVIM_MODE_UNKNOWN;
	switch (mode)
	{
		case SvPb::DeviceModeType::RunMode:
			DesiredMode = SVIM_MODE_ONLINE;
			break;
		case SvPb::DeviceModeType::StopMode:
			DesiredMode = SVIM_MODE_OFFLINE;
			break;
		case SvPb::DeviceModeType::RegressionMode:
			DesiredMode = SVIM_MODE_REGRESSION;
			break;
		case SvPb::DeviceModeType::TestMode:
			DesiredMode = SVIM_MODE_TEST;
			break;
		case SvPb::DeviceModeType::EditMode:
			DesiredMode = SVIM_MODE_EDIT;
			break;
		default:
			DesiredMode = SVIM_MODE_UNKNOWN;
			break;
	}


	HRESULT status {E_INVALIDARG};

	if (DesiredMode != SVIM_MODE_UNKNOWN)
	{
		status = SVVisionProcessorHelper::Instance().SetConfigurationMode(DesiredMode);
	}
	
	switch (status)
	{

		case S_OK:
		
			response.set_new_mode(mode);
			//@Todo[MEC][8.00] [13.03.2018] Set old mode??
			result = true;
			break;
		
		case E_INVALIDARG:
			err.set_error_code(SvPenv::ErrorCode::BadRequest);
			err.set_message("Invalid Mode");
			break;
		case SVMSG_64_SVIM_MODE_NOT_REMOTELY_SETABLE:
			err.set_error_code(SvPenv::ErrorCode::Forbidden);
			err.set_message("Mode not remotely setable");
			break;
		case 	SVMSG_63_SVIM_IN_WRONG_MODE:
			err.set_error_code(SvPenv::ErrorCode::Forbidden);
			err.set_message("SVIM in wrong mode");
			break;

		default:
			std::stringstream ssmsg;
			ssmsg << "Mode not setable " << status;
			err.set_error_code(SvPenv::ErrorCode::InternalError);
			err.set_message(ssmsg.str());
			break;
	}
	return result;
}