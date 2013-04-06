/*************************************************************************
	> File Name: ConnectionException.cpp
# Author: rsq
# mail: rsqmail@163.com
	> Created Time: 2013年04月06日 星期六 11时10分24秒
 ************************************************************************/
#include "Connection.h"
ConnectionException::ConnectionException(const char *exceptStr) : 
	std::exception(),
	mExceptionStr(exceptStr)
{
}
ConnectionException::~ConnectionException() throw()
{
}
//----------------------------------------------------------------
const char* ConnectionException::what() const throw()
{
	return mExceptionStr.c_str();
}
