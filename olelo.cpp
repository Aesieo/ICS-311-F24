#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <typeinfo>

struct Node{
   int data;
   std::string olelo;
   std::string english;
   std::string explanation;
   std::string color;
   Node *left, *right, *parent;

   Node(int data, std::string olelo, std::string english, std::string explanation):
      data(data),
      olelo(olelo),
      english(english),
      explanation(explanation),
      color("RED"),
      left(NULL),
      right(NULL),
      parent(NULL)
      {
      }
};

class RedBlackTree {
private:
   Node* root;
   Node* NIL;
   std::unordered_map<std::string, std::vector<std::string>> hashMap;  
   std::unordered_map<std::string, std::vector<std::string>> hashMapE;  

   void leftRotate(Node* x){
      Node* y = x->right;
      x->right = y->left;

      if(y->left != NIL){
         y->left->parent = x;
      }

      y->parent = x->parent;

      if (x->parent == NULL){
         root = y;
      }

      else if (x == x->parent->left){
         x->parent->left = y;
      }  

      else{
         x->parent->right = y; 
      }
      y->left = x;
      x->parent = y;
   }

   void rightRotate(Node* x){
      Node* y = x->left;
      x->left = y->right;

      if(y->right != NIL){
         y->right->parent = x;
      }

      y->parent = x->parent;

      if(x->parent == NULL){
         root = y;
      }

      else if(x == x->parent->right){
         x->parent->right = y;
      }  

      else{
         x->parent->left = y; 
      }
      y->right = x;
      x->parent = y;
   }

   void insertFixup(Node* z){
      while(z != root && z->parent->color == "RED"){
         if(z->parent == z->parent->parent->left){
            Node* u = z->parent->parent->right;
            if(u->color == "RED"){
               z->parent->color = "BLACK";
               u->color = "BLACK";
               z->parent->parent->color = "RED";
               z = z->parent->parent;
            }

            else{
               if(z == z->parent->right){
                  z = z->parent;
                  leftRotate(z);
               }
               z->parent->color = "BLACK";
               z->parent->parent->color = "RED";
               rightRotate(z->parent->parent);
            }
         }
         else{
            Node* u = z->parent->parent->left;
            if(u->color == "RED"){
               z->parent->color = "BLACK";
               u->color = "BLACK";
               z->parent->parent->color = "RED";
               z = z->parent->parent;
            } 
            
            else{
               if(z == z->parent->left){
                  z = z->parent;
                  rightRotate(z);
               }
               z->parent->color = "BLACK";
               z->parent->parent->color = "RED";
               leftRotate(z->parent->parent);
            }
         }       
      }
      root->color = "BLACK";
   }

   void inorderHelper(Node* node){
      if(node != NIL){
         inorderHelper(node->left);
         std::cout << node->olelo << std::endl;
         inorderHelper(node->right);
      }
   }      

   void treeMinimum(Node* node){
      while(node->left != NIL){
         node = node->left;
      }
      std::cout << node->olelo << std::endl;
   }

   void treeMaximum(Node* node){
      while(node->right != NIL){
         node = node->right;
      }
      std::cout << node->olelo << std::endl;
   }
   
   Node* searchHelper(Node* node, std::string olelo){
      while(node != NIL && olelo != node->olelo){
         if(olelo < node->olelo){
            node = node->left;
         }
         else{
            node = node->right;
         }
      }
      return node;
   }

public:
   RedBlackTree(){
      NIL = new Node(0, "", "", "");
      NIL->color = "BLACK";
      NIL->left = NIL->right = NIL;
      root = NIL;
   }

