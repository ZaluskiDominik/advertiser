#ifndef READER_H
#define READER_H

#include "requests.h"

class Reader
{
public:
    explicit Reader(Request& req)
        :data(req.data), ss(&data, QIODevice::ReadOnly)
    {}

    template<class Type> Type read()
    {
        Type inst;
        ss >> inst;
        return inst;
    }

private:
    QByteArray& data;
    QDataStream ss;
};

#endif // READER_H
