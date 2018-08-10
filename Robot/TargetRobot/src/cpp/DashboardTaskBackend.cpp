#include "DashboardTask.h"
#include "robotlib/RobotDataStream.h"
#ifdef WIN32
#include "FRCWindowsRobot.h"
#endif
#include <iostream>
#include <fstream>

bool initialized = false;
std::ofstream file;
bool receivedheader = false;

void initializeFile()
{
	file.open("output.txt");
}

SerialActionRunner DashboardTask::ActionRunner("DashboardRunner", 200);



DashboardOSCMessage::DashboardOSCMessage(std::string address, std::string value)
{
	Address = address;
	StringValue = value;
}

DashboardOSCMessage::DashboardOSCMessage(std::string address, int value)
{
	Address = address;
	IntValue = value;
}

DashboardOSCMessage::DashboardOSCMessage(std::string address, double value)
{
	Address = address;
	DoubleValue = value;
}

DashboardOSCAction::DashboardOSCAction(DashboardOSCMessage message)
{
	MessageContents = message;
}

bool DashboardOSCAction::isFinished()
{
	return ranOnce;
}

void DashboardOSCAction::update()
{
	DashboardTask::ProcessOscMessage(MessageContents);
	ranOnce = true;
}

void DashboardOSCAction::done()
{

}

void DashboardOSCAction::start()
{

}

std::vector<std::string> DashboardOSCAction::getName()
{
	std::vector<std::string> temp;
	temp.push_back("ProcessOSC");
	return temp;
}

class DashboardPacketListener : public osc::OscPacketListener
{
protected:

	virtual void ProcessMessage(const osc::ReceivedMessage& message,
		const IpEndpointName& remoteEndpoint)
	{
		(void)remoteEndpoint;

		try {

			//osc::ReceivedMessageArgument; 
			osc::ReceivedMessageArgumentIterator arg = message.ArgumentsBegin();

			std::string address(message.AddressPattern());

			if (address == "/LogHeader")
			{
				if (!receivedheader)
				{
					initializeFile();
					bool firstpassed = false;
					while (arg != message.ArgumentsEnd())
					{
						if (firstpassed)
							file << ",";
						file << arg->AsString();
						firstpassed = true;
						arg++;
					}
					file << std::endl;
					receivedheader = true;
					std::cout << "Got Header. " << std::endl;
				}
			}
			else if (address == "/LogData")
			{
				if (receivedheader) {
					bool firstpassed = false;
					while (arg != message.ArgumentsEnd())
					{
						if (firstpassed)
							file << ",";
						file << arg->AsDouble();
						firstpassed = true;
						arg++;
					}
					file << std::endl;
				}
				else
				{
				}
			}

			//DashboardTask::ProcessOscData(message);
		}
		catch (...) {
			std::stringstream output;
			output << "Failed to process OSC message" << message.AddressPattern();
			RobotReporter::LogMessage(RobotReporter::Error, output.str());
		}
	}
};

void DashboardTask::ProcessOscData(const osc::ReceivedMessage& message)
{
	//osc::ReceivedMessageArgument; 
	osc::ReceivedMessageArgumentIterator arg = message.ArgumentsBegin();

	std::string address(message.AddressPattern());

	switch (arg->TypeTag())
	{
	case osc::INT32_TYPE_TAG:
	{
		DashboardOSCMessage DashboardMessage(address, (int) arg->AsInt32());
		return;
	}
	break;
	case osc::DOUBLE_TYPE_TAG:
	{
		DashboardOSCMessage DashboardMessage(address, arg->AsDouble());
		return;
	}
	break;
	case osc::STRING_TYPE_TAG:
	{
		std::string value(arg->AsString());
		DashboardOSCMessage DashboardMessage(address, value);
		ProcessOscData(DashboardMessage);
		return;
	}
	break;
	}

	RobotReporter::LogMessage(RobotReporter::Error, "Failed to process OSC message " + address);
}

void DashboardTask::ProcessOscData(DashboardOSCMessage message)
{
	frc::ProcessSimulationOscData(message);
	ActionRunner.queueAction(new DashboardOSCAction(message));
}

void DashboardTask::StartOSCListener()
{
	std::thread * oscReceiveThread = new std::thread(&RunOSCListener);
}

void DashboardTask::RunOSCListener()
{
	DashboardPacketListener listener;
	UdpListeningReceiveSocket s(
		IpEndpointName(IpEndpointName::ANY_ADDRESS, BROADCASTPORT),
		&listener);

	s.RunUntilSigInt();
}