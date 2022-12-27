#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDesktopServices>
#include "highlighter.h"
#include <QMainWindow>
#include <QLabel>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();

    void on_actionFind_triggered();

    void on_actionReplace_triggered();

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_textEdit_textChanged();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionRedo_triggered();

    void on_textEdit_undoAvailable(bool b);

    void on_textEdit_copyAvailable(bool b);

    void on_textEdit_redoAvailable(bool b);

    void on_actionFontColor_triggered();

    void on_actionEditorBackgroundColor_triggered();

    void on_actionFontBackgroundColor_triggered();

    void on_actionWrap_triggered();

    void on_actionFont_triggered();

    void on_actionTool_triggered();

    void on_actionStatus_triggered();

    void on_actionSelectAll_triggered();

    void on_actionExit_triggered();

    void on_textEdit_cursorPositionChanged();

    //void on_actionGotoNumber_triggered(bool checked);


private slots:
    void loadFile1();
    void loadFile2();
    void loadFile3();
    void loadFile4();
    void loadFile5();
    void lists();
    void RecentHistory();

private:
    Ui::MainWindow *ui;
    QLabel statusCursorLabel;
    QLabel statusLabel;
    QString filePath;
    QList<QString> diflist;
    QList<QString> list;
    bool textChanged;
    bool userEditConfirmed();
    QAction *one,*two,*three,*four,*five;
    int sign=0;

};
#endif // MAINWINDOW_H
