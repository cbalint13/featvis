/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */

#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>

#include <qtosgwidget.hpp>


class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(int&, char**&);
  QtOSGWidget *osgWidget;

 private slots:
   void open();
   void about();

 private:
  void createActions();
  void createMenus();

  QMenu *fileMenu;
  QMenu *helpMenu;
  QAction *openAct;
  QAction *exitAct;
  QAction *aboutAct;
  QLabel *infoLabel;
  QProgressBar *pgBar;
};

#endif // __MAINWINDOW_HPP__
