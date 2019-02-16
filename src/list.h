template <typename T>
class List {
private:
    class Node {
    public:
        T *data;
        Node *next;
        Node *prev;

        Node(const T &elem, Node *next, Node *prev) {
            this->data = new T(elem);
            this->next = next;
            this->prev = prev;
        }

        ~Node() {
            if(data) {
                delete data;
            }
        }
    };

public:
    List() {
        tail = head = nullptr;
    }

    ~List() {
        while(head) {
            Node *next = head->next;
            delete head;
            head = next;
        }
    }

    void append(const T &elem) {
        Node *new_node = new Node(elem, nullptr, tail);
        if(tail) {
            tail->next = new_node;
        }
        tail = new_node;
        if(!head) {
            head = tail;
        }
    }

    void prepend(const T &elem) {
        Node *new_node = new Node(elem, head, nullptr);
        if(head) {
            head->prev = new_node;
        }
        head = new_node;
        if(!tail) {
            tail = head;
        }
    }

    // FIXME: Should be a class that supports *,++,--
    const Node *iterator() {
        return head;
    }

private:
    Node *tail, *head;
};