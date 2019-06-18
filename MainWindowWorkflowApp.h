#ifndef MAIN_WINDOW_WORKFLOW_APP_H
#define MAIN_WINDOW_WORKFLOW_APP_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>
#include <RemoteService.h>

//#include <JsonValidator.h>

class WorkflowAppWidget;
class SimCenterWidget;
class QPushButton;
class QLabel;
class SimCenterPreferences;


class MainWindowWorkflowApp : public QMainWindow
{
  Q_OBJECT
    
public:
    explicit MainWindowWorkflowApp(QString appName, WorkflowAppWidget *theApp, RemoteService *theRemoteInterface, QWidget *parent = 0);
    ~MainWindowWorkflowApp();
    void setCopyright(QString &copyright);
    void setVersion(QString &version);
    void setDocumentationURL(QString &version);
    void setAbout(QString &about);
    void setFeedbackURL(QString &feedback);
    void setFeatureURL(QString &featureRequest);
    void setCite(QString &cite);

 protected:

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

    void about();
    void manual();
    void submitFeedback();
    void submitFeatureRequest();
    void version();
    void preferences();
    void copyright();
    void cite();

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

    QString currentFile;
    WorkflowAppWidget *inputWidget;
    RemoteService *theRemoteInterface;
    //SimCenterWidget *currentWidget;

    QPushButton *loginButton;
    QLabel *errorLabel;

    QString versionText;
    QString aboutText;
    QString copyrightText;
    QString manualURL;
    QString feedbackURL;
    QString featureRequestURL;
    QString citeText;
    bool isAutoLogin;

    SimCenterPreferences *thePreferences;
};

#endif // MAIN_WINDOW_WORKFLOW_APP_H
