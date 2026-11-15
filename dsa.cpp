// ============================================================
// PRN        : 202501100145
// Seed S     : 0145 (145)
// Scenario   : Cold-drink flavours (PRN mod 3 = 0)
// Hash func  : Mid-square (PRN is odd)
// Collision  : Separate chaining (PRN mod 2 = 1)
// Table size : m = 23 (smallest prime > 20 keys; assumption stated in report)
// ============================================================
#include <iostream>
#include <vector>
#include <list>
#include <string>
using namespace std;

// ---------- 1. Personalization key (set from your PRN at runtime) ----------
long long PRN           = 0;
int       S             = 0;
bool      USE_DIVISION  = true;    // even PRN -> division, odd -> mid-square
bool      USE_PROBING   = true;    // PRN mod 2 = 0 -> probing, 1 -> chaining
int       SCENARIO      = 0;
const long long DEFAULT_PRN = 202501100145LL;
const int M             = 23;
const int N             = 20;

void setupKey(long long prn) {
    PRN          = prn;
    S            = (int)(PRN % 10000);
    USE_DIVISION = (PRN % 2 == 0);
    USE_PROBING  = (PRN % 2 == 0);
    SCENARIO     = (int)(PRN % 3);
}

string scenarioName() {
    if (SCENARIO == 0) return "Cold-drink flavours";
    if (SCENARIO == 1) return "Student marks";
    return "Mango varieties";
}

// ---------- 3. Hash functions ----------
int divisionHash(int key) {
    return key % M;
}

// Mid-square: square the key, take the middle two digits, reduce mod M
// Square the key and extract the middle two digits
int midSquareHash(int key) {
    int sq  = key * key;
    int mid = (sq / 10) % 100;     // middle digits of a up-to-4-digit square
    return mid % M;
}

int hashFn(int key) {
    return USE_DIVISION ? divisionHash(key) : midSquareHash(key);
}

// ---------- 4. Hash table storage ----------
vector<int>       table_(M, -1);   // for linear probing (-1 = empty)
vector<list<int>> chains(M);       // for separate chaining

bool searchKey(int key) {
    int h = hashFn(key);
    if (USE_PROBING) {
        int i = h;
        do {
            if (table_[i] == key) return true;
            if (table_[i] == -1)  return false;
            i = (i + 1) % M;
        } while (i != h);
        return false;
    }
    for (int x : chains[h]) if (x == key) return true;
    return false;
}

void insertKey(int key) {
    if (searchKey(key)) {                       // duplicate-key handling
        cout << "Insert " << key << " -> duplicate, ignored\n";
        return;
    }
    int h = hashFn(key);
    if (USE_PROBING) {
        int i = h, probes = 1;
        while (table_[i] != -1) {
            i = (i + 1) % M;
            probes++;
            if (i == h) {
                cout << "Insert " << key << " -> table full\n";
                return;
            }
        }
        table_[i] = key;
        cout << "Insert " << key << " -> h(" << key << ") = " << h
             << (probes > 1 ? "  COLLISION" : "")
             << ", placed at index " << i << " (" << probes << " probe(s))\n";
    } else {
        bool collision = !chains[h].empty();
        chains[h].push_back(key);
        cout << "Insert " << key << " -> h(" << key << ") = " << h
             << (collision ? "  COLLISION, appended to chain" : "") << "\n";
    }
}

void display() {
    for (int i = 0; i < M; i++) {
        cout << "  [" << i << "] ";
        if (USE_PROBING) {
            if (table_[i] != -1) cout << table_[i];
        } else {
            for (int x : chains[i]) cout << x << " -> ";
            if (!chains[i].empty()) cout << "NULL";
        }
        cout << "\n";
    }
}

int main() {
    long long prn = DEFAULT_PRN;
    cout << "Enter PRN (press Enter to use default " << DEFAULT_PRN << "): ";
    string line;
    getline(cin, line);
    if (!line.empty()) {
        try { prn = stoll(line); }
        catch (...) { prn = 0; }
        if (prn <= 0) {
            cout << "Invalid PRN. Please enter digits only.\n";
            return 1;
        }
    }
    setupKey(prn);
    cout << "\n";

    cout << "PRN       : " << PRN << "\n";
    cout << "Seed S    : " << S << "\n";
    cout << "Scenario  : " << scenarioName() << "\n";
    cout << "Hash      : " << (USE_DIVISION ? "Division (key mod m)" : "Mid-square") << "\n";
    cout << "Collision : " << (USE_PROBING ? "Linear probing" : "Separate chaining") << "\n";
    cout << "Table size: " << M << "\n\n";

    // Dataset: v[i] = (S + 13*i) mod 100, i = 0..19
    vector<int> data;
    for (int i = 0; i < N; i++) data.push_back((S + 13 * i) % 100);

    cout << "Dataset   : ";
    for (int v : data) cout << v << " ";
    cout << "\n\n";

    for (int i = 0; i < N; i++) {
        cout << "--- Step " << i + 1 << " ---\n";
        insertKey(data[i]);
        display();
        cout << "\n";
    }

    // Duplicate-key test
    cout << "--- Duplicate key test ---\n";
    insertKey(data[0]);

    // Search test
    cout << "\n--- Search test ---\n";
    cout << data[5] << (searchKey(data[5]) ? " found" : " not found") << "\n";
    cout << "101 " << (searchKey(101) ? "found" : "not found") << "\n";

    return 0;
}