// WindowsRobot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Robot.h"
#include <iostream>
#include <thread>
#include <chrono>

#define WINDOWS_ROBOT

int main()
{
	Robot * robot = new Robot();
	robot->RobotInit();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

    return 0;
}

