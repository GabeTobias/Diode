#pragma once

#include <string>
#include <iostream>
#include <time.h>

#include <glm/glm.hpp>

#include "Time.h"
#include <glm\gtx\string_cast.hpp>

namespace _LOG_
{
	inline const char* ExtendStr(int val) { return (val >= 10) ? "" : "0"; }

	inline std::string TimeStampStr(double time)
	{
		//TODO: This seems inefficient and bugy maybe?
		std::string fin = ExtendStr(Time::getMinutes(time)) + std::to_string(Time::getMinutes(time)) + ":";
		fin += ExtendStr(Time::getSeconds(time)) + std::to_string(Time::getSeconds(time)) + ":";
		fin += ExtendStr(Time::getMillis(time) * 100) + std::to_string(int(Time::getMillis(time) * 100));

		return fin;
	}

	inline void Console_Log(std::string str)
	{
		std::string timeStamp = TimeStampStr(Time::getTime());

		std::cout << "|";
		std::cout << timeStamp;
		std::cout << "| " << str << std::endl;
	}

	inline void Console_Log (int val, std::string str = "") { Console_Log(str + ": " + std::to_string(val)); }
	inline void Console_Log (float val, std::string str = "") { Console_Log(str + ": " + std::to_string(val)); }
	inline void Console_Log (double val, std::string str = "") { Console_Log(str + ": " + std::to_string(val)); }

	inline void Console_Log (glm::vec2 val, std::string str = "") { Console_Log(str + ": " + glm::to_string(val)); }
	inline void Console_Log (glm::vec3 val, std::string str = "") { Console_Log(str + ": " + glm::to_string(val)); }
	inline void Console_Log (glm::vec4 val, std::string str = "") { Console_Log(str + ": " + glm::to_string(val)); }
}

#define LOG _LOG_::Console_Log
#define TIMESTMP _LOG_::TimeStampStr#pragma once
