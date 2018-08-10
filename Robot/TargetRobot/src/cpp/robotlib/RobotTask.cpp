#include "robotlib/RobotTask.h"
#ifndef WIN32
#include "WPILib.h"
#include "pthread.h"
#else
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#endif
#include "robotlib/RobotDataStream.h"
#include <math.h>
#include "Robot.h"
#include "robotlib/TaskMetricsTask.h"
#include "robotlib/ActionMetricsTask.h"
#include <sstream>
#include "DashboardTask.h"
#include "RobotLoggerTask.h"

std::vector<RobotTask*> TaskSchedule::TaskList;

void TaskMetricsData::ResetRuns()
{
	Lock();
	runCounter = 0;
	Unlock();
}

uint32_t TaskMetricsData::GetRuns()
{
	Lock();
	uint32_t temp = runCounter;
	Unlock();
	return temp;
}

void TaskMetricsData::IncrementRuns()
{
	Lock();
	runCounter++;
	Unlock();
}

void TaskMetricsData::SetPeriod(uint32_t period)
{
	Lock();
	targetPeriod = period;
	Unlock();
}

uint32_t TaskMetricsData::GetPeriod()
{
	Lock();
	uint32_t temp = targetPeriod;
	Unlock();
	return temp;
}

void TaskSchedule::AddTask(RobotTask* task, std::string taskname, uint32_t period)
{
	TaskList.push_back(task);
	task->ExecInit(taskname, period);
}

void TaskSchedule::SetTaskPriorty(int priority, std::string taskname)
{
#ifndef WIN32
	sched_param sch;
	sch.sched_priority = priority;

	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch) != 0)
	{
		std::stringstream output;
		output << "Failed to set task: " << taskname << " priority: " << priority << " Error: " << " " << strerror(errno) << std::endl;
		RobotReporter::LogMessage(RobotReporter::Error, output.str());
	}
	else {
		std::stringstream output;
		output << "Set priority for task: " << taskname << " priority: " << priority << std::endl;
		RobotReporter::LogMessage(RobotReporter::Error, output.str());
	}
#endif
}

void TaskSchedule::SetTaskPriorty(int priority, std::string taskname, std::thread * thread)
{

#ifndef WIN32
	sched_param sch;
	sch.sched_priority = priority;

	if (pthread_setschedparam(thread->native_handle(), SCHED_FIFO, &sch) != 0)
	{
		std::stringstream output;
		output << "Failed to set task: " << taskname << " priority: " << priority << " Error: " << " " << strerror(errno) << std::endl;
		RobotReporter::LogMessage(RobotReporter::Error, output.str());
	}
	else {
		std::stringstream output;
		output << "Set priority for task: " << taskname << " priority: " << priority << std::endl;
		RobotReporter::LogMessage(RobotReporter::Error, output.str());
	}
#endif
}

void TaskSchedule::AddLibraryTasks()
{
	TaskContainer * LoggingSuperLoop = new TaskContainer();
	LoggingSuperLoop->AddTask(new RobotLoggerTask(), "RobotLogger");
	LoggingSuperLoop->AddTask(new RobotReporter(), "RobotReporter");
	TaskSchedule::AddTask(LoggingSuperLoop, "LoggingSuperLoop", 10);

	TaskContainer * DashboardSuperLoop = new TaskContainer();
	DashboardSuperLoop->AddTask(new DashboardTask(), "DashboardTask");
	DashboardSuperLoop->AddTask(new OSCReporter(), "OSCReporter");
	TaskSchedule::AddTask(DashboardSuperLoop, "DashboardSuperLoop", 100);

#if defined(TASK_METRICS) || defined (ACTION_METRICS)
	TaskContainer * MetricsSuperLoop = new TaskContainer();
#endif
#ifdef TASK_METRICS
	MetricsSuperLoop->AddTask(new TaskMetricsTask(), "TaskMetricsTask");
#endif

#ifdef ACTION_METRICS
	MetricsSuperLoop->AddTask(new ActionMetricsTask(), "ActionMetricsTask");
#endif
#if defined(TASK_METRICS) || defined (ACTION_METRICS)
	TaskSchedule::AddTask(MetricsSuperLoop, "MetricsSuperLoop", 1);
#endif
}

