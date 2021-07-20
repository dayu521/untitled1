#include<queue>
#include<stack>
#include<iostream>
#include<tuple>

#include"doctest/doctest.h"

template<typename T,typename V>
class BST
{
public:
    struct Node
    {
        Node * left_=nullptr;
        Node * right_=nullptr;
        T key_;
        V value_;
    };

public:
    BST()
    {
        root_=null_=new Node{};
        null_->left_=null_->right_=null_;
    }
    ~BST(){release();}

    Node * insert(const T & key,V v);
    bool remove(const T & key);
    Node * & find(const T & key);
    Node * & find_min(Node * &root)
    {
        auto * it=&root;
        while ((*it)->left_!=null_) {
            it=&((*it)->left_);
        }
        return *it;
    }
    Node * & find_max(Node * &root)
    {
        auto * it=&root;
        while ((*it)->right_!=null_) {
            it=&((*it)->right_);
        }
        return *it;
    }
    const Node * get_null()const
    {
        return null_;
    }
    void zhong();
    void xian();
    void hou();
    void di_zhong(){zhong(root_);}
    void di_xian(){xian(root_);}
    void di_hou(){hou(root_);}
private:
//    bool insert(Node * & root, std::pair<const T &,V &> pair);
//    bool remove(Node * & root, const T & key);
    void zhong(Node * root);
    void xian(Node * root);
    void hou(Node * root);
    void release();
private:
    Node * root_=nullptr;
    Node * null_=nullptr;
};

template<typename T, typename V>
typename BST<T, V>::Node * BST<T, V>::insert(const T &key, V v)
{
    auto & target_node=find(key);
    if(target_node!=null_)
        return null_;
    target_node=new Node{null_,null_,key,std::move(v)};
    return target_node;
}

template<typename T, typename V>
bool BST<T, V>::remove(const T &key)
{
    auto & target_node=find(key);
    if(target_node==null_)
        return false;
    if(target_node->left_!=null_&&target_node->right_!=null_){
        auto & min=find_min(target_node->right_);
        auto old=target_node;
        target_node=min;
        min=min->right_;
        target_node->left_=old->left_;
        target_node->right_=old->right_;
        delete old;
        return true;
    }else{
        auto old=target_node;
        if(target_node->left_==null_){
            target_node=target_node->right_;
        }else
            target_node=target_node->left_;
        delete old;
        return true;
    }
}

template<typename T, typename V>
void BST<T, V>::release()
{
    std::queue<Node *> q{};
    if(root_!=null_)
        q.push(root_);
    while (q.size()>0) {
        auto c=q.front();
        q.pop();
        if(c->left_!=null_){
            q.push(c->left_);
        }
        if(c->right_!=null_){
            q.push(c->right_);
        }
        delete c;
    }
    delete null_;
    root_=null_=nullptr;
}

template<typename T, typename V>
typename BST<T,V>::Node * & BST<T, V>::find(const T &key)
{
    auto * iterator=&root_;
    while (*iterator!=null_) {
        if(key>(*iterator)->key_){
            iterator=&((*iterator)->right_);
        }else if(key<(*iterator)->key_){
            iterator=&((*iterator)->left_);
        }else
            break;
    }
    return *iterator;
}

std::vector<std::tuple<int,int>> di_v{};
std::vector<std::tuple<int,int>> v{};

void fun(const auto & key,const auto & value,std::vector<std::tuple<int,int>> & v)
{
    v.push_back({key,value});
}

template<typename T, typename V>
void BST<T, V>::zhong()
{
    std::stack<Node *> st{};
    st.push(root_);

    while(true){
        auto c=st.top();
        while (c!=null_) {
            st.push(c->left_);
            c=st.top();
        }
        st.pop();
        if(st.empty())
            break ;
        c=st.top();
        std::cout<<c->key_<<':'<<c->value_<<std::endl;
        st.pop();
        st.push(c->right_);
    }
}

template<typename T, typename V>
void BST<T, V>::xian()
{
    std::stack<Node *> st{};
    st.push(root_);

    while(true){
        auto c=st.top();
        while (c!=null_) {
            std::cout<<c->key_<<':'<<c->value_<<std::endl;
            st.push(c->left_);
            c=st.top();
        }
        st.pop();
        if(st.empty())
            break ;
        c=st.top();
        st.pop();
        st.push(c->right_);
    }
}

template<typename T, typename V>
void BST<T, V>::hou()
{
    std::stack<Node *> st{};
    st.push(root_);

    while(true){
        auto c=st.top();
        while (c!=null_) {
            st.push(c->left_);
            c=st.top();
        }
        st.pop();
        if(st.empty())
            break ;
        c=st.top();

        if(c==null_){
            st.pop();
            c=st.top();
            st.top()=null_;
            std::cout<<c->key_<<':'<<c->value_<<std::endl;
            fun(c->key_,c->value_,v);
        }else{
            st.push(null_);
            st.push(c->right_);
        }
    }
}

template<typename T, typename V>
void BST<T, V>::zhong(Node *root)
{
    if(root!=null_){
        zhong(root->left_);
        std::cout<<root->key_<<':'<<root->value_<<std::endl;
        zhong(root->right_);
    }
}

template<typename T, typename V>
void BST<T, V>::xian(Node *root)
{
    if(root!=null_){
        std::cout<<root->key_<<':'<<root->value_<<std::endl;
        xian(root->left_);
        xian(root->right_);
    }
}

template<typename T, typename V>
void BST<T, V>::hou(Node *root)
{
    if(root!=null_){
        hou(root->left_);
        hou(root->right_);
        std::cout<<root->key_<<':'<<root->value_<<std::endl;
        fun(root->key_,root->value_,di_v);
    }
}

TEST_CASE("bst遍历")
{
    BST<int,int> t{};
//    const auto &&keys={1,2,3,8,574,89};
    const auto &&keys={1,2,6,55,13,9,3,8,574,89};
    for(auto i: keys){
        t.insert(i,i);
    }
    MESSAGE("=======中序遍历========");
    t.zhong();
    MESSAGE("========先序遍历=======");
    t.xian();
    MESSAGE("========后序遍历=======");
    t.hou();
    MESSAGE("========递归后序遍历=======");
    t.di_hou();
    CHECK(v==di_v);
}

#include<fstream>
TEST_CASE("dsad")
{
    std::ofstream f("1.txt");
    if(!f.is_open()){
        CHECK_FALSE("文件打开错误");
    }
    f<<"a"<<'\r'<<"b";
}
