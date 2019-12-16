
#include <QStandardPaths>
#include "T_Configuration.hpp"

QSettings T_Configuration::settings("configFileNewTerRaTron.ini", QSettings::IniFormat);

int T_Configuration::settingsDBTargetInt{};
T_DBTarget::tagDBTarget T_Configuration::settingsDBTargetToEnum{};

int T_Configuration::settingsDBNameInt{};
RCT_DBName T_Configuration::settingsDBNameToEnum{ T_DBName::TRS_DB };

QString T_Configuration::settingsPath{};

T_Configuration::T_Configuration(){}

void T_Configuration::saveSettings()
{
	settings.beginGroup("General");
	settings.setValue("defaultPathToOpenFile", settingsPath);
	settings.setValue("dbTarget", settingsDBTargetToEnum);
	settings.endGroup();
}

void T_Configuration::loadSettings()
{ 
	settings.beginGroup("General");
	settingsDBTargetInt = settings.value("dbTarget", T_DBTarget::DEVL).toInt();
	settingsDBTargetToEnum = static_cast<T_DBTarget::tagDBTarget>(settingsDBTargetInt);
	settingsPath = settings.value("defaultPathToOpenFile", QStandardPaths::writableLocation
							     (QStandardPaths::DocumentsLocation)).toString();
	settings.endGroup();
}

void T_Configuration::setSettingsPath(QString str)
{
	settingsPath = str;
}

QString T_Configuration::getSettingsPath()
{
	return settingsPath;
}

void T_Configuration::setSettingsDBTarget(T_DBTarget::tagDBTarget enumType)
{
	settingsDBTargetToEnum = enumType;
}

T_DBTarget::tagDBTarget T_Configuration::getSettingsDBTarget()
{
	return settingsDBTargetToEnum;
}

RCT_DBName T_Configuration::getSettingsDBName()
{
	return settingsDBNameToEnum;
}