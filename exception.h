#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>

class Exception : public QException
{
public:
    void raise() const override { throw *this; }
    Exception *clone() const override { return new Exception(*this); }
};

#endif // EXCEPTION_H
