#include "qlabelmy.h"
#include "mainwindow.h"
qlabelmy::qlabelmy(QWidget *parent):
    QLabel(parent)
{

}

void qlabelmy::mousePressEvent(QMouseEvent *ev){


    emit MousePressed(ev);
}
