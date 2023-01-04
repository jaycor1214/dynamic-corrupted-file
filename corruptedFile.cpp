// Jack Corley, CSCI163
#include <bits/stdc++.h>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;
//TRIE

const int ALPHABET_SIZE = 26;

struct TrieNode
{
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    int order;
};

struct TrieNode *getNode(void)
{
    struct TrieNode *pNode =  new TrieNode;
    pNode->isEndOfWord = false;
    pNode->order = -1;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
    return pNode;
}

void insert(struct TrieNode *root, string key, int orderInput = -1)
{
    struct TrieNode *pCrawl = root;
    for (int i = 0; i < key.length(); i++)
    {
        int index = key[i] - 'a';
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }
    pCrawl->isEndOfWord = true;
    pCrawl->order = orderInput;
}

bool search(struct TrieNode *root, string key)
{
    struct TrieNode *pCrawl = root;
    for (int i = 0; i < key.length(); i++)
    {
        int index = key[i] - 'a';
        if (!pCrawl->children[index])
            return false;

        pCrawl = pCrawl->children[index];
    }
    return (pCrawl->isEndOfWord);
}

int searchValue(struct TrieNode *root, string key)
{
    struct TrieNode *pCrawl = root;
    for (int i = 0; i < key.length(); i++)
    {
        int index = key[i] - 'a';
        if (!pCrawl->children[index])
            return false;

        pCrawl = pCrawl->children[index];
    }
    return 1 + (pCrawl->order);
}

bool isInWord(struct TrieNode *root, string key)
{
    struct TrieNode *pCrawl = root;
    for (int i = 0; i < key.length(); i++)
    {
        int index = key[i] - 'a';
        if (!pCrawl->children[index])
            return false;
        pCrawl = pCrawl->children[index];
    }

    return true;
}

string buildWord(string fileName){
  string corrupted;
  ifstream instream;
  instream.open(fileName);
  if(instream.fail()){
    cout << "failed to open file" << endl;
    exit(1);
  }
  string word;
  instream >> word;
    corrupted = word;

  instream.close();
  return corrupted;
}

//Build trie from file
vector<string> buildDictionary(string fileName){
  vector<string> dictionary;
  ifstream instream;
  instream.open(fileName);
  if(instream.fail()){
    cout << "failed to open file" << endl;
    exit(1);
  }

  string word;
  instream >> word;
  while(!instream.eof()){
    dictionary.push_back(word);
    instream >> word;
  }
  instream.close();
  return dictionary;
}

//MEAT AND POTATOES - FIND ALL VALID SENTANCES
void getStrings(string input, vector<string> & answer, struct TrieNode *dictionary, string prev = "", int pos = 0){
  while(true){
    prev += input[pos];
    pos++;
    // prev not in dictionary, abandon
    if(!isInWord(dictionary, prev)){return;}


    if(search(dictionary, prev)){
      if(input[pos] != '.' && input[pos] != ',' && input[pos] != '!' && input[pos] != '?'){
        getStrings(input, answer, dictionary, prev, pos);
      }

      prev = "";
      if(input[pos] == '.' || input[pos] == ',' || input[pos] == '!' || input[pos] == '?'){pos+=2;}
      else{pos++;}
      string temp1 = input.substr(0,pos-1);
      string temp2 = input.substr(pos-1, input.length());
      //whitespace
      input = temp1 + " " + temp2;
    }
    //exit condition
    if(input[pos] == '*'){
      if(prev == ""){
        input = input.substr(0, pos-4);
        answer.push_back(input);
      }
      return;
    }
  }
}

vector<int> getRank(struct TrieNode *dictionary, vector<string> sentances){
  vector<int> ranking;
  vector<string> toOrder;
  //Remove PERIOD
  for(size_t i = 0; i < sentances.size(); i++){
    string sentance = sentances[i];
    for(size_t j = 0; j < sentance.length(); j++){
      if(sentance[j] == '.' || sentance[j] == ',' || sentance[j] == '!' || sentance[j] == '?'){
        for(size_t k = j; k < sentance.length(); k++){
          sentance[k] = sentance[k+1];
        }
        sentance = sentance.substr(0,sentance.length());
      }
    }
    toOrder.push_back(sentance);
  }
  //rank
  for(size_t i = 0; i < toOrder.size(); i++){
    string sentance = toOrder[i];
    string temp;
    stringstream stream(sentance);
    int word = 0;
    int wordCount = 0;
      while (stream >> temp) {
        word += searchValue(dictionary,temp);
        wordCount++;
      }
      ranking.push_back(word/wordCount);
  }
  return ranking;
}

int main(){
struct TrieNode *root = getNode();

  // cout << "enter the filename of the corrupted file: ";
  // cin >> corrFile;
  // cout << "Enter the filename of the dictionary file: ";
  // cin >> dictFile;

  // THESE ARE WHERE YOU MUST PUT IN THE FILE NAMES DIRECTLY
  vector<string> dictionary = buildDictionary("shortened.txt"); // where corrFile should go
  string corruptedWord = buildWord("corrupted.txt"); // where dictFile should go.
  // AFTER THESE YOU ARE GOOD

  cout << "constructing trie..." << endl;
  for(int i = 0; i < dictionary.size(); i++){
    insert(root, dictionary[i], i);
  }

  vector<string> answer;
  //prepare string for function
  corruptedWord += "the";
  corruptedWord += '*';
  cout << "Building possible sentances..." << endl;
  getStrings(corruptedWord, answer, root);
  cout << "Ranking sentances..." << endl;
  vector<int> ranks = getRank(root, answer);
  cout << endl << "ANSWER: " << endl;
  for(size_t i = 0; i < answer.size(); i++){
    cout << i+1 << ". " << answer[i] << "| with average rank " << ranks[i] <<endl;
  }
  string output;
  cout << "If you wish to output the most likely sentance, type text file: ";
  cin >> output;

// Find the most likely sentance
  int bestSentance = 1410065407;
  int bestPos = 0;
  for(size_t i = 0; i < ranks.size(); i++){
    if(ranks[i] < bestSentance){bestPos = i; bestSentance = ranks[i];}
  }

  // output winning sentance to file
  ofstream outstream;
  outstream.open(output);
  if(outstream.fail()){
    cout << "failed to open file" << endl;
    exit(1);
  }else{
    outstream << answer[bestPos];
  }
return 1;
}
