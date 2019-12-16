#pragma once

#include <QtWidgets>
#include "T_TronDBBundle.hpp"

class Ui_waitingNoticesDlg;

class T_WaitingNoticesDlg : public QDialog
{
	Q_OBJECT
public:
	T_WaitingNoticesDlg(T_TronDBBundle& rbundle, QWidget *parent = 0);
	virtual ~T_WaitingNoticesDlg();

private:
	void initializeGUI();

private:
	Ui_waitingNoticesDlg *m_ui;
	T_TronDBBundle &m_rDBBundle;
};
