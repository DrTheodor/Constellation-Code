#include "scriptgenwindow.h"

#include <QtCore/QVariant>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QCoreApplication>
#include "mainwindow.h"


ScriptGenWindow::ScriptGenWindow(QString *basePath) : QWizard()
{
    this->setupUi();

    QFile file(*basePath + "/Scripts/Scripts.json");
    file.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVariantMap mainMap = (doc.object()).toVariantMap();
    QVariantList localScripts = mainMap["Scripts"].toList();
    foreach (QVariant variant, localScripts) {
        QVariantMap map = variant.toMap();
        Script script;

        script.name = map["name"].toString();
        script.description = map["description"].toString();
        script.path = map["path"].toString();

        scripts.append(script);
        QListWidgetItem *item = new QListWidgetItem(script.name);
        item->setToolTip(script.description);
        generatorsList->addItem(item);
    }

    this->basePath = *basePath;
}

void ScriptGenWindow::setupUi()
{
    this->resize(400, 300);
    this->setWizardStyle(QWizard::NStyles);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
    this->setWindowTitle("Script Generator");


    wizardPage1 = new QWizardPage();
    wizardPage1->setObjectName(QString::fromUtf8("wizardPage1"));

    generatorsList = new QListWidget(wizardPage1);
    generatorsList->setGeometry(QRect(0, 0, 375, 250));

    connect(generatorsList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked()));

    this->addPage(wizardPage1);

    wizardPage2 = new QWizardPage();
    wizardPage2->setObjectName(QString::fromUtf8("wizardPage2"));

    enabled = new QCheckBox(wizardPage2);
    enabled->setObjectName(QString::fromUtf8("enabled"));
    enabled->setGeometry(QRect(0, 0, 61, 17));
    connect(enabled, SIGNAL(toggled(bool)), this, SLOT(toggleComponent(bool)));

    componentList = new QListWidget(wizardPage2);
    componentList->setObjectName(QString::fromUtf8("componentList"));
    componentList->setGeometry(QRect(100, 0, 271, 251));
    connect(componentList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked()));

    this->addPage(wizardPage2);

    connect(button(WizardButton::NextButton), SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(button(WizardButton::FinishButton), SIGNAL(clicked()), this, SLOT(finishClicked()));

    this->button(WizardButton::NextButton)->setEnabled(false);
    this->enabled->setEnabled(false);
}

void ScriptGenWindow::toggleComponent(bool enabled)
{
    Component selectedComponent = components.at(componentList->selectionModel()->currentIndex().row());
    selectedComponent.enabled = enabled;
    components.removeAt(componentList->selectionModel()->currentIndex().row());
    components.insert(componentList->selectionModel()->currentIndex().row(), selectedComponent);
}

void ScriptGenWindow::itemClicked()
{
    if(currentId() == 0) {
        this->button(WizardButton::NextButton)->setEnabled(true);
        componentList->clear();
    } else {
        Component selectedComponent = components.at(componentList->selectionModel()->currentIndex().row());
        if(!selectedComponent.name.startsWith("#main")) {
            enabled->setEnabled(true);
            if(selectedComponent.enabled) {
                enabled->setCheckState(Qt::CheckState::Checked);
            } else {
                enabled->setCheckState(Qt::CheckState::Unchecked);
            }
        } else {
            enabled->setEnabled(false);
        }
    }
}

void ScriptGenWindow::nextClicked()
{
    components.clear();
    componentList->clear();

    QFile file(basePath + "/Scripts/Data/" + scripts.at(generatorsList->selectionModel()->currentIndex().row()).path);
    file.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVariantMap mainMap = (doc.object()).toVariantMap();
    QVariantList localComponents = mainMap["Components"].toList();
    foreach (QVariant variant, localComponents) {
        QVariantMap map = variant.toMap();

        Component component;

        component.name = map["name"].toString();
        component.text = map["text"].toString();

        components.append(component);
        if(!component.name.startsWith("#main")) {
            componentList->addItem(map["name"].toString());
        } else {
            QListWidgetItem *item = new QListWidgetItem();
            item->setForeground(QBrush(QColor(160, 160, 160)));
            item->setText("Main script");
            componentList->addItem(item);
        }
    }
}

void ScriptGenWindow::finishClicked()
{
    QString finalString;

    foreach(Component component, components) {
        if(component.enabled) {
            finalString.append(component.text + '\n');
        }
    }
    MainWindow::getInstance()->insertText(finalString);
    qDebug() << finalString;
}



