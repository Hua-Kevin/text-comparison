
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QSessionManager;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    /*MainWindow() {
        QPushButton *pButton = new QPushButton("this");
        QObject::connect(pButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    }*/

    void loadFile(const QString &fileName);

    void setColor(int, const QString &s);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void instru();
    void boldChange();
    void stripBold();
    void shorten();
    void documentWasModified();
    void on_findButton_clicked();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QTextEdit *textEdit;
    QPushButton *findButton;
    QLineEdit *lineEdit;
    QString curFile;
    QStringList textOfFile1;
    QStringList textOfFile2;
};

#endif
