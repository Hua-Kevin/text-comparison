// infix_iterator.h
//
// Lifted from Jerry Coffin's 's prefix_ostream_iterator
#if !defined(INFIX_ITERATOR_H_)
#define  INFIX_ITERATOR_H_
#include <ostream>
#include <iterator>
template <class T,
          class charT=char,
          class traits=std::char_traits<charT> >
class infix_ostream_iterator :
    public std::iterator<std::output_iterator_tag,void,void,void,void>
{
    std::basic_ostream<charT,traits> *os;
    charT const* delimiter;
    bool first_elem;
public:
    typedef charT char_type;
    typedef traits traits_type;
    typedef std::basic_ostream<charT,traits> ostream_type;
    infix_ostream_iterator(ostream_type& s)
        : os(&s),delimiter(0), first_elem(true)
    {}
    infix_ostream_iterator(ostream_type& s, charT const *d)
        : os(&s),delimiter(d), first_elem(true)
    {}
    infix_ostream_iterator<T,charT,traits>& operator=(T const &item)
    {
        // Here's the only real change from ostream_iterator:
        // Normally, the '*os << item;' would come before the 'if'.
        if (!first_elem && delimiter != 0)
            *os << delimiter;
        *os << item;
        first_elem = false;
        return *this;
    }
    infix_ostream_iterator<T,charT,traits> &operator*() {
        return *this;
    }
    infix_ostream_iterator<T,charT,traits> &operator++() {
        return *this;
    }
    infix_ostream_iterator<T,charT,traits> &operator++(int) {
        return *this;
    }
};
#endif

#include <QtWidgets>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <QRegularExpression>
#include <QValidator>

#include <QFile>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>


#include "mainwindow.h"

