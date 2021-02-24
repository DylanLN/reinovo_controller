#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/reinovo_controller/main_window.hpp"
namespace reinovo_controller {

using namespace Qt;

using namespace std;

//连接按键
void MainWindow::SshClient()
{
    if (sshclient == NULL)
    {
        string url,masterurl = ui.master_url->text().toStdString();
        url = masterurl.substr(masterurl.find(":",0)+3,masterurl.find("11311",0)-1);
        masterurl = url.substr(0,url.find(":",0));
        sshclient = new ConnectSshClient(ui.users->text().toStdString(), masterurl, ui.passwd->text().toStdString(),"host");
    //    sshclient = new ConnectSshClient();
        if(sshclient->ConnectSsh()){
            //打开线程
            fsshclient =true;
            connect(this,SIGNAL(AppendText(QString)),this,SLOT(SshOutputAppendText(QString)));
            ssh_thread = boost::thread(boost::bind(&MainWindow::SshThread,this));
            cout << "Connect Ssh ok" << endl;
            ui.ssh_connect->setText("断开");
        }else{
            delete sshclient;
            sshclient = NULL;
            cout << "Connect Ssh error" << endl;
        }
    }else{
        //回收线程
        fsshclient = false;
        sshclient->Sshwrite("ls\n");
        ssh_thread.join();

        disconnect(this,SIGNAL(AppendText(QString)),this,SLOT(SshOutputAppendText(QString)));
        delete sshclient;
        sshclient = NULL;
        ui.ssh_output->appendPlainText("断开连接");
        ui.ssh_connect->setText("连接");
    }
}

//回车槽函数
void MainWindow::SshWrite()
{
    if(sshclient != NULL)
        sshclient->Sshwrite(ui.ssh_input->text().toStdString() + "\n");
    else {
        ui.ssh_output->appendPlainText("未连接");
    }
}

//
void MainWindow::SshThread()
{
    while (fsshclient ==true && sshclient != NULL)
    {
        usleep(300*1000);
        string str = sshclient->SshRead();
        if(str != "")
            emit AppendText(QString::fromStdString(str));
        cout << str << endl;
        // cout << "read" <<endl;
    }
}

void MainWindow::OpenMaster()
{
    if (frosmaster == false )
    {
        string url,masterurl = ui.master_url->text().toStdString();
        url = masterurl.substr(masterurl.find(":",0)+3,masterurl.size());
        masterurl = url.substr(0,url.find(":",0));
        rosmaster = new ConnectSshClient(ui.users->text().toStdString(), masterurl, ui.passwd->text().toStdString(),"host");
        if(rosmaster->ConnectSsh()){
            sleep(1);
            frosmaster =true;
            rosmaster->Sshwrite("roscore\n");
            cout << "Open ros master ok" << endl;
            ui.open_master->setText("关闭master");
        }else {
            delete rosmaster;
            rosmaster = NULL;
        }
    }else{
        frosmaster = false;
        delete rosmaster;
        rosmaster = NULL;
        ui.open_master->setText("开启master");
    }
}

//yong yu duo xian cheng xie ru
void MainWindow::SshOutputAppendText(const QString &text)
{
    ui.ssh_output->appendPlainText(text);
}

}
