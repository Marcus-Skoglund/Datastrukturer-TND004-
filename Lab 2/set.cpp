#include "set.h"
#include "node.h"

int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/

/*
 *  Default constructor :create an empty Set
 */
int Set::get_count_nodes() { // O(1)
    return Set::Node::count_nodes;
}

/*
 *  Default constructor :create an empty Set
 */
Set::Set() : counter{0} { // O(1)
    // IMPLEMENT before Lab2 HA
    head = new Node{};
    tail = new Node{};

    head->next = tail;
    tail->prev = head;
}

/*
 *  Conversion constructor: convert val into a singleton {val}
 */
Set::Set(int val) : Set{} {  // create an empty list // O(1)
    // IMPLEMENT before Lab2 HA
    insert_node(tail, val);
}

/*
 * Constructor to create a Set from a sorted vector of unique ints
 * \param list_of_values is an increasingly sorted vector of unique ints
 */
Set::Set(const std::vector<int>& list_of_values) : Set{} {  // create an empty list // O(n)
    // IMPLEMENT before Lab2 HA
    for (int val : list_of_values) {
        insert_node(tail, val);
    }
}

/*
 * Copy constructor: create a new Set as a copy of Set S
 * \param S Set to copied
 * Function does not modify Set S in any way
 */
Set::Set(const Set& S) : Set{} {  // create an empty list // O(n)
    // IMPLEMENT before Lab2 HA
    Node* current = S.head->next;
    while (current != S.tail) {
        insert_node(tail, current->value);
        current = current->next;
    }
}

/*
 * Transform the Set into an empty set
 * Remove all nodes from the list, except the dummy nodes
 */
void Set::make_empty() { // O(n)
    // IMPLEMENT before Lab2 HA
    Node* current = head->next;
    while (current != tail) {
        Node* temp = current;
        current = current->next;
        remove_node(temp);
    }
    counter = 0;
}

/*
 * Destructor: deallocate all memory (Nodes) allocated for the list
 */
Set::~Set() { // O(n)
    // IMPLEMENT before Lab2 HA
    make_empty();
    remove_node(head);
    remove_node(tail);
}

/*
 * Assignment operator: assign new contents to the *this Set, replacing its current content
 * \param S Set to be copied into Set *this
 * Use copy-and swap idiom -- TNG033: lecture 5
 */
Set& Set::operator=(Set S) { // O(1)?
    // IMPLEMENT before Lab2 HA
    std::swap(head, S.head);
    std::swap(tail, S.tail);
    std::swap(counter, S.counter);
    return *this;
}

/*
 * Test whether val belongs to the Set
 * Return true if val belongs to the set, otherwise false
 * This function does not modify the Set in any way
 */
bool Set::is_member(int val) const { // O(n)
    // IMPLEMENT before Lab2 HA
    Node* current = head->next;
    while (current != tail) {
        if (current->value == val) {
            return true;
        }
        if (current->value > val) {
            return false; // Early exit since list is sorted
        }
        current = current->next;
    }
    return false;
}

/*
 * Three-way comparison operator: to test whether *this == S, *this < S, *this > S
 * Return std::partial_ordering::equivalent, if *this == S
 * Return std::partial_ordering::less, if *this < S
 * Return std::partial_ordering::greater, if *this > S
 * Return std::partial_ordering::unordered, otherwise
 *
 * Requirement: must iterate through each set no more than once
 */
std::partial_ordering Set::operator<=>(const Set& S) const { // O(n)
    // IMPLEMENT before Lab2 HA
    Node* p1 = head->next;
    Node* p2 = S.head->next;
    bool less_than = true;
    bool greater_than = true;

    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            less_than = false;
            p1 = p1->next;
        }
        else if (p1->value > p2->value) {
            greater_than = false;
            p2 = p2->next;
        }
        else {
            p1 = p1->next;
            p2 = p2->next;
        }
    }

    // checks if there is still slots unexplored

    if (p1 != tail) {
        less_than = false;
    }
    if (p2 != S.tail) {
        greater_than = false;
    }

    if (less_than && greater_than) {
        return std::partial_ordering::equivalent;
    }
    else if (less_than) {
        return std::partial_ordering::less;
    }
    else if (greater_than) {
        return std::partial_ordering::greater;
    }
    else {
        return std::partial_ordering::unordered;
    }
}

