#include "start.h"
#include "./ui_start.h"



Start::Start(QWidget *parent): QMainWindow(parent), ui(new Ui::Start)
{

    ui->setupUi(this);


}

Start::~Start()
{
    delete ui;
}
