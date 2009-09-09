#include "ScheduledUpdateTask.h"

#pragma comment(lib,"Mstask.lib")
ScheduledUpdateTask::ScheduledUpdateTask(std::wstring path, std::wstring name) 
	: m_command(path)
{
	pITS.CoCreateInstance(CLSID_CTaskScheduler,NULL);
	CComPtr<IUnknown> iUnk;
	pITS->Activate(name.c_str(),IID_ITask,&iUnk);
	if (!iUnk) {
		pITS->NewWorkItem(name.c_str(),CLSID_CTask,IID_ITask,&iUnk);
		if (!iUnk) 
			return;
		}
	pITask = iUnk;
	pIPersistFile = iUnk;
}

bool ScheduledUpdateTask::configure(Interval interval) {
	if (!pITask || !pIPersistFile) 
		return false;
	OSVERSIONINFO version = {sizeof(OSVERSIONINFO)};
	GetVersionEx(&version);

	//conf main task
	pITask->SetComment(L"Id-updater scheduled task");
	pITask->SetApplicationName(m_command.c_str());
	pITask->SetAccountInformation(L"",NULL);
	if (version.dwMajorVersion <= 5)
		pITask->SetFlags(TASK_FLAG_INTERACTIVE);
	if (interval == NEVER) 
		pITask->SetFlags(TASK_FLAG_DISABLED);

	//configure trigger
	WORD triggerIndex = 0;
	CComPtr<ITaskTrigger> iTrigger;
	pITask->GetTrigger(triggerIndex,&iTrigger);
	if (!iTrigger) 
		pITask->CreateTrigger(&triggerIndex,&iTrigger);
	TASK_TRIGGER trigDat;
	ZeroMemory(&trigDat,sizeof(trigDat));
	trigDat.cbTriggerSize= sizeof(trigDat);
	trigDat.wBeginYear = 1980;
	trigDat.wBeginMonth = 1;
	trigDat.wBeginDay = 1;
	trigDat.wStartHour = 12;

	switch(interval) {
		case DAILY :
			trigDat.TriggerType = TASK_TIME_TRIGGER_DAILY;
			trigDat.Type.Daily.DaysInterval = 1;
			break;
		case WEEKLY :
			trigDat.TriggerType = TASK_TIME_TRIGGER_WEEKLY;
			trigDat.Type.Weekly.WeeksInterval = 1;
			trigDat.Type.Weekly.rgfDaysOfTheWeek  = TASK_MONDAY;
			break;
		case MONTHLY :
			trigDat.TriggerType = TASK_TIME_TRIGGER_MONTHLYDOW;
			trigDat.Type.MonthlyDOW.rgfDaysOfTheWeek = TASK_MONDAY;
			trigDat.Type.MonthlyDOW.wWhichWeek = TASK_FIRST_WEEK;
			trigDat.Type.MonthlyDOW.rgfMonths = 0xFFF;
			break;
		case NEVER :
			trigDat.TriggerType = TASK_TIME_TRIGGER_ONCE;
			trigDat.rgFlags = TASK_TRIGGER_FLAG_DISABLED ;
		}
	iTrigger->SetTrigger(&trigDat);
	pIPersistFile->Save(NULL,TRUE);
	return true;
}