#include "drmPredefinedEventWidget.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <LineEditRV.h>
#include <RandomVariablesContainer.h>
#include <QSplitter>
#include <QTextEdit>
#include <QComboBox>

DRMPredefinedEventWidget::DRMPredefinedEventWidget(QWidget *parent) : SimCenterAppWidget(parent) {
    table = new QTableWidget(this);
    addButton = new QPushButton("Add Event", this);
    removeButton = new QPushButton("Remove Selected", this);

    setupTable();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();

    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(table);
    leftLayout->addLayout(buttonLayout);
    leftWidget->setLayout(leftLayout);

    // Right side: information viewer for selected DRM
    infoViewer = new QTextEdit(this);
    infoViewer->setReadOnly(true);
    infoViewer->setHtml(R"(
    <h3>Predefined DesignSafe DRM Events</h3>
    <p>Select a DRM event from the dropdown to view its details.</p>
    <p>Available parameters to modify:</p>
    <ul>
      <li><b>x00, x01, x02</b>: Origin of the local coordinate system</li>
      <li><b>dt</b>: Time step for the simulation</li>
      <li><b>numSteps</b>: Number of steps for the simulation</li>
    </ul>
    <p>All other parameters (filepath, crd_scale, distance_tolerance, T00-T09) are predefined for each DRM load.</p>
    )");

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(infoViewer);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    connect(addButton, &QPushButton::clicked, this, &DRMPredefinedEventWidget::addEventRow);
    connect(removeButton, &QPushButton::clicked, this, &DRMPredefinedEventWidget::removeSelectedRows);
    
    // Connect table selection change to update info viewer
    connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex &current, const QModelIndex &) {
                if (current.isValid()) {
                    QComboBox *drmCombo = qobject_cast<QComboBox*>(table->cellWidget(current.row(), 0));
                    if (drmCombo) {
                        updateInfoViewer(drmCombo->currentText());
                    }
                }
            });
}

