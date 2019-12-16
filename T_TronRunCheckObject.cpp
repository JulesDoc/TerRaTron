
#include "T_TronRunCheckObject.hpp"
#include <QMessageBox>

T_TronRunCheckObject::T_TronRunCheckObject(const T_DatabaseConnectionInfo& rcConnectionInfo, QObject *parent)
	:m_connectionInfo(rcConnectionInfo),
	QObject(parent)
{}

T_TronRunCheckObject::~T_TronRunCheckObject()
{}

RT_TronDBBundle T_TronRunCheckObject::getDBBundle()
{
	if (m_apDBBundle.get() == 0)
	{
		qInfo() << "Connecting to the database" << m_connectionInfo.getDBTarget() << "...";
		m_apDBBundle = APT_TronDBBundle(new T_TronDBBundle(m_connectionInfo));
	}
	return *m_apDBBundle;
}

void T_TronRunCheckObject::initialize()
{
	// Ensure database connection
	getDBBundle();
}

void T_TronRunCheckObject::runCheckFile(const QString &fileName, bool bApplyDeepVal)
{
	T_NtcElect aNtcElect;
	T_TronValidatorWithDeepVal checker(getDBBundle());
	checker.validateNtcElectFile(fileName, bApplyDeepVal, aNtcElect);
	emit runCheckCompleted(aNtcElect);
}

void T_TronRunCheckObject::runCheckNoticeContent(const QString &strNoticeContent, bool bApplyDeepVal)
{
	T_NtcElect aNtcElect;
	T_TronValidatorWithDeepVal checker(getDBBundle());
	checker.validateNtcElectContent(strNoticeContent, bApplyDeepVal, aNtcElect);
	emit runCheckCompleted(aNtcElect);
}

