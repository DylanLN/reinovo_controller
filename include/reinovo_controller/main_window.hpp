/**
 * @file /include/reinovo_controller/main_window.hpp
 *
 * @brief Qt based gui for reinovo_controller.
 *
 * @date November 2010
 **/
#ifndef reinovo_controller_MAIN_WINDOW_H
#define reinovo_controller_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"

//m
#include "teleop.h"
#include "ConnectSshClient.h"
//
#include "boost/thread.hpp"
#include "boost/bind.hpp"


/*****************************************************************************
** Namespace
*****************************************************************************/

namespace reinovo_controller {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();


	void closeEvent(QCloseEvent *event); // Overloaded function

    //
    std::map<std::string,std::string> get_url();
	bool ConnectAll();
    void ui_init();
	void SshThread();
//槽函数
public Q_SLOTS:
    void SshWrite();
    void SshClient();
	//用来编辑 ssh output
	void SshOutputAppendText(const QString &text);
	//打开master
    void OpenMaster();
	void speed_init();
	void speed_cmd();
    void speed_stop();
//信号
signals:
	//这个信号只是为了让text可以再多线程里面操作
	void AppendText(const QString &text);
private:
	Ui::MainWindowDesign ui;

	//变量
	int init_argc;
	char** init_argv;

	Teleop *teleop;

	//ssh
    bool fsshclient;
    boost::thread ssh_thread;
	ConnectSshClient *sshclient;
    uint8_t frosmaster;
    ConnectSshClient *rosmaster;
};

}  // namespace reinovo_controller

#endif // reinovo_controller_MAIN_WINDOW_H
