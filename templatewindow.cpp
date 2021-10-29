#include "templatewindow.h"
#include "mainwindow.h"

#include <QSettings>
#include <QCloseEvent>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>


TemplateWindow::TemplateWindow()
{
    qRegisterMetaTypeStreamOperators<QMap<QString,QString>>("templateList");
    readSettings();
    createUi();

    setWindowTitle("Constellation Templates");
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowIcon(QIcon(":/images/templates.png"));
}

TemplateWindow::~TemplateWindow()
{
    delete window;
}

void TemplateWindow::readSettings()
{
    QSettings settings("MineLines", "Constellation Code");

    templateList = settings.value("Templates").value<QMap<QString,QString>>();
}

void TemplateWindow::writeSettings()
{
    QSettings settings("MineLines", "Constellation Code");

    settings.setValue("Templates", QVariant::fromValue(templateList));
}

void TemplateWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void TemplateWindow::createUi()
{
    window = new QWidget;
    setCentralWidget(window);

    QVBoxLayout *box = new QVBoxLayout(window);
    QPushButton *deleteButton = new QPushButton("Remove");

    actionButton = new QPushButton("Use");
    list = new QListWidget();
    title = new QLineEdit();
    text = new QTextEdit();
    editCheckbox = new QCheckBox("Edit");

    text->setReadOnly(true);
    title->setReadOnly(true);
    text->setTabStopDistance(12);

    connect(actionButton, SIGNAL(released()), this, SLOT(performAction()));
    connect(deleteButton, SIGNAL(released()), this, SLOT(performDelete()));
    connect(list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    connect(editCheckbox, SIGNAL(toggled(bool)), this, SLOT(editToggled()));

    title->setPlaceholderText("Unique template name");

    box->addWidget(list);
    box->addWidget(title);
    box->addWidget(text);
    box->addWidget(editCheckbox);
    box->addWidget(actionButton);
    box->addWidget(deleteButton);

    list->addItems(templateList.keys());
}

void TemplateWindow::performAction()
{
    if(isEditing) {
        if(!title->text().isEmpty() && !text->toPlainText().isEmpty()) {
            QModelIndex index = list->currentIndex();
            templateList.insert(title->text(), text->toPlainText());

            list->clear();
            list->addItems(templateList.keys());
            list->setCurrentIndex(index);

            text->setText("");
            title->setText("");

            editCheckbox->toggle();
        }
    } else {
        if(list->selectedItems().count() > 0) {
             MainWindow::getInstance()->insertText(templateList[list->selectedItems()[0]->text()]);
        }
    }
}

void TemplateWindow::performDelete()
{
    if(list->selectedItems().count() > 0) {
        QModelIndex index = list->currentIndex();

        templateList.remove(list->selectedItems()[0]->text());

        text->setText("");
        title->setText("");

        list->clear();
        list->addItems(templateList.keys());
        list->setCurrentIndex(index);
    }
}

void TemplateWindow::itemClicked(QListWidgetItem *item)
{
    title->setText(item->text());
    text->setText(templateList[item->text()]);
}

void TemplateWindow::editToggled()
{
    isEditing = !isEditing;
    if(isEditing) {
        actionButton->setText("Add");
        title->setReadOnly(false);
        text->setReadOnly(false);
    } else {
        actionButton->setText("Use");
        title->setReadOnly(true);
        text->setReadOnly(true);
    }
}
