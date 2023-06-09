#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>

class VisitableNodeInterface
{
public:
    virtual void accept(NodeVisitorInterface *_visitor) = 0;
};
