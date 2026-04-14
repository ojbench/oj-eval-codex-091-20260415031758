#include <bits/stdc++.h>
using namespace std;

// We implement an ESet-like container for long long using a Treap (randomized BST)
// supporting: insert unique, erase, find, lower_bound, upper_bound, iterator via inorder.
// Additionally we implement order-statistics to answer range count [l, r].

struct Treap {
    struct Node {
        long long key;
        uint32_t pr;
        int sz;
        Node *l, *r, *p;
        Node(long long k, uint32_t pr): key(k), pr(pr), sz(1), l(nullptr), r(nullptr), p(nullptr) {}
    };
    Node* root = nullptr;
    mt19937 rng;

    Treap(): rng(123456789) {}

    ~Treap(){ clear(root); root = nullptr; }

    Treap(const Treap& other): rng(123456789) { root = clone(other.root, nullptr); }
    Treap& operator=(const Treap& other){
        if(this==&other) return *this;
        clear(root); root = clone(other.root, nullptr);
        return *this;
    }
    Treap(Treap&& other) noexcept: root(other.root), rng(123456789) { other.root = nullptr; }
    Treap& operator=(Treap&& other) noexcept{
        if(this==&other) return *this;
        clear(root); root = other.root; other.root = nullptr; return *this;
    }

    static int getsz(Node* x){ return x? x->sz:0; }
    static void pull(Node* x){ if(x){ x->sz = 1 + getsz(x->l) + getsz(x->r); if(x->l) x->l->p = x; if(x->r) x->r->p = x; } }
    
    static void clear(Node* x){ if(!x) return; clear(x->l); clear(x->r); delete x; }
    
    static Node* clone(Node* x, Node* parent){
        if(!x) return nullptr;
        Node* y = new Node(x->key, x->pr);
        y->l = clone(x->l, y);
        y->r = clone(x->r, y);
        y->p = parent; pull(y);
        return y;
    }

    static Node* rotate_right(Node* y){
        Node* x = y->l; Node* T2 = x->r;
        x->r = y; y->l = T2;
        if(T2) T2->p = y;
        x->p = y->p; y->p = x;
        pull(y); pull(x);
        return x;
    }
    static Node* rotate_left(Node* x){
        Node* y = x->r; Node* T2 = y->l;
        y->l = x; x->r = T2;
        if(T2) T2->p = x;
        y->p = x->p; x->p = y;
        pull(x); pull(y);
        return y;
    }

    Node* _insert(Node* root, Node* node){
        if(!root) return node;
        if(node->key < root->key){
            root->l = _insert(root->l, node);
            root->l->p = root;
            if(root->l->pr < root->pr) root = rotate_right(root);
        }else{
            root->r = _insert(root->r, node);
            root->r->p = root;
            if(root->r->pr < root->pr) root = rotate_left(root);
        }
        pull(root);
        return root;
    }

    bool insert(long long k){
        Node* cur = root; Node* par = nullptr;
        while(cur){
            if(k == cur->key) return false;
            par = cur;
            cur = (k < par->key) ? par->l : par->r;
        }
        Node* n = new Node(k, rng());
        root = _insert(root, n);
        root->p = nullptr;
        return true;
    }

    Node* _erase(Node* root, long long k, bool& removed){
        if(!root) return nullptr;
        if(k < root->key){
            root->l = _erase(root->l, k, removed);
            if(root->l) root->l->p = root;
        }else if(k > root->key){
            root->r = _erase(root->r, k, removed);
            if(root->r) root->r->p = root;
        }else{
            removed = true;
            if(!root->l){
                Node* r = root->r; if(r) r->p = root->p; delete root; return r;
            }else if(!root->r){
                Node* l = root->l; if(l) l->p = root->p; delete root; return l;
            }else{
                if(root->l->pr < root->r->pr){
                    root = rotate_right(root);
                    root->r = _erase(root->r, k, removed);
                    if(root->r) root->r->p = root;
                }else{
                    root = rotate_left(root);
                    root->l = _erase(root->l, k, removed);
                    if(root->l) root->l->p = root;
                }
            }
        }
        pull(root);
        return root;
    }

    bool erase(long long k){ bool rem=false; root = _erase(root, k, rem); if(root) root->p=nullptr; return rem; }

