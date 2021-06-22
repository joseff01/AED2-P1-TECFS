#ifndef PERSONAL_LINKED_LIST_NODE_H
#define PERSONAL_LINKED_LIST_NODE_H

template <typename T>
class Node
{
public:
    //! Holds reference to next node
    Node<T> *next;

    //! Holds reference to previos node
    Node<T> *prev;

    //! Holds template type value of node
    T value;

    /*!
     * \brief Construct a new Node object, the value for it must be supplied when construfcted
     * 
     * \param value the value being assigned to the node
     */
    Node(const T value) : next(nullptr), prev(nullptr), value(value) {}
};

#endif