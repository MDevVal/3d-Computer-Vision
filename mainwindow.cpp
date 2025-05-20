//
// (c) Georg Umlauf, 2021
//
#include "mainwindow.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass) {
  ui->setupUi(this);
  ui->glwidget->setFocusPolicy(Qt::StrongFocus);
  ui->glwidget->setFocus();

  connect(ui->pushButton, &QPushButton::clicked, ui->glwidget,
          &GLWidget::openFileDialog);

  connect(ui->horizontalSlider, &QSlider::valueChanged, this,
          &MainWindow::updatePointSize);

  connect(ui->horizontalSliderDepth, &QSlider::valueChanged, this,
          &MainWindow::updateKdDepth);

  updatePointSize(3);
  updateKdDepth(3);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updatePointSize(int value) {
  std::cout << "new pointsize: " << value << std::endl;
  ui->glwidget->setPointSize(value);
}

void MainWindow::updateKdDepth(int value) {
  std::cout << "new kd-depth: " << value << std::endl;
  ui->glwidget->setKdDepth(value);
}
