#ifndef ZIPUTILS_H
#define ZIPUTILS_H
#include <QDir>
#include <QString>

namespace ZipUtils {

bool ZipFolder(QDir directoryToZip, QString zipFilePath);

bool UnzipFile(QString zipFilePath, QDir directoryToUnzip);

}
#endif // ZIPUTILS_H
