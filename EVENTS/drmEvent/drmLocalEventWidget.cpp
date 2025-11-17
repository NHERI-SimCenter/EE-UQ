#include "drmLocalEventWidget.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <LineEditRV.h>
#include <RandomVariablesContainer.h>
#include <QSplitter>
#include <QTextEdit>

DRMLocalEventWidget::DRMLocalEventWidget(QWidget *parent) : SimCenterAppWidget(parent) {
    table = new QTableWidget(this);
    addButton = new QPushButton("Add Event", this);
    removeButton = new QPushButton("Remove Selected", this);
    browseFileButton = new QPushButton("Set HDF File Path for Selected Row...", this);

    setupTable();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(browseFileButton);
    buttonLayout->addStretch();

    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(table);
    leftLayout->addLayout(buttonLayout);
    leftWidget->setLayout(leftLayout);

    // Right side: parameter explanations
    QTextEdit *explanationText = new QTextEdit(this);
    explanationText->setReadOnly(true);
    explanationText->setHtml(R"(
    <h3>DRM Event Parameters</h3>
    <ul>
      <li><b>name</b>: Name of the DRM event row.</li>
      <li><b>filePath</b>: Path to the HDF5 file containing DRM data.</li>
      <li><b>factor</b>: Scale factor for the input motion (can be a number or a random variable).</li>
      <li><b>crd_scale</b>: Coordinate scaling factor.</li>
      <li><b>distance_tolerance</b>: Tolerance for matching coordinates.</li>
      <li><b>do_coordinate_transformation</b>: 1 to apply coordinate transformation, 0 otherwise.</li>
      <li><b>T00-T22</b>: Transformation matrix components (3x3, row-major).</li>
      <li><b>x00-x02</b>: Origin of the local coordinate system.</li>
      <li><b>dt</b>: Time step for the simulation. <i>If set to -1, it will be read/calculated from the DRM file.</i></li>
      <li><b>numStep</b>: Number of steps for the simulation. <i>If set to -1, it will be read/calculated from the DRM file.</i></li>
    </ul>
    <p>Only the <b>factor</b> column supports random variables. Enter a variable name to register it as a random variable.</p>
    <p>For more information, see:</p>
    <p><b><a href="https://opensees.github.io/OpenSeesDocumentation/user/manual/model/pattern/H5DRM.html">https://opensees.github.io/OpenSeesDocumentation/user/manual/model/pattern/H5DRM.html</a></b></p>
    )");

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(explanationText);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    connect(addButton, &QPushButton::clicked, this, &DRMLocalEventWidget::addEventRow);
    connect(removeButton, &QPushButton::clicked, this, &DRMLocalEventWidget::removeSelectedRows);
    connect(browseFileButton, &QPushButton::clicked, this, [this]() {
        auto selected = table->selectionModel()->selectedRows();
        if (selected.count() == 1) {
            int row = selected.first().row();
            QString fileName = QFileDialog::getOpenFileName(this, "Select HDF File for DRM Event", QString(), "HDF Files (*.hdf *.h5);;All Files (*)");
            if (!fileName.isEmpty()) {
                table->setItem(row, 1, new QTableWidgetItem(fileName));
            }
        }
    });
}

