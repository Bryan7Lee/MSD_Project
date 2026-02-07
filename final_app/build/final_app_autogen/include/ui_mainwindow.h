/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QPushButton *profile1;
    QPushButton *profile2;
    QPushButton *profile3;
    QPushButton *profile4;
    QLabel *profileLabel;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QSpinBox *speed1;
    QSpinBox *speed2;
    QSpinBox *speed3;
    QSpinBox *speed4;
    QSpinBox *speed5;
    QPushButton *stage1;
    QPushButton *stage2;
    QPushButton *stage3;
    QPushButton *stage4;
    QPushButton *stage5;
    QPushButton *applyScroll;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        vboxLayout = new QVBoxLayout(centralwidget);
        vboxLayout->setObjectName("vboxLayout");
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        profile1 = new QPushButton(centralwidget);
        profile1->setObjectName("profile1");

        hboxLayout->addWidget(profile1);

        profile2 = new QPushButton(centralwidget);
        profile2->setObjectName("profile2");

        hboxLayout->addWidget(profile2);

        profile3 = new QPushButton(centralwidget);
        profile3->setObjectName("profile3");

        hboxLayout->addWidget(profile3);

        profile4 = new QPushButton(centralwidget);
        profile4->setObjectName("profile4");

        hboxLayout->addWidget(profile4);


        vboxLayout->addLayout(hboxLayout);

        profileLabel = new QLabel(centralwidget);
        profileLabel->setObjectName("profileLabel");
        profileLabel->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(profileLabel);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        speed1 = new QSpinBox(groupBox);
        speed1->setObjectName("speed1");

        gridLayout->addWidget(speed1, 0, 0, 1, 1);

        speed2 = new QSpinBox(groupBox);
        speed2->setObjectName("speed2");

        gridLayout->addWidget(speed2, 0, 1, 1, 1);

        speed3 = new QSpinBox(groupBox);
        speed3->setObjectName("speed3");

        gridLayout->addWidget(speed3, 0, 2, 1, 1);

        speed4 = new QSpinBox(groupBox);
        speed4->setObjectName("speed4");

        gridLayout->addWidget(speed4, 0, 3, 1, 1);

        speed5 = new QSpinBox(groupBox);
        speed5->setObjectName("speed5");

        gridLayout->addWidget(speed5, 0, 4, 1, 1);

        stage1 = new QPushButton(groupBox);
        stage1->setObjectName("stage1");

        gridLayout->addWidget(stage1, 1, 0, 1, 1);

        stage2 = new QPushButton(groupBox);
        stage2->setObjectName("stage2");

        gridLayout->addWidget(stage2, 1, 1, 1, 1);

        stage3 = new QPushButton(groupBox);
        stage3->setObjectName("stage3");

        gridLayout->addWidget(stage3, 1, 2, 1, 1);

        stage4 = new QPushButton(groupBox);
        stage4->setObjectName("stage4");

        gridLayout->addWidget(stage4, 1, 3, 1, 1);

        stage5 = new QPushButton(groupBox);
        stage5->setObjectName("stage5");

        gridLayout->addWidget(stage5, 1, 4, 1, 1);


        vboxLayout->addWidget(groupBox);

        applyScroll = new QPushButton(centralwidget);
        applyScroll->setObjectName("applyScroll");

        vboxLayout->addWidget(applyScroll);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Mouse Scroll Config", nullptr));
        profile1->setText(QCoreApplication::translate("MainWindow", "Profile 1", nullptr));
        profile2->setText(QCoreApplication::translate("MainWindow", "Profile 2", nullptr));
        profile3->setText(QCoreApplication::translate("MainWindow", "Profile 3", nullptr));
        profile4->setText(QCoreApplication::translate("MainWindow", "Profile 4", nullptr));
        profileLabel->setText(QCoreApplication::translate("MainWindow", "Profile 1", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Scroll Speed Stages", nullptr));
        stage1->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        stage2->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        stage3->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        stage4->setText(QCoreApplication::translate("MainWindow", "4", nullptr));
        stage5->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        applyScroll->setText(QCoreApplication::translate("MainWindow", "Apply Scroll Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
