#include "tcpfilesender.h"

TCPFileSender::TCPFileSender(QWidget *parent)
    : QDialog(parent)
{
    loadSize = 1024 * 4;
    totalBytes=0;
    bytesWritten = 0;
    bytesToWrite = 0;
    clientProgressBar = new QProgressBar;
    clientStatusLabel = new QLabel(tr("客戶端就緒"));
    startButtom = new QPushButton(tr("開始"));
    quitButtom = new QPushButton(tr("關閉"));
    openButtom = new QPushButton(tr("開檔"));
    startButtom ->setEnabled(false);
    buttomBox = new QDialogButtonBox;
    buttomBox ->addButton(startButtom,QDialogButtonBox::ActionRole);
    buttomBox ->addButton(quitButtom,QDialogButtonBox::RejectRole);
    buttomBox ->addButton(openButtom,QDialogButtonBox::ActionRole);


    QVBoxLayout *mainLayout =new QVBoxLayout;
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttomBox);
    setLayout(mainLayout);
    setWindowTitle(tr("檔案傳送"));

    connect(startButtom,SIGNAL(clicked()),this,SLOT(start()));
    connect(openButtom,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(quitButtom,SIGNAL(clicked()),this,SLOT(close()));
    connect(&tcpClient,SIGNAL(connected()),this,SLOT(startTransfer()));
    connect(&tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
}

TCPFileSender::~TCPFileSender() = default;

void TCPFileSender::start(){
    startButtom->setEnabled(false);
    bytesWritten=0;
    clientStatusLabel->setText(tr("連接中..."));
    tcpClient.connectToHost(QHostAddress::LocalHost,16998);
}
void TCPFileSender::startTransfer(){
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly)){
        QMessageBox::warning(this,tr("應用程式"),tr("無法讀取 %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }
    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf("/")-1);
    sendOut <<qint64(0)<<qint64(0)<<currentFile;
    totalBytes = totalBytes + outBlock.size();

    sendOut.device()->seek(0);
    sendOut << totalBytes << qint64(outBlock.size() - sizeof(qint64) * 2);
    bytesToWrite = totalBytes - tcpClient.write(outBlock);
    clientStatusLabel->setText(tr("已連接"));
    qDebug() << currentFile << totalBytes;
    outBlock.resize(0);

}
void TCPFileSender::updateClientProgress(qint64 numBytes){
    bytesWritten = bytesWritten + (int)numBytes;

    if (bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));
        bytesToWrite -= (int) tcpClient.write(outBlock);
        outBlock.resize(0);
    }
    else
    {
        localFile->close();
    }

    clientProgressBar->setMaximum(totalBytes);
    clientProgressBar->setValue(bytesWritten);
    clientStatusLabel->setText(tr("已傳送 %1 Bytes").arg(bytesWritten));

}
void TCPFileSender::openFile(){
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())startButtom->setEnabled(true);
}