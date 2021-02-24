/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/reinovo_controller/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace reinovo_controller {

using namespace Qt;

using namespace std;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
    : QMainWindow(parent),
		init_argc(argc),
        init_argv(argv)
{
	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application

	setWindowIcon(QIcon(":/images/reinovo.png"));
    ui_init();
    ConnectAll();
}

MainWindow::~MainWindow() {
    if (teleop != NULL )
    {
        delete teleop;
        teleop = NULL;
    }
    if (sshclient != NULL )
    {
        if (fsshclient)
        {
            fsshclient = false;
            sshclient->Sshwrite("ls\n");
            ssh_thread.join();
        }   
        ssh_thread.interrupt();
        ssh_thread.join();
        delete sshclient;
        teleop = NULL;
    }
    if(rosmaster != NULL){
        delete rosmaster;
        rosmaster = NULL;
    }
}

/*********************************************************************************
                以下为pu tong函数
*********************************************************************************/

bool MainWindow::ConnectAll(){
	//速度
	connect(ui.long_2,SIGNAL(clicked()),this,SLOT(speed_init()));
    connect(ui.jog ,SIGNAL(clicked()),this,SLOT(speed_init()));
    //
    connect(ui.ssh_connect ,SIGNAL(clicked()),this,SLOT(SshClient()));
    connect(ui.ssh_enter ,SIGNAL(clicked()),this,SLOT(SshWrite()));
    //open master
    connect(ui.open_master,SIGNAL(clicked()),this,SLOT(OpenMaster()));
}

std::map<std::string,std::string> MainWindow::get_url()
{
    std::map<std::string,std::string> remappings;
    remappings["__master"] = ui.master_url->text().toStdString();
    remappings["__hostname"] = ui.hostname->text().toStdString();
    return remappings;
}

void MainWindow::ui_init()
{
    teleop = NULL;
    sshclient = NULL;

    fsshclient = false;
    frosmaster = false;
    cout << "ui_init" <<endl;
}

/*********************************************************************************
				以下为槽函数
*********************************************************************************/
//速度初始化
void MainWindow::speed_init()
{
    string text = QObject::sender()->objectName().toStdString();
    cout << text <<endl;

    if (text == "long_2" && ui.long_2->isChecked()){
        // string text=ui->teleop_topic->text().toStdString();
        // if (text=="")
        // {
		// 	cout << "命名为空" << endl;
        // }else{
        if(ui.jog->isChecked()){
            disconnect(ui.vx_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vx_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));

            disconnect(ui.vx_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vx_low, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vy_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vy_low, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vth_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vth_low, SIGNAL(released()),this,SLOT(speed_stop()));

            ui.jog->blockSignals(true);
            ui.jog->setCheckState(Qt::CheckState::Unchecked);
            ui.jog->blockSignals(false);
        }else {
            teleop = new Teleop(get_url(), "cmd_vel");
        }
            connect(ui.vx_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.vx_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.vy_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.vy_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.vth_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.vth_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            connect(ui.v_stop,SIGNAL(clicked()),this,SLOT(speed_stop()));
        // }
    }else if (text == "jog" && ui.jog->isChecked()) {
        if(ui.long_2->isChecked()){
            disconnect(ui.vx_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vx_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.v_stop,SIGNAL(clicked()),this,SLOT(speed_stop()));

            ui.long_2->blockSignals(true);
            ui.long_2->setCheckState(Qt::CheckState::Unchecked);
            ui.long_2->blockSignals(false);
        }else {
            teleop = new Teleop(get_url(), "cmd_vel");
        }
        connect(ui.vx_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
        connect(ui.vx_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
        connect(ui.vy_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
        connect(ui.vy_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
        connect(ui.vth_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
        connect(ui.vth_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));

        connect(ui.vx_plus, SIGNAL(released()),this,SLOT(speed_stop()));
        connect(ui.vx_low, SIGNAL(released()),this,SLOT(speed_stop()));
        connect(ui.vy_plus, SIGNAL(released()),this,SLOT(speed_stop()));
        connect(ui.vy_low, SIGNAL(released()),this,SLOT(speed_stop()));
        connect(ui.vth_plus, SIGNAL(released()),this,SLOT(speed_stop()));
        connect(ui.vth_low, SIGNAL(released()),this,SLOT(speed_stop()));
    }else{
        string str = QObject::sender()->objectName().toStdString();
        cout << str <<endl;
        if(str == "long_2"){
            disconnect(ui.vx_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vx_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_plus,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_low,SIGNAL(clicked()),this,SLOT(speed_cmd()));
            disconnect(ui.v_stop,SIGNAL(clicked()),this,SLOT(speed_stop()));
        }else if (str == "jog") {
            disconnect(ui.vx_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vx_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vy_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_plus, SIGNAL(pressed()),this,SLOT(speed_cmd()));
            disconnect(ui.vth_low, SIGNAL(pressed()),this,SLOT(speed_cmd()));

            disconnect(ui.vx_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vx_low, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vy_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vy_low, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vth_plus, SIGNAL(released()),this,SLOT(speed_stop()));
            disconnect(ui.vth_low, SIGNAL(released()),this,SLOT(speed_stop()));
        }
        delete teleop;
        teleop = NULL;
	}
}
//
void MainWindow::speed_cmd()
{
    string str = QObject::sender()->objectName().toStdString();
    if (str ==  "vx_plus")
    {
        teleop->vx_pub(ui.vx_value->value());
    }else if (str == "vx_low") {
        teleop->vx_pub(-ui.vx_value->value());
    }else if (str == "vy_plus") {
        teleop->vy_pub(ui.vy_value->value());
    }else if (str == "vy_low") {
        teleop->vy_pub(-ui.vy_value->value());
    }else if (str == "vth_plus") {
        teleop->vth_pub(ui.vth_value->value());
    }else if (str == "vth_low") {
        teleop->vth_pub(-ui.vth_value->value());
    }
}
void MainWindow::speed_stop()
{
    teleop->v_stop();
}


/*********************************************************************************
                以下为其他函数
*********************************************************************************/

void MainWindow::closeEvent(QCloseEvent *event)
{
	QMainWindow::closeEvent(event);
}

}  // namespace reinovo_controller