void TaskSchedule::LaunchTasks()
{
	TaskSchedule::AddLibraryTasks();

	TaskSchedule::SetTaskPriorty(99, "DS_Task");

	int priority = 80;
	for (std::vector<RobotTask*>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		RobotReporter::LogMessage(RobotReporter::Pass, "Launching: " + (*i)->GetTaskName());
		(*i)->Launch(priority);
		priority--;
	}
}

std::string RobotTask::GetTaskName()
{
	return taskname_;
}

uint32_t RobotTask::GetTaskPeriod()
{
	return task_period_;
}

void RobotTask::ExecInit(std::string taskname, uint32_t task_period)
{
	taskname_ = taskname;
	uint32_t capped_period = std::min(task_period, (uint32_t) 1000);
	capped_period = std::max((uint32_t) 1, capped_period);
	task_period_ = capped_period;

#ifdef TASK_METRICS
	taskMetricsData = new TaskMetricsData();
	taskMetricsData->SetPeriod(task_period_);
	DataStore::RegisterDataItem("/taskmetrics/" + taskname, (DataItem *)taskMetricsData);
#endif
	Init();
}

uint32_t RobotTask::getTimeMS()
{
#ifndef WIN32
	return (uint32_t)(Timer::GetFPGATimestamp() * 1000.0);
#else
	return timeGetTime();
#endif
}

void RobotTask::Launch(int priority)
{
	Start();
	lastLoopEndTargetMS = getTimeMS();
	running_thread = std::thread(&RobotTask::ThreadProcess, this);
	TaskSchedule::SetTaskPriorty(priority, taskname_, &running_thread);
}

#ifdef TASK_METRICS
void RobotTask::OverridePeriod(uint32_t task_period)
{
	taskMetricsData->SetPeriod(task_period);
}
#endif

void RobotTask::ThreadProcess()
{
	while (true)
	{
		uint32_t loopStartMS = getTimeMS();
		
		if (RobotState::IsOperatorControl() && RobotState::IsEnabled())
		{
			Run();
		}
		else if (RobotState::IsAutonomous() && RobotState::IsEnabled())
		{
			Autonomous();
		}
		else
		{
			Disable();
		}
		Always();

#ifdef TASK_METRICS
		taskMetricsData->IncrementRuns();
#endif

		// Note this code will fail after about 590 hours
		// FRC robots don't expect to operate that long, and it's more efficient to be less defensive
		uint32_t targetLoopEnd = lastLoopEndTargetMS + (int32_t)(1000 / task_period_);
		lastLoopEndTargetMS = targetLoopEnd;
		uint32_t thisLoopEnd = getTimeMS();

		uint32_t minsleep = std::max((int32_t)(targetLoopEnd - thisLoopEnd), (int32_t) 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(minsleep));
	}
}

void TaskContainer::AddTask(RobotTask * Task, std::string taskname)
{
	Task->ExecInit(taskname, 0);
	TaskList.push_back(Task);
}

void TaskContainer::Run()
{
	for (std::vector<RobotTask *>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		(*i)->Run();
		(*i)->Always();
#ifdef TASK_METRICS
		(*i)->taskMetricsData->IncrementRuns();
#endif
	}
	
}

void TaskContainer::Disable()
{
	for (std::vector<RobotTask *>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		(*i)->Disable();
		(*i)->Always();
#ifdef TASK_METRICS
		(*i)->taskMetricsData->IncrementRuns();
#endif
	}
}

void TaskContainer::Always()
{

}

void TaskContainer::Autonomous()
{
	for (std::vector<RobotTask *>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		(*i)->Autonomous();
		(*i)->Always();
#ifdef TASK_METRICS
		(*i)->taskMetricsData->IncrementRuns();
#endif
	}
}

void TaskContainer::Start()
{
	for (std::vector<RobotTask *>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		(*i)->Start();
	}
}

void TaskContainer::Init()
{
	// This intentionally doesn't dispatch to the subtask init routiunes, as they're
	// called from execinit
#ifdef TASK_METRICS
	for (std::vector<RobotTask *>::iterator i = TaskList.begin();
		i != TaskList.end();
		i++)
	{
		(*i)->OverridePeriod(task_period_);
	}
#endif
}