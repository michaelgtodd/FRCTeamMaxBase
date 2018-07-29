#pragma once

#include "robotlib/RobotTask.h"
#include "robotlib/RobotAction.h"

class ParallelCounterSampleTask : public RobotTask
{
public:
	void Always();
	void Run();
	void Disable();
	void Autonomous();
	void Start();
	void Init();
private:
	ParallelActionRunner ActionRunner;
public:
	ParallelCounterSampleTask() : ActionRunner("Tom's Sample Runner", 15) {}
};