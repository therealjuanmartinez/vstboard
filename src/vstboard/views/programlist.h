#ifndef PROGRAMLIST_H
#define PROGRAMLIST_H

//#include <QWidget>
#include "precomp.h"

namespace Ui {
    class ProgramList;
}

class ProgramList : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramList(QWidget *parent = 0);
    ~ProgramList();

    void SetModel(QStandardItemModel *model);

private:
    Ui::ProgramList *ui;
    QStandardItemModel *model;
    int currentGrp;
    int currentPrg;
    int currentGrpDragging;

signals:
    void ChangeProg(const QModelIndex &index);

public slots:
    void OnProgChange(const QModelIndex &index);
    void OnDragOverGroups( QWidget *source, QModelIndex index);
    void rowsInserted ( const QModelIndex & parent, int start, int end );
    void OnGrpStartDrag(QModelIndex index);
    void ShowCurrentGroup();

private slots:
    void on_listProgs_clicked(QModelIndex index);
    void on_listGrps_clicked(QModelIndex index);
};

#endif // PROGRAMLIST_H
