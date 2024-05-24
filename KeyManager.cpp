#pragma once
#include "SymmetricKeyManager.h"
#include <unordered_map>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

struct KeyInfo {
    string key;
    time_t expiryDate;
    bool isRevoked;
    bool isUsed;
};

class KeyManager {
private:
    SymmetricKeyManager keyManager;
    unordered_map<string, string> users;
    unordered_map<string, string> securityAnswers;
    unordered_map<string, KeyInfo> keys;

    string encrypt(const string& symmetricKey, const unsigned char* encryptionKey, size_t keyLength) {
        string encryptedKey = symmetricKey;
        for (size_t i = 0; i < symmetricKey.length(); ++i) {
            encryptedKey[i] = symmetricKey[i] ^ encryptionKey[i % keyLength];
        }
        return encryptedKey;
    }

    string decrypt(const string& encryptedKey, const unsigned char* encryptionKey, size_t keyLength) {
        string decryptedKey = encryptedKey;
        for (size_t i = 0; i < encryptedKey.length(); ++i) {
            decryptedKey[i] = encryptedKey[i] ^ encryptionKey[i % keyLength];
        }
        return decryptedKey;
    }

    time_t calculateExpiryDate(int daysValid) {
        auto now = chrono::system_clock::now();
        auto expiryDate = now + chrono::hours(24 * daysValid);
        return chrono::system_clock::to_time_t(expiryDate);
    }

    string formatTime(time_t time) {
        struct tm timeInfo;
        localtime_s(&timeInfo, &time); // Use localtime_s for safer time conversion
        stringstream ss;
        ss << put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    KeyManager() {
        users = {
            {"nimra", "123"},
            {"aqsa", "456"},
            {"moiza", "789"},
            {"kinza", "012"}
        };

        securityAnswers = {
            {"nimra", "black"},
            {"aqsa", "coal"},
            {"moiza", "batmenton"},
            {"kinza", "cat"}
        };
    }

    bool authenticateUser(const string& username, const string& password) {
        auto it = users.find(username);
        if (it != users.end()) {
            return it->second == password;
        }
        return false;
    }

    void revokeKey(const string& key) {
        auto it = keys.find(key);
        if (it != keys.end()) {
            it->second.isRevoked = true;
            cout << "Key revoked successfully." << endl;
        }
        else {
            cout << "Key not found." << endl;
        }
    }

    void generateOutputAndVerifyKey(const unsigned char* encryptionKey) {
        string username, password;

        cout << "Enter your username: ";
        cin >> username;
        cout << "Enter your password: ";
        cin >> password;

        if (!authenticateUser(username, password)) {
            cout << "Authentication failed." << endl;
            return;
        }

        int keyLengthBits;
        cout << "Enter the desired key length in bits: ";
        cin >> keyLengthBits;

        char viewKey;
        cout << "Do you want to view the key? (y/n): ";
        cin >> viewKey;
        if (tolower(viewKey) == 'n') {
            cout << "You chose not to view the key." << endl;
            return;
        }

        string securityQuestion, correctAnswer;
        if (username == "nimra") {
            securityQuestion = "What is your favorite color?";
            correctAnswer = "black";
        }
        else if (username == "aqsa") {
            securityQuestion = "What is your favorite subject?";
            correctAnswer = "coal";
        }
        else if (username == "moiza") {
            securityQuestion = "What is your favorite sport?";
            correctAnswer = "batmenton";
        }
        else if (username == "kinza") {
            securityQuestion = "What is your favorite pet?";
            correctAnswer = "cat";
        }
        else {
            cout << "Invalid username." << endl;
            return;
        }

        string securityAnswer;
        cout << "Security Question: " << securityQuestion << endl;
        cin >> securityAnswer;

        if (securityAnswer != correctAnswer) {
            cout << "Incorrect answer to the security question." << endl;
            return;
        }

        string symmetricKey = keyManager.generateKey(keyLengthBits);
        size_t keyLength = strlen(reinterpret_cast<const char*>(encryptionKey));
        string encryptedKey = encrypt(symmetricKey, encryptionKey, keyLength);
        string decryptedKey = decrypt(encryptedKey, encryptionKey, keyLength);

        if (decryptedKey == symmetricKey) {
            cout << "Key verification successful: The encrypted key was decrypted correctly." << endl;
        }
        else {
            cout << "Key verification failed: The encrypted key could not be decrypted to the original key." << endl;
            return;
        }

        int daysValid = 30; // Setting the key expiry to 30 days by default
        time_t expiryDate = calculateExpiryDate(daysValid);

        KeyInfo keyInfo = { symmetricKey, expiryDate, false, false };
        keys[symmetricKey] = keyInfo;

        cout << "Generated Symmetric Key: " << symmetricKey << endl;
        cout << "Key Expiry Date: " << formatTime(expiryDate) << endl;

        if (username == "nimra" && password == "123") {
            string option;
            do {
                cout << "Do you want to add or remove people, view key details, revoke a key, or generate another key? (add/remove/view/revoke/generate/exit): ";
                cin >> option;
                if (option == "add") {
                    cout << "Enter new username: ";
                    string newUsername;
                    cin >> newUsername;
                    cout << "Enter new password: ";
                    string newPassword;
                    cin >> newPassword;
                    users[newUsername] = newPassword;
                    cout << "User added successfully." << endl;
                }
                else if (option == "remove") {
                    cout << "Enter username to remove: ";
                    string removeUsername;
                    cin >> removeUsername;
                    auto it = users.find(removeUsername);
                    if (it != users.end()) {
                        users.erase(it);
                        cout << "User removed successfully." << endl;
                    }
                    else {
                        cout << "User not found." << endl;
                    }
                }
                else if (option == "view") {
                    cout << "All Keys and Details:" << endl;
                    for (const auto& entry : keys) {
                        const KeyInfo& info = entry.second;
                        cout << "Key: " << entry.first
                            << ", Expiry: " << formatTime(info.expiryDate)
                            << ", Revoked: " << (info.isRevoked ? "Yes" : "No")
                            << ", Used: " << (info.isUsed ? "Yes" : "No") << endl;
                    }
                }
                else if (option == "revoke") {
                    cout << "You are limited to this functionality." << endl;
                    string keyToRevoke;
                    cout << "Enter the key to revoke: ";
                    cin >> keyToRevoke;
                    revokeKey(keyToRevoke);
                    cout << "Key revoked successfully." << endl;

                    // Update key information after revoking
                    auto it = keys.find(keyToRevoke);
                    if (it != keys.end()) {
                        it->second.isRevoked = true;
                    }
                    else {
                        cout << "Key not found." << endl;
                    }                   
                }
                else if (option == "generate") {
                    return;
                }
            } while (option != "exit");
        }
    }
};