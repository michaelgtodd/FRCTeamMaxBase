#include "Robot.h"
#include "robotlib/RobotTask.h"
#include "robotlib/RobotDataStream.h"
#include "robotlib/RobotAutonomous.h"
#include "SampleAutonomous.h"
#include "robotlib/PowerReportingTask.h"
#include "iostream"

void Robot::RobotInit() 
{
	std::cout << "Initializing Robot..." << std::endl;

	MaxLog::InitializeMaxLog();

	MaxAutonomousManagerInstance.RegisterAutonomous(new SampleAutonomous);

	// Task names cannot contain spaces at this time
	taskschedule.AddTask((RobotTask*)&ControlTaskInstance, "ControlTask", 100);
	taskschedule.AddTask((RobotTask*)&MaxAutonomousManagerInstance, "AutoManager", 100);
	taskschedule.AddTask(new PowerReportingTask(), "PowerReporting", 20);

	taskschedule.LaunchTasks();
}

void Robot::Disabled() { }
void Robot::Autonomous() { }
void Robot::OperatorControl() { }
void Robot::Test() { }

#ifndef WIN32
START_ROBOT_CLASS(Robot)
#endif