#include <ZipUtils.h>
#include <zip.h>
#include <unzip.h>
#include <QDirIterator>
#include <QDebug>

bool ZipUtils::ZipFolder(QDir directoryToZip, QString zipFilePath)
{
    zipFile newZipFile = zipOpen(zipFilePath.toStdString().c_str(), APPEND_STATUS_CREATE);

    //Checking if the file is opened
    if(NULL == newZipFile)
        return false;

    int result = ZIP_OK;

    QDirIterator dirIt(directoryToZip, QDirIterator::Subdirectories);

    //We need to find the parent to get relative path including directory name
    QDir parentDir(directoryToZip.absolutePath());
    parentDir.cdUp();

    //Looping through files and directories
    while (dirIt.hasNext())
    {
        QFileInfo fileInfo(dirIt.next());

        if(fileInfo.isDir())
            continue;

        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        QFile afile(fileInfo.filePath());


        if(afile.exists())
        {

            if(!afile.open(QFile::ReadOnly))
                return false;

            QByteArray fileByteArray = afile.readAll();

            QString relFilePath = parentDir.relativeFilePath(fileInfo.absoluteFilePath());

            result = zipOpenNewFileInZip(newZipFile,
                                         relFilePath.toStdString().c_str(),
                                         NULL,
                                         NULL,
                                         0,
                                         NULL,
                                         0,
                                         NULL,
                                         Z_DEFLATED,
                                         Z_BEST_COMPRESSION);

            //checking if file entry is opened
            if(result != ZIP_OK)
                return false;

            result = zipWriteInFileInZip(newZipFile, fileByteArray.data(), fileByteArray.length());

            //checking if writing was successful
           if(result != ZIP_OK)
                return false;

            //close file inside zip
            result = zipCloseFileInZip(newZipFile);

            //checking if file entry closing was successful
            if(result != ZIP_OK)
                 return false;
        }

    }



    //Closing zip file
    result = zipClose(newZipFile, "Compressed directory");

    return (result == ZIP_OK);
}

bool ZipUtils::UnzipFile(QString zipFilePath, QDir directoryToUnzip)
{
    //make sure the output directory exists
    if(true != directoryToUnzip.mkpath("."))
        return false;

    unzFile zipFile = unzOpen(zipFilePath.toStdString().c_str());

    //Checking if the file is opened
    if(nullptr == zipFile)
        return false;

    int result = UNZ_OK;

    if(unzGoToFirstFile(zipFile) != UNZ_OK)
    {
        unzClose(zipFile);
        return false;
    }

    bool lastFile = false;
    while(!lastFile)
    {
        unz_file_info fileInfo;

        //Read filename size

        if(UNZ_OK != unzGetCurrentFileInfo(zipFile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0))
        {
            unzClose(zipFile);
            return false;
        }

        //Read file name
        char* filenameBuffer = new char[fileInfo.size_filename + 1];
        unzGetCurrentFileInfo(zipFile, &fileInfo, filenameBuffer, fileInfo.size_filename + 1, nullptr, 0, nullptr, 0);
        QString filename(filenameBuffer);
        delete[] filenameBuffer;


        //Open the current compressed file
        if (UNZ_OK != unzOpenCurrentFile(zipFile))
        {
            unzClose(zipFile);
            return false;
        }

        //Create a buffer with size equale to the uncompresses file size
        int fileSize = static_cast<int>(fileInfo.uncompressed_size);
        char* fileBuffer = new char[fileInfo.uncompressed_size];

        //Read the file in a buffer of the same size
        if(fileSize != unzReadCurrentFile(zipFile, fileBuffer, fileInfo.uncompressed_size))
        {
            delete[] fileBuffer;
            unzCloseCurrentFile(zipFile);
            unzClose(zipFile);
            return false;
        }

        //Close the current compressed file
        if(UNZ_OK != unzCloseCurrentFile(zipFile))
        {
            delete[] fileBuffer;
            unzClose(zipFile);
            return false;
        }

        //This will always overwrite
        QFile extractedFile(directoryToUnzip.filePath(filename));
        if(extractedFile.open(QFile::WriteOnly))
        {
            extractedFile.write(fileBuffer, fileSize);
            extractedFile.close();
        }
        else
        {
            delete[] fileBuffer;
            unzClose(zipFile);
            return false;
        }

        delete[] fileBuffer;

        //Checking if we had already reached the last files
        if(result == UNZ_END_OF_LIST_OF_FILE)
        {
            lastFile = true;
        }
        else
            //moving to next file
            result = unzGoToNextFile(zipFile);
    }

    result = unzClose(zipFile);

    return (result == UNZ_OK);
}
