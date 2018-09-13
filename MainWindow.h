#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>
#include <RemoteService.h>

//#include <JsonValidator.h>

class InputWidgetEE_UQ;
class SimCenterWidget;
class QPushButton;
class QLabel;



class MainWindow : public QMainWindow
{
  Q_OBJECT
    
    public:
  explicit MainWindow(RemoteService *theRemoteInterface, QWidget *parent = 0);
  ~MainWindow();

 signals:
    void attemptLogin(QString, QString);
    void logout();

  public slots:
    // for menu items
    void newFile();
    void open();
    void openFile(QString filename);
    bool save();
    bool saveAs();

    // for main actions
    void onRunButtonClicked();
    void onRemoteRunButtonClicked();
    void onRemoteGetButtonClicked();
    void onExitButtonClicked();

    // for login
    void onLoginButtonClicked();
    void onLoginSubmitButtonClicked();
    void attemptLoginReturn(bool);
    void logoutReturn(bool);

    // for error messages
    void statusMessage(QString message);
    void errorMessage(QString message);
    void fatalMessage(QString message);


 private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    void createActions();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    // some variables for logging in
    QWidget *loginWindow;
    QLineEdit *nameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginSubmitButton;
    bool loggedIn;
    int numTries;

    //Ui::MainWindow *ui;

    QString currentFile;
    InputWidgetEE_UQ *inputWidget;
    RemoteService *theRemoteInterface;
    //SimCenterWidget *currentWidget;

    QPushButton *loginButton;
    QLabel *errorLabel;
};

#endif // MAINWINDOW_H
