#ifndef __LIST__
#define __LIST__

//#include <stdlib.h>
#include <iostream.h>

template<class T>
struct ListNode
{
  T* element;
  ListNode* next;
  ListNode* prev;

  ListNode(T* e = NULL, ListNode* n = NULL, ListNode* p = NULL) 
    : element(e), next(n), prev(p) { }
};

template<class T>
struct ListIterator
{
  ListNode<T>* listp;

  ListIterator(ListNode<T>* p = NULL) : listp(p) {} 
  
  inline const ListIterator<T>& ListIterator<T>::operator++ (int);
  inline const ListIterator<T>& ListIterator<T>::operator-- (int);
  inline T* operator() () const;
  //  bool operator! () const { return listp == NULL; }
  inline bool ok() const { return listp != NULL;}
};



template <class T>
class List
{ 
public:
  List<T>::List(const bool resp=true);
  ~List ();

  const List& operator= (const List&);
  
  void print_list(ostream&) const;
    
  bool is_empty() const { return list_head->next == NULL; }
  bool find(const T*, ListIterator<T>&);
  T* get_nth(const int n) const;
  int  number_of_elements() const;

  void first(ListIterator<T>& li) const { li.listp = list_head->next; };
  void last(ListIterator<T>& li)  const { li.listp = list_head->prev; };
  //  void header(ListIterator<T>& li)  { li.listp = list_head; };


  void remove(ListIterator<T>&);
  bool remove(const T*);

  void insert_first( T* );
  void insert_last( T* );

  void delete_list();

  void set_deletion_responsibility(const bool r) { responsible_for_deletion = r; }
    
private:

  ListNode<T>*  list_head;
  bool responsible_for_deletion;
};

#endif __LIST__
