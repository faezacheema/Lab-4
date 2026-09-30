
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;


// Node: one bit

struct Bit {
    int bit;        // 0 or 1
    Bit* next;      // towards the LSB
    Bit* prev;      // towards the MSB
    Bit(int b) : bit(b), next(nullptr), prev(nullptr) {}
};


// BinaryDLL: a binary number stored in a doubly linked list

class BinaryDLL {
private:
    Bit* head;   // MSB
    Bit* tail;   // LSB
    int size;    // number of bits

    void pushBack(int b) {              // add a bit at the LSB end
        Bit* n = new Bit(b);
        if (tail == nullptr) head = tail = n;
        else { tail->next = n; n->prev = tail; tail = n; }
        size++;
    }

    void pushFront(int b) {             // add a bit at the MSB end
        Bit* n = new Bit(b);
        if (head == nullptr) head = tail = n;
        else { n->next = head; head->prev = n; head = n; }
        size++;
    }

    void popFront() {                   // remove the MSB
        Bit* old = head;
        head = head->next;
        if (head != nullptr) head->prev = nullptr; else tail = nullptr;
        delete old;
        size--;
    }

    void clear() {
        while (head != nullptr) popFront();
    }

    // Add leading zeros until the length is a multiple of 8 (and at least 8)
    void padToBlock() {
        while (size == 0 || size % 8 != 0) pushFront(0);
    }

    // Remove whole leading blocks of 8 zeros (keeps at least one block)
    void trimZeroBlocks() {
        while (size > 8) {
            Bit* p = head;
            bool allZero = true;
            for (int i = 0; i < 8; i++, p = p->next)
                if (p->bit != 0) { allZero = false; break; }
            if (!allZero) break;
            for (int i = 0; i < 8; i++) popFront();
        }
    }

    // Shift left by one place (multiply by 2): append a 0 at the LSB end
    void shiftLeft() {
        pushBack(0);
        padToBlock();
    }

public:
    BinaryDLL() : head(nullptr), tail(nullptr), size(0) {}

    // Deep copy so two objects never share nodes
    BinaryDLL(const BinaryDLL& other) : head(nullptr), tail(nullptr), size(0) {
        for (Bit* p = other.head; p != nullptr; p = p->next) pushBack(p->bit);
    }

    BinaryDLL& operator=(const BinaryDLL& other) {
        if (this != &other) {
            clear();
            for (Bit* p = other.head; p != nullptr; p = p->next) pushBack(p->bit);
        }
        return *this;
    }

    ~BinaryDLL() { clear(); }

    // Store Binary Number
    // Returns false if the text contains anything other than 0 and 1.
    bool store(const string& s) {
        if (s.empty()) return false;
        for (char c : s) if (c != '0' && c != '1') return false;
        clear();
        for (char c : s) pushBack(c - '0');
        padToBlock();           // 8-bit grouping; extends to more blocks if needed
        return true;
    }

    // Display, grouped in blocks of 8 bits
    void display() const {
        int count = 0;
        for (Bit* p = head; p != nullptr; p = p->next) {
            cout << p->bit;
            if (++count % 8 == 0 && p->next != nullptr) cout << ' ';
        }
    }

    // 1's Complement: traverse and flip every bit
    void onesComplement() {
        for (Bit* p = head; p != nullptr; p = p->next)
            p->bit = 1 - p->bit;
    }

    // 2's Complement: 1's complement, then add 1 (fixed width)
    void twosComplement() {
        onesComplement();

        BinaryDLL one;                      // same width, value = 1
        for (int i = 0; i < size - 1; i++) one.pushBack(0);
        one.pushBack(1);

        *this = add(*this, one, true);      // true = keep width, drop final carry
    }

