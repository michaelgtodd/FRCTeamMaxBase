#include "robotlib\RobotAutonomous.h"
#include "robotlib\RobotDataStream.h"
#ifndef WIN32
#include "Timer.h"
#endif

AutonomousManager AutonomousManagerInstance;

uint32_t AutonomousTask::getTimeMS()
{
#ifndef WIN32
	return (uint32_t)(Timer::GetFPGATimestamp() * 1000.0);
#else
	return timeGetTime();
#endif
}

RegisterAutonomousAction::RegisterAutonomousAction(AutonomousTask * AutonomousTask)
{
	AutonomousTask_ = AutonomousTask;
}

bool RegisterAutonomousAction::isFinished()
{
	return ranOnce;
}

void RegisterAutonomousAction::update()
{
	AutonomousManagerInstance.ProcessRegisterAutonomous(AutonomousTask_);
	ranOnce = true;
}

void RegisterAutonomousAction::done() {}
void RegisterAutonomousAction::start() {}

std::vector<std::string> RegisterAutonomousAction::getName()
{
	std::vector<std::string> myName;
	myName.push_back("RegisterAuto");
	return myName;
}

SelectAutonomousAction::SelectAutonomousAction(std::string AutonomousName)
{
	AutonomousName_ = AutonomousName;
}

bool SelectAutonomousAction::isFinished()
{
	return ranOnce;
}

void SelectAutonomousAction::update()
{
	AutonomousManagerInstance.ProcessSelectAutonomous(AutonomousName_);
	ranOnce = true;
}

void SelectAutonomousAction::done() {}
void SelectAutonomousAction::start() {}

std::vector<std::string> SelectAutonomousAction::getName()
{
	std::vector<std::string> myName;
	myName.push_back("SelectAuto");
	return myName;
}

void AutonomousManager::Run()
{
	EndAutonomous();
}

void AutonomousManager::Always()
{
	Lock();
	ActionRunner.Run();
	Unlock();
}

void AutonomousManager::Disable()
{
	EndAutonomous();
}

void AutonomousManager::Start()
{

}

void AutonomousManager::EndAutonomous()
{
	if (LastStateAutonomous)
	{
		SelectedAutonomous->End();
	}
	LastStateAutonomous = false;
	AutoRunning = false;
}

void AutonomousManager::Autonomous()
{
	AutoRunning = true;
	if (!LastStateAutonomous)
	{
		RobotReporter::LogMessage(RobotReporter::Pass, "Starting auto...");
		SelectedAutonomous->Init();
	}

	SelectedAutonomous->Autonomous();

	LastStateAutonomous = true;
}

void AutonomousManager::RegisterAutonomous(AutonomousTask * AutonomousTask)
{
	ActionRunner.queueAction(new RegisterAutonomousAction(AutonomousTask));
}

void AutonomousManager::SelectAutonomous(std::string AutonomousName)
{
	ActionRunner.queueAction(new SelectAutonomousAction(AutonomousName));
}

void AutonomousManager::ProcessRegisterAutonomous(AutonomousTask * AutonomousTask)
{
	AutoList.push_back(AutonomousTask);
	AutoNameList.push_back(AutonomousTask->GetName());
	if (AutoList.size() == 1)
		SelectedAutonomous = AutonomousTask;
}

void AutonomousManager::ProcessSelectAutonomous(std::string AutonomousName)
{
	if (!AutoRunning)
	{
		for (std::vector<AutonomousTask *>::iterator i = AutoList.begin();
			i != AutoList.end();
			i++)
		{
			if (AutonomousName == (*i)->GetName())
			{
				if (SelectedAutonomous->GetName() != AutonomousName)
				{
					RobotReporter::LogMessage(RobotReporter::Pass, "Selecting " + AutonomousName + " Auto.");
				}
				SelectedAutonomous = (*i);
				return;
			}
		}
	}
	RobotReporter::LogMessage(RobotReporter::Error, "Invalid autonomous " + AutonomousName);
}

void AutonomousManager::Lock()
{
#ifdef WIN32
	WaitForSingleObject(mutex, INFINITE);
#else
	pthread_mutex_lock(&mutex);
#endif
}

void AutonomousManager::Unlock()
{
#ifdef WIN32
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock(&mutex);
#endif
}

std::vector<std::string> AutonomousManager::GetAutonomousList()
{
	Lock();
	std::vector<std::string> result = AutoNameList;
	Unlock();
	return result;
}

std::string AutonomousManager::GetSelectedAuto()
{
	Lock();
	std::string result = SelectedAutonomous->GetName();
	Unlock();
	return result;
}

void AutonomousManager::Init() {}