#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), server(1111)
{
    server.startServer();
}

Dialog::~Dialog()
{

}
