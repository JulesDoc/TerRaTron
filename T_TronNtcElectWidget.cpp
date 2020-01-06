
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

#include "T_TronNtcElectWidget.hpp"
#include "T_TronSaveNoticeToDatabaseDlg.hpp"
#include "ui_tronNtcElectWidget.h"
#include "codeeditor.h"


T_TronNtcElectWidget::T_TronNtcElectWidget(QWidget *parent) : QWidget(parent)
{
	initializeGUI();
}

T_TronNtcElectWidget::~T_TronNtcElectWidget()
{
	delete m_ui;
}

void T_TronNtcElectWidget::initializeGUI()
{
	m_ui = new Ui_tronNtcElectWidget;
	m_ui->setupUi(this);

	connect(m_ui->browser_treeWidget, &QTreeWidget::currentItemChanged, this, &T_TronNtcElectWidget::handleClicked);
	connect(m_ui->errorsWarn_textBrowser, &QTextBrowser::anchorClicked, this, &T_TronNtcElectWidget::onAnchorClicked);
	connect(m_ui->fileStatus_textBrowser, &QTextBrowser::anchorClicked, this, &T_TronNtcElectWidget::onAnchorClicked);
	connect(m_ui->comboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &T_TronNtcElectWidget::showNotices);

	m_highlighter1 = new T_NtcElectHighlighter(false, m_ui->fileContent_textEdit->document());
}

void T_TronNtcElectWidget::setRunCheckResult(const T_NtcElect& rcNtcElect)
{
	//PRECONDITION(!rcNtcElect.isUnknown());
	m_fileUpdated = true;
	setNtcElect(rcNtcElect);
	clearAll();
	fillTreeWidget();
	fillFileStatus();
	T_NtcElectMessageContainer::T_WritableOptions writeOptions;
	writeOptions.m_messageWritableOptions.m_bWriteWithLinks = false;
	writeOptions.m_messageWritableOptions.m_bWriteMessageLocation = true;
	fillMessages(writeOptions);
	emit setResultCompleted();
}

void T_TronNtcElectWidget::fillTreeWidget()
{
	const QFileInfo &fileInfo = m_NtcElect.getFileInfo();
	m_ui->browser_treeWidget->setColumnCount(1);

	QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_ui->browser_treeWidget);
	parentItem->setText(0, fileInfo.fileName());
	parentItem->setToolTip(0, QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
	parentItem->setIcon(0, QIcon());

	QTreeWidgetItem *headItemSection = new QTreeWidgetItem(parentItem);
	headItemSection->setData(0, SectionNameRole, T_TronSectionName(T_TronSectionName::HEAD).asDisplayString());
	headItemSection->setText(0, tr("Head Section"));
	headItemSection->setIcon(0, correspondingIcon(m_NtcElect.getHeadSection().getErrorCount(),
												  m_NtcElect.getHeadSection().getWarningCount()));

	//Multiple notices
	bool allNoticesFreeOfErrors = true;
	const int indexNoticeCount = m_NtcElect.getNoticeSectionCount();
	for (int i = 0; i < indexNoticeCount; ++i) {

		QTreeWidgetItem *noticeItemSection = new QTreeWidgetItem(parentItem);
		noticeItemSection->setText(0, tr("Notice %1").arg(i + 1));
		noticeItemSection->setData(0, SectionNameRole, T_TronSectionName(T_TronSectionName::NOTICE).asDisplayString());
		noticeItemSection->setData(0, NoticeIndexRole, i);
		noticeItemSection->setIcon(0, correspondingIcon(m_NtcElect.getNoticeSectionAt(i).getErrorCount(),
														m_NtcElect.getNoticeSectionAt(i).getWarningCount()));
		m_ui->browser_treeWidget->expandItem(parentItem);
		if (m_NtcElect.getNoticeSectionAt(i).getErrorCount() > 0) allNoticesFreeOfErrors = false;
	}

	if (allNoticesFreeOfErrors && (m_NtcElect.getErrorCount() > 0))
		parentItem->setIcon(0, correspondingIcon(m_NtcElect.getErrorCount(), 0));

	//Filling ComboBox
	m_ui->comboBox->clear();
	m_ui->comboBox->addItem(tr("Show all notices"), ShowAll);
	if (m_NtcElect.hasWarnings())
		m_ui->comboBox->addItem(tr("Show notices with messages"), ShowMessages);
	if (m_NtcElect.hasErrors())
		m_ui->comboBox->addItem(tr("Show notices with errors"), ShowErrors);
}
	
