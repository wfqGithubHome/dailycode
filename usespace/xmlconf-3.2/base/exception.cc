#include "exception.h"

namespace maxnet{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZEROException::ZEROException()
{
	m_strDebugMessage = "Generic Error";
}

ZEROException::ZEROException(char *ErrMSG)
{
	m_strDebugMessage = ErrMSG;
}

ZEROException::ZEROException(int ErrCode)
{
	m_nErrorCode = ErrCode;
}

ZEROException::ZEROException(int ErrCode, char *ErrMSG)
{
	m_nErrorCode = ErrCode;
	m_strDebugMessage = ErrMSG;
}


ZEROException::~ZEROException()
{

}

std::string ZEROException::getMessage()
{
	return m_strDebugMessage;
}

int	   ZEROException::getCode()
{
	return 	m_nErrorCode;
}	

DBException::DBException()
{
	m_strDebugMessage = "DB Generic Error";
}

DBException::DBException(char *ErrMSG)
{
	m_strDebugMessage = ErrMSG;
}

DBException::DBException(int ErrCode)
{
	m_nErrorCode = ErrCode;
}

SocketException::SocketException()
{
	m_strDebugMessage = "Socket Generic Error";
}

SocketException::SocketException(char *ErrMSG)
{
	m_strDebugMessage = ErrMSG;
}

SocketException::SocketException(int ErrCode)
{
	m_nErrorCode = ErrCode;
}

BillException::BillException()
{
	m_strDebugMessage = "Socket Generic Error";
}

BillException::BillException(char *ErrMSG)
{
	m_strDebugMessage = ErrMSG;
}

BillException::BillException(int ErrCode)
{
	m_nErrorCode = ErrCode;
}


IptablesException::IptablesException()
{
	m_strDebugMessage = "Iptables Generic Error";
}

IptablesException::IptablesException(char *ErrMSG)
{
	m_strDebugMessage = ErrMSG;
}

IptablesException::IptablesException(int ErrCode)
{
	m_nErrorCode = ErrCode;
}

IptablesException::IptablesException(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char *buf = new char[2048];
	vsnprintf(buf, 2047, fmt, args);
	m_strDebugMessage = buf;
	delete buf;
	buf = NULL;
	va_end(args);

}


}
