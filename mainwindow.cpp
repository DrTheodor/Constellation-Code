#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QProcess>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercsharp.h>

#include "settingswindow.h"
#include "templatewindow.h"
#include "mainwindow.h"
#include "csharpapi.h"
#include "discord.h"

static const int MARGIN_SCRIPT_FOLD_INDEX = 2;
static MainWindow *_instance;

MainWindow::MainWindow()
{
    textEdit = new QsciScintilla;
    tabControl = new QTabWidget;

    setCentralWidget(tabControl);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(tabControl, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(tabControl, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabClose);

    tabControl->setTabsClosable(true);
    tabControl->setContentsMargins(0,0,0,0);
    tabControl->setMovable(true);

    templateWindow = new TemplateWindow();
    settingsWindow = new SettingsWindow();
    QString basePath = QCoreApplication::applicationDirPath();
    scriptGenWindow = new ScriptGenWindow(&basePath);

    discord->SetupDiscord();

    tabControl->addTab(createTextEdit(), "New File");
    _instance = this;
}


void MainWindow::onMargin(int position, int modifiers)
{
    MarginClick(position, modifiers);
}

void MainWindow::onTabChanged()
{
    textEdit = (QsciScintilla*)tabControl->currentWidget();
    if(textEdit != nullptr) {
        if(!textEdit->objectName().isEmpty()) {
            setWindowTitle(tr("%1[*] - Constellation Code").arg(strippedName(textEdit->objectName())));
        } else {
            setWindowTitle(tr("New File[*] - %2").arg(tr("Constellation Code")));
        }
        setWindowModified(tabControl->currentWidget()->isWindowModified());
    }
}



void MainWindow::openByProtocol(int instanceId, QByteArray message)
{
    if(message != nullptr) {
        bool createTab = true;
        for(int i = 0; i < tabControl->count(); i++) {
            if(message == tabControl->widget(i)->objectName()) {
                createTab = false;
                tabControl->setCurrentIndex(i);
                onTabChanged();
                break;
            }
        }
        if(createTab) {
            loadFile(message);
        }

        this->raise();
        this->show();
    }
}


void MainWindow::setupTheme(QString name)
{
    themeFile = name;

    QFile file(name);
    file.open(QFile::ReadOnly);
    QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    QJsonArray itemSelection = json.value("itemSelection").toArray();
    QJsonArray mouseSelection = json.value("mouseSelection").toArray();
    QJsonArray caretLineColor = json.value("caretLineColor").toArray();
    QColor tabColor = QColor(json.value("tabColor").toString());
    QColor selectedTabColor = QColor(json.value("selectedTabColor").toString());
    QColor menuForeColor = QColor(json.value("menuForeColor").toString());
    QColor marginColor = QColor(json.value("marginColor").toString());
    QColor backgroundColor = QColor(json.value("background").toString());

    Theme = ThemeStructure {
            .mouseSelection = { (mouseSelection[0].toInt()), mouseSelection[1].toInt(), mouseSelection[2].toInt(), mouseSelection[3].toInt() },
            .caretLineColor = QColor(caretLineColor[0].toInt(), caretLineColor[1].toInt(), caretLineColor[2].toInt(), caretLineColor[3].toInt()),
            .background = backgroundColor,
            .marginColor = marginColor,
            .foreground = QColor(json.value("foreground").toString()),
            .commentColor = QColor(json.value("commentColor").toString()),
            .keywordColor = QColor(json.value("keywordColor").toString()),
            .quotesColor = QColor(json.value("quotesColor").toString()),
            .marginForeground = QColor(json.value("marginForeground").toString()),
            .matchedBrace = QColor(json.value("matchedBrace").toString()),
            .unmatchedBrace = QColor(json.value("unmatchedBrace").toString()),
            .matchedBraceFore = QColor(json.value("matchedBraceFore").toString()),
            .unmatchedBraceFore = QColor(json.value("unmatchedBraceFore").toString()),
            .caretColor = QColor(json.value("caretColor").toString()),
            .brackColor = QColor(json.value("brackColor").toString()),
            .commentItalic = json.value("commentItalic").toBool(true),
            .caretWidth = (short)json.value("caretWidth").toInt(2),
            .numColor = QColor(json.value("numColor").toString())
    };

    qApp->setStyleSheet(tr(
    "QToolBar { border-bottom: 2px %1; border-top: 2px %1;}"
    "QToolBar::item:selected { background: rgba(%3,%4,%5,%6); }"
    "QToolButton:hover { background: rgba(%3,%4,%5,%6); border-radius: 2px; }"
    "QMenu::item:selected { background: rgba(%3,%4,%5,%6); }"
    "QMenu { border-radius: 0px; }"
    "QMenuBar::item:selected { background: rgba(%3,%4,%5,%6); }"
    "QTabBar::tab { background: %7; color: %8; }"
    "QTabBar::tab:selected { background: %9; }"
    "QTabWidget::pane { margin: 0px; padding: 0px; }"
    "QTabBar::close-button { image: url(:/images/close.png); }"
    "QWidget { background: %1; color: %2; }"
    "QPushButton { border-radius: 2px; border: 0.5px solid rgba(%3,%4,%5,%6); min-height: 15px; min-width: 50px; background: %10 }"
    "QPushButton:pressed { background: rgba(%3,%4,%5,%6); border-radius: 2px; }"
    "QListWidget::item:hover { background: rgba(%3,%4,%5,%6); }"
    "QListWidget::item:selected, QListView::item:selected { color: %2; background: rgba(%3,%4,%5,%6); }"
    "QScrollBar:vertical { border: none; background-color: %1; min-height: 30px; width: 5px; }"
    "QScrollBar::handle:vertical { background-color: %1; }"
    "QScrollBar::handle:vertical:hover { background-color: rgba(%3,%4,%5,%6); border-radius: 1px; }"
    "QScrollBar::sub-line:vertical { background: none; }"
    "QScrollBar::add-line:vertical { background: none; }"
    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { background: none; }"
    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }")
            .arg(marginColor.name())
            .arg(menuForeColor.name())
            .arg(itemSelection[0].toString())
            .arg(itemSelection[1].toString())
            .arg(itemSelection[2].toString())
            .arg(itemSelection[3].toString())
            .arg(tabColor.name())
            .arg(menuForeColor.name())
            .arg(selectedTabColor.name())
            .arg(backgroundColor.name()));

    standardFont = new QFont("JetBrains Mono", 10.0, 50);
    commentFont = standardFont;
}

QWidget* MainWindow::createTextEdit()
{
    QsciScintilla *sci = new QsciScintilla;
    CSharpApi *lex = new CSharpApi(this);

    sci->setUtf8(true);
    sci->setLexer(lex);

    sci->zoomTo(4);
    sci->setEdgeColumn(50);

    sci->setWrapMode(QsciScintilla::WrapWord);
    sci->setWrapVisualFlags(QsciScintilla::WrapFlagByText);
    sci->setWrapIndentMode(QsciScintilla::WrapIndentSame);

    sci->setAutoIndent(true);
    sci->setIndentationGuides(false);
    sci->setIndentationsUseTabs(true);
    sci->setIndentationWidth(4);
    sci->setTabWidth(4);

    sci->setMarginLineNumbers(1, true);
    sci->setMarginWidth(1, 50);

    sci->SendScintilla(QsciScintilla::SCI_SETSTYLEBITS, 5);

    lex->setFoldComments(true);

    sci->setMarginWidth(MARGIN_SCRIPT_FOLD_INDEX, 0);
    sci->SendScintilla(QsciScintilla::SCI_SETMARGINTYPEN,  MARGIN_SCRIPT_FOLD_INDEX, QsciScintilla::SC_MARGIN_SYMBOL);
    sci->SendScintilla(QsciScintilla::SCI_SETMARGINMASKN, MARGIN_SCRIPT_FOLD_INDEX, QsciScintilla::SC_MASK_FOLDERS);
    sci->setMarginWidth(MARGIN_SCRIPT_FOLD_INDEX, 20);

    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDER, QsciScintilla::SC_MARK_PLUS);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDEROPEN, QsciScintilla::SC_MARK_MINUS);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDEREND, QsciScintilla::SC_MARK_EMPTY);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDERMIDTAIL, QsciScintilla::SC_MARK_EMPTY);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDEROPENMID, QsciScintilla::SC_MARK_EMPTY);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDERSUB, QsciScintilla::SC_MARK_EMPTY);
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, QsciScintilla::SC_MARKNUM_FOLDERTAIL, QsciScintilla::SC_MARK_EMPTY);

    sci->SendScintilla(QsciScintilla::SCI_SETFOLDFLAGS, 16);
    sci->SendScintilla(QsciScintilla::SCI_SETMARGINSENSITIVEN, MARGIN_SCRIPT_FOLD_INDEX, 1);

    sci->setAutoCompletionSource(QsciScintilla::AcsAll);
    sci->setAutoCompletionCaseSensitivity(true);
    sci->setAutoCompletionReplaceWord(false);
    sci->setAutoCompletionUseSingle(QsciScintilla::AcusNever);
    sci->setAutoCompletionThreshold(1);

    lex->setStylePreprocessor(true);

    sci->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    sci->setCaretLineVisible(false);


    if(Theme.caretLineColor != nullptr) {
        sci->setCaretLineVisible(true);
        sci->setCaretLineBackgroundColor(Theme.caretLineColor);
    }

    lex->setColor(Theme.quotesColor, 19);
    lex->setColor(Theme.quotesColor, 13);
    lex->setColor(Theme.quotesColor, 12);
    lex->setColor(Theme.foreground, 11);
    lex->setColor(Theme.brackColor, 10);
    lex->setColor(Theme.quotesColor, 9);
    lex->setColor(Theme.quotesColor, 8);
    lex->setColor(Theme.quotesColor, 7);
    lex->setColor(Theme.quotesColor, 6);
    lex->setColor(Theme.keywordColor, 5);
    lex->setColor(Theme.numColor, 4);
    lex->setColor(Theme.quotesColor, 3);
    lex->setColor(Theme.commentColor, 2);
    lex->setColor(Theme.commentColor, 1);

    (*commentFont).setItalic(Theme.commentItalic);
    lex->setFont(*commentFont, 2);
    lex->setFont(*commentFont, 1);
    lex->setFont(*standardFont, 11);
    lex->setFont(*standardFont, 10);
    lex->setFont(*standardFont, 5);
    lex->setFont(*standardFont, 4);

    sci->setEdgeColor(Theme.foreground);
    sci->setCaretForegroundColor(Theme.caretColor);
    sci->setCaretWidth(Theme.caretWidth);
    sci->setSelectionBackgroundColor(QColor(Theme.mouseSelection[0], Theme.mouseSelection[1],
                                                    Theme.mouseSelection[2], Theme.mouseSelection[3]));
    lex->setPaper(Theme.background);
    sci->setMarginsForegroundColor(Theme.marginForeground);
    sci->setMarginsBackgroundColor(Theme.marginColor);
    sci->setMarginsFont(*standardFont);

    sci->SendScintilla(QsciScintilla::SCI_SETFOLDMARGINCOLOUR, MARGIN_SCRIPT_FOLD_INDEX, Theme.marginColor);
    sci->SendScintilla(QsciScintilla::SCI_SETFOLDMARGINHICOLOUR, MARGIN_SCRIPT_FOLD_INDEX, Theme.marginColor);

    sci->setUnmatchedBraceBackgroundColor(Theme.unmatchedBrace);
    sci->setUnmatchedBraceForegroundColor(Theme.unmatchedBraceFore);
    sci->setMatchedBraceBackgroundColor(Theme.matchedBrace);
    sci->setMatchedBraceForegroundColor(Theme.matchedBraceFore);

    connect(sci, SIGNAL(textChanged()),
            this, SLOT(documentWasModified()));
    connect(sci, &QsciScintilla::SCN_MARGINCLICK, this, &MainWindow::onMargin);

    return sci;
}


