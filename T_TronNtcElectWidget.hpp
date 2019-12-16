#pragma once

#include <QWidget>
#include <QPointer>
#include <QIcon>
#include <QTreeWidgetItem>
#include <QComboBox>

#include "T_NtcElect.hpp"
#include "T_NtcElectMessageContainer.hpp"
#include "T_NtcElectHighlighter.hpp"

class Ui_tronNtcElectWidget;

class T_TronNtcElectWidget : public QWidget
{
	Q_OBJECT

public:
	T_TronNtcElectWidget(QWidget* parent = nullptr);
	virtual ~T_TronNtcElectWidget();

	QWidget* getBrowser() const;
	QComboBox* getComboBox() const;
	
private:
	void initializeGUI();
	
public:
	void fillTreeWidget();
	void fillFileStatus();
	void fillMessages(const T_NtcElectMessageContainer::T_WritableOptions& writeOptions = T_NtcElectMessageContainer::T_WritableOptions());
	const QIcon &correspondingIcon(const int& numErr, const int& numWar);
	void highlightErrorLine(const int& line);
	T_NtcElect &getNtcElect();
	//const T_NtcElect &getNtcElect() const;

public slots:
	void setRunCheckResult(const T_NtcElect& rcNtcElect);
	void handleClicked(QTreeWidgetItem* item);
	void handleAlertFileNotUpdated();
	void onAnchorClicked(const QUrl& link);
	void clearAll();
	void showNotices(QString);

signals:
	void setResultCompleted();
	
private:
	void setNtcElect(const T_NtcElect& rcNtcElect);
	
private slots:
	
private:
	Ui_tronNtcElectWidget* m_ui;
	T_NtcElect m_NtcElect;
	QPointer<T_NtcElectHighlighter> m_highlighter1;
	QVector<QTreeWidgetItem*> m_treeWidgetItemsVector{};
	bool m_fileUpdated = true;

	enum {
		SectionNameRole = Qt::UserRole,
		NoticeIndexRole
	};

	enum BrowserDisplayOption {
		ShowAll,
		ShowMessages,
		ShowErrors
	};
};