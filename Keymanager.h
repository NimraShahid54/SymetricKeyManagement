#include "KeyManager.h"

int main() {
    unsigned char encryptionKey[] = { 'k', 'e', 'y', 'f', 'o', 'r', 'e', 'n', 'c', 'r', 'y', 'p', 't', 'i', 'o', 'n' };

    KeyManager keyManager;

    char choice = 'y';
    while (tolower(choice) == 'y')
    {
        keyManager.generateOutputAndVerifyKey(encryptionKey);
        cout << "Do you want to generate another key? (y/n): ";
        cin >> choice;
    }

    return 0;
}