bool MainWindow::MarginClick(int position, int modifiers)
{
    int lineClick = textEdit->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, position);
    if ((modifiers & QsciScintilla::SCMOD_SHIFT) && (modifiers & QsciScintilla::SCMOD_CTRL)) {
        FoldAll();
    } else {
        int levelClick = textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDLEVEL, lineClick);
        if (levelClick & QsciScintilla::SC_FOLDLEVELHEADERFLAG) {
            if (modifiers & QsciScintilla::SCMOD_SHIFT) {
                EnsureAllChildrenVisible(lineClick, levelClick);
            } else if (modifiers & QsciScintilla::SCMOD_CTRL) {
                ToggleFoldRecursive(lineClick, levelClick);
            } else {
                textEdit->SendScintilla(QsciScintilla::SCI_TOGGLEFOLD, lineClick);
            }
        }
    }
    return true;
}

void MainWindow::FoldAll()
{
    textEdit->SendScintilla(QsciScintilla::SCI_COLOURISE, 0, -1);
    int maxLine = textEdit->SendScintilla(QsciScintilla::SCI_GETLINECOUNT);
    bool expanding = true;
    for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) {
        if (textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDLEVEL, lineSeek) & QsciScintilla::SC_FOLDLEVELHEADERFLAG) {
            expanding = !textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDEXPANDED, lineSeek);
            break;
        }
    }
    for (int line = 0; line < maxLine; line++) {
        int level = textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDLEVEL, line);
        if ((level & QsciScintilla::SC_FOLDLEVELHEADERFLAG) &&
                (QsciScintilla::SC_FOLDLEVELBASE == (level & QsciScintilla::SC_FOLDLEVELNUMBERMASK))) {
            if (expanding) {
                textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, 1);
                Expand(line, true, false, 0, level);
                line--;
            } else {
                int lineMaxSubord = textEdit->SendScintilla(QsciScintilla::SCI_GETLASTCHILD, line, -1);
                textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, (long)0);
                if (lineMaxSubord > line)
                    textEdit->SendScintilla(QsciScintilla::SCI_HIDELINES, line + 1, lineMaxSubord);
            }
        }
    }
}

