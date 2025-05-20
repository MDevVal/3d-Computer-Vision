//
// (c) Georg Umlauf, 2021
//
#pragma once

#include "ui_mainwindow.h"
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

protected slots:
  void updatePointSize(int);
  void updateKdDepth(int);

private:
  Ui::MainWindowClass *ui;
};
