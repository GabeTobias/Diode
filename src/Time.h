#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>

namespace Time 
{
	// Settings
	inline double frameRateTarget = 60;
	inline float timeScale = 1.0f;

	// Current Framerate
	inline float frameStartTime = 0;
	inline float DeltaTime = 1.0f;			//TODO: Time::DeltaTime
	inline int frameRate = 0;

	// Limter
	inline auto next_frame = std::chrono::steady_clock::now();

	inline double getTime() { return glfwGetTime(); }

	inline int	  getHours(double time = glfwGetTime()) { return int(time / (60.f * 60.f)) % 60; }
	inline int	  getMinutes(double time = glfwGetTime()) { return int(time / (60.f)) % 60; }
	inline int	  getSeconds(double time = glfwGetTime()) { return int(time) % 60; }
	inline double getMillis(double time = glfwGetTime()) { return time - int(time); }

	inline void StartFrame()
	{
		frameStartTime = glfwGetTime();
		next_frame += std::chrono::milliseconds(int(round(1000.0 / frameRateTarget)));
	}

	inline void EndFrame()
	{
		// wait for end of frame
		std::this_thread::sleep_until(next_frame);

		//Second Timer
		static int second = 0;
		static int frameCount = 0;

		//Store Time
		double time = glfwGetTime();
		frameCount++;

		//Print Framerate to Console
		if (int(time) != second)
		{
			//Time Calculations
			DeltaTime = (time - frameStartTime) / (1.0f / frameRateTarget);
			frameRate = frameCount;
			frameCount = 0;
			
			//Update Timer
			second = int(time);
		}
	}

	class Timer
	{
	public:
		float previousTime = 0;
		
		void reset() 
		{
			previousTime = getTime();
		}

		float delta(float interval) 
		{
			return (getTime() - previousTime) / interval;
		}
		
		bool getInterval(float interval)
		{
			if (getTime() - previousTime > interval)
			{
				previousTime = getTime();
				return true;
			}

			return false;
		}
	};
}