void MainWindow::ToggleFoldRecursive(int line, int level)
{
    if (textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDEXPANDED, line)) {
        textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, (long)0);
        Expand(line, false, true, 0, level);
    } else {
        textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, 1);
        Expand(line, true, true, 100, level);
    }
}

void MainWindow::Expand(int &line, bool doExpand, bool force, int visLevels, int level) {
    int lineMaxSubord = textEdit->SendScintilla(QsciScintilla::SCI_GETLASTCHILD, line, level & QsciScintilla::SC_FOLDLEVELNUMBERMASK);
    line++;
    while (line <= lineMaxSubord) {
        if (force) {
            if (visLevels > 0)
                textEdit->SendScintilla(QsciScintilla::SCI_SHOWLINES, line, line);
            else
                textEdit->SendScintilla(QsciScintilla::SCI_HIDELINES, line, line);
        } else {
            if (doExpand)
                textEdit->SendScintilla(QsciScintilla::SCI_SHOWLINES, line, line);
        }
        int levelLine = level;
        if (levelLine == -1)
            levelLine = textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDLEVEL, line);
        if (levelLine & QsciScintilla::SC_FOLDLEVELHEADERFLAG) {
            if (force) {
                if (visLevels > 1)
                    textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, 1);
                else
                    textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, (long)0);
                Expand(line, doExpand, force, visLevels - 1);
            } else {
                if (doExpand) {
                    if (!textEdit->SendScintilla(QsciScintilla::SCI_GETFOLDEXPANDED, line))
                        textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, 1);
                    Expand(line, true, force, visLevels - 1);
                } else {
                    Expand(line, false, force, visLevels - 1);
                }
            }
        } else {
            line++;
        }
    }
}