    Treap::Node* lower_bound(long long k) const{
        Node* cur = root; Node* ans = nullptr;
        while(cur){
            if(cur->key >= k){ ans = cur; cur = cur->l; }
            else cur = cur->r;
        }
        return ans;
    }
    Treap::Node* upper_bound(long long k) const{
        Node* cur = root; Node* ans = nullptr;
        while(cur){
            if(cur->key > k){ ans = cur; cur = cur->l; }
            else cur = cur->r;
        }
        return ans;
    }
    Treap::Node* find(long long k) const{
        Node* cur = root;
        while(cur){
            if(k == cur->key) return cur;
            cur = (k < cur->key) ? cur->l : cur->r;
        }
        return nullptr;
    }

    static Node* min_node(Node* x){ if(!x) return nullptr; while(x->l) x=x->l; return x; }
    static Node* max_node(Node* x){ if(!x) return nullptr; while(x->r) x=x->r; return x; }

    static Node* next(Node* x){
        if(!x) return nullptr;
        if(x->r){ x=x->r; while(x->l) x=x->l; return x; }
        Node* p=x->p; while(p && p->r==x){ x=p; p=p->p; } return p;
    }
    static Node* prev(Node* x){
        if(!x) return nullptr;
        if(x->l){ x=x->l; while(x->r) x=x->r; return x; }
        Node* p=x->p; while(p && p->l==x){ x=p; p=p->p; } return p;
    }

    int count_leq(long long k) const{
        int cnt=0; Node* cur=root;
        while(cur){
            if(cur->key<=k){ cnt += 1 + getsz(cur->l); cur = cur->r; }
            else cur = cur->l;
        }
        return cnt;
    }

    int size() const { return getsz(root); }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // There are multiple ESet instances indexed by id, following the sample harness.
    // The operations are:
    // 0 a b: emplace b into set a
    // 1 a b: erase b from set a
    // 2 a: copy construct set[++lst] = set[a]
    // 3 a b: find b in set a -> print true/false, and set iterator state
    // 4 a l r: print count in [l, r] for set a
    // 5: iterator -- and print or -1
    // 6: iterator ++ and print or -1

    const int MAXS = 200000; // ample for tests; uses dynamic treaps per set
    vector<Treap> sets(25); // as per sample uses s[25]

    // iterator state across operations
    Treap::Node* it = nullptr; int it_a = -1; bool valid = false;

    int op; int lst=0; // track copies as in sample
    while ( (cin >> op) ){
        long long a,b,c;
        switch(op){
            case 0: // emplace
                cin >> a >> b;
                if(a<0 || a>= (long long)sets.size()) break;
                if(sets[a].insert(b)){
                    it = sets[a].find(b);
                    it_a = (int)a; valid = true;
                }
                break;
            case 1: // erase
                cin >> a >> b;
                if(valid && it_a==(int)a && it && it->key==b) valid=false;
                if(a>=0 && a< (long long)sets.size()) sets[a].erase(b);
                break;
            case 2: // copy
                cin >> a;
                if(++lst >= (int)sets.size()) sets.resize(max((int)sets.size()*2, lst+1));
                sets[lst] = sets[a]; // deep copy via Treap::operator=
                break;
            case 3: // find
                cin >> a >> b;
                if(a<0 || a>= (long long)sets.size()){ cout << "false\n"; break; }
                {
                    Treap::Node* res = sets[a].find(b);
                    if(res){ cout << "true\n"; it = res; it_a = (int)a; valid = true; }
                    else { cout << "false\n"; }
                }
                break;
            case 4: // range count
                cin >> a >> b >> c;
                if(a<0 || a>= (long long)sets.size()){ cout << 0 << "\n"; break; }
                if(b>c){ cout << 0 << "\n"; break; }
                {
                    int cnt = sets[a].count_leq(c) - sets[a].count_leq(b-1);
                    cout << cnt << "\n";
                }
                break;
            case 5: // iterator -- and print
                if(valid && it){
                    Treap::Node* prv = Treap::prev(it);
                    if(prv){ it = prv; cout << it->key << "\n"; }
                    else { valid=false; cout << -1 << "\n"; }
                }else{ cout << -1 << "\n"; }
                break;
            case 6: // iterator ++ and print
                if(valid && it){
                    Treap::Node* nx = Treap::next(it);
                    if(nx){ it = nx; cout << it->key << "\n"; }
                    else { valid=false; cout << -1 << "\n"; }
                }else{ cout << -1 << "\n"; }
                break;
            default:
                // ignore unknown
                break;
        }
    }

    return 0;
}
