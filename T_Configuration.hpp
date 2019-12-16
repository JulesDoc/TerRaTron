#pragma once

#include <QSettings>

#include "T_DBTarget.hpp"
#include "T_Database.hpp"
#include "T_DBName.hpp"

class T_Configuration
{
public:
	T_Configuration();
	static void saveSettings();
	static void loadSettings();
	static void setSettingsPath(QString);
	static QString getSettingsPath();
	static void setSettingsDBTarget(T_DBTarget::tagDBTarget);
	static T_DBTarget::tagDBTarget getSettingsDBTarget();
	static RCT_DBName getSettingsDBName();

private:
	static RCT_DBName settingsDBNameToEnum;
	static int settingsDBNameInt;
	static T_DBTarget::tagDBTarget settingsDBTargetToEnum;
	static int settingsDBTargetInt;
	static QSettings settings;
	static QString settingsPath;
};

