#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>
#include <QJsonObject>
#include <JsonValidator.h>

class InputWidgetEE_UQ;
class SimCenterWidget;
class QPushButton;
class QLabel;


class MainWindow : public QMainWindow
{
  Q_OBJECT
    
    public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  public slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();


  //void selectionChangedSlot(const QItemSelection &, const QItemSelection &);
    void connectMenuItems(SimCenterWidget  *widget);
    void disconnectMenuItems(SimCenterWidget  *widget);

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


    //Ui::MainWindow *ui;

    QString currentFile;
    InputWidgetEE_UQ *inputWidget;
    SimCenterWidget *currentWidget;

    QPushButton *loginButton;
    QLabel *errorLabel;

};

#endif // MAINWINDOW_H
