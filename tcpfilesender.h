#ifndef TCPFILESENDER_H
#define TCPFILESENDER_H

#include <QDialog>
#include <QtNetwork>
#include <QtWidgets>


class TCPFileSender : public QDialog
{
    Q_OBJECT
private:
    QProgressBar *clientProgressBar;
    QLabel *clientStatusLabel;
    QPushButton *startButtom;
    QPushButton *quitButtom;
    QPushButton *openButtom;
    QDialogButtonBox *buttomBox;
    QTcpSocket tcpClient;

    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 loadSize;
    QString fileName;
    QFile *localFile;
    QByteArray outBlock;
public:
    explicit TCPFileSender(QWidget *parent = nullptr);
    ~TCPFileSender() override;
public slots:
    void start();
    void startTransfer();
    void updateClientProgress(qint64 numBytes);
    void openFile();
};
#endif // TCPFILESENDER_H


