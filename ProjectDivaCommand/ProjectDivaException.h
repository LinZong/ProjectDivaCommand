#pragma once
#include <exception>
class ProjectDivaException :public std::exception
{
public:
	ProjectDivaException():Errormsg((char*)"Unexcepted fatal error happened.")
	{
		
	}
	ProjectDivaException(const char * Emsg) :Errormsg(Emsg)
	{

	}
	virtual const char* what()const throw()
	{
		return Errormsg;
	}
private:
	const char * Errormsg;
};
class InvalidLyricFileException :public ProjectDivaException
{
public:
	InvalidLyricFileException():ProjectDivaException("The lyric file is invalid.")
	{

	}
};
