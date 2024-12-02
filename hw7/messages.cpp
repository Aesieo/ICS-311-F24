#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <sstream>

class Graph {
private:
    std::unordered_map<std::string, std::vector<std::string>> adjList;

public:
    void addUser(const std::string& user){
        if (adjList.find(user) == adjList.end()){
            adjList[user] = std::vector<std::string>();
        }
    }

    void addEdge(const std::string& user1, const std::string& user2){
        adjList[user1].push_back(user2);
        adjList[user2].push_back(user1);
    }

    const std::vector<std::string>& connected(const std::string& user) const{
        return adjList.at(user);
    }
};

std::string runLengthEncoding(const std::string& message){
    std::ostringstream encoded;
    char letter = message[0];
    int count = 0;

    for (char c : message){
        if (c == letter){
            count++;
        } 
        else{
            encoded << letter << count;
            letter = c;
            count = 1;
        }
    }

    encoded << letter << count;
    return encoded.str();
}

int gcd(int a, int b){
     while (b != 0){
         int temp = b;
         b = a % b;
         a = temp;
     }

     return a;
}

int modulusExp(int base, int pow, int mod){
    int temp = 1;

    base = base % mod;
    while (pow > 0){
        if (pow % 2 != 0) temp = (temp * base) % mod;
        pow = pow/2;
        base = (base * base) % mod;       
    }

    return temp;
}

std::pair<std::pair<int, int>, std::pair<int, int>> rsaKeys(int p, int q){
    int n = p * q;
    int phi = (p - 1)* (q - 1);
    int e = 3;
    int d = 1;

    while (gcd(e, phi) != 1) e++;
    while ((d * e) % phi != 1) d++;

    return {{e, n},{d, n}};
}

std::vector<int> StoI(const std::string& strMessage){
    std::vector<int> intMessage;
    for (char ch : strMessage) intMessage.push_back(static_cast<int>(ch));
    return intMessage;
}

std::string ItoS(const std::vector<int>& intMessage){
    std::string strMessage;
    for (int i : intMessage) strMessage += static_cast<char>(i);
    return strMessage;
}

std::vector<int> rsaEncryption(const std::string& strMessage, std::pair<int, int> publicKey){
    std::vector<int> encryptedMessage;
    std::vector<int> intMessage = StoI(strMessage);

    for (int i : intMessage){
        encryptedMessage.push_back(modulusExp(i, publicKey.first, publicKey.second));
    }
    return encryptedMessage;
}

std::string rsaDecryption(const std::vector<int>& encryptedMessage, std::pair<int, int> privateKey){
    std::vector<int> decryptedMessage;

    for (int i : encryptedMessage){
        decryptedMessage.push_back(modulusExp(i, privateKey.first, privateKey.second));
    }
    return ItoS(decryptedMessage);
}

int hash(const std::string& message){
    int hash = 0;
    int prime = 17;

    for (char c : message){
        hash = (hash + c) % prime;
    }

    return hash;
}

std::string sign(const std::string& message, std::pair<int, int> privateKey){
    int hashMessage = hash(message);
    return std::to_string(modulusExp(hashMessage, privateKey.first, privateKey.second));
}

bool verify(const std::string& message, int signature, std::pair<int, int> publicKey){
    int decryptedHash = modulusExp(signature, publicKey.first, publicKey.second);
    int hashMessage = hash(message);
    return decryptedHash == hashMessage;
}

std::string boolToString(bool val){
    return val ? "true" : "false";
}

std::vector<std::string> breadthFirstSearch(const Graph& graph, const std::string& sender, const std::string& receiver){
    std::unordered_map<std::string, std::string> parent;
    std::queue<std::string> queue;
    std::unordered_map<std::string, bool> visited;
    std::vector<std::string> path;

    queue.push(sender);
    visited[sender] = true;

    while (!queue.empty()){
        std::string current = queue.front();
        queue.pop();

        if (current == receiver){
            while (current != sender){
                path.insert(path.begin(), current);
                current = parent[current];
            }
            path.insert(path.begin(), sender);
            return path;
        }

        for (auto& user : graph.connected(current)){
            if (!visited[user]){
                visited[user] = true;
                parent[user] = current;
                queue.push(user);
            }
        }
    }
    return {};
}

struct Message{
    std::string sender;
    std::string receiver;
    std::string metadata;
    std::string content;
    std::vector<std::string> path;
};

Message sendRunLengthEncoded(const Graph& graph, const std::string& sender, const std::string& receiver, const std::string& content){
    std::string encodedMessage = runLengthEncoding(content);
    std::vector<std::string> path = breadthFirstSearch(graph, sender, receiver);
    return { sender, receiver, "Run-length encoded", encodedMessage, path };
}

Message sendRSAMessage(const Graph& graph, const std::string& sender, const std::string& receiver, const std::string& content){
    auto receiverKeys = rsaKeys(11, 13);
    auto receiverPublicKey = receiverKeys.first;

    std::vector<int> encryptedMessage = rsaEncryption(content, receiverPublicKey);
    std::vector<std::string> path = breadthFirstSearch(graph, sender, receiver);
    
    return { sender, receiver, "RSA Encrypted", ItoS(encryptedMessage), path };
}

