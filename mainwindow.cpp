#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include "codeeditor.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QTextStream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    textChanged=false;
    if(!userEditConfirmed())
        return;
    filePath ="";
    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件--编辑器"));
    textChanged=false;
    statusLabel.setMaximumWidth(180);
    statusLabel.setText("length: "+QString::number(0)+"     lines: "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);


    statusCursorLabel.setMaximumWidth(180);
    statusCursorLabel.setText("Ln: "+QString::number(0)+"     Col: "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author=new QLabel(ui->statusbar);
    author->setText(tr("胡福生"));
    ui->statusbar->addPermanentWidget(author);
    ui->actionCopy->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionPaste->setEnabled(false);
    QPlainTextEdit::LineWrapMode mode=ui->textEdit->lineWrapMode();
    if(mode == QTextEdit::NoWrap)
    {
        ui->textEdit->setLineWrapMode((QPlainTextEdit::WidgetWidth));
        ui->actionWrap->setChecked(false);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(true);
    }
    Highlighter*newLight = new Highlighter(ui->textEdit->document());
    ui->actionStatus->setChecked(true);
    ui->actionTool->setChecked(true);
    ui->actionGotoNumber->setChecked(false);
    //on_actionGotoNumber_triggered(false);
    connect(ui->actionGotoNumber,SIGNAL(triggered(bool)),ui->textEdit,SLOT(hideLineNumberArea(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg(this,ui->textEdit);
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg(this,ui->textEdit);
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    MainWindow*newWindow=new MainWindow;
    newWindow->show();
}


void MainWindow::on_actionOpen_triggered()
{

    if(!userEditConfirmed())
        return;

    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files (*.txt));;All(*.*)"));
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }
    filePath=filename;
    if(list.contains(filePath))
        sign=1;
    list.append(filePath);
    lists();
    ui->textEdit->clear();
    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged=false;
    RecentHistory();
}


void MainWindow::on_actionSave_triggered()
{
    if(filePath=="")
    {
        QString filename = QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files (*.txt))"));
        QFile file(filename);
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
             QMessageBox::warning(this,"..","保存文件失败");
             return;
        }
        file.close();
        filePath =filename;
    }
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
         QMessageBox::warning(this,"..","保存文件失败");
         return;
    }
    QTextStream out(&file);
    QString text =ui->textEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChanged=false;
}


void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"另保存文件",".",tr("Text files (*.txt))"));
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
         QMessageBox::warning(this,"..","另保存文件失败");
         return;
    }
    filePath =filename;
    QTextStream out(&file);
    QString text =ui->textEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChanged)
    {
        this->setWindowTitle("*"+this->windowTitle());
        textChanged=true;
    }
    statusLabel.setText("length: "+QString::number(ui->textEdit->toPlainText().length())+"     lines: "+QString::number(ui->textEdit->document()->lineCount()));
}

bool MainWindow::userEditConfirmed()
{
    if(textChanged)
    {
        QString path=(filePath !="")?filePath:"无标题.txt";
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存道\n")+"\""+path+"\"?");
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        int r=msg.exec();
        switch (r)
        {
            case QMessageBox::Yes:
                on_actionSaveAs_triggered();
                break;
            case QMessageBox::No:
                textChanged=false;
                break;
            case QMessageBox::Cancel:
                return false;
        }
    }
    return true;
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
        ui->actionCopy->setEnabled(b);
        ui->actionCut->setEnabled(b);
}


void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color =QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid())
    {
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit{color:%1}").arg(color.name()));
    }
}


void MainWindow::on_actionEditorBackgroundColor_triggered()
{
    QColor color =QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid())
    {
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit{background-color:%1}").arg(color.name()));
    }
}


void MainWindow::on_actionFontBackgroundColor_triggered()
{

}


void MainWindow::on_actionWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode=ui->textEdit->lineWrapMode();
    if(mode == QTextEdit::NoWrap)
    {
        ui->textEdit->setLineWrapMode((QPlainTextEdit::WidgetWidth));
        ui->actionWrap->setChecked(true);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(false);
    }

}


void MainWindow::on_actionFont_triggered()
{
    bool ok=false;
    QFont font=QFontDialog::getFont(&ok,this);
    if(ok)
        ui->textEdit->setFont(font);
}


void MainWindow::on_actionTool_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionTool->setChecked(!visible);

}


void MainWindow::on_actionStatus_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionStatus->setChecked(!visible);

}


void MainWindow::on_actionSelectAll_triggered()
{
    ui->textEdit->selectAll();
}


void MainWindow::on_actionExit_triggered()
{
    if(userEditConfirmed())
        exit(0);
}


void MainWindow::on_textEdit_cursorPositionChanged()
{
    int col=0;
    int ln=0;
    int flg=-1;
    int pos=ui->textEdit->textCursor().position();
    QString text = ui->textEdit->toPlainText();
    for(int i=0;i<pos;i++)
    {
        if(text[i]=='\n')
        {
            ln ++;
            flg=i;
        }
    }
    flg++;
    col=pos-flg;
    statusCursorLabel.setText("ln: "+QString::number(ln +1)+"     Col: "+QString::number(col+1));
}


