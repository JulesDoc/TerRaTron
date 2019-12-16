#pragma once

#include <QtWidgets>
#include "T_NtcElect.hpp"
#include "T_Database.hpp"

class Ui_tronSaveNoticeToDatabaseDlg;

class T_TronSaveNoticeToDatabaseDlg : public QDialog
{
	Q_OBJECT
public:
	T_TronSaveNoticeToDatabaseDlg(RCT_DatabaseConnectionInfo rcConnectionInfo, QWidget *parent = 0);
	virtual ~T_TronSaveNoticeToDatabaseDlg();
	void accept() override;
	void reject() override;
	void launchSaveToDBDlg(const T_NtcElect& rNtcElect);

private:
	void initializeGUI();
	void setNtcElect(const T_NtcElect& rcNtcElect);
	T_NtcElect& getNtcElect();

private slots:
	void enableAccept();

private:
	Ui_tronSaveNoticeToDatabaseDlg *m_ui;
	T_DatabaseConnectionInfo m_connectionInfo;
	T_NtcElect m_NtcElect;
};