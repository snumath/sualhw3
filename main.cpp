/*
 * C++ Program to Construct an Expression Tree for a Given Prefix Expression
 */
#include <iostream>
#include <cstdlib>
#include <cstdio>     
#include <cstring>
#include <cmath>
#include <stack>
#include <cassert>
#include <vector>
using namespace std;

// I Failed to print expression tree of differentiation
// Changing diffTree = 1 would make my codes to implement the differentiation run, however, it does not run
bool diffTree = 1;


stack<double>* SV;
stack<double>* SD;

#if diffTree
stack<Node*>* SD;
#endif


class Node{
    public:

    Node(){
    }

    virtual ~Node(){
    };

	virtual void eval_function() = 0;
    virtual void diff_function(Node* x) = 0;
    virtual void show() = 0;
    #if diffTree
    virtual void diff(Node* x) = 0;
    #endif
};

class CNode : public Node{
    public:
    double val;

    CNode(double val = NAN) : val(val){
    }

    void eval_function(){
        SV->push(val);
    }

    void diff_function(Node* x){
        SV->push(val);
        SD->push(0);
    }

    void show(){
        cout << val;
    }

    #if diffTree
    void diff(Node* x){
        CNode* re = new CNode(0);
        SN->push(re);
    }
    #endif
};

class VNode : public Node{
    // This node contains a variable value
    public:
    double val;
    string name;

    VNode(double val = NAN, string name = "") : val(val), name(name){
    }

    VNode(string name = "") : Node(), name(name){
    }

    void eval_function(){
    	SV->push(val);
    }

    void diff_function(Node *x){
        SV->push(val);
        if (this == x)
            SD->push(1);
        else
            SD->push(0);
    }

    void show(){
        cout << name;
    }

    #if diffTree
    void diff(Node* x){
        CNode* re;
        if (this == x)
            re = newCNode(1);
        else
            re = new CNode(0);
        SN->push(re);
    }
    #endif
};


class ONode : public Node{
    // This node contains a operation
    public:
    char op;
    Node* left;
    Node *right;

    ONode(char op, Node* left, Node* right) : op(op), left(left), right(right){
    }

    void eval_function(){
        assert(left!=NULL && right!=NULL);
	    left->eval_function();
	    right->eval_function();
	    this->calc_eval_function();
    }

    void calc_eval_function()
    {
    	double x = NAN;
    	double rx = SV->top();
        SV->pop();
    	double lx = SV->top();
        SV->pop();
    	switch (op)
    	{
    	case '+':
    		x = lx + rx;
		    break;
    	case '-':
		    x = lx - rx;
    		break;
	    case '*':
    		x = lx * rx;
		    break;
	    case '/':
    		x = lx / rx;
		    break;
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }
	    SV->push(x);
    }

    void diff_function(Node* x){
        assert(left!=NULL && right!=NULL);
	    left->diff_function(x);
	    right->diff_function(x);
	    this->calc_diff_function(x);
    }

    void calc_diff_function(Node* x){
    	double val = NAN;
    	double rx = SV->top();
        SV->pop();
    	double lx = SV->top();
        SV->pop();

        double dval = NAN;
        double drx = SD->top();
        SD->pop();
        double dlx = SD->top();
        SD->pop();

    	switch (op)
    	{
    	case '+':{
    		val = lx + rx;
            dval = dlx + drx;
		    break;
        }
    	case '-':{
		    val = lx - rx;
            dval = dlx - drx;
    		break;
        }
	    case '*':{
    		val = lx * rx;
            dval = lx * drx + dlx * rx;
		    break;
        }
	    case '/':{
    		val = lx / rx;
            dval = dlx/rx - lx*drx/rx/rx;
		    break;
        }
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }
	    SV->push(val);
        SD->push(dval);
    }



    void show()
    {
        cout << '(';
        left->show();
    	switch (op)
    	{
    	case '+':
            cout << '+';
		    break;
    	case '-':
		    cout << '-';
    		break;
	    case '*':
    		cout << '*';
		    break;
	    case '/':
    		cout << '/';
		    break;
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }
        right->show();
        cout << ')';
    }
    