//void MainWindow::on_actionGotoNumber_triggered(bool checked)
//{
//    ui->textEdit->hideLineNumberArea(!checked);
//}

void MainWindow::loadFile1(){
    filePath=diflist[0];
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}

void MainWindow::loadFile2(){
    filePath=diflist[1];
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}

void MainWindow::loadFile3(){
    filePath=diflist[2];
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}

void MainWindow::loadFile4(){
    filePath=diflist[3];
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}

void MainWindow::loadFile5(){
    filePath=diflist[4];
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }
    ui->textEdit->clear();
    QTextStream in(&file);
    in.setCodec("gbk");
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged=false;
}

void MainWindow::lists(){
    int a;
    a=diflist.length();
    for(int j=a-1;j>=0;j--)
        diflist.removeAt(j);

    for(int i=list.length()-1; i>=0;i--)
    {
       if(!diflist.contains(list[i]))
       {
          diflist.append(list[i]);
       }
    }

}

void MainWindow::RecentHistory(){
    if((diflist.length()==2&&sign==0)||(diflist.length()==1&&sign==1))
    {
        ui->menu_6->removeAction(one);
    }

    if((diflist.length()==3&&sign==0)||(diflist.length()==2&&sign==1))
    {
        ui->menu_6->removeAction(one);
        ui->menu_6->removeAction(two);
    }

    if((diflist.length()==4&&sign==0)||(diflist.length()==3&&sign==1))
    {
        ui->menu_6->removeAction(one);
        ui->menu_6->removeAction(two);
        ui->menu_6->removeAction(three);
    }

    if((diflist.length()==5&&sign==0)||(diflist.length()==4&&sign==1))
    {
        ui->menu_6->removeAction(one);
        ui->menu_6->removeAction(two);
        ui->menu_6->removeAction(three);
        ui->menu_6->removeAction(four);
    }
    if((diflist.length()>=6&&sign==0)||(diflist.length()>=5&&sign==1))
    {
        ui->menu_6->removeAction(one);
        ui->menu_6->removeAction(two);
        ui->menu_6->removeAction(three);
        ui->menu_6->removeAction(four);
        ui->menu_6->removeAction(five);
    }

    if(diflist.length()==1)
    {
       one = new QAction(QFileInfo(diflist[0]).fileName());
       ui->menu_6->addAction(one);
       connect(one,SIGNAL(triggered()),this,SLOT(loadFile1()));
    }

    if(diflist.length()==2)
    {
       one = new QAction(QFileInfo(diflist[0]).fileName());
       ui->menu_6->addAction(one);
       connect(one,SIGNAL(triggered()),this,SLOT(loadFile1()));
       two = new QAction(QFileInfo(diflist[1]).fileName());
       ui->menu_6->addAction(two);
       connect(two,SIGNAL(triggered()),this,SLOT(loadFile2()));
    }

    if(diflist.length()==3)
    {
       one = new QAction(QFileInfo(diflist[0]).fileName());
       ui->menu_6->addAction(one);
       connect(one,SIGNAL(triggered()),this,SLOT(loadFile1()));
       two = new QAction(QFileInfo(diflist[1]).fileName());
       ui->menu_6->addAction(two);
       connect(two,SIGNAL(triggered()),this,SLOT(loadFile2()));
       three = new QAction(QFileInfo(diflist[2]).fileName());
       ui->menu_6->addAction(three);
       connect(three,SIGNAL(triggered()),this,SLOT(loadFile3()));
    }

    if(diflist.length()==4)
    {
       one = new QAction(QFileInfo(diflist[0]).fileName());
       ui->menu_6->addAction(one);
       connect(one,SIGNAL(triggered()),this,SLOT(loadFile1()));
       two = new QAction(QFileInfo(diflist[1]).fileName());
       ui->menu_6->addAction(two);
       connect(two,SIGNAL(triggered()),this,SLOT(loadFile2()));
       three = new QAction(QFileInfo(diflist[2]).fileName());
       ui->menu_6->addAction(three);
       connect(three,SIGNAL(triggered()),this,SLOT(loadFile3()));
       four = new QAction(QFileInfo(diflist[3]).fileName());
       ui->menu_6->addAction(four);
       connect(four,SIGNAL(triggered()),this,SLOT(loadFile4()));
    }

    if(diflist.length()>=5)
    {
       one = new QAction(QFileInfo(diflist[0]).fileName());
       ui->menu_6->addAction(one);
       connect(one,SIGNAL(triggered()),this,SLOT(loadFile1()));
       two = new QAction(QFileInfo(diflist[1]).fileName());
       ui->menu_6->addAction(two);
       connect(two,SIGNAL(triggered()),this,SLOT(loadFile2()));
       three = new QAction(QFileInfo(diflist[2]).fileName());
       ui->menu_6->addAction(three);
       connect(three,SIGNAL(triggered()),this,SLOT(loadFile3()));
       four = new QAction(QFileInfo(diflist[3]).fileName());
       ui->menu_6->addAction(four);
       connect(four,SIGNAL(triggered()),this,SLOT(loadFile4()));
       five = new QAction(QFileInfo(diflist[4]).fileName());
       ui->menu_6->addAction(five);
       connect(five,SIGNAL(triggered()),this,SLOT(loadFile5()));
    }
    sign=0;
}

