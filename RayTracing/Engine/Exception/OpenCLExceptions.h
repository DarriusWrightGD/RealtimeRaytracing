#pragma once
#include <string>
#include <iostream>

//#define FILEINFO(fileLine,fileName) std::string returnString; returnString.append("Line : ").(fileLine).append(",FileName : ").append(fileName) return returnString;
//#define GETFILEINFO FILEINFO(__LINE__,__FILE__)


static inline void errCheck(int errCode, std::string additionalInfo)
{

	//std::string finalS = std::string("Line : " __LINE__).append(__LINE__);

	std::string errResult;
	if(errCode < 0)
	{

		switch(errCode)
		{
		case -1:
			errResult = "CL_DEVICE_NOT_FOUND";
			break;
		case -2:
			errResult = "CL_DEVICE_NOT_AVAILABLE";
			break;
		case -3:
			errResult = "CL_COMPILER_NOT_AVAILABLE";
			break;
		case -4:
			errResult = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			break;
		case -5:
			errResult = "CL_OUT_OF_RESOURCES";
			break;
		case -6:
			errResult = "CL_OUT_OF_HOST_MEMORY";
			break;
		case -7:
			errResult = "CL_PROFILING_INFO_NOT_AVAILABLE";
			break;
		case -8:
			errResult = "CL_MEM_COPY_OVERLAP ";
			break;
		case -9:
			errResult = "CL_IMAGE_FORMAT_MISMATCH ";
			break;
		case -10:
			errResult = "CL_IMAGE_FORMAT_NOT_SUPPORTED ";
			break;
		case -11:
			errResult = "CL_IMAGE_FORMAT_NOT_SUPPORTED  ";
			break;
		case -12:
			errResult = "CL_MAP_FAILURE  ";
			break;
		case -30:
			errResult = "CL_INVALID_VALUE  ";
			break;
		case -31:
			errResult = "CL_INVALID_DEVICE_TYPE  ";
			break;
		case -32:
			errResult = "CL_INVALID_PLATFORM  ";
		case -33:
			errResult = "CL_INVALID_DEVICE  ";
			break;
		case -34:
			errResult = "CL_INVALID_CONTEXT  ";
			break;
		case -35:
			errResult = "CL_INVALID_QUEUE_PROPERTIES  ";
			break;
		case -36:
			errResult = "CL_INVALID_COMMAND_QUEUE  ";
			break;
		case -37:
			errResult = "CL_INVALID_HOST_PTR  ";
			break;
		case -38:
			errResult = "CL_INVALID_MEM_OBJECT  ";
			break;
		case -39:
			errResult = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR  ";
			break;
		case -40:
			errResult = "CL_INVALID_IMAGE_SIZE  ";
			break;
		case -41:
			errResult = "CL_INVALID_SAMPLER  ";
			break;
		case -42:
			errResult = "CL_INVALID_BINARY ";
			break;
		case -43:
			errResult = "CL_INVALID_BUILD_OPTIONS  ";
		case -44:
			errResult = "CL_INVALID_PROGRAM  ";
			break;
		case -45:
			errResult = "CL_INVALID_PROGRAM_EXECUTABLE  ";
			break;
		case -46:
			errResult = "CL_INVALID_KERNEL_NAME  ";
			break;
		case -47:
			errResult = "CL_INVALID_KERNEL_DEFINITION  ";
			break;
		case -48:
			errResult = "CL_INVALID_KERNEL  ";
			break;
		case -49:
			errResult = "CL_INVALID_ARG_INDEX ";
			break;
		case -50:
			errResult = "CL_INVALID_ARG_VALUE  ";
			break;
		case -51:
			errResult = "CL_INVALID_ARG_SIZE  ";
			break;
		case -52:
			errResult = "CL_INVALID_KERNEL_ARGS  ";
		case -53:
			errResult = "CL_INVALID_WORK_DIMENSION  ";
			break;
		case -54:
			errResult = "CL_INVALID_WORK_GROUP_SIZE  ";
			break;
		case -55:
			errResult = "CL_INVALID_WORK_ITEM_SIZE  ";
			break;
		case -56:
			errResult = "CL_INVALID_GLOBAL_OFFSET  ";
			break;
		case -57:
			errResult = "CL_INVALID_EVENT_WAIT_LIST  ";
			break;
		case -58:
			errResult = "CL_INVALID_EVENT ";
			break;
		case -59:
			errResult = "CL_INVALID_OPERATION  ";
			break;
		case -60:
			errResult = "CL_INVALID_GL_OBJECT  ";
			break;
		case -61:
			errResult = "CL_INVALID_BUFFER_SIZE  ";
			break;
		case -62:
			errResult = "CL_INVALID_MIP_LEVEL  ";
		case -63:
			errResult = "CL_INVALID_GLOBAL_WORK_SIZE  ";
			break;
		}

		std::cout<< additionalInfo << " : " << errResult << ", " << errCode << std::endl;

#ifdef _DEBUG || DEBUG
		system("pause");
#endif

		exit(1);
	}
}
class OpenCLExceptions
{
public:
	OpenCLExceptions(void);
	~OpenCLExceptions(void);

};


