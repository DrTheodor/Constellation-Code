#ifndef SCRIPTGENWINDOW_H
#define SCRIPTGENWINDOW_H

#include <QWidget>
#include <QWizardPage>
#include <QListWidget>
#include <QCheckBox>
#include "mainwindow.h"
#include "Script.h"
#include "Component.h"

class ScriptGenWindow : public QWizard
{
    Q_OBJECT
public:
    explicit ScriptGenWindow(QString *basePath);
    QWizardPage *wizardPage1;
    QListWidget *generatorsList;
    QWizardPage *wizardPage2;
    QCheckBox *enabled;
    QListWidget *componentList;

private:
    void readData();
    void setupUi();

    QList<Component> components;
    QList<Script> scripts;
    QString basePath;


private slots:
    void itemClicked();
    void nextClicked();
    void finishClicked();
    void toggleComponent(bool enabled);
};
#endif // SETTINGSWINDOW_H
