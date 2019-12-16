#include "T_WaitingNoticesDlg.hpp"
#include "ui_waitingNoticesDlg.h"
#include "T_TronDBTransaction.hpp"

T_WaitingNoticesDlg::T_WaitingNoticesDlg(T_TronDBBundle& rDBBundle, QWidget* parent) 
	: m_rDBBundle(rDBBundle), QDialog(parent)
{
	initializeGUI();
}

T_WaitingNoticesDlg::~T_WaitingNoticesDlg()
{
	delete m_ui;
}

void T_WaitingNoticesDlg::initializeGUI()
{
	m_ui = new Ui_waitingNoticesDlg;
	m_ui->setupUi(this);
	m_ui->sqlTableWidget->initialize(m_rDBBundle);

	const T_GUISqlStatement aSqlQuery = T_TronDBTransaction::getWaitingNtcElectGUISqlStatement();
	m_ui->sqlTableWidget->setSqlQuery(aSqlQuery);
}