void DRMPredefinedEventWidget::setupTable() {
    QStringList headers = {"name", "x00", "x01", "x02", "dt", "numSteps"};
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    
    // Set custom column widths
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->setColumnWidth(0, 300);  // name (wider for dropdown)
    table->setColumnWidth(1, 90);   // x00
    table->setColumnWidth(2, 90);   // x01
    table->setColumnWidth(3, 90);   // x02
    table->setColumnWidth(4, 90);   // dt
    table->setColumnWidth(5, 90);   // numSteps
    
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DRMPredefinedEventWidget::addEventRow() {
    int row = table->rowCount();
    table->insertRow(row);
    setDefaultRow(row);
}

void DRMPredefinedEventWidget::setDefaultRow(int row) {
    // Create ComboBox for DRM selection
    QComboBox *drmCombo = new QComboBox();
    QStringList drmFiles = getAvailableDRMFiles();
    drmCombo->addItems(drmFiles);
    table->setCellWidget(row, 0, drmCombo);
    
    // Connect to update info viewer when selection changes
    connect(drmCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &DRMPredefinedEventWidget::onDRMSelectionChanged);
    
    // Set default values for editable parameters
    table->setItem(row, 1, new QTableWidgetItem("0.0")); // x00
    table->setItem(row, 2, new QTableWidgetItem("0.0")); // x01
    table->setItem(row, 3, new QTableWidgetItem("0.0")); // x02
    table->setItem(row, 4, new QTableWidgetItem("-1"));  // dt (default -1)
    table->setItem(row, 5, new QTableWidgetItem("-1"));  // numSteps (default -1)
    
    // Update info viewer if this is the first item and select the row
    if (row == 0 && !drmFiles.isEmpty()) {
        updateInfoViewer(drmFiles.first());
        table->selectRow(0); // Select the first row automatically
    }
}

void DRMPredefinedEventWidget::removeSelectedRows() {
    auto selected = table->selectionModel()->selectedRows();
    for (int i = selected.count() - 1; i >= 0; --i) {
        table->removeRow(selected.at(i).row());
    }
}

void DRMPredefinedEventWidget::onDRMSelectionChanged() {
    QComboBox *combo = qobject_cast<QComboBox*>(sender());
    if (combo) {
        // Find which row this combo box belongs to
        for (int row = 0; row < table->rowCount(); ++row) {
            if (table->cellWidget(row, 0) == combo) {
                // Check if this row is currently selected
                QModelIndexList selected = table->selectionModel()->selectedRows();
                if (!selected.isEmpty() && selected.first().row() == row) {
                    updateInfoViewer(combo->currentText());
                }
                break;
            }
        }
    }
}

QStringList DRMPredefinedEventWidget::getAvailableDRMFiles() const {
    // TODO: This should be populated with actual available DRM files from DesignSafe
    // For now, return some example DRM files
    return QStringList{
        "HaywardFaultSW4_Site1",
        "HaywardFaultSW4_Site2",
        "HaywardFaultSW4_Site3",
    };
}

QJsonObject DRMPredefinedEventWidget::getDRMFileInfo(const QString &drmName) const {
    // TODO: This should fetch actual information from DesignSafe API
    // For now, return example data for each DRM file
    QJsonObject info;
    
    if (drmName == "HaywardFaultSW4_Site1") {
        info["description"] = "Hayward Fault SW4 Site 1";
        info["magnitude"] = 6.5;
        info["distance"] = "1 km";
        info["filePath"] = "tapis://designsafe.storage.community/SimCenter/Datasets/SimulationAndExperimentalDatasets/DRM_records_for_opensees/HaywardFault_SW4_DRM_Site1.h5drm";
        info["crd_scale"] = 1.0;
        info["distance_tolerance"] = 0.001;
        info["T00"] = 1.0; info["T01"] = 0.0; info["T02"] = 0.0;
        info["T10"] = 0.0; info["T11"] = 1.0; info["T12"] = 0.0;
        info["T20"] = 0.0; info["T21"] = 0.0; info["T22"] = 1.0;
        info["duration"] = 27.5;
        info["timeStep"] = 0.0107783;
        info["numPoints"] = 2551;
    } else if (drmName == "HaywardFaultSW4_Site2") {
        info["description"] = "Hayward Fault SW4 Site 2";
        info["magnitude"] = 6.5;
        info["distance"] = "18 km";
        info["filePath"] = "tapis://designsafe.storage.community/SimCenter/Datasets/SimulationAndExperimentalDatasets/DRM_records_for_opensees/HaywardFault_SW4_DRM_Site2.h5drm";
        info["crd_scale"] = 1.0;
        info["distance_tolerance"] = 0.001;
        info["T00"] = 1.0; info["T01"] = 0.0; info["T02"] = 0.0;
        info["T10"] = 0.0; info["T11"] = 1.0; info["T12"] = 0.0;
        info["T20"] = 0.0; info["T21"] = 0.0; info["T22"] = 1.0;
        info["duration"] = 27.5;
        info["timeStep"] = 0.0107783;
        info["numPoints"] = 2551;
    } else if (drmName == "HaywardFaultSW4_Site3") {
        info["description"] = "Hayward Fault SW4 Site 3";
        info["magnitude"] = 6.5;
        info["distance"] = "1 km";
        info["filePath"] = "tapis://designsafe.storage.community/SimCenter/Datasets/SimulationAndExperimentalDatasets/DRM_records_for_opensees/HaywardFault_SW4_DRM_Site3.h5drm";
        info["crd_scale"] = 1.0;
        info["distance_tolerance"] = 0.001;
        info["T00"] = 1.0; info["T01"] = 0.0; info["T02"] = 0.0;
        info["T10"] = 0.0; info["T11"] = 1.0; info["T12"] = 0.0;
        info["T20"] = 0.0; info["T21"] = 0.0; info["T22"] = 1.0;
        info["duration"] = 27.5;
        info["timeStep"] = 0.0107783;
        info["numPoints"] = 2551;
    }
    return info;
}

void DRMPredefinedEventWidget::updateInfoViewer(const QString &drmName) {
    if (drmName.isEmpty()) {
        infoViewer->setHtml(R"(
        <h3>Predefined DesignSafe DRM Events</h3>
        <p>Select a DRM event from the dropdown to view its details.</p>
        )");
        return;
    }
    
    QJsonObject info = getDRMFileInfo(drmName);
    
    QString html = QString(R"(
    <h3>%1</h3>
    <p><b>Description:</b> %2</p>
    <p><b>Magnitude:</b> %3</p>
    <p><b>Distance:</b> %4</p>
    <p><b>Duration:</b> %5 seconds</p>
    <p><b>Time Step:</b> %6 seconds</p>
    <p><b>Number of Points:</b> %7</p>
    <hr>
    <h4>Predefined Parameters:</h4>
    <ul>
      <li><b>File Path:</b> %8</li>
      <li><b>Coordinate Scale:</b> %9</li>
      <li><b>Distance Tolerance:</b> %10</li>
      <li><b>Transformation Matrix:</b><br>
          [%11, %12, %13]<br>
          [%14, %15, %16]<br>
          [%17, %18, %19]</li>
    </ul>
    <p><i>You can modify x00, x01, x02, dt, and numSteps in the table.</i></p>
    )")
    .arg(drmName)
    .arg(info["description"].toString())
    .arg(info["magnitude"].toDouble())
    .arg(info["distance"].toString())
    .arg(info["duration"].toDouble())
    .arg(info["timeStep"].toDouble())
    .arg(info["numPoints"].toInt())
    .arg(info["filePath"].toString())
    .arg(info["crd_scale"].toDouble())
    .arg(info["distance_tolerance"].toDouble())
    .arg(info["T00"].toDouble()).arg(info["T01"].toDouble()).arg(info["T02"].toDouble())
    .arg(info["T10"].toDouble()).arg(info["T11"].toDouble()).arg(info["T12"].toDouble())
    .arg(info["T20"].toDouble()).arg(info["T21"].toDouble()).arg(info["T22"].toDouble());
    
    infoViewer->setHtml(html);
}

QJsonArray DRMPredefinedEventWidget::getEventsAsJson() const {
    QJsonArray array;
    for (int row = 0; row < table->rowCount(); ++row) {
        QJsonObject obj;
        
        // Get DRM name from combo box
        QComboBox *drmCombo = qobject_cast<QComboBox*>(table->cellWidget(row, 0));
        QString drmName = drmCombo ? drmCombo->currentText() : "";
        obj["name"] = drmName;
        
        // Get predefined parameters for this DRM
        QJsonObject drmInfo = getDRMFileInfo(drmName);
        obj["filePath"] = drmInfo["filePath"].toString();
        obj["crd_scale"] = drmInfo["crd_scale"].toDouble();
        obj["distance_tolerance"] = drmInfo["distance_tolerance"].toDouble();
        obj["do_coordinate_transformation"] = 1;
        obj["T00"] = drmInfo["T00"].toDouble();
        obj["T01"] = drmInfo["T01"].toDouble();
        obj["T02"] = drmInfo["T02"].toDouble();
        obj["T10"] = drmInfo["T10"].toDouble();
        obj["T11"] = drmInfo["T11"].toDouble();
        obj["T12"] = drmInfo["T12"].toDouble();
        obj["T20"] = drmInfo["T20"].toDouble();
        obj["T21"] = drmInfo["T21"].toDouble();
        obj["T22"] = drmInfo["T22"].toDouble();
        
        // Get user-configurable parameters with automatic calculation for -1 values
        double x00 = table->item(row, 1) ? table->item(row, 1)->text().toDouble() : 0.0;
        double x01 = table->item(row, 2) ? table->item(row, 2)->text().toDouble() : 0.0;
        double x02 = table->item(row, 3) ? table->item(row, 3)->text().toDouble() : 0.0;
        double dt = table->item(row, 4) ? table->item(row, 4)->text().toDouble() : -1.0;
        int numSteps = table->item(row, 5) ? table->item(row, 5)->text().toInt() : -1;
        
        // Auto-calculate dt and numSteps if they are -1
        if (dt < 0.0) {
            dt = drmInfo["timeStep"].toDouble();
        }
        if (numSteps == -1) {
            double duration = drmInfo["duration"].toDouble();
            numSteps = static_cast<int>(duration / dt);
        }
        
        obj["x00"] = x00;
        obj["x01"] = x01;
        obj["x02"] = x02;
        obj["dT"] = dt;
        obj["numSteps"] = numSteps;
        obj["index"] = row;
        
        array.append(obj);
    }
    return array;
}

bool DRMPredefinedEventWidget::outputToJSON(QJsonObject &jsonObject) {
    jsonObject["EventClassification"] = "Earthquake";
    jsonObject["system"] = "predefined-designsafe";
    QJsonArray events = getEventsAsJson();
    jsonObject["Events"] = events;
    jsonObject["type"] = "DRM";
    return true;
}

bool DRMPredefinedEventWidget::inputFromJSON(QJsonObject &jsonObject) {
    table->setRowCount(0);
    if (!jsonObject.contains("Events") || !jsonObject["Events"].isArray())
        return false;
    
    QJsonArray events = jsonObject["Events"].toArray();
    for (const QJsonValue &val : events) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        int row = table->rowCount();
        table->insertRow(row);
        
        // Set DRM name in combo box
        QComboBox *drmCombo = new QComboBox();
        QStringList drmFiles = getAvailableDRMFiles();
        drmCombo->addItems(drmFiles);
        QString drmName = obj.value("name").toString();
        int index = drmFiles.indexOf(drmName);
        if (index >= 0) {
            drmCombo->setCurrentIndex(index);
        }
        table->setCellWidget(row, 0, drmCombo);
        
        connect(drmCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
                this, &DRMPredefinedEventWidget::onDRMSelectionChanged);
        
        // Set user-configurable parameters
        table->setItem(row, 1, new QTableWidgetItem(QString::number(obj.value("x00").toDouble(0.0))));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("x01").toDouble(0.0))));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("x02").toDouble(0.0))));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(obj.value("dT").toDouble(-1.0))));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(obj.value("numSteps").toInt(-1))));
    }
    return true;
}

bool DRMPredefinedEventWidget::outputAppDataToJSON(QJsonObject &jsonObject) {
    jsonObject["Application"] = "DRM";
    jsonObject["ApplicationData"] = QJsonObject();
    jsonObject["EventClassification"] = "Earthquake";
    return true;
}

bool DRMPredefinedEventWidget::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

bool DRMPredefinedEventWidget::copyFiles(QString &destDir) {
    // For predefined events, files are already on DesignSafe, no need to copy
    return true;
}

void DRMPredefinedEventWidget::clear() {
    table->setRowCount(0);
}
