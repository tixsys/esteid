#include <QApplication>

#include "idupdater.h"
#include "ProcessStarter.h"
#include "ScheduledUpdateTask.h"
#include <iostream>

using std::endl;
int printhelp() {
	std::cout 	<< "-help\t\t\t-this help"<< endl
			<< "-noautocheck\t\t-dont perform check at startup" << endl
			<< "-autoupdate\t\t-update automatically" << endl
			<< "-url http://foo.bar\t-use alternate url" << endl
			<< "-daily|-monthly|-weekly|-never\t"
			<< "configure scheduled task to run at given interval" << endl;
	return 0;
	}

struct comWrap {
	comWrap() {CoInitialize(NULL);}
	~comWrap() {CoUninitialize();}
};

bool confTask(QStringList args) {
	comWrap _w;
	ScheduledUpdateTask task(args[0].toStdWString() ,L"id updater task");
	if (args.contains("-daily")) 
		return task.configure(ScheduledUpdateTask::DAILY);
	if (args.contains("-weekly"))
		return task.configure(ScheduledUpdateTask::WEEKLY);
	if (args.contains("-monthly"))
		return task.configure(ScheduledUpdateTask::MONTHLY);
	if (args.contains("-never"))
		return task.configure(ScheduledUpdateTask::NEVER);
	return false;
}

int main(int argc, char *argv[])
{	
//	std::ofstream run("c:\\windows\\temp\\run.log",std::ios_base::app);
	QApplication app(argc, argv);
	QStringList args = app.arguments();
	if (args.contains("-help") || args.contains("-?") || args.contains("/?"))
		return printhelp();
	if (confTask(args)) 
		return 0;
	ProcessStarter proc(argv[0],"");
	bool quitNow = proc.Run();
//	run << "executed proc.run, result : " << quitNow << std::endl;
	QString url = "http://kaidokert.com/work/updater/";
	int urlAt = args.indexOf("-url");
	if (urlAt > 0 && urlAt < args.size()-1 ) 
		url = args[urlAt+1];
	idupdater form(url,
		!args.contains("-noautocheck"),args.contains("-autoupdate"));
	if (!quitNow) {
//		run << "showing form .. " << std::endl;
		form.show();
		}
	else {
//		run << "quitting " << std::endl;
		app.exit(0);
		return 0;
	}
	return app.exec();
}