void MainWindow::EnsureAllChildrenVisible(int line, int level)
{
    textEdit->SendScintilla(QsciScintilla::SCI_SETFOLDEXPANDED, line, 1);
    Expand(line, true, true, 100, level);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    for(int i = 0; i <= (tabControl->count() - 1); i++) {
        if (maybeSave(i)) {
            discord->Shutdown();
            event->accept();
        } else {
            event->ignore();
        }
    }
}

void MainWindow::newFile()
{
    tabControl->addTab(createTextEdit(), "New File");
}

void MainWindow::onTabClose(int index)
{
    if (maybeSave(index)) {
        auto widget = tabControl->widget(index);
        QsciScintilla* sci = ((QsciScintilla*)widget);

        tabControl->removeTab(index);

        sci->SendScintilla(sci->SCI_EMPTYUNDOBUFFER);

        widget->deleteLater();
        sci->deleteLater();
        delete sci->lexer();
        delete sci;

        if(tabControl->count() == 0) {
            tabControl->addTab(createTextEdit(), "New File");
        }
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::openSettingsWindow()
{
    settingsWindow->show();
}

void MainWindow::openTemplateWindow()
{
    templateWindow->show();
}

void MainWindow::openScriptGenerator()
{
    scriptGenWindow->show();
}

void MainWindow::toggleDRPC()
{
    discord->Toggle();
    discord->UpdateDiscord();
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Constellation Code"),
            tr("<b>Constellation Code v3rc5</b> is a best "
               "lightweight code editor for <b>Unity</b>."));
}

void MainWindow::documentWasModified()
{
    tabControl->currentWidget()->setWindowModified(textEdit->isModified());
    setWindowModified(textEdit->isModified());
}

bool MainWindow::save()
{
    if(!textEdit->objectName().isEmpty()) {
        return saveFile(textEdit->objectName());
    } else {
        return saveFile();
    }
}

bool MainWindow::saveAs()
{
    return saveFile();
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    prefAct = new QAction(tr("Preferences..."), this);
    prefAct->setStatusTip(tr("Open preferences window"));
    connect(prefAct, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));

    templateAct = new QAction(tr("Templates..."), this);
    templateAct->setStatusTip(tr("Open template's window"));
    connect(templateAct, SIGNAL(triggered()), this, SLOT(openTemplateWindow()));

    generatorAct = new QAction(tr("Generate Script..."), this);
    generatorAct->setStatusTip(tr("Open Script Generator window"));
    connect(generatorAct, SIGNAL(triggered()), this, SLOT(openScriptGenerator()));

    discordRPCAct = new QAction(tr("Toggle RPC"), this);
    discordRPCAct->setStatusTip(tr("Enable or Disable DiscordRPC"));
    discordRPCAct->setCheckable(true);
    connect(discordRPCAct, SIGNAL(triggered()), this, SLOT(toggleDRPC()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(prefAct);
    editMenu->addAction(templateAct);
    editMenu->addAction(generatorAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addSeparator();
    helpMenu->addAction(discordRPCAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("MineLines", "Constellation Code");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    QString theme = settings.value("theme", ":/themes/dark.theme").toString();
    bool gRPC = settings.value("rpc", true).toBool();

    setupTheme(theme);
    resize(size);
    move(pos);
    discord->Toggle(gRPC);
    discordRPCAct->setChecked(gRPC);
}

void MainWindow::writeSettings()
{
    QSettings settings("MineLines", "Constellation Code");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("theme", themeFile);
    settings.setValue("rpc", discord->rpc);
}

bool MainWindow::maybeSave(int index)
{
    if (tabControl->widget(index)->isWindowModified()) {
        QMessageBox msgBox("Constellation Code", "The document has been modified.\n\nDo you want to save your changes?", QMessageBox::Warning,
                            QMessageBox::Save, QMessageBox::Discard, QMessageBox::Cancel);
        msgBox.deleteLater();
        int ret = msgBox.exec();

        if (ret == QMessageBox::Save)
            return saveFile(tabControl->widget(index)->objectName(), index);
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;

}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Constellation Code"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    if(!fileName.endsWith(".theme")) {
        QTextStream in(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        tabControl->addTab(createTextEdit(), strippedName(fileName));
        tabControl->setCurrentIndex(tabControl->count() - 1);
        textEdit->setText(in.readAll());
        tabControl->currentWidget()->setWindowModified(false);
        QApplication::restoreOverrideCursor();

        textEdit->setObjectName(fileName);
        setWindowTitle(tr("%1[*] - %2").arg(strippedName(textEdit->objectName())).arg(tr("Constellation Code")));
        setWindowModified(false);
        statusBar()->showMessage(tr("File loaded"), 2000);
    } else {
        themeFile = fileName;
        writeSettings();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

bool MainWindow::saveFile(const QString &fileName, int index)
{
    if(!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Constellation Code"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName, file.errorString()));
            return false;
        }

        QTextStream out(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        out << textEdit->text();
        QApplication::restoreOverrideCursor();

        textEdit->setObjectName(fileName);
        statusBar()->showMessage(tr("File saved"), 2000);
        textEdit->setWindowModified(false);

        setWindowTitle(tr("%1[*] - Constellation Code").arg(strippedName(textEdit->objectName())));
        setWindowModified(false);
        return true;
    } else {
        QString file = QFileDialog::getSaveFileName(this);
        if (file.isEmpty())
            return false;
        tabControl->setTabText(index, strippedName(file));
        textEdit->setObjectName(file);
        textEdit->setWindowModified(false);

        setWindowTitle(tr("New File[*] - %2").arg(tr("Constellation Code")));
        setWindowModified(false);
        return saveFile(file, index);
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::insertText(const QString &text)
{
    textEdit->insert(text);
}

MainWindow *MainWindow::getInstance()
{
    return _instance;
}

