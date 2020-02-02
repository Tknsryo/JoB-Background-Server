#ifndef GLOBAL_H
#define GLOBAL_H
#include<QByteArrayList>
#include<QMutex>
extern QByteArrayList *dataList;
extern QByteArrayList *dataForGame;
extern QMutex *mutex1, *mutex2;
extern bool gameOn;
#endif // GLOBAL_H
