#include "SampleAutonomous.h"
#include "robotlib/RobotDataStream.h"
#include "ControlTask.h"
#include "iostream"
#include <math.h>

void SampleAutonomous::Init()
{
	MaxLog::LogInfo("Starting Auto");

	std::cout << "Auto Init" << std::endl;

#ifndef WIN32
	StartTime = Timer::GetFPGATimestamp();
#else
	StartTime = 0;
#endif
	LastMessage = 0;
}

void SampleAutonomous::ControllerUpdate(MaxControl * controls)
{
	// can get controller data here
}

void SampleAutonomous::Autonomous()
{
#ifndef WIN32
	double CurrentTime = Timer::GetFPGATimestamp();
#else
	double CurrentTime = 0;
#endif
	RunTime = CurrentTime - StartTime;
	if (floor(RunTime) > LastMessage)
	{
		LastMessage = (int) floor(RunTime);
		std::cout << "Running at " << std::to_string(LastMessage) << " seconds" << std::endl;
		MaxLog::LogInfo("Running at " + std::to_string(LastMessage) + " seconds");
	}

	// Send data to the control task to send to other tasks
	ControlTaskInstance.UpdateAutonomousData(control);
}

void SampleAutonomous::End()
{
#ifndef WIN32
	double CurrentTime = Timer::GetFPGATimestamp();
#else
	double CurrentTime = 0;
#endif
	RunTime = CurrentTime - StartTime;
	LastMessage = (int) floor(RunTime);
	MaxLog::LogInfo("Ending Auto after " + std::to_string(LastMessage) + " seconds");
}

std::string SampleAutonomous::GetName()
{
	return "Sample_Auto";
}