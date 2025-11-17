#ifndef DRMPREDEFINEDEVENTWIDGET_H
#define DRMPREDEFINEDEVENTWIDGET_H

#include <SimCenterAppWidget.h>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextEdit>
#include <QComboBox>

class DRMPredefinedEventWidget : public SimCenterAppWidget {
    Q_OBJECT
public:
    explicit DRMPredefinedEventWidget(QWidget *parent = nullptr);
    QJsonArray getEventsAsJson() const;
    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;
    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &destDir) override;
    void clear() override;

private slots:
    void addEventRow();
    void removeSelectedRows();
    void onDRMSelectionChanged();

private:
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *removeButton;
    QTextEdit *infoViewer;
    
    void setupTable();
    void setDefaultRow(int row);
    void updateInfoViewer(const QString &drmName);
    QStringList getAvailableDRMFiles() const;
    QJsonObject getDRMFileInfo(const QString &drmName) const;
};

#endif // DRMPREDEFINEDEVENTWIDGET_H
