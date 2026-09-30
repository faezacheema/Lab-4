
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
using namespace std;

// Node of the doubly linked list (one song)

struct Song {
    int id;
    string name;
    int minutes;
    int seconds;
    Song* next;   // pointer to the next song
    Song* prev;   // pointer to the previous song

    Song(int i, const string& n, int m, int s)
        : id(i), name(n), minutes(m), seconds(s), next(nullptr), prev(nullptr) {
    }
};


// Playlist class: wraps the DLL and all operations

class Playlist {
private:
    Song* head;      // first song
    Song* tail;      // last song
    Song* current;   // song that is "playing" right now (nullptr = nothing playing)

    static void printSong(const Song* s) {
        cout << "ID: " << setw(4) << left << s->id
            << " | Name: " << setw(25) << left << s->name
            << " | Duration: " << s->minutes << ":"
            << setw(2) << setfill('0') << right << s->seconds << setfill(' ')
            << endl;
    }

    Song* findById(int id) const {
        for (Song* p = head; p != nullptr; p = p->next)
            if (p->id == id) return p;
        return nullptr;
    }

public:
    Playlist() : head(nullptr), tail(nullptr), current(nullptr) {}

    // Destructor frees every node so there are no memory leaks
    ~Playlist() {
        Song* p = head;
        while (p != nullptr) {
            Song* nextNode = p->next;
            delete p;
            p = nextNode;
        }
    }

    // Copying is disabled: two playlists must never share the same nodes
    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;

    //  Add Song: insert at the end 
    bool addSong(int id, const string& name, int m, int s) {
        if (findById(id) != nullptr) {
            cout << "A song with ID " << id << " already exists.\n";
            return false;
        }
        Song* node = new Song(id, name, m, s);
        if (head == nullptr) {          // empty list
            head = tail = node;
        }
        else {                        // link after the current tail
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        cout << "Song added.\n";
        return true;
    }

    // Delete Song by ID 
    bool deleteSong(int id) {
        Song* target = findById(id);
        if (target == nullptr) {
            cout << "No song with ID " << id << " found.\n";
            return false;
        }

        // If the deleted song is the one playing, move "current" so it never dangles
        if (current == target)
            current = (target->next != nullptr) ? target->next : target->prev;

        // Unlink from the previous side
        if (target->prev != nullptr) target->prev->next = target->next;
        else head = target->next;                    // deleting the first node

        // Unlink from the next side
        if (target->next != nullptr) target->next->prev = target->prev;
        else tail = target->prev;                    // deleting the last node

        delete target;
        cout << "Song deleted.\n";
        return true;
    }

    //  Display forward: head to tail 
    void displayForward() const {
        if (head == nullptr) { cout << "Playlist is empty.\n"; return; }
        cout << "--- Playlist (first to last) ---\n";
        for (Song* p = head; p != nullptr; p = p->next) printSong(p);
    }

    //  Display backward: tail to head (uses the prev pointers) 
    void displayBackward() const {
        if (tail == nullptr) { cout << "Playlist is empty.\n"; return; }
        cout << "--- Playlist (last to first) ---\n";
        for (Song* p = tail; p != nullptr; p = p->prev) printSong(p);
    }

    // ---- Search by ID ----
    void searchSong(int id) const {
        Song* s = findById(id);
        if (s == nullptr) { cout << "Song not found.\n"; return; }
        cout << "Song found:\n";
        printSong(s);
    }

    //  Play Next 
    void playNext() {
        if (head == nullptr) { cout << "Playlist is empty.\n"; return; }
        if (current == nullptr) current = head;              // nothing playing yet: start at first
        else if (current->next != nullptr) current = current->next;
        else { cout << "Already at the last song.\n"; }
        cout << "Now playing: ";
        printSong(current);
    }

    //  Play Previous 
    void playPrevious() {
        if (head == nullptr) { cout << "Playlist is empty.\n"; return; }
        if (current == nullptr) current = tail;              // nothing playing yet: start at last
        else if (current->prev != nullptr) current = current->prev;
        else { cout << "Already at the first song.\n"; }
        cout << "Now playing: ";
        printSong(current);
    }


    // For every node we swap its next and prev pointers, then swap head and tail.
    // No node is created or copied, so it is O(n) time and O(1) extra space.
    void reversePlaylist() {
        if (head == nullptr || head == tail) {
            cout << "Nothing to reverse.\n";
            return;
        }
        Song* p = head;
        while (p != nullptr) {
            Song* oldNext = p->next;   // remember where we were going
            p->next = p->prev;
            p->prev = oldNext;
            p = oldNext;               // move on using the saved pointer
        }
        Song* tmp = head;
        head = tail;
        tail = tmp;
        cout << "Playlist reversed.\n";
    }
};


// Input helpers

static int readInt(const string& prompt) {
    int x;
    cout << prompt;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input, try again: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return x;
}

int main() {
    Playlist playlist;
    int choice;

    do {
        cout << "\ PLAYLIST MENU \n"
            << "1. Add Song\n"
            << "2. Delete Song\n"
            << "3. Display Playlist Forward\n"
            << "4. Display Playlist Backward\n"
            << "5. Search Song\n"
            << "6. Play Next Song\n"
            << "7. Play Previous Song\n"
            << "8. Reverse Playlist\n"
            << "0. Exit\n";
        choice = readInt("Enter choice: ");

        switch (choice) {
        case 1: {
            int id = readInt("Song ID: ");
            string name;
            cout << "Song name: ";
            getline(cin, name);
            int m = readInt("Duration - minutes: ");
            int s = readInt("Duration - seconds (0-59): ");
            if (m < 0 || s < 0 || s > 59) {
                cout << "Invalid duration.\n";
                break;
            }
            playlist.addSong(id, name, m, s);
            break;
        }
        case 2: playlist.deleteSong(readInt("Song ID to delete: ")); break;
        case 3: playlist.displayForward(); break;
        case 4: playlist.displayBackward(); break;
        case 5: playlist.searchSong(readInt("Song ID to search: ")); break;
        case 6: playlist.playNext(); break;
        case 7: playlist.playPrevious(); break;
        case 8: playlist.reversePlaylist(); break;
        case 0: cout << "Goodbye!\n"; break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}