MainWindow::MainWindow() : textEdit(new QTextEdit), findButton(new QPushButton("Find")), lineEdit(new QLineEdit(";([B])")) {

    /*QWidget * wdg = new QWidget(this);
    QVBoxLayout *vlay = new QVBoxLayout(wdg);

    QPushButton *btn1 = new QPushButton("btn1");
    btn1->setText(tr("Find Text: "));

    vlay->addWidget(btn1);
    vlay->addWidget(textEdit);
    wdg->setLayout(vlay);

    setCentralWidget(wdg);*/

    QWidget *window = new QWidget(this);
    //*findButton = new QPushButton("Find");
    //*lineEdit = new QLineEdit();
    //lineEdit = findChild<QLineEdit*>("lineEdit");
    QGridLayout *layout = new QGridLayout(window);
    //layout->addWidget(findButton, 0, 0);
    //layout->addWidget(lineEdit, 0, 1);

    layout->addWidget(findButton, 0, 0);
    layout->addWidget(lineEdit, 0, 1);
    layout->addWidget(textEdit, 1, 0, 1, 2);
    setCentralWidget(window);
    //setCentralWidget(textEdit);

    createActions();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

    connect(findButton, SIGNAL(clicked()), this, SLOT(on_findButton_clicked()));


#ifndef QT_NO_SESSIONMANAGER
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile() {
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

bool MainWindow::save() {
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs() {
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about(){
   QMessageBox::about(this, tr("About Application"),
            tr("The Application is used to compare between 2 text files, edit the text file, find or delete keywords, etc. \n"
               "Made by Kevin Hua, for practicing use."));
}

void MainWindow::instru(){
   QMessageBox::about(this, tr("About Application"),
            tr("1: Input file 1 that you should like to be shown on the app or type it out manually\n"
               "2: Under Interactions, you can use one of the 3 actions to manipulate your text or type in the keyword and click find to find the keywords. \n"
               "3: After you are finish, you can save it if it was an existing file or save as a new file or just create a new file and overwrite the existing text."));
}

void MainWindow::boldChange() {         //why does the first attempt fail then second attempt works?        Will figure it out later.
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QFile::ReadOnly | QFile::Text)) {
                QMessageBox::warning(this, tr("Application"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName), file.errorString()));
                return;
            }

            QTextStream in(&file);
            QStringList fileText2 = in.readAll().split("\n");


            /*while (!in.atEnd()) {
                QString line = in.readLine();
                textOfFile2.append(line);
            }

            QStringList fileText2 = textOfFile2;*/

            //in.readAll()

            QTextDocument *document = textEdit->document();
            if (document->isEmpty()) {
                QMessageBox::warning(this, tr("Application"), tr("No text in the TextBox."));
                return;
            }
            QStringList fileText1 = document->toPlainText().split("\n");

            //fileText1.append("\na");
            //fileText2.append("\na");

            int m = fileText1.size(); //x
            int n = fileText2.size(); //y

            QString output = "";
            QString text = "";

       //int opt[m+1][n+1];
            // Initializing a single row
            std::vector<int> row(n+1, 0);

            // Initializing the 2-D vector
            std::vector<std::vector<int>> opt(m+1, row) ;

            /*for (int i = m-1; i >= 0; i--) {
                for (int j = n-1; j >= 0; j--) {
                    opt[i][j] = 0;
                }
            }*/



            /*for (int index = 0; index < document->lineCount(); index++) {
                //QMessageBox::information(this, tr("Button Clicked"), tr(document->findBlockByLineNumber(index).text().toStdString().c_str()));
                //QMessageBox::information(this, tr("Button Clicked"), tr(fileText1[index].toStdString().c_str()));
            }*/

            //QMessageBox::information(this, tr("Button Clicked"), tr(std::to_string(document->lineCount()).c_str()));
            //QMessageBox::information(this, tr("Button Clicked"), tr(document->findBlockByLineNumber(5).text().toStdString().c_str()));



            /*for (int index = 0; index < document->size(); index++) {
                document->findBlock(index);
            }*/

            std::string t1 = std::to_string(m);
            std::string t2 = std::to_string(n);
            //QMessageBox::information(this, tr("Button Clicked"), tr(t1.c_str()));   //0
            //QMessageBox::information(this, tr("Button Clicked"), tr(t2.c_str()));


            for (int i = m-1; i >= 0; i--) {
                for (int j = n-1; j >= 0; j--) {
                    //QMessageBox::information(this, tr("Button Clicked"), tr("boldchange hit")); //not hit
                    if (fileText1[i] == fileText2[j]) {
                        opt[i][j] = opt[i+1][j+1] + 1;
                        //QMessageBox::information(this, tr("Button Clicked"), tr(std::to_string(i).c_str() + ", " + std::to_string(j).c_str() + ", " + std::to_string(opt[i][j]).c_str() + ", ")); //not hit
                    } else {
                        opt[i][j] = std::max(opt[i+1][j], opt[i][j+1]);
                       // QMessageBox::information(this, tr("Button Clicked"), tr(std::to_string(i).c_str() + ", " + std::to_string(j).c_str() + ", " + std::to_string(opt[i][j]).c_str() + ", ")); //not hit
                    }
                }
            }

            QString fileOneChanges;
            QString fileTwoChanges;

            textEdit -> clear();

            QString marking = lineEdit->text();
            QTextCursor cursor( textEdit->textCursor() );

            QTextCharFormat format;
            format.setFontWeight( QFont::DemiBold );
            format.setForeground( QBrush( QColor( "black" ) ) );
            cursor.setCharFormat( format );

            // recover LCS itself and print out non-matching lines to standard output
            int i = 0, j = 0;
            while (i < m && j < n) {
                if (fileText1[i].trimmed() == fileText2[j].trimmed()) {
                    if (!fileOneChanges.isEmpty()) {
                        fileOneChanges = (marking) + "\n" + fileOneChanges + (marking) + "\n";
                        format.setForeground( QBrush( QColor( "red" ) ) );
                        cursor.setCharFormat( format );
                        cursor.insertText(fileOneChanges);
                        output.append(fileOneChanges);
                    }
                    output += (fileText1[i]) + "\n";
                    format.setForeground( QBrush( QColor( "black" ) ) );
                    cursor.setCharFormat( format );
                    cursor.insertText((fileText1[i]) + "\n");
                    i++;
                    j++;

                    fileOneChanges = "";
                    fileTwoChanges = "";
                } else if (opt[i+1][j] > opt[i][j+1]) {        //} else if (opt[i+1][j] >= opt[i][j+1]) {
                    fileOneChanges.append(fileText1[i++].trimmed() + "\n");
                } else {
                    fileTwoChanges.append(fileText2[j++].trimmed() + "\n");
                }
            }

            // dump out one remainder of one string if the other is exhausted
            while (i < m || j < n) {
                if (i == m) {
                    fileTwoChanges.append(fileText2[j++].trimmed() + "\n");
                } else if (j == n) {
                    fileOneChanges.append(fileText1[i++].trimmed() + "\n");
                }
            }


            if (!fileOneChanges.isEmpty()) {
                fileOneChanges = (marking) + "\n" + fileOneChanges + (marking) + "\n";
                format.setForeground( QBrush( QColor( "red" ) ) );
                cursor.setCharFormat( format );
                cursor.insertText(fileOneChanges);
                output.append(fileOneChanges);
            }
            //setColor(7, output);      //7 is red, 2 is black
            //textEdit -> setText(output);
            //textEdit -> setStyleSheet("color: blue;");
        }
    }
}

