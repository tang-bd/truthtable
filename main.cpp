#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

class Node {
public:
    virtual bool eval(map<string, bool> &interpr) = 0;
};

class Atom: public Node {
public:
    string symbol;

    Atom(string _symbol): symbol(_symbol) {}

    bool eval(map<string, bool> &interpr) {
        auto value = interpr.find(symbol);
        if(value == interpr.end()) {
            throw "Value not provided";
        } else {
            return value->second;
        }
    }
};

class Neg: public Node {
public:
    Node *child;

    Neg(Node *_child): child(_child) {}

    ~Neg() {
        delete child;
    }

    bool eval(map<string, bool> &interpr) {
        return !child->eval(interpr);
    }
};

class And: public Node {
public:
    Node *lchild, *rchild;

    And(Node *_lchild, Node *_rchild): lchild(_lchild), rchild(_rchild) {}

    ~And() {
        delete lchild;
        delete rchild;
    }

    bool eval(map<string, bool> &interpr) {
        return lchild->eval(interpr) && rchild->eval(interpr);
    }
};

class Or: public Node {
public:
    Node *lchild, *rchild;

    Or(Node *_lchild, Node *_rchild): lchild(_lchild), rchild(_rchild) {}

    ~Or() {
        delete lchild;
        delete rchild;
    }

    bool eval(map<string, bool> &interpr) {
        return lchild->eval(interpr) || rchild->eval(interpr);
    }
};

class Xor: public Node {
public:
    Node *lchild, *rchild;

    Xor(Node *_lchild, Node *_rchild): lchild(_lchild), rchild(_rchild) {}

    ~Xor() {
        delete lchild;
        delete rchild;
    }

    bool eval(map<string, bool> &interpr) {
        return (lchild->eval(interpr) && !rchild->eval(interpr)) || 
            (rchild->eval(interpr) && !lchild->eval(interpr));
    }
};

class Imp: public Node {
public:
    Node *lchild, *rchild;

    Imp(Node *_lchild, Node *_rchild): lchild(_lchild), rchild(_rchild) {}

    ~Imp() {
        delete lchild;
        delete rchild;
    }

    bool eval(map<string, bool> &interpr) {
        return !lchild->eval(interpr) || rchild->eval(interpr);
    }
};

class BiImp: public Node {
public:
    Node *lchild, *rchild;

    BiImp(Node *_lchild, Node *_rchild): lchild(_lchild), rchild(_rchild) {}

    ~BiImp() {
        delete lchild;
        delete rchild;
    }

    bool eval(map<string, bool> &interpr) {
        return (lchild->eval(interpr) && rchild->eval(interpr)) || 
            (!rchild->eval(interpr) && !lchild->eval(interpr));
    }
};

class Ast {
public:
    Node *root;
    set<string> atoms;

    Ast(): root(nullptr) {}
    Ast(Node *_root, set<string> _atoms): root(_root), atoms(_atoms) {}

    bool eval(map<string, bool> &interpr) {
        if(root) {
            return root->eval(interpr);
        } else {
            throw "Evaluation of an empty tree";
        }
    }
};

class Parser {
private:
    string text;
    unsigned pos;

public:
    Parser(string _text): text(_text), pos(0) {}

    Atom* parse_atom() {
        unsigned begin = pos;
        while(pos < text.length() && text[pos] != '(' && text[pos] != ')' && text[pos] != '&' && text[pos] != '!'
            && text[pos] != '|' && text[pos] != '^' && text[pos] != '>' && text[pos] != '=' && text[pos] != ' ') {
            pos++;
        }
        string symbol = text.substr(begin, pos - begin);
        return new Atom(symbol);
    }

