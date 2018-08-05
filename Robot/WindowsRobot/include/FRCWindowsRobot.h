#pragma once

#include <vector>
#include <Windows.h>
#include "DashboardTask.h"

namespace RobotState 
{
	bool IsOperatorControl();
	bool IsEnabled();
	bool IsAutonomous();
};



namespace frc
{
	void ProcessSimulationOscData(DashboardOSCMessage message);

	class WindowsRobotMutex
	{
	public:
		static void Lock();
		static void Unlock();
	private:
		static HANDLE mutex;
	};

	class SampleRobot
	{
	public:
		void virtual RobotInit() = 0;
		void virtual Disabled() = 0;
		void virtual Autonomous() = 0;
		void virtual OperatorControl() = 0;
		void virtual Test() = 0;
	};

	class JoystickData
	{
	public:
		double X = 0;
		double Y = 0;
		double Z = 0;
	};

	class JoystickManager
	{
	public:
		static double GetJoystickX(int Joystick);
		static double GetJoystickY(int Joystick);
		static double GetJoystickZ(int Joystick);
		static void SetJoystickData(int Joystick, JoystickData Data);
		static JoystickData GetJoystickData(int Joystick);
	private:
		static void initialize();
		static std::vector<frc::JoystickData> Joysticks;
		static bool initialized;
	};

	class Joystick
	{
	public:
		Joystick(int number);
		double GetX();
		double GetY();
		double GetZ();
	private:
		int JoystickNumber = 0;
		Joystick() = default;
	};
}