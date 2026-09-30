
#include <iostream>
#include <vector>
#include <limits>
using namespace std;


// Node: one person in the circle

struct Person {
    int id;
    Person* next;
    Person(int i) : id(i), next(nullptr) {}
};


// Create Circle: build a circular list of N people (IDs 1..N)
// Returns a pointer to person 1 (the head).

Person* createCircle(int n) {
    Person* head = new Person(1);
    Person* last = head;
    for (int i = 2; i <= n; i++) {
        last->next = new Person(i);
        last = last->next;
    }
    last->next = head;      // close the circle: last person -> first person
    return head;
}


// Elimination Process
// Starting from person 1, count k people; the k-th one is eliminated.
// Counting then resumes from the person right after the eliminated one.
// The eliminated IDs are stored in 'order'. Returns the survivor's ID.

int eliminate(Person* head, int n, int k, vector<int>& order) {
    // 'prev' always trails 'cur' by one node, so we can unlink cur.
    // Since the list is circular, the node before head is the last node.
    Person* prev = head;
    while (prev->next != head) prev = prev->next;
    Person* cur = head;

    int remaining = n;
    while (remaining > 1) {
        // Move forward k-1 times, so cur lands on the k-th person
        for (int step = 1; step < k; step++) {
            prev = cur;
            cur = cur->next;
        }

        // Eliminate cur: bypass it in the circle, then free it
        order.push_back(cur->id);
        prev->next = cur->next;
        Person* dead = cur;
        cur = cur->next;            // counting resumes from the next person
        delete dead;
        remaining--;
    }

    int survivor = cur->id;
    delete cur;                     // free the last node too (no memory leak)
    return survivor;
}


// Input helper: read an integer >= minValue

static int readPositive(const char* prompt, int minValue) {
    int x;
    cout << prompt;
    while (!(cin >> x) || x < minValue) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a number >= " << minValue << ": ";
    }
    return x;
}

int main() {
    int n = readPositive("Enter number of people (N): ", 1);
    int k = readPositive("Enter step count (k): ", 1);

    Person* head = createCircle(n);

    vector<int> order;
    int survivor = eliminate(head, n, k, order);

    // Display Eliminated Order
    cout << "\nElimination order: ";
    if (order.empty()) cout << "(none)";
    for (size_t i = 0; i < order.size(); i++) {
        cout << order[i];
        if (i + 1 < order.size()) cout << " -> ";
    }
    cout << endl;

    // Display Survivor
    cout << "Survivor: Person " << survivor << endl;

    return 0;
}