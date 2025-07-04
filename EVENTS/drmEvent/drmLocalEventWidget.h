#ifndef DRMLocaleventWidget_H
#define DRMLocaleventWidget_H

#include <SimCenterAppWidget.h>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>

class DRMLocalEventWidget : public SimCenterAppWidget {
    Q_OBJECT
public:
    explicit DRMLocalEventWidget(QWidget *parent = nullptr);
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

private:
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *browseFileButton;
    void setupTable();
    void setDefaultRow(int row);
};

#endif // DRMLocaleventWidget_H
