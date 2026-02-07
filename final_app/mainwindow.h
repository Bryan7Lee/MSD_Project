#pragma once

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectProfile(int index);
    void selectScrollStage(int index);
    void applyScrollValues();

private:
    Ui::MainWindow *ui;

    int currentProfile = 0;
    int currentScrollStage = 0;

    QVector<int> scrollSpeeds;
    void updateScrollHighlight();
};
