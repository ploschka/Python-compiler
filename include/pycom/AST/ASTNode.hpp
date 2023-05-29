#pragma once
#include <pycom/interface/ASTNodeInterface.hpp>
#include <pycom/interface/VisitableNodeInterface.hpp>

class BaseASTNode : public ASTNodeInterface, public VisitableNodeInterface
{
};