void T_TronNtcElectWidget::showNotices(QString str)
{
	
	auto topLevelItem = m_ui->browser_treeWidget->topLevelItem(0);
	for (int i = 0; i < topLevelItem->childCount(); i++)
	{
		QTreeWidgetItem *item = topLevelItem->child(i);
		item->setHidden(false);
		T_TronSectionName sectionName = item->data(0, SectionNameRole).toString();
		if (sectionName.getSectionName() == T_TronSectionName::NOTICE)
		{
			int noticeIndex = item->data(0, NoticeIndexRole).toInt();
			const auto &rcSection = m_NtcElect.getNoticeSectionAt(noticeIndex);
			int errorCount = rcSection.getErrorCount();
			int warningCount = rcSection.getWarningCount();
			if (m_ui->comboBox->currentData().toInt() == ShowMessages && warningCount == 0 && errorCount == 0)
				item->setHidden(true);
			if (m_ui->comboBox->currentData().toInt() == ShowErrors && errorCount == 0)
				item->setHidden(true);
		}
		
	}

}

void T_TronNtcElectWidget::fillFileStatus()
{
	m_ui->fileStatus_textBrowser->clear();
	const QFileInfo &fileInfo = m_NtcElect.getFileInfo();
	QTextCharFormat format;
	QFont font("Times", 10, QFont::Bold);
	m_ui->fileStatus_textBrowser->setFont(font);

	QString fileLocation = QString::fromStdString("File name: ") + QString::fromStdString("<a href = \"") + fileInfo.absoluteFilePath() +
		QString::fromStdString("\" >") + fileInfo.absoluteFilePath() + QString::fromStdString("</a>") +
		((!m_fileUpdated) ? QString::fromStdString("*") : QString::fromStdString(""));
	m_ui->fileStatus_textBrowser->append(fileLocation);
	m_ui->fileStatus_textBrowser->insertPlainText("\n");

	format.setAnchor(false);
	m_ui->fileStatus_textBrowser->setCurrentCharFormat(format);
	m_ui->fileStatus_textBrowser->insertPlainText(QString("Total number of errors: %1 \n").arg(m_NtcElect.getErrorCount()));
	m_ui->fileStatus_textBrowser->insertPlainText(QString("Total number of warnings: %1 \n").arg(m_NtcElect.getWarningCount()));

	m_ui->stackedWidget->setCurrentIndex(1);
}


void T_TronNtcElectWidget::handleAlertFileNotUpdated()
{
	m_fileUpdated = false;
	fillFileStatus();
}


void T_TronNtcElectWidget::fillMessages(const T_NtcElectMessageContainer::T_WritableOptions& writeOptions)
{
	m_ui->errorsWarn_textBrowser->clear();
	T_NtcElectMessageContainer aMessageContainer;
	m_NtcElect.getMessageContainer(aMessageContainer);
	QString html;
	T_XHtmlStreamWriter writer(&html);
	aMessageContainer.toXhtml(writer, &writeOptions);
	m_ui->errorsWarn_textBrowser->document()->setDefaultStyleSheet(T_XHtmlWritable::defaultStyleSheet());
	m_ui->errorsWarn_textBrowser->setHtml(html);
}

