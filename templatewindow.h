#ifndef TEMPLATEWINDOW_H
#define TEMPLATEWINDOW_H

#include <QMainWindow>
#include <QMap>

class QLineEdit;
class QTextEdit;
class QListWidgetItem;
class QListWidget;
class MainWindow;
class QPushButton;
class QCheckBox;

class TemplateWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit TemplateWindow(MainWindow *mw);
    virtual ~TemplateWindow();

private:
    void readSettings();
    void writeSettings();
    void createUi();
    QMap<QString,QString> templateList;
    QLineEdit *title;
    QTextEdit *text;
    QListWidget *list;
    QPushButton *actionButton;
    QWidget *window;
    QCheckBox *editCheckbox;
    bool isEditing = false;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void performAction();
    void editToggled();
    void itemClicked(QListWidgetItem *item);
    void performDelete();
};
#endif
