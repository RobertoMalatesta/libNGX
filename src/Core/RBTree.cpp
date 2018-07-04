#include "Core/Core.h"

namespace ngx::Core {

    RBTreeNode::RBTreeNode() {

    }

    bool RBTreeNode::IsBlack() {
        return false;
    }

    void RBTreeNode::SetBlack() {
    }

    bool RBTreeNode::IsRed() {
        return false;
    }

    void RBTreeNode::SetRed() {
    }

    void RBTreeNode::CopyColor(ngx::Core::RBTreeNode *Node) {

    }

    int RBTreeNode::Compare(ngx::Core::RBTreeNode *Node) {
        return 0;
    }

    RBTreeNode* RBTreeNode::CreateFromAllocator(ngx::Core::MemAllocator *Allocator, uint DateSize) {
        return nullptr;
    }

    void RBTreeNode::FreeFromAllocator(ngx::Core::RBTreeNode *Node) {
    }
}