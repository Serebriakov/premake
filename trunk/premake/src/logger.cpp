#include <ctime>
#include <cstring>

#include "logger.h"


Logger::Logger() : m_DefaultTargetStream(&std::cout)
{
}

/*
Return the one and only instance of the class
*/
Logger &Logger::Instance()
{
	static Logger instance;
	return instance;
}

/*
Log a string into the default output stream
*/
void Logger::Log(const std::string &String, bool WriteTimestamp)
{
	LogInto(*m_DefaultTargetStream, String, WriteTimestamp);
}

/*
Log a string into a specific stream
*/
void Logger::LogInto(std::ostream &Stream, const std::string &String, bool WriteTimestamp)
{
	if( WriteTimestamp )
	{
		time_t t;
		time(&t);
		char *time_str = ctime(&t);
		// overwrite the '\n' character at the end of the string returned by ctime
		time_str[strlen(time_str)-1] = '\0';
		Stream << "[" << time_str << "] ";
	}

	Stream << String << std::endl;
	Stream.flush();
}

/*
Change the default logging location
*/
void Logger::SetDefaultLogTarget(std::ostream &Target)
{
	m_DefaultTargetStream = &Target;
}
