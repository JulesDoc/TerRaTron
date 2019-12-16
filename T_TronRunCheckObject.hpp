
#pragma once

#include "T_TronValidatorWithDeepVal.hpp"
#include "T_TronDBBundle.hpp"

class /*T_TERRANTCHANDLERLIBDEF_CLASS*/ T_TronRunCheckObject : public QObject
{
	Q_OBJECT
public:
	T_TronRunCheckObject(const T_DatabaseConnectionInfo& rcConnectionInfo, QObject *parent = 0);
	virtual ~T_TronRunCheckObject();

public slots:
	void initialize();
	void runCheckFile(const QString &fileName, bool bApplyDeepVal);
	void runCheckNoticeContent(const QString &strNoticeContent, bool bApplyDeepVal);

signals:
	// Emited when runCheck finishes
	void runCheckCompleted(const T_NtcElect& rcNtcElect);

private:
	RT_TronDBBundle getDBBundle();

	// Attributes
	// **********
private:
	T_DatabaseConnectionInfo m_connectionInfo;
	APT_TronDBBundle m_apDBBundle;
};
