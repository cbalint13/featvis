/*
 * License: GPLv3
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Copyright 2012, 2023
 *       Cristian Balint < cristian dot balint at gmail dot com >
 */

#include <QTextStream>
#include <QCommandLineParser>
#include <QtWidgets/QApplication>

#include <mainwindow.hpp>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  const QString optInput = "input";
  const QList<QCommandLineOption> sCmdOptions = {
    {{"i", optInput}, "input HDF4 file.", optInput}
  };

  QCommandLineParser parser;
  parser.setApplicationDescription("FeatVIS");
  parser.addOptions(sCmdOptions);

  QStringList args = QCoreApplication::arguments();
  QTextStream cout(stdout);
  if (argc > 1 &&
      !parser.parse(args) &&
      !parser.isSet(optInput)) {
    cout << QString("Usage: -i <path_to_hdf4_project_file>\n");
    return 1;
  }

  MainWindow w(argc, argv);
  w.show();

  return a.exec();
}
