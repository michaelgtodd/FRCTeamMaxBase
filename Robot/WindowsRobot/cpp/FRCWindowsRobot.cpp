#pragma once

#include "FRCWindowsRobot.h"

std::vector<frc::JoystickData> frc::JoystickManager::Joysticks;
bool frc::JoystickManager::initialized;
HANDLE frc::WindowsRobotMutex::mutex;

void frc::ProcessSimulationOscData(DashboardOSCMessage message)
{
	if (message.Address == "/robot1/input/JoyLeft")
	{
		frc::WindowsRobotMutex::Lock();
		frc::JoystickData joystick = frc::JoystickManager::GetJoystickData(1);
		joystick.Y = message.DoubleValue;
		frc::JoystickManager::SetJoystickData(1, joystick);
		frc::WindowsRobotMutex::Unlock();
	}
	else if (message.Address == "/robot1/input/JoyRight")
	{
		frc::WindowsRobotMutex::Lock();
		frc::JoystickData joystick = frc::JoystickManager::GetJoystickData(1);
		joystick.Z = message.DoubleValue;
		frc::JoystickManager::SetJoystickData(1, joystick);
		frc::WindowsRobotMutex::Unlock();
	}
}

void frc::WindowsRobotMutex::Lock()
{
	WaitForSingleObject(mutex, INFINITE);
}

void frc::WindowsRobotMutex::Unlock()
{
	ReleaseMutex(mutex);
}

bool RobotState::IsOperatorControl()
{
	return false;
}

bool RobotState::IsEnabled()
{
	return false;
}

bool RobotState::IsAutonomous()
{
	return false;
}

frc::Joystick::Joystick(int number)
{
	JoystickNumber = number;
}

double frc::Joystick::GetX()
{
	return frc::JoystickManager::GetJoystickX(JoystickNumber);
}

double frc::Joystick::GetY()
{
	return frc::JoystickManager::GetJoystickY(JoystickNumber);
}

double frc::Joystick::GetZ()
{
	return frc::JoystickManager::GetJoystickZ(JoystickNumber);
}

double frc::JoystickManager::GetJoystickX (int Joystick)
{
	initialize();
	frc::WindowsRobotMutex::Lock();
	double temp = Joysticks[0].X;
	frc::WindowsRobotMutex::Unlock();
	return temp;
}

double frc::JoystickManager::GetJoystickY(int Joystick)
{
	initialize();
	frc::WindowsRobotMutex::Lock();
	double temp = Joysticks[0].Y;
	frc::WindowsRobotMutex::Unlock();
	return temp;
}

double frc::JoystickManager::GetJoystickZ(int Joystick)
{
	initialize();
	frc::WindowsRobotMutex::Lock();
	double temp = Joysticks[0].Z;
	frc::WindowsRobotMutex::Unlock();
	return temp;
}

void  frc::JoystickManager::SetJoystickData(int Joystick, frc::JoystickData data)
{
	initialize();
	frc::WindowsRobotMutex::Lock();
	Joysticks[0] = data;
	frc::WindowsRobotMutex::Unlock();
}

frc::JoystickData frc::JoystickManager::GetJoystickData(int Joystick)
{
	initialize();
	frc::WindowsRobotMutex::Lock();
	JoystickData temp = Joysticks[0];
	frc::WindowsRobotMutex::Unlock();
	return temp;
}

void frc::JoystickManager::initialize() 
{
	frc::WindowsRobotMutex::Lock();
	if (!initialized)
	{
		JoystickData temp;
		Joysticks.push_back(temp);
		initialized = true;
	}
	frc::WindowsRobotMutex::Unlock();
}