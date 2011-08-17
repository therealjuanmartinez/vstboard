#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "optimizerstep.h"
#include "mutexdebug.h"

class Optimizer
{
public:
    Optimizer();
    ~Optimizer();
    void SetNbThreads(int nb);
    void NewListOfNodes(const QList<RendererNode*> & listNodes);
    void Optimize();
    OptimizerStep* GetStep(int step);
    void SetStep(int step, OptimizerStep* s);
    QMap<int, RendererNode* > GetThreadNodes(int thread);
    QList<RendererNode*> GetListOfNodes();
//    void BuildModel( QStandardItemModel *model);
//    void UpdateView( QStandardItemModel *model );

protected:
    void Clear();
    QMap<int,OptimizerStep*>listOfSteps;
    int nbThreads;
    DMutex mutex;
};

#endif // OPTIMIZER_H
