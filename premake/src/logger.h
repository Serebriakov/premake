#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <string>


/*
Class that can log events into different streams
The class cannot be instantiated (singleton)
*/
class Logger
{
private :
	std::ostream				*m_DefaultTargetStream;							// pointer to the default stream to log into

	Logger();
	Logger(const Logger &);
	Logger &operator=(const Logger &);


public :
	// get the one and only instance
	static Logger &Instance();

	// log a string into the default stream
	void Log(const std::string &String, bool WriteTimestamp = true);

	// log a string into a specific stream
	void LogInto(std::ostream &Stream, const std::string &String, bool WriteTimestamp = true);

	// change the default logging location
	void SetDefaultLogTarget(std::ostream &Target);
};

#endif
