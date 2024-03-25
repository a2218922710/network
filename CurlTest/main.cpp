
#include "networkhandle.h"

#include <qglobal.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    //test
    NetworkHandle::GlobalInit();

    qDebug()<<"msg : " << NetworkHandle::Get("https://www.baidu.com/");

    NetworkHandle::CleanUp();

    return 0;
}