void DRMLocalEventWidget::setupTable() {
    QStringList headers = {"name", "filePath", "factor", "crd_scale", "distance_tolerance", "do_coordinate_transformation",
                          "T00", "T01", "T02", "T10", "T11", "T12", "T20", "T21", "T22",
                          "x00", "x01", "x02", "dt", "numSteps"}; // Added dt and numStep
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    // Set custom column widths
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->setColumnWidth(0, 80);   // name
    table->setColumnWidth(1, 260);  // filePath (wider for long paths)
    table->setColumnWidth(2, 60);   // factor
    table->setColumnWidth(3, 70);   // crd_scale
    table->setColumnWidth(4, 140);  // distance_tolerance (wider)
    table->setColumnWidth(5, 220);  // do_coordinate_transformation (even wider)
    for (int i = 6; i <= 14; ++i)   // T00-T22
        table->setColumnWidth(i, 40);
    for (int i = 15; i <= 17; ++i)  // x00-x02
        table->setColumnWidth(i, 50);
    table->setColumnWidth(18, 40);  // dt
    table->setColumnWidth(19, 60);  // numStep
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DRMLocalEventWidget::addEventRow() {
    int row = table->rowCount();
    table->insertRow(row);
    setDefaultRow(row);
}

void DRMLocalEventWidget::setDefaultRow(int row) {
    table->setItem(row, 0, new QTableWidgetItem("")); // name
    table->setItem(row, 1, new QTableWidgetItem("")); // filePath

    // Use LineEditRV for factor
    auto factorEdit = new LineEditRV(RandomVariablesContainer::getInstance());
    factorEdit->setText("1.0");
    table->setCellWidget(row, 2, factorEdit); // factor

    table->setItem(row, 3, new QTableWidgetItem("1.0")); // crd_scale
    table->setItem(row, 4, new QTableWidgetItem("0.001")); // distance_tolerance
    table->setItem(row, 5, new QTableWidgetItem("1")); // do_coordinate_transformation
    table->setItem(row, 6, new QTableWidgetItem("1.0")); // T00
    table->setItem(row, 7, new QTableWidgetItem("0.0")); // T01
    table->setItem(row, 8, new QTableWidgetItem("0.0")); // T02
    table->setItem(row, 9, new QTableWidgetItem("0.0")); // T10
    table->setItem(row, 10, new QTableWidgetItem("1.0")); // T11
    table->setItem(row, 11, new QTableWidgetItem("0.0")); // T12
    table->setItem(row, 12, new QTableWidgetItem("0.0")); // T20
    table->setItem(row, 13, new QTableWidgetItem("0.0")); // T21
    table->setItem(row, 14, new QTableWidgetItem("1.0")); // T22
    table->setItem(row, 15, new QTableWidgetItem("0.0")); // x00
    table->setItem(row, 16, new QTableWidgetItem("0.0")); // x01
    table->setItem(row, 17, new QTableWidgetItem("0.0")); // x02
    table->setItem(row, 18, new QTableWidgetItem("-1")); // dt default -1
    table->setItem(row, 19, new QTableWidgetItem("-1")); // numStep default -1
}

void DRMLocalEventWidget::removeSelectedRows() {
    auto selected = table->selectionModel()->selectedRows();
    for (int i = selected.count() - 1; i >= 0; --i) {
        table->removeRow(selected.at(i).row());
    }
}

QJsonArray DRMLocalEventWidget::getEventsAsJson() const {
    QJsonArray array;
    for (int row = 0; row < table->rowCount(); ++row) {
        QJsonObject obj;
        obj["name"] = table->item(row, 0) ? table->item(row, 0)->text() : "";
        obj["filePath"] = table->item(row, 1) ? table->item(row, 1)->text() : "";
        // Use LineEditRV for factor
        LineEditRV* factorEdit = qobject_cast<LineEditRV*>(table->cellWidget(row, 2));
        if (factorEdit) {
            factorEdit->outputToJSON(obj, "factor");
        } else {
            obj["factor"] = 1.0;
        }
        obj["crd_scale"] = table->item(row, 3) ? table->item(row, 3)->text().toDouble() : 1.0;
        obj["distance_tolerance"] = table->item(row, 4) ? table->item(row, 4)->text().toDouble() : 0.001;
        obj["do_coordinate_transformation"] = table->item(row, 5) ? table->item(row, 5)->text().toInt() : 1;
        obj["T00"] = table->item(row, 6) ? table->item(row, 6)->text().toDouble() : 1.0;
        obj["T01"] = table->item(row, 7) ? table->item(row, 7)->text().toDouble() : 0.0;
        obj["T02"] = table->item(row, 8) ? table->item(row, 8)->text().toDouble() : 0.0;
        obj["T10"] = table->item(row, 9) ? table->item(row, 9)->text().toDouble() : 0.0;
        obj["T11"] = table->item(row, 10) ? table->item(row, 10)->text().toDouble() : 1.0;
        obj["T12"] = table->item(row, 11) ? table->item(row, 11)->text().toDouble() : 0.0;
        obj["T20"] = table->item(row, 12) ? table->item(row, 12)->text().toDouble() : 0.0;
        obj["T21"] = table->item(row, 13) ? table->item(row, 13)->text().toDouble() : 0.0;
        obj["T22"] = table->item(row, 14) ? table->item(row, 14)->text().toDouble() : 1.0;
        obj["x00"] = table->item(row, 15) ? table->item(row, 15)->text().toDouble() : 0.0;
        obj["x01"] = table->item(row, 16) ? table->item(row, 16)->text().toDouble() : 0.0;
        obj["x02"] = table->item(row, 17) ? table->item(row, 17)->text().toDouble() : 0.0;
        obj["dT"] = table->item(row, 18) ? table->item(row, 18)->text().toDouble() : -1.0;
        obj["numSteps"] = table->item(row, 19) ? table->item(row, 19)->text().toInt() : -1;
        obj["index"] = row;
        array.append(obj);
    }
    return array;
}

bool DRMLocalEventWidget::outputToJSON(QJsonObject &jsonObject) {
    jsonObject["EventClassification"]="Earthquake";
    jsonObject["system"]="local";
    QJsonArray events = getEventsAsJson();
    jsonObject["Events"] = events;
    jsonObject["type"] = "DRM";
    return true;
}

bool DRMLocalEventWidget::inputFromJSON(QJsonObject &jsonObject) {
    table->setRowCount(0);
    if (!jsonObject.contains("Events") || !jsonObject["Events"].isArray())
        return false;
    QJsonArray events = jsonObject["Events"].toArray();
    for (const QJsonValue &val : events) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(obj.value("name").toString()));
        table->setItem(row, 1, new QTableWidgetItem(obj.value("filePath").toString()));
        // Use LineEditRV for factor
        auto factorEdit = new LineEditRV(RandomVariablesContainer::getInstance());
        factorEdit->inputFromJSON(obj, "factor");
        table->setCellWidget(row, 2, factorEdit);
        table->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("crd_scale").toDouble(1.0))));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(obj.value("distance_tolerance").toDouble(0.001))));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(obj.value("do_coordinate_transformation").toInt(1))));
        table->setItem(row, 6, new QTableWidgetItem(QString::number(obj.value("T00").toDouble(1.0))));
        table->setItem(row, 7, new QTableWidgetItem(QString::number(obj.value("T01").toDouble(0.0))));
        table->setItem(row, 8, new QTableWidgetItem(QString::number(obj.value("T02").toDouble(0.0))));
        table->setItem(row, 9, new QTableWidgetItem(QString::number(obj.value("T10").toDouble(0.0))));
        table->setItem(row, 10, new QTableWidgetItem(QString::number(obj.value("T11").toDouble(1.0))));
        table->setItem(row, 11, new QTableWidgetItem(QString::number(obj.value("T12").toDouble(0.0))));
        table->setItem(row, 12, new QTableWidgetItem(QString::number(obj.value("T20").toDouble(0.0))));
        table->setItem(row, 13, new QTableWidgetItem(QString::number(obj.value("T21").toDouble(0.0))));
        table->setItem(row, 14, new QTableWidgetItem(QString::number(obj.value("T22").toDouble(1.0))));
        table->setItem(row, 15, new QTableWidgetItem(QString::number(obj.value("x00").toDouble(0.0))));
        table->setItem(row, 16, new QTableWidgetItem(QString::number(obj.value("x01").toDouble(0.0))));
        table->setItem(row, 17, new QTableWidgetItem(QString::number(obj.value("x02").toDouble(0.0))));
        table->setItem(row, 18, new QTableWidgetItem(QString::number(obj.value("dT").toDouble(-1.0))));
        table->setItem(row, 19, new QTableWidgetItem(QString::number(obj.value("numSteps").toInt(-1))));
    }
    return true;
}

bool DRMLocalEventWidget::outputAppDataToJSON(QJsonObject &jsonObject) {
    jsonObject["Application"] = "DRM";
    jsonObject["ApplicationData"] = QJsonObject();
    jsonObject["EventClassification"] = "Earthquake";
    return true;
}

bool DRMLocalEventWidget::inputAppDataFromJSON(QJsonObject &jsonObject) {
    // if (!jsonObject.contains("Application") || !jsonObject["Application"].isString() ||
    //     jsonObject["Application"].toString() != "DRM") {
    //     return false;
    // }
    // if (!jsonObject.contains("EventClassification") || jsonObject["EventClassification"].toString() != "Earthquake") {
    //     return false;
    // }
    return true;
}

bool DRMLocalEventWidget::copyFiles(QString &destDir) {
    // Implement as needed
    return true;
}

void DRMLocalEventWidget::clear() {
    table->setRowCount(0);
}