    #if diffTree
    void diff(Node* x){
        left->diff();
        right->diff();

        Node* ln = SN->top();
        SN->pop();
        Node* rn = SN->top();
        SN->pop();

        ONode* re;

        switch (op)
    	{
    	case '+':
            re = new ONode('+', ln, rn);
    		break;
    	case '-':
            re = new ONode('-', ln, rn);
    		break;
	    case '*':{
            ONode* lo = new ONode('*', ln, right);
            ONode* ro = new ONode('*', left, rn);
            re = new ONode('+', lo, ro);
		    break;
        }
	    case '/':{
            ONode* lo = new ONode('/', ln, right);
            ONode* ro = new ONode('*', right, right);
            ONode* ro2 = new ONode('/', left, ro);
            ONode* ro3 = new ONode('*', ro2, rn);
            re = new ONode('-', lo, ro3);
		    break;
        }
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }

        SD->push(re);
    }
    #endif
};



class PNode : public Node{
    // This node is to implement power-n
    public:
    int power;
    Node* left;

    PNode(int _power, Node* left) : power(_power), left(left){
    }

    void eval_function(){
        assert(left!=NULL);
	    left->eval_function();
        this->calc_eval_function();
    }

    void calc_eval_function(){
        double x;
    	double lx = SV->top();
        SV->pop();
        x = pow(lx,power);

        SV->push(x);
    }
    void diff_function(Node* x){
        assert(left!=NULL);
	    left->diff_function(x);
        this->calc_diff_function(x);
    }

    void calc_diff_function(Node* x){
        double val, dval;
    	double lx = SV->top();
        SV->pop();
        val = pow(lx,power);
        SV->push(val);

        double dlx = SD->top();
        SD->pop();
        dval = power * pow(lx,power-1) * dlx;
        SD->push(dval);
    }

    void show(){
        left->show();
        cout << "^" << power;
    }


    #if diffTree
    void diff(Node* x){
        // Assume power is not zero, one
        left->diff();
        PNode* ln = new PNode(power - 1, left);
        Node* rn = SN->top();
        SN->pop();
        ONode* re = new ONode('*', ln, rn);
        SN->push(re);
    }
    #endif

};


class UNode : public Node{
    // This node is to implement unary node
    public:
    char op;
    Node* left;

    UNode(char op, Node* left) : op(op), left(left){
    }

    void eval_function(){
        assert(left!=NULL);
	    left->eval_function();
        this->calc_eval_function();
    }

    void calc_eval_function(){
        double x;
    	double lx = SV->top();
        SV->pop();
        switch (op)
    	{
    	case 'c':
		    x = cos(lx);
		    break;
    	case 's':
		    x = sin(lx);
    		break;
	    case 't':
    		x = tan(lx);
		    break;
	    case '-':
    		x = -lx;
		    break;
        case 'l':
            x = log(lx);
            break;
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }

        SV->push(x);
    }

    void diff_function(Node* x){
        assert(left!=NULL);
	    left->diff_function(x);
        this->calc_diff_function(x);
    }

    void calc_diff_function(Node* x){
        double val, dval;
    	double lx = SV->top();
        double dlx = SD->top();
        SV->pop();
        SD->pop();

        switch (op)
    	{
    	case 'c':{
		    val = cos(lx);
            dval = -sin(lx)*dlx;
		    break;
        }
    	case 's':{
		    val = sin(lx);
            dval = cos(lx)*dlx;
    		break;
        }
	    case 't':{
    		val = tan(lx);
            dval = dlx/cos(lx)/cos(lx);
		    break;
        }
	    case '-':{
            val = -lx;
    		dval = -dlx;
		    break;
        }
        case 'l':{
            val = log(lx);
            dval = dlx/lx;
            break;
        }
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }

        SV->push(val);
        SD->push(dval);
    }

    void show()
    {
    	switch (op)
    	{
    	case 'c':
            cout << "cos(";
		    break;
    	case 's':
		    cout << "sin(";
    		break;
	    case 't':
    		cout << "tan(";
		    break;
	    case '-':
    		cout << "-(";
		    break;
	    case 'l':
    		cout << "log(";
		    break;
	    default:
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
	    }
        left->show();
        cout << ')';
    }


