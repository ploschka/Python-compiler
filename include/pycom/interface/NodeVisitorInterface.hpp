#pragma once

class NodeVisitorInterface
{
public:
    // virtual void visitThisNode(ThisNode* acceptor) const = 0;
    // virtual void visitThatNode(ThatNode* acceptor) const = 0;
    virtual ~NodeVisitorInterface() = default;
};