Message receiveRSAMessage(const Graph& graph, const std::string& sender, const std::string& receiver, const std::string& content){
    auto receiverKeys = rsaKeys(11, 13); 
    auto receiverPrivateKey = receiverKeys.second;

    std::string decryptedMessage = rsaDecryption(StoI(content), receiverPrivateKey);
    std::vector<std::string> path = breadthFirstSearch(graph, sender, receiver);

   return { sender, receiver, "RSA Decrypted", decryptedMessage, path };
}

Message signRSAMessage(const Graph& graph, const std::string& sender, const std::string& receiver, const std::string& content){
    auto senderKeys = rsaKeys(11, 13);
    auto senderPrivateKey = senderKeys.second;

    std::string signedMessage = sign(content, senderPrivateKey);
    std::vector<std::string> path = breadthFirstSearch(graph, sender, receiver);

    return { sender, receiver, "RSA Signature", signedMessage, path };
}

Message verifyRSAMessage(const Graph& graph, const std::string& sender, const std::string& receiver, const std::string& content){
    auto senderKeys = rsaKeys(11, 13);
    auto senderPublicKey = senderKeys.first;
    auto senderPrivateKey = senderKeys.second;

    std::string signedMessage = sign(content, senderPrivateKey);
    bool verifiedMessage = verify(content, std::stoi(signedMessage), senderPublicKey);
    std::vector<std::string> path = breadthFirstSearch(graph, sender, receiver);

    return { sender, receiver, "RSA Verification", boolToString(verifiedMessage), path };
}

int main(){
    Graph network;

    network.addUser("Vic");
    network.addUser("Joana");
    network.addUser("Andy");

    network.addEdge("Vic", "Joana");
    network.addEdge("Joana", "Andy");

    /* Run-Length Encoding */

    std::cout << "\n========Run-Length Encoding========\n";

    std::string rleContent = "YAAARRMMMMMMMAATEEEYYYYYYY";

    Message rleMessage = sendRunLengthEncoded(network, "Vic", "Andy", rleContent);

    std::cout << "Sender: " << rleMessage.sender << "\n";
    std::cout << "Receiver: " << rleMessage.receiver << "\n";
    std::cout << "Metadata: " << rleMessage.metadata << "\n";
    std::cout << "Message Body: " << rleMessage.content << "\n";
    std::cout << "Path: ";
    for (auto& user : rleMessage.path){
        std::cout << user << " ";
    }
   
    std::cout << "\n===================================\n";

    /* RSA-Encrypted */

    std::cout << "\n===========RSA-Encrypted===========\n";

    std::string rsaContent = "Is mayonnaise a DSA?";

    Message rsaMessageEncrypted = sendRSAMessage(network, "Joana", "Vic", rsaContent);

    std::cout << "Sender: " << rsaMessageEncrypted.sender << "\n";
    std::cout << "Receiver: " << rsaMessageEncrypted.receiver << "\n";
    std::cout << "Metadata: " << rsaMessageEncrypted.metadata << "\n";
    std::cout << "Message Body (Encrypted): ";
    for (int i : rsaMessageEncrypted.content){
        std::cout << i << " ";
    }
    std::cout << "\n";
    std::cout << "Path: ";
    for (auto& user : rsaMessageEncrypted.path){
        std::cout << user << " ";
    }

    std::cout << "\n===================================\n";

    std::cout << "\n===========RSA-Decrypted===========\n";

    Message rsaMessageDecrypted = receiveRSAMessage(network, "Joana", "Vic", rsaMessageEncrypted.content);

    std::cout << "Sender: " << rsaMessageDecrypted.sender << "\n";
    std::cout << "Receiver: " << rsaMessageDecrypted.receiver << "\n";
    std::cout << "Metadata: " << rsaMessageDecrypted.metadata << "\n";
    std::cout << "Message Body (Decrypted): " << rsaMessageDecrypted.content << "\n";
    std::cout << "Path: ";
    for (auto& user : rsaMessageDecrypted.path){
        std::cout << user << " ";
    }

    std::cout << "\n===================================\n";

    /* RSA Signature */

    std::cout << "\n===========RSA Signature===========\n";

    Message rsaMessageSigned = signRSAMessage(network, "Andy", "Vic", rsaContent);

    std::cout << "Sender: " << rsaMessageSigned.sender << "\n";
    std::cout << "Receiver: " << rsaMessageSigned.receiver << "\n";
    std::cout << "Metadata: " << rsaMessageSigned.metadata << "\n";
    std::cout << "Message Body (Signed): " << rsaMessageSigned.content << "\n";
    std::cout << "Path: ";
    for (auto& user : rsaMessageSigned.path){
        std::cout << user << " ";
    }

    std::cout << "\n===================================\n";

    std::cout << "\n==========RSA Verification=========\n";

    Message rsaMessageVerified = verifyRSAMessage(network, "Andy", "Vic", rsaContent);

    std::cout << "Sender: " << rsaMessageVerified.sender << "\n";
    std::cout << "Receiver: " << rsaMessageVerified.receiver << "\n";
    std::cout << "Metadata: " << rsaMessageVerified.metadata << "\n";
    std::cout << "Message Body (Verified): " << rsaMessageVerified.content << "\n";
    std::cout << "Path: ";
    for (auto& user : rsaMessageVerified.path){
        std::cout << user << " ";
    }

    std::cout << "\n===================================\n";

    return 0;
}
