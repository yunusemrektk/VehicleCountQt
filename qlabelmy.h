#ifndef QLABELMY_H
#define QLABELMY_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <opencv2/features2d.hpp>

class qlabelmy : public QLabel
{
    Q_OBJECT
public:
    explicit qlabelmy(QWidget *parent=0);
    void mousePressEvent(QMouseEvent *ev) override;
    int x,y;
    int ClickCount=1;
signals:
    void MousePressed(QMouseEvent *ev);

};

#endif // QLABELMY_H
