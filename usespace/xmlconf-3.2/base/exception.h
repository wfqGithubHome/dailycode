#ifndef _PEXCEPTION_
#define _PEXCEPTION_

#include <string>
#include <stdarg.h>
#include "../global.h"
//using namespace std;
/**
 * Generic exception class
 */
namespace maxnet{
class ZEROException
{
  public:
    /// Default Constructor
    ZEROException();
    
    /// Constructor
    ZEROException(char *ErrMSG);
	
	/// Constructor
    ZEROException(int ErrCode);

	/// Constructor
	ZEROException(int ErrCode, char *ErrMSG);

    /// Destructor
    virtual ~ZEROException();

    /// return error message
    virtual std::string getMessage(); 

	virtual int	   getCode();
    
  protected:
    /// A variable to contain a string error message.
	std::string 	m_strDebugMessage;
	int			m_nErrorCode;
    
};


/**
 * Class representing a Database specific error
 */
class DBException : public ZEROException
{
  public:
    /// default constructor
    DBException();
    /// constructor taking an error string argument
    DBException(char *ErrMSG);
	/// Constructor
    DBException(int ErrCode);
};

/**
 * Class representing a Database specific error
 */
class SocketException : public ZEROException
{
  public:
    /// default constructor
    SocketException();
    /// constructor taking an error string argument
    SocketException(char *ErrMSG);
	/// Constructor
    SocketException(int ErrCode);
};

/**
 * Class representing a Database specific error
 */
class BillException : public ZEROException
{
  public:
    /// default constructor
    BillException();
    /// constructor taking an error string argument
    BillException(char *ErrMSG);
	/// Constructor
    BillException(int ErrCode);
};

/**
 * Class representing a Database specific error
 */
class IptablesException : public ZEROException
{
  public:
    /// default constructor
    IptablesException();
    /// constructor taking an error string argument
    IptablesException(char *ErrMSG);

	IptablesException(const char* fmt, ...);
	/// Constructor
    IptablesException(int ErrCode);
};

}
#endif //_PEXCEPTION_