void MainWindow::stripBold() {
    QTextDocument *document = textEdit->document();
    QString marking = lineEdit->text();
    if (document->isEmpty()) {
        QMessageBox::warning(this, tr("Application"), tr("No text in the TextBox."));
        return;
    }
    QStringList text = document->toPlainText().split("\n");
    QStringList newText;
    for (QString str : text) {
        if (!str.contains(marking)) {
            newText.append(str);
        }
    }
    QString output = newText.join("\n");
    textEdit->setPlainText(output);
}

void MainWindow::shorten() {//shorten the file by deleting multiple lines containing keywords and replace with ... deleted n lines...
    QTextDocument *document = textEdit->document();
    QString marking = lineEdit->text();
    if (document->isEmpty()) {
        QMessageBox::warning(this, tr("Application"), tr("No text in the TextBox."));
        return;
    }

    QString output;
    QStringList text = document->toPlainText().split("\n");

    QRegularExpression regex(R"(N(\d)+ [GXYZ]-*(\d)+)");   //("N(\\d)+ [GXYZ]-*(\\.)*(\\d)+");

    QStringList alist;
    QStringList allList;
    try {
        //String lineRead = reader.readLine(); // Reads the first line of data in the file
        for (QString lineRead : text) {
            //static QRegularExpressionMatch match = regex.match(lineRead);
            if (regex.match(lineRead).hasMatch()) {
                alist.append(lineRead);
            } else {
                if (alist.size() < 10) { // if the list is smaller than 10, no change
                    for (int k = 0; k < alist.size(); k++) {
                        allList.append(alist[k]);
                    }
                } else {
                    for (int f = 0; f < 3; f++) {
                        allList.append(alist[f]);
                    }
                    int newSize = alist.size() - 6;
                    allList.append("......");
                    allList.append("......(Chopped " + QString::number(newSize) + " lines)......");
                    allList.append("......");
                    for (int k = alist.size() - 3; k < alist.size(); k++) {
                        allList.append(alist[k]);
                    }
                }
                alist.clear();
                allList.append(lineRead);
            }
            //lineRead = reader.readLine();
        }
        output = allList.join("\n");
    } catch (...) {

    }
    textEdit->setPlainText(output);
}

void MainWindow::setColor(int col, const QString &s) {
    QTextCharFormat tf;
    tf = textEdit->currentCharFormat();
    tf.setForeground(QBrush((Qt::GlobalColor)col));
    textEdit->setCurrentCharFormat(tf);
    textEdit->append(s);



}

void MainWindow::documentWasModified() {
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::on_findButton_clicked() {

    //QMessageBox::information(this, tr("Button Clicked"), tr("on_findButton_clicked() hit"));

    QString searchString = lineEdit->text();
    QTextDocument *document = textEdit->document();

    bool found = false;

    // undo previous change (if any)
    document->undo();

    if (searchString.isEmpty()) {
        QMessageBox::information(this, tr("Empty Search Field"),
                                 tr("The search field is empty. "
                                    "Please enter a word and click Find."));
    } else {
        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat defaultFormat = QTextCharFormat();
        QTextCursor tc(document);
        tc.select(QTextCursor::Document);
        QTextCharFormat docFormat = tc.charFormat();
        docFormat.setBackground(defaultFormat.background());
        docFormat.setForeground(defaultFormat.foreground());
        tc.mergeCharFormat(docFormat);

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor,
                                             QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.movePosition(QTextCursor::WordRight,
                                             QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();

        if (found == false) {
            QMessageBox::information(this, tr("Word Not Found"),
                                     tr("Sorry, the word cannot be found."));
        }
    }
}

void MainWindow::createActions() {

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD



    QMenu *actionMenu = menuBar()->addMenu(tr("&Interactions"));
    QAction *boldAct = actionMenu->addAction(tr("&Bold Changes"), this, &MainWindow::boldChange);
    boldAct->setStatusTip(tr("Get 2nd File and compare then bold changes."));

    QAction *stripBold = actionMenu->addAction(tr("Strip &Markings"), this, &MainWindow::stripBold);
    stripBold->setStatusTip(tr("Strip the lines that contains the Markings."));

    QAction *shorten = actionMenu->addAction(tr("Sho&rten"), this, &MainWindow::shorten);
    shorten->setStatusTip(tr("Remove lines contain a certain keywords and replace with number of lines removed."));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutInstru = helpMenu->addAction(tr("&Instructions"), this, &MainWindow::instru);
    aboutInstru->setStatusTip(tr("Show the application's Instructions box"));


#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave() {
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);

    //QStringList textOfFile1;

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);

    /*QTextStream in2(&file);
    while (!in2.atEnd()) {
        QString line = in2.readLine();
        textOfFile1.append(line);
    }*/
}

bool MainWindow::saveFile(const QString &fileName) {
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName) {
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "BoldIT";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}


#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}
#endif