   void insert(int data, std::string olelo, std::string english, std::string explanation){
      Node* new_node = new Node(data, olelo, english, explanation);
      new_node->left = NIL;
      new_node->right = NIL;
      
      Node* parent = NULL;
      Node* current = root;

      std::istringstream iss(new_node->olelo);

      while(iss){
         std::string word;
         iss >> word;
         if(hashMap.count(word) > 0){
            hashMap.at(word).push_back(new_node->olelo);
         }

         else{
            hashMap[word] = {};
            hashMap.at(word).push_back(new_node->olelo);
         }
         hashMap.at(word).erase(std::unique(hashMap.at(word).begin(), hashMap.at(word).end()), hashMap.at(word).end());
      }

 
      std::istringstream ess(new_node->english);
 
      while(ess){
         std::string wordE;
         ess >> wordE;
         if(hashMapE.count(wordE) > 0){
            hashMapE.at(wordE).push_back(new_node->english);
         }

         else{
            hashMapE[wordE] = {};
            hashMapE.at(wordE).push_back(new_node->english);
         }

         hashMapE.at(wordE).erase(std::unique(hashMapE.at(wordE).begin(), hashMapE.at(wordE).end()), hashMapE.at(wordE).end());
      }

      while(current != NIL){
         parent = current;
         if(new_node->data < current->data){
            current = current->left;
         }
         else{
            current = current->right;
         }
      }
     
      new_node->parent = parent;
      
      if(parent == NULL){
         root = new_node;
      }
      
      else if(new_node->data < parent->data){
         parent->left = new_node;
      }

      else{
         parent->right = new_node;
      }
      
      if(new_node->parent == NULL){
         new_node->color = "BLACK";
         return;
      }
      
      if(new_node->parent->parent == NULL){
         return;
      }
     
      insertFixup(new_node);
   }
   
   void inorder(){ 
      inorderHelper(root);
   }
   
   void first(){
      treeMinimum(root);
   }
   
   void last(){
      treeMaximum(root);
   }

   void member(std::string olelo){
      if(searchHelper(root, olelo)->olelo != ""){
         std::cout << olelo << " is a member." << std::endl;
      }
      else{
         std::cout << olelo << " is not a member." << std::endl;
      }
   }

   void successor(std::string olelo){
      if(searchHelper(root, olelo)->olelo != ""){
         Node* p = searchHelper(root, olelo);
         if(p->right != NIL){
            treeMinimum(p->right);
         }
         else{
            Node* u = p->parent;
            while(u != NIL && p == u->right){
               p = u;
               u = u->parent;
            }
            std::cout << u->olelo << std::endl;
         }
      }
      else{
         std::cout << olelo << " is not a member, so it has no successors." << std::endl;
      }
   }

   void predecessor(std::string olelo){
      if(searchHelper(root, olelo)->olelo != ""){
         Node* p = searchHelper(root, olelo);
         if(p->left != NIL){
            treeMinimum(p->left);
         }
         else{
            Node* u = p->parent;
            while(u != NIL && p == u->left){
               p = u;
               u = u->parent;
            }
            std::cout << u->olelo << std::endl;
         }
      }
      else{
         std::cout << olelo << " is not a member, so it has no predecessors." << std::endl;
      }
   }
   void meHua(std::string oleloWord){
      std::cout << "Sayings containing \"" << oleloWord << "\": " << std::endl;
      for (auto x: hashMap.at(oleloWord)){
         std::cout << x << std::endl;
      }
   }
   
   void withWord(std::string englishWord){
      std::cout << "English translations containing \"" << englishWord << "\":" << std::endl;
      for (auto x: hashMapE.at(englishWord)){
         std::cout << x << std::endl;
      }   
   }
};

int main(){
   std::ifstream sayings("/home/u/uyr/ICS311/olelo.txt");
   std::string saying;
   std::vector<std::string> olelo_list;
   std::vector<std::string> english_list;
   std::vector<std::string> e_explain_list;
   std::string  stringType;   
   std::string baseString = "olelo";

   RedBlackTree rt;

   while (getline (sayings, saying)) {
      stringType = saying[0];
      
      if (stringType.compare("H") == 0){
         olelo_list.push_back(saying.substr(2, saying.length() - 1));
      }
      else if (stringType.compare("E") == 0){
         english_list.push_back(saying.substr(2, saying.length() - 1));
      }
      else{
         e_explain_list.push_back(saying.substr(2, saying.length() -1));
      }    
   }

   
   for(size_t i = 0; i < olelo_list.size(); i++){
      rt.insert(olelo_list[i].compare(baseString), olelo_list[i], english_list[i], e_explain_list[i]);
   }
 
   std::string testString = "Malama";   

   rt.insert(testString.compare(baseString), testString,  "", "");
   rt.inorder();
   //rt.first();
   //rt.member("Malama");
   //rt.member("Aloha");
   //rt.successor("Malama");
   //rt.predecessor("Malama");
   //rt.successor("Aloha");
   //rt.predecessor("Aloha");
   //rt.meHua("ka");
   //rt.withWord("the");
   return 0;
}