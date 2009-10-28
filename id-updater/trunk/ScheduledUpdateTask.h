#pragma once

#ifdef WIN32
#include <atlbase.h>
#include <comutil.h>
#include <Mstask.h>
#include <string>

class ScheduledUpdateTask
{
	CComPtr<ITaskScheduler> pITS;
	CComQIPtr<ITask> pITask;
	CComQIPtr<IPersistFile> pIPersistFile;
	std::wstring m_command;
	std::wstring m_name;
public:
	enum Interval {
		DAILY,
		WEEKLY,
		MONTHLY,
		NEVER
	} ;
	ScheduledUpdateTask(std::wstring command,std::wstring name);
	bool configure(Interval interval,bool autoupdate);
	bool remove();
};
#else
class ScheduledUpdateTask
{
public:
	ScheduledUpdateTask(void);
};
#endif
