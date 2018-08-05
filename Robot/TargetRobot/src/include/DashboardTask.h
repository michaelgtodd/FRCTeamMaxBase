#pragma once
#include "robotlib/RobotTask.h"
#include "robotlib/RobotAction.h"
#include "robotlib/osc/OscTypes.h"
#include "robotlib/osc/OscReceivedElements.h"

class DashboardOSCMessage
{
	friend class DashboardOSCAction;
public:
	enum DashboardOSCMessageType { DoubleType, StringType, IntType };
	DashboardOSCMessage() = default;
	DashboardOSCMessage(std::string address, std::string value);
	DashboardOSCMessage(std::string address, int value);
	DashboardOSCMessage(std::string address, double value);
	std::string Address = "NONE";
	DashboardOSCMessageType MessageType = StringType;
	std::string StringValue = "NONE";
	double DoubleValue = 0.0;
	int IntValue = 0;
};

class DashboardOSCAction : public RobotAction
{
public:
	DashboardOSCAction(DashboardOSCMessage message);
	bool isFinished();
	void update();
	void done();
	void start();

	std::vector<std::string> getName();
private:
	DashboardOSCAction();
	DashboardOSCMessage MessageContents;
	bool ranOnce = false;
};

class DashboardTask : public RobotTask
{
public:
	void Always();
	void Run();
	void Disable();
	void Autonomous();
	void Start();
	void Init();

	static void ProcessOscData(const osc::ReceivedMessage& m);
	static void ProcessOscData(DashboardOSCMessage message);

	static void ProcessOscMessage(DashboardOSCMessage message);

private:
	static void StartOSCListener();
	static void RunOSCListener();
	static SerialActionRunner ActionRunner;
};