#include <QApplication>

#include "idupdater.h"
#include "ProcessStarter.h"
#include "ScheduledUpdateTask.h"
#include <iostream>

#ifndef UPDATER_URL
#define UPDATER_URL "http://kaidokert.com/work/updater/"
#endif

using std::endl;
int printhelp() {
	std::cout 	<< "-help\t\t\t-this help"<< endl
			<< "-noautocheck\t\t-dont perform check at startup" << endl
			<< "-autoupdate\t\t-update automatically" << endl
			<< "-url http://foo.bar\t-use alternate url" << endl
			<< "-daily|-monthly|-weekly|-never|-remove\t"
			<< "configure scheduled task to run at given interval, or remove it" << endl;
	return 0;
	}

struct comWrap {
	comWrap() {CoInitialize(NULL);}
	~comWrap() {CoUninitialize();}
};

bool confTask(QStringList args) {
	comWrap _w;
	ScheduledUpdateTask task(args[0].toStdWString() ,L"id updater task");
	bool autoupdate = args.contains("-autoupdate");
	if (args.contains("-daily")) 
		return task.configure(ScheduledUpdateTask::DAILY,autoupdate);
	if (args.contains("-weekly"))
		return task.configure(ScheduledUpdateTask::WEEKLY,autoupdate);
	if (args.contains("-monthly"))
		return task.configure(ScheduledUpdateTask::MONTHLY,autoupdate);
	if (args.contains("-never"))
		return task.configure(ScheduledUpdateTask::NEVER,autoupdate);
	if (args.contains("-remove")) 
		return task.remove();
	return false;
}

int main(int argc, char *argv[])
{	
#ifdef _DEBUG
	char logfile[MAX_PATH * 4];
	GetWindowsDirectoryA(logfile,sizeof(logfile));
	_snprintf(logfile+strlen(logfile),MAX_PATH,"\\temp\\run%u.log",GetCurrentProcessId());
	std::ofstream run(logfile,std::ios_base::app);
	run << "started run" << std::endl;
#endif
	QApplication app(argc, argv);
	QStringList args = app.arguments();
	if (args.contains("-help") || args.contains("-?") || args.contains("/?"))
		return printhelp();
	if (confTask(args)) 
		return 0;
	ProcessStarter proc(argv[0],
		args.contains("-autoupdate") ? "-autoupdate" : 	"");
	bool quitNow = proc.Run();
#ifdef _DEBUG
	run << "executed proc.run, result : " << (char *) (quitNow ? "true" : "false") << std::endl;
#endif
	QString url = UPDATER_URL;
	int urlAt = args.indexOf("-url");
	if (urlAt > 0 && urlAt < args.size()-1 ) 
		url = args[urlAt+1];
	idupdater form(url,
		!args.contains("-noautocheck"),args.contains("-autoupdate"));
	if (!quitNow) {
#ifdef _DEBUG
		run << "showing form .. " << std::endl;
#endif
		form.show();
		}
	else {
#ifdef _DEBUG
		run << "quitting " << std::endl;
#endif
		app.exit(0);
		return 0;
	}
	return app.exec();
}
