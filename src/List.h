#ifndef __LIST__
#define __LIST__

//#include <stdlib.h>
//#include <iostream.h>

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
  
  const ListIterator<T>& ListIterator<T>::operator++ (int);
  const ListIterator<T>& ListIterator<T>::operator-- (int);
  T* operator() () const;
  //  bool operator! () const { return listp == NULL; }
  bool ok() const { return listp != NULL;}

  //  friend class List;
};



template <class T>
class List
{ 
public:
  List ();
  ~List ();

  const List& operator= (const List&);



  T* get_nth(const int n) const;
  
  void print_list(ostream&) const;
    
  bool is_empty() const { return list_head->next == NULL; }
  bool find(const T*, ListIterator<T>&);
  int  number_of_elements() const;

  void first(ListIterator<T>& li) { li.listp = list_head->next; };
  void last(ListIterator<T>& li)  { li.listp = list_head->prev; };
  //  void header(ListIterator<T>& li)  { li.listp = list_head; };


  void remove(ListIterator<T>&, bool delete_elements = true);
  bool remove(const T*, bool delete_elements = true);
  //  void insert( T* );
  void insert_first( T* );
  void insert_last( T* );

  void delete_list(bool delete_elements = true);
    
private:

  ListNode<T>*  list_head;
  //  ListNode<T>*  current_pos;
};

#endif __LIST__