    #if diffTree
    void diff(Node* x){
        left->diff();
        Node* ln = SN->top();
        SN->pop();
        switch (op)
    	{
    	case 'c':{
            Node* ls =  new UNode('s', ln);
            UNode* re = new UNode('-', ls);
		    break;
        }
    	case 's':
		    UNode* re = new UNode('c', ln);
    		break;
	    case 't':{
            UNode* ls = new UNode('c', ln);
            PNode* re = PNode(-2, ls);
		    break;
        }
	    case '-':
            UNode* re = new UNode('-', ln);
		    break;
	    case 'l':
            PNode* re = new PNode(-1, ln);
		    break;
	    default:{
    		cerr<<"op["<<op<<"] not yet implemented!!"<<endl;
		    assert(false);
		    break;
        }
	    }

        SN->push(re);
    }
    #endif
};

void say(string yeah = "Hello, World!"){
    cout << yeah << endl;
}


double eval_function(Node* root)
{
    assert(SD->size()==0);
	assert(SV->size()==0);
	root->eval_function();

	assert(SV->size()==1);
	double val = SV->top();
    SV->pop();
	return val;
};

double diff_function(Node* root, Node* x)
{
    assert(SD->size()==0);
	assert(SV->size()==0);
	root->diff_function(x);

    assert(SD->size()==1);
	double dval = SD->top();
    SV->pop();
    SD->pop();
	return dval;
};

void grad_function(Node* root, vector<VNode*>* list, vector<double>* grad){
    grad->clear();
    double temp;

    for (auto iter = list->begin(); iter != list->end(); ++iter){
        temp = diff_function(root, *iter);
        grad->push_back(temp);
    }
}

void update_system(vector<Node*> list, vector<double>* dx, vector<double>* grad){
    dx->clear();
    /* Set relations manually*/
    double t1 = grad->at(0);
    double t2 = grad->at(1);

    dx->push_back(t1);
    dx->push_back(t2);
}

void taylor(){

}


#if diffTree
Node* diff(Node* root)
{
    assert(SN->size()==0);
	root->diff();

	assert(SN->size()==1);
	auto re = SN->top();
    SN->pop();
	return re;
};
#endif


void show(Node* root){
    root->show();
    cout << endl;
}

void showVar(vector<VNode*> list){
    for (auto i : list){
        cout << "'" << i->name << "' = " << i->val << ", ";
    }
    cout << endl;
}


int main(){

    SV = new stack<double>;
    SD = new stack<double>;

    vector<VNode*> list;
    VNode* x1 = new VNode("x1");
    
    VNode* x2 = new VNode("x2");
    VNode* x3 = new VNode("x3");
    VNode* x4 = new VNode("x4");

    ONode* op1 = new ONode('+', x1, x2);
    ONode* op2 = new ONode('*', op1, x3);
    UNode* op3 = new UNode('l', x3);
    ONode* op4 = new ONode('*', op3, x4);
    PNode* op5 = new PNode(2, op4);
    ONode* op6 = new ONode('+', op2, op5);

    list.push_back(x1);
    list.push_back(x2);
    list.push_back(x3);
    list.push_back(x4);

    // (x1*x2)*x3 + (log(x3)*x4)^2

    x1->val = -1.9;
	x2->val = 2;
	x3->val = 10;
    x4->val = 15;

    show(op6);
    showVar(list);

    vector<double> grad;
    grad_function(op6, &list, &grad);

    cout << "Evaluation Value = " << eval_function(op6) << endl;

    cout << "Gradient Value = (";
    for (auto iter = grad.begin(); iter != prev(grad.end()); ++iter)
        cout << *iter << ", ";
    cout << grad.back() << ")" << endl;




    cout << endl << endl << endl << "Problem 4: print" << endl;
    list.clear();

    VNode* x = new VNode("x");
    VNode* y = new VNode("y");

    PNode* p1 = new PNode(2,x);
    PNode* p2 = new PNode(3,y);
    PNode* p3 = new PNode(2,y);

    ONode* a1 = new ONode('+', p1, p2);
    ONode* a2 = new ONode('-', a1, p3);

    list.push_back(x);
    list.push_back(y);
    x->val = 0.0;
    y->val = 1.0;

    show(a2);
    showVar(list);
    cout << "Evaluation Value = " << eval_function(a2) << endl;

    grad_function(a2, &list, &grad);
    cout << "Gradient Value = (";
    for (auto iter = grad.begin(); iter != prev(grad.end()); ++iter)
        cout << *iter << ", ";
    cout << grad.back() << ")" << endl;




    return 0;
}
