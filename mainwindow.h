#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "csharpapi.h"
#include "settingswindow.h"
#include "templatewindow.h"
#include "scriptgenwindow.h"

class QAction;
class QMenu;
class QsciScintilla;
class QsciAbstractAPIs;
class list;
class Discord;
class QSharedMemory;
class ScriptGenWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow();
    static MainWindow *getInstance();
    void insertText(const QString &text);
    struct ThemeStructure {
           int mouseSelection[4];
           QColor caretLineColor;
           QColor background;
           QColor marginColor;
           QColor foreground;
           QColor commentColor;
           QColor keywordColor;
           QColor quotesColor;
           QColor marginForeground;
           QColor matchedBrace;
           QColor unmatchedBrace;
           QColor matchedBraceFore;
           QColor unmatchedBraceFore;
           QColor caretColor;
           QColor brackColor;
           bool commentItalic;
           short caretWidth;
           QColor numColor;
    };
    ThemeStructure Theme;
    void loadFile(const QString &fileName = 0);
    QString *basePath;


public slots:
    void openByProtocol(int instanceId, QByteArray message);

protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void openSettingsWindow();
    void openTemplateWindow();
    void openScriptGenerator();
    void onTabChanged();
    void onTabClose(int index);
    void toggleDRPC();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave(int index);
    void setupText();
    void setupTheme(QString name);
    void onMargin(int position, int modifiers);
    bool MarginClick(int position, int modifiers);
    void Expand(int &line, bool doExpand,
                  bool force = false, int visLevels = 0, int level = -1);
    void FoldAll();
    void ToggleFoldRecursive(int line, int level);
    void EnsureAllChildrenVisible(int line, int level);
    bool saveFile(const QString &fileName = 0, int index = 0);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QWidget *createTextEdit();
    SettingsWindow *settingsWindow;
    TemplateWindow *templateWindow;
    ScriptGenWindow *scriptGenWindow;

    QsciScintilla *textEdit;
    QTabWidget *tabControl;
    CSharpApi *lexCs;
    QString curFile;
    QString themeFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *prefAct;
    QAction *templateAct;
    QAction *generatorAct;
    QAction *discordRPCAct;
    Discord *discord;
    QFont *standardFont;
    QFont *commentFont;

};

#endif
