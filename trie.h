#ifndef TRIE_H
#define TRIE_H

#include <stdlib.h>
#include <vector>

namespace trie
{

template <class T>
class Node
{
public:
    Node() { mContent = NULL; mMarker = false; }
    ~Node() {}
    T *content() { return mContent; }
    void setContent(T t);
    bool wordMarker() { return mMarker; }
    void setWordMarker() { mMarker = true; }
    Node<T>* findChild(T t);
    void appendChild(Node<T>* child) { mChildren.push_back(child); }
    std::vector<Node<T>*> children() { return mChildren; }

private:
    T *mContent;
    bool mMarker;
    std::vector<Node*> mChildren;
};

template <class T>
class Trie {
public:
    Trie() { root = new Node<T>(); };
    ~Trie() {};
    void insert(std::vector<T> v);
    bool searchWord(std::vector<T> v);
    Node<T> *getRootNode() { return root; };
    void deleteWord(std::vector<T> v);
private:
    Node<T> *root;
};

template <class T>
void Node<T>::setContent(T t)
{
    if (mContent == NULL)
    {
        mContent = new T;
    }
    *mContent = t;
}

template <class T>
Node<T>* Node<T>::findChild(T t)
{
    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        Node<T>* tmp = mChildren.at(i);
        if (tmp->content() != NULL)
        {
            if (*tmp->content() == t)
            {
                return tmp;
            }
        }
    }

    return NULL;
}

template <class T>
void Trie<T>::insert(std::vector<T> v)
{
    Node<T>* current = root;

    if (v.size() == 0)
    {
        // sure, we'll insert an empty word
        current->setWordMarker();
        return;
    }

    for (unsigned int i = 0; i < v.size(); i++)
    {        
        Node<T>* child = current->findChild(v[i]);
        if (child != NULL)
        {
            current = child;
        }
        else
        {
            Node<T>* tmp = new Node<T>();
            tmp->setContent(v[i]);
            current->appendChild(tmp);
            current = tmp;
        }
        if (i == v.size() - 1)
            current->setWordMarker();
    }
}

template <class T>
bool Trie<T>::searchWord(std::vector<T> v)
{
    Node<T>* current = root;

    for (unsigned int i = 0; i < v.length(); i++)
    {
        Node<T>* tmp = current->findChild(v[i]);
        if (tmp == NULL)
            return false;
        current = tmp;
    }

    return current->wordMarker();
}

}

#endif // TRIE_H
