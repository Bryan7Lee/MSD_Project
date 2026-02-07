#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Default scroll speeds
    scrollSpeeds = {1, 3, 5, 8, 12};

    // Profile buttons
    connect(ui->profile1, &QPushButton::clicked, this, [=]{ selectProfile(0); });
    connect(ui->profile2, &QPushButton::clicked, this, [=]{ selectProfile(1); });
    connect(ui->profile3, &QPushButton::clicked, this, [=]{ selectProfile(2); });
    connect(ui->profile4, &QPushButton::clicked, this, [=]{ selectProfile(3); });

    // Scroll stage buttons
    connect(ui->stage1, &QPushButton::clicked, this, [=]{ selectScrollStage(0); });
    connect(ui->stage2, &QPushButton::clicked, this, [=]{ selectScrollStage(1); });
    connect(ui->stage3, &QPushButton::clicked, this, [=]{ selectScrollStage(2); });
    connect(ui->stage4, &QPushButton::clicked, this, [=]{ selectScrollStage(3); });
    connect(ui->stage5, &QPushButton::clicked, this, [=]{ selectScrollStage(4); });

    connect(ui->applyScroll, &QPushButton::clicked, this, &MainWindow::applyScrollValues);

    updateScrollHighlight();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectProfile(int index)
{
    currentProfile = index;
    ui->profileLabel->setText(QString("Profile %1").arg(index + 1));

    // Later: send macro profile over serial
}

void MainWindow::selectScrollStage(int index)
{
    currentScrollStage = index;
    updateScrollHighlight();

    // Later: send active scroll stage to Pico
}

void MainWindow::applyScrollValues()
{
    scrollSpeeds[0] = ui->speed1->value();
    scrollSpeeds[1] = ui->speed2->value();
    scrollSpeeds[2] = ui->speed3->value();
    scrollSpeeds[3] = ui->speed4->value();
    scrollSpeeds[4] = ui->speed5->value();

    // Later: send scroll table via serial
}

void MainWindow::updateScrollHighlight()
{
    QList<QPushButton*> buttons = {
        ui->stage1, ui->stage2, ui->stage3, ui->stage4, ui->stage5
    };

    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setStyleSheet(
            i == currentScrollStage
            ? "background:#3daee9; color:black; font-weight:bold;"
            : ""
        );
    }
}
