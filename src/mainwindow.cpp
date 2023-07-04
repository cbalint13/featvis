/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */


#include <QMessageBox>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>

#include <osgDB/ReadFile>

#include <qapplication.h>

#include <mainwindow.hpp>
#include <osghdf.hpp>


MainWindow::MainWindow(int& argc, char**& argv) {

  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  osgWidget = new QtOSGWidget(widget);
  osgWidget->setObjectName(QString::fromUtf8("osgWidget"));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(osgWidget->sizePolicy().hasHeightForWidth());
  osgWidget->setSizePolicy(sizePolicy);

  infoLabel = new QLabel(tr("<i>Choose a menu option</i>"));
  infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  infoLabel->setAlignment(Qt::AlignCenter);

  pgBar = new QProgressBar();
  pgBar->setRange(0, 100);
  pgBar->setTextVisible(false);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(osgWidget);
  layout->addWidget(infoLabel);
  layout->addWidget(pgBar);
  widget->setLayout(layout);
  createActions();
  createMenus();

  statusBar()->setSizeGripEnabled(false);
  statusBar()->addPermanentWidget(pgBar,1);
  statusBar()->setMaximumHeight(10);

  setWindowTitle(tr("FeatVIS"));
  setMinimumSize(160, 160);
  resize(800, 600);

  if (argc > 1) {
    // preload scene from CLI
    QString fileName = argv[2];
    auto group = LoadMDSHDFVset(fileName.toLocal8Bit(), pgBar);
    if (group.valid()) {
      infoLabel->setText(tr("Loaded: " + fileName.toLocal8Bit()));
      osgWidget->setScene(group.get());
    } else {
      infoLabel->setText(tr("Load Failed: " + fileName.toLocal8Bit()));
    }
    pgBar->setValue(0);
  }

}

void MainWindow::createActions() {
  const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/icons/hdf.png"));
  openAct = new QAction(openIcon, tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Load HDF file"));
  connect(openAct, &QAction::triggered, this, &MainWindow::open);

  exitAct = new QAction(tr("&Exit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Quit Application"));
  connect(exitAct, &QAction::triggered, this, &QWidget::close);

  const QIcon logoIcon = QIcon::fromTheme("document-open", QIcon(":/icons/logo-icon.png"));
  aboutAct = new QAction(logoIcon, tr("&About"), this);
  aboutAct->setShortcut(tr("CTRL+H"));
  aboutAct->setStatusTip(tr("About FeatVIS"));
  connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(exitAct);
  fileMenu->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void MainWindow::open() {
  infoLabel->setText(tr("Invoked <b>File|Open</b>"));
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open HDF"), QDir::currentPath(), tr("HDF Files (*.hdf *.h4)"));
  if (!fileName.isNull()) {
    infoLabel->setText(tr("Loading: " + fileName.toLocal8Bit()));
    qApp->processEvents();
    auto group = LoadMDSHDFVset(fileName.toLocal8Bit(), pgBar);
    if (group.valid()) {
      infoLabel->setText(tr("Loaded: " + fileName.toLocal8Bit()));
      osgWidget->setScene(group.get());
    } else {
      infoLabel->setText(tr("Load Failed: " + fileName.toLocal8Bit()));
    }
  } else {
    infoLabel->setText(tr("Load Canceled."));
  }
  pgBar->setValue(0);
}

void MainWindow::about() {
  setWindowIcon(QIcon(":/icons/logo-indexed.png"));
  infoLabel->setText(tr("Invoked <b>Help|About</b>"));
  QMessageBox::about(this, tr("FeatVIS"),
    tr("<b><font color=#555555>Feat</font><font color=#0055ff>VIS</font></b><br>"
       "Multi dimensional features visualization<br><br>"
       "© <i>2012 - 2023</i><br>"
       "Author: <b>Cristian Balint</b><br>"
       "E-mail: cristian.balint@gmail.com<br>"
       "License: <a href='https://www.gnu.org/licenses/gpl-3.0.en.html'>GPLv3</a>"));
}