    Ast parse_expr() {
        Ast tree;
        while(pos < text.length()) {
            if(text[pos] == '(') {
                if(tree.root) {
                    throw pos + 1;
                }
                pos++;
                tree = parse_expr();
                while(pos < text.length() && text[pos] == ' ') {
                    pos++;
                }
                if(pos == text.length()) {
                    throw pos;
                }
                if(text[pos] != ')') {
                    throw pos + 1;
                } else {
                    pos++;
                }
            } else if (text[pos] == ')') {
                return tree;
            } else if (text[pos] == '!') {
                if(tree.root) {
                    throw pos + 1;
                }
                pos++;
                Ast child = parse_expr();
                tree.atoms.insert(child.atoms.begin(), child.atoms.end());
                return Ast(new Neg(child.root), tree.atoms);
            } else if (text[pos] == '&') {
                pos++;
                if(tree.root) {
                    Ast lchild = tree, rchild = parse_expr();
                    tree.atoms.insert(lchild.atoms.begin(), lchild.atoms.end());
                    tree.atoms.insert(rchild.atoms.begin(), rchild.atoms.end());
                    return Ast(new And(lchild.root, rchild.root), tree.atoms);
                } else {
                    throw pos + 1;
                }
            } else if (text[pos] == '|') {
                pos++;
                if(tree.root) {
                    Ast lchild = tree, rchild = parse_expr();
                    tree.atoms.insert(lchild.atoms.begin(), lchild.atoms.end());
                    tree.atoms.insert(rchild.atoms.begin(), rchild.atoms.end());
                    return Ast(new Or(lchild.root, rchild.root), tree.atoms);
                } else {
                    throw pos + 1;
                }
            } else if (text[pos] == '^') {
                pos++;
                if(tree.root) {
                    Ast lchild = tree, rchild = parse_expr();
                    tree.atoms.insert(lchild.atoms.begin(), lchild.atoms.end());
                    tree.atoms.insert(rchild.atoms.begin(), rchild.atoms.end());
                    return Ast(new Xor(lchild.root, rchild.root), tree.atoms);
                } else {
                    throw pos + 1;
                }
            } else if (text[pos] == '>') {
                pos++;
                if(tree.root) {
                    Ast lchild = tree, rchild = parse_expr();
                    tree.atoms.insert(lchild.atoms.begin(), lchild.atoms.end());
                    tree.atoms.insert(rchild.atoms.begin(), rchild.atoms.end());
                    return Ast(new Imp(lchild.root, rchild.root), tree.atoms);
                } else {
                    throw pos + 1;
                }
            } else if (text[pos] == '=') {
                pos++;
                if(tree.root) {
                    Ast lchild = tree, rchild = parse_expr();
                    tree.atoms.insert(lchild.atoms.begin(), lchild.atoms.end());
                    tree.atoms.insert(rchild.atoms.begin(), rchild.atoms.end());
                    return Ast(new BiImp(lchild.root, rchild.root), tree.atoms);
                } else {
                    throw pos + 1;
                }
            } else if (text[pos] == ' ') {
                pos++;
            } else {
                if(tree.root) {
                    throw pos + 1;
                }
                Atom *atom = parse_atom();
                tree.atoms.insert(atom->symbol);
                tree = Ast(atom, tree.atoms);
            }
        }
        return tree;
    }
};

void print_truth_table(Ast &tree) {
    for(int i = 0; i < pow(2, tree.atoms.size()); i++){
        map<string, bool> interpr;
        int j = 0;
        for(auto a : tree.atoms) {
            interpr[a] = (i >> j) % 2 == 1 ? true : false;
            cout << a << " : " << interpr[a] << endl;
            j++;
        }
        cout << "Result: " << tree.eval(interpr) << endl;
    }
}

int main() {
    string text;

    getline(cin, text);
    reverse(text.begin(), text.end());

    Ast tree;
    Parser parser(text);

    try {
        tree = parser.parse_expr();
        print_truth_table(tree);
    } catch(char const *s) {
        cerr << "Error: " << s << endl;
        return 1;
    } catch(unsigned pos) {
        cerr << "Error: Position " << pos << endl;
    }
    
    return 0;
}