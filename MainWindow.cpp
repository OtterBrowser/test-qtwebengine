/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2016 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QtCore/QTimer>
#include <QtWebEngineWidgets/QWebEngineHistory>
#include <QtWebEngineWidgets/QWebEngineSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_ui->webView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

	const QStringList arguments(QCoreApplication::arguments());

	if (arguments.count() > 1)
	{
		m_ui->webView->load(QUrl::fromUserInput(arguments.at(1)));
	}

	connect(m_ui->addressLineEdit, SIGNAL(returnPressed()), this, SLOT(addressChanged()));
	connect(m_ui->goButton, SIGNAL(clicked()), this, SLOT(addressChanged()));
	connect(m_ui->cloneButton, SIGNAL(clicked()), this, SLOT(cloneHistory()));
	connect(m_ui->zoomSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setZoom(int)));
	connect(m_ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanged(QUrl)));
	connect(m_ui->webView->page(), SIGNAL(featurePermissionRequested(QUrl,QWebEnginePage::Feature)), this, SLOT(featurePermissionRequested(QUrl,QWebEnginePage::Feature)));
}

MainWindow::~MainWindow()
{
	delete m_ui;
}

void MainWindow::cloneHistory()
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::ReadWrite);
	stream << *(m_ui->webView->page()->history());

	QWebEnginePage *page(new QWebEnginePage(m_ui->webView));
	page->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

	m_ui->webView->setPage(page);

	stream.device()->reset();
	stream >> *(page->history());
}

void MainWindow::acceptFeatureRequest()
{
	if (!m_featureRequests.isEmpty())
	{
		const QPair<QUrl, QWebEnginePage::Feature> featureRequest(m_featureRequests.dequeue());

		m_ui->webView->page()->setFeaturePermission(featureRequest.first, featureRequest.second, QWebEnginePage::PermissionGrantedByUser);
	}
}

void MainWindow::addressChanged()
{
	m_ui->webView->load(QUrl::fromUserInput(m_ui->addressLineEdit->text()));
}

void MainWindow::featurePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature)
{
	if (m_ui->featureRequestsPolicyComboBox->currentIndex() == 1)
	{
		m_ui->webView->page()->setFeaturePermission(url, feature, QWebEnginePage::PermissionGrantedByUser);
	}
	else if (m_ui->featureRequestsPolicyComboBox->currentIndex() == 2)
	{
		m_featureRequests.enqueue(qMakePair(url, feature));

		QTimer::singleShot(2500, this, SLOT(acceptFeatureRequest()));
	}
}

void MainWindow::urlChanged(const QUrl &url)
{
	m_ui->addressLineEdit->setText(url.toString());
}

void MainWindow::setZoom(int zoom)
{
	m_ui->webView->setZoomFactor(zoom / 100.);
}
