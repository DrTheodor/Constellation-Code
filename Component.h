#ifndef COMPONENT_H
#define COMPONENT_H
#include <QString>

class Component
{
public:
    Component() {}

    QString name;
    QString text;
    bool enabled = true;
};
#endif // COMPONENT_H