void T_TronNtcElectWidget::handleClicked(QTreeWidgetItem *item)
{
	m_ui->stackedWidget->setCurrentIndex(0);
	m_ui->fileContent_textEdit->clear();
	m_ui->errorsWarn_textBrowser->clear();
	//QModelIndex index = m_ui->browser_treeWidget->currentIndex();
	//QModelIndex parent = index.parent();

	if (m_ui->browser_treeWidget->itemAbove(item) == nullptr)
	{
		//When click on the file, it shows all the messages and file status
		fillFileStatus();
		T_NtcElectMessageContainer::T_WritableOptions writeOptions;
		writeOptions.m_messageWritableOptions.m_bWriteWithLinks = false;
		writeOptions.m_messageWritableOptions.m_bWriteMessageLocation = true;
		fillMessages(writeOptions);
	}

	else {
		T_TronSectionName sectionName = item->data(0, SectionNameRole).toString();
		//HEAD
		if (sectionName.getSectionName() == T_TronSectionName::HEAD)
		{
			//Filling file content for head
			QString head;
			m_NtcElect.getHeadFileText(head);
			m_ui->fileContent_textEdit->setPlainText(head);
			//It shows error or warnings messages.
			T_NtcElectMessageContainer::T_WritableOptions writeOptions;
			writeOptions.makeWriteHeadMessagesOnly();
			fillMessages(writeOptions);
		}

		//NOTICE
		if (sectionName.getSectionName() == T_TronSectionName::NOTICE)
		{
			int noticeIndex = item->data(0, NoticeIndexRole).toInt();
			QString notice;
			m_NtcElect.getNoticeSectionFileText(noticeIndex, notice);
			//m_NtcElect.getNoticeSectionFileText(index.row() - 1, notice);
			m_ui->fileContent_textEdit->setPlainText(notice);
			T_NtcElectMessageContainer::T_WritableOptions writeOptions;
			writeOptions.makeWriteNoticeMessagesOnly(noticeIndex);
			fillMessages(writeOptions);
		}
	}
}

void T_TronNtcElectWidget::onAnchorClicked(const QUrl &link)
{
	if (sender() == m_ui->fileStatus_textBrowser)
	{
		if (QFileInfo(link.toString()).isFile()) QDesktopServices::openUrl(link);
		fillFileStatus();
	}
	else
	{
		T_TronLocation locationUrl = T_TronLocation::fromUrl(link);
		int line = locationUrl.line();
		highlightErrorLine(line);
	}
}

void T_TronNtcElectWidget::highlightErrorLine(const int& line)
{
	int sizeDocument = m_ui->fileContent_textEdit->document()->lineCount();
	if (line > (sizeDocument / 2))
	{
		int limit = sizeDocument - line;
		m_ui->fileContent_textEdit->moveCursor(QTextCursor::End);
		for (int i = 0; i <= limit; ++i) {
			m_ui->fileContent_textEdit->moveCursor(QTextCursor::Up);
		}
	}
	else
	{
		m_ui->fileContent_textEdit->moveCursor(QTextCursor::Start);
		for (int i = 0; i < line; ++i) {
			m_ui->fileContent_textEdit->moveCursor(QTextCursor::Down);
		}
	}

	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;
	QColor lineColor = QColor(Qt::red).lighter(160);
	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);

	QTextCursor cursor(m_ui->fileContent_textEdit->document()->findBlockByLineNumber(line - 1));
	selection.cursor = cursor;
	selection.cursor.clearSelection();
	extraSelections.append(selection);
	m_ui->fileContent_textEdit->setExtraSelections(extraSelections);
}

const QIcon &T_TronNtcElectWidget::correspondingIcon(const int& numErr, const int& numWar)
{
	static QIcon okIcon = QIcon(QString::fromLatin1(":/icons/ok.png"));
	static QIcon warningIcon = QIcon(QString::fromLatin1(":/icons/warning.png"));
	static QIcon errorIcon = QIcon(QString::fromLatin1(":/icons/error.png"));

	if (numErr > 0)
		return errorIcon;
	if (numErr == 0 && numWar > 0)
		return warningIcon;
	return okIcon;
}

void T_TronNtcElectWidget::clearAll()
{
	m_ui->browser_treeWidget->clear();
	m_ui->errorsWarn_textBrowser->clear();
	m_ui->fileContent_textEdit->clear();
}

void T_TronNtcElectWidget::setNtcElect(const T_NtcElect& rcNtcElect)
{
	m_NtcElect = rcNtcElect;
}

T_NtcElect &T_TronNtcElectWidget::getNtcElect()
{
	return m_NtcElect;
}

QComboBox* T_TronNtcElectWidget::getComboBox() const
{
	return m_ui->comboBox;
}

QWidget* T_TronNtcElectWidget::getBrowser() const
{
	return m_ui->browser_widget;
}