/*
 * Test whether Set *this and S represent the same set
 * Return true, if *this has same elemnts as set S
 * Return false, otherwise
 *
 * Requirement: must iterate through each set no more than once
 */
bool Set::operator==(const Set& S) const { // O(n)
    // IMPLEMENT before Lab2 HA
    return (*this <=> S) == std::partial_ordering::equivalent;   // break early?
}

/*
 * Modify Set *this such that it becomes the union of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator+=(const Set& S) { // O(n), O(n + m)
    // IMPLEMENT
    Node* p1 = head->next;
    Node* p2 = S.head->next;

    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            p1 = p1->next;
        }
        else if (p1->value > p2->value) {
            insert_node(p1, p2->value);
            p2 = p2->next;
        }
        else {
            p1 = p1->next;
            p2 = p2->next;
        }
    }

    while (p2 != S.tail) {
        insert_node(tail, p2->value);
        p2 = p2->next;
    }
    return *this;
}

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator*=(const Set& S) { // O(n), O(n + m)
    // IMPLEMENT
    Node* p1 = head->next;
    Node* p2 = S.head->next;

    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            Node* to_delete = p1;
            p1 = p1->next;
            remove_node(to_delete);
        }
        else if (p1->value > p2->value) {
            p2 = p2->next;
        }
        else {
            p1 = p1->next;
            p2 = p2->next;
        }
    }

    while (p1 != tail) {
        Node* to_delete = p1;
        p1 = p1->next;
        remove_node(to_delete);
    }

    return *this;
}

/*
 * Modify Set *this such that it becomes the Set difference between Set *this and Set S
 * Set *this is modified and then returned
 */
Set& Set::operator-=(const Set& S) { // O(n), O(n + m)
    // IMPLEMENT
    Node* p1 = head->next;
    Node* p2 = S.head->next;

    while (p1 != tail && p2 != S.tail) {
        if (p1->value < p2->value) {
            p1 = p1->next;
        }
        else if (p1->value > p2->value) {
            p2 = p2->next;
        }
        else {
            Node* to_delete = p1;
            p1 = p1->next;
            p2 = p2->next;
            remove_node(to_delete);
        }
    }
    return *this;
}


/* ******************************************** *
 * Private Member Functions -- Implementation   *
 * ******************************************** */

/*
 * Insert a new Node storing val before the Node pointed by p
 * \param p pointer to a Node
 * \param val value to be inserted before position p
 */
void Set::insert_node(Node* p, int val) { // O(1)
    // IMPLEMENT before Lab2 HA
    Node* newNode = new Node(val, p, p->prev);  // value of place, pointer to next node (tail), pointer to previous node
    p->prev->next = newNode;
    p->prev = newNode;
    ++counter;
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) { // O(1)
    // IMPLEMENT before Lab2 HA
    if (p == nullptr) return;

    if (p->prev != nullptr) {
        p->prev->next = p->next;
    }
    if (p->next != nullptr) {
        p->next->prev = p->prev;
    }

    delete p;
    
    // Only decrement counter for real nodes
    if (p != head && p != tail) {
        --counter;
    }
}

/*
 * Write Set *this to stream os
 */
void Set::write_to_stream(std::ostream& os) const { // O(n)
    if (is_empty()) {
        os << "Set is empty!";
    } else {
        Set::Node* ptr{head->next};

        os << "{ ";
        while (ptr != tail) {
            os << ptr->value << " ";
            ptr = ptr->next;
        }
        os << "}";
    }
}
