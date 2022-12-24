#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}
void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::hideLineNumberArea(bool flag)
{
    if(flag)
    {
        lineNumberArea->hide();
        setViewportMargins(0, 0, 0, 0);
    }
    else
    {
        lineNumberArea->show();
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    }
}
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::red);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
          if (block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);
                painter.setPen(Qt::black);
                painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                                 Qt::AlignRight, number);
            }

            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++blockNumber;
        }
    }

void CodeEditor::mouseDoubleClickEvent(QMouseEvent* e)
{
    QWidget::mouseDoubleClickEvent(e);

    if (e->button() != Qt::LeftButton)
        return;

    QRegExp urlRex = QRegExp("(http|ftp|https):\\/\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&:/~\\+#]*[\\w\\-\\@?^=%&/~\\+#])?");

    //获取光标所在的位置
    QTextCursor cursor = textCursor();
    QString str = cursor.block().text();
    int blockPos = cursor.positionInBlock();

    //匹配正则，如果光标所在位置是一个链接,那么直接打开
    int pos = 0;
    int length = 0;
    while ((pos = urlRex.indexIn(str, pos)) != -1) {
        length = urlRex.matchedLength();
        if (blockPos - pos < length)
        {
            auto temp = str.mid(pos, length - 1);
            QUrl url(temp);
            QDesktopServices::openUrl(QUrl(url));
            break;
        }
        pos += urlRex.matchedLength();
    }
}
