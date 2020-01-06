#include "T_TronSaveNoticeToDatabaseDlg.hpp"
#include "T_TronDBTransaction.hpp"
#include "T_LogGroupKey.hpp"
#include "T_SimpleSendMail.hpp"
#include "T_WaitingNoticesDlg.hpp"
#include "ui_tronSaveNoticeToDatabaseDlg.h"


T_TronSaveNoticeToDatabaseDlg::T_TronSaveNoticeToDatabaseDlg(RCT_DatabaseConnectionInfo rcConnectionInfo, QWidget* parent)
	: QDialog(parent),
	m_connectionInfo(rcConnectionInfo)
{
	initializeGUI();
}

T_TronSaveNoticeToDatabaseDlg::~T_TronSaveNoticeToDatabaseDlg()
{
	delete m_ui;
}

void T_TronSaveNoticeToDatabaseDlg::initializeGUI()
{
	m_ui = new Ui_tronSaveNoticeToDatabaseDlg;
	m_ui->setupUi(this);
	m_ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Save");
	
	QPushButton *okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(false);
	connect(m_ui->brRegistryNumber_lineEdit, &QLineEdit::textChanged, this, &T_TronSaveNoticeToDatabaseDlg::enableAccept);
}

void T_TronSaveNoticeToDatabaseDlg::launchSaveToDBDlg(const T_NtcElect& rNtcElect)
{
	setNtcElect(rNtcElect);
	QDialog::open();
	QPushButton *okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(false);
	m_ui->brRegistryNumber_lineEdit->clear();
	m_ui->receivedOn_dateEditWidget->setDate(QDate::currentDate());
	QString userSystemName = T_Database::getSystemUserName();
	m_ui->mail_lineEdit->setText(userSystemName.append("@itu.int"));
}

void T_TronSaveNoticeToDatabaseDlg::enableAccept()
{
	QValidator *validator = new QIntValidator(1, 9999999, this);
	QString str = m_ui->brRegistryNumber_lineEdit->text();
	int pos = 0;

	m_ui->brRegistryNumber_lineEdit->setValidator(validator);
	QValidator::State state = validator->validate(str, pos);

	if (state == 0 || m_ui->brRegistryNumber_lineEdit->text() == "" ) 
	{ 
		QPushButton *okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
		okButton->setEnabled(false);
		return;
	}

	QPushButton *okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(true);
}

void T_TronSaveNoticeToDatabaseDlg::accept()
{

	T_TronBRIntern aBRIntern;

	// Input
	aBRIntern.setDateRcv(T_DateRcv(m_ui->receivedOn_dateEditWidget->getDate()));
	aBRIntern.setRegistryNo(QVariant(m_ui->brRegistryNumber_lineEdit->text()).toInt());
	aBRIntern.setInternalRmks(m_ui->remarks_textEdit->toPlainText());
	// Output
	T_LogGroupKey aGroupKey;

	bool bAutoCommit = true;
	T_Database db(m_connectionInfo, T_DBAppRole::PROCESSING);
	T_TronDBTransaction aTronDBTransaction(db, bAutoCommit);
	aTronDBTransaction.runStore(aBRIntern, getNtcElect(), &aGroupKey);

	if (m_ui->mail_groupBox->isChecked())
	{
		T_SimpleSendMail aSimpleSendMail;
		QString userSystemName = T_Database::getSystemUserName();
		const QStringList to = { m_ui->mail_lineEdit->text() };
		const QStringList cc{};
		const QString from = userSystemName.append("@itu.int");
		const QString content{};
		const QString subject = { "ITU" };
		aSimpleSendMail.sendMail(from, to, cc, subject, content);
	}
	QDialog::accept();
}

void T_TronSaveNoticeToDatabaseDlg::reject()
{
	QDialog::reject();
}

void T_TronSaveNoticeToDatabaseDlg::setNtcElect(const T_NtcElect& rcNtcElect)
{
	m_NtcElect = rcNtcElect;
}

T_NtcElect &T_TronSaveNoticeToDatabaseDlg::getNtcElect()
{
	return m_NtcElect;
}