    // Binary Addition 
    // Walk both numbers from LSB to MSB, adding bit + bit + carry.
    // keepWidth = false: a final carry adds a new block (unsigned addition)
    // keepWidth = true : a final carry is dropped (used for 2's complement)
    static BinaryDLL add(const BinaryDLL& a, const BinaryDLL& b, bool keepWidth = false) {
        BinaryDLL result;
        Bit* pa = a.tail;
        Bit* pb = b.tail;
        int carry = 0;

        while (pa != nullptr || pb != nullptr) {
            int sum = carry;
            if (pa != nullptr) { sum += pa->bit; pa = pa->prev; }
            if (pb != nullptr) { sum += pb->bit; pb = pb->prev; }
            result.pushFront(sum % 2);      // bit to store
            carry = sum / 2;                // carry to next column
        }
        if (carry == 1 && !keepWidth) result.pushFront(1);

        result.padToBlock();
        return result;
    }

    // Binary Multiplication: repeated addition + shifting 
    // For each bit of the multiplier (LSB first): if it is 1, add the
    // multiplicand to the result; then shift the multiplicand left by one.
    static BinaryDLL multiply(const BinaryDLL& a, const BinaryDLL& b) {
        BinaryDLL result;
        result.store("0");
        BinaryDLL shifted = a;              // multiplicand, shifted each round

        for (Bit* p = b.tail; p != nullptr; p = p->prev) {
            if (p->bit == 1) result = add(result, shifted);
            shifted.shiftLeft();
        }
        result.trimZeroBlocks();
        return result;
    }

    // Conversion to Decimal
    // Uses a digit array, so it works for numbers of any length
    // (no overflow). Process bits MSB -> LSB: value = value * 2 + bit.
    string toDecimal() const {
        vector<int> digits(1, 0);           // little-endian decimal digits
        for (Bit* p = head; p != nullptr; p = p->next) {
            int carry = p->bit;
            for (size_t i = 0; i < digits.size(); i++) {
                int v = digits[i] * 2 + carry;
                digits[i] = v % 10;
                carry = v / 10;
            }
            if (carry > 0) digits.push_back(carry);
        }
        string s;
        for (int i = (int)digits.size() - 1; i >= 0; i--) s += char('0' + digits[i]);
        return s;
    }
};


// Input helpers

static void readBinary(const string& label, BinaryDLL& target) {
    string s;
    cout << "Enter binary number " << label << ": ";
    while (cin >> s && !target.store(s)) {
        cout << "Only 0 and 1 are allowed. Try again: ";
    }
}

int main() {
    BinaryDLL A, B;
    readBinary("A", A);
    readBinary("B", B);

    int choice;
    do {
        cout << "\n BINARY DLL MENU \n"
            << "1. Store new A and B\n"
            << "2. 1's Complement of A\n"
            << "3. 2's Complement of A\n"
            << "4. Add A + B\n"
            << "5. Multiply A x B\n"
            << "6. Convert A and B to Decimal\n"
            << "0. Exit\n"
            << "Enter choice: ";
        if (!(cin >> choice)) break;

        switch (choice) {
        case 1:
            readBinary("A", A);
            readBinary("B", B);
            break;
        case 2: {
            BinaryDLL t = A;
            t.onesComplement();
            cout << "A     = "; A.display(); cout << endl;
            cout << "1's C = "; t.display(); cout << endl;
            break;
        }
        case 3: {
            BinaryDLL t = A;
            t.twosComplement();
            cout << "A     = "; A.display(); cout << endl;
            cout << "2's C = "; t.display(); cout << endl;
            break;
        }
        case 4: {
            BinaryDLL r = BinaryDLL::add(A, B);
            cout << "A     = "; A.display(); cout << endl;
            cout << "B     = "; B.display(); cout << endl;
            cout << "A + B = "; r.display();
            cout << "  (decimal " << r.toDecimal() << ")\n";
            break;
        }
        case 5: {
            BinaryDLL r = BinaryDLL::multiply(A, B);
            cout << "A     = "; A.display(); cout << endl;
            cout << "B     = "; B.display(); cout << endl;
            cout << "A x B = "; r.display();
            cout << "  (decimal " << r.toDecimal() << ")\n";
            break;
        }
        case 6:
            cout << "A = "; A.display(); cout << " -> " << A.toDecimal() << endl;
            cout << "B = "; B.display(); cout << " -> " << B.toDecimal() << endl;
            break;
        case 0: cout << "Goodbye!\n"; break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}