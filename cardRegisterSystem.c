#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <openssl/sha.h>

#define FILE_NAME "cardsDetails.txt"
#define LOG_FILE "audit.log"
#define MAX_LINE 400
#define MAX_ATTEMPTS 3

typedef struct {
    char cardNumber[20];
    char name[50];
    char expiryDate[10];
    char pinHash[65];
    char role[10];
} Card;

void get_timestamp(char *buffer);
void log_attempt(const Card *card, const char *status);
void hash_pin(const char *pin, char *outputBuffer);
bool isCardExpired(const char *expiryDate);
bool findCard(Card *card);
void saveCard(const Card *card);
void registerCard(Card *card);
bool verifyPIN(const char *storedHash);
bool authenticate(Card *card);
void rehash_pin(const char *pin, char *outputBuffer);

int main() {
    Card card;
    memset(&card, 0, sizeof(card));

    printf("Insert card (enter card number): ");
    scanf("%19s", card.cardNumber);

    if (!findCard(&card)) {
        registerCard(&card);
    }

    if (authenticate(&card)) {
        printf("Access Granted. Welcome, %s!\n", card.name);

        if (strcmp(card.role, "admin") == 0) {
            printf("Admin Access: You can manage users or system setting.\n");
        } else {
            printf("User Access: Limited privileges granted.\n");
        }
    } else {
        printf("Access Denied.\n");
    }

    return 0;
}

void get_timestamp (char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", t);
}

void log_attempt (const Card *card, const char *status) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    char timestamp[64];
    get_timestamp(timestamp);

    if (strlen(card->name) > 0)
        fprintf(log, "[%s] Card: %s | Name: %s | Status: %s\n", timestamp, card->cardNumber, card->name, status);
    else
        fprintf(log, "[%s] Card: %s | Status: %s\n", timestamp, card->cardNumber, status);

    fclose(log);
}

void hash_pin (const char *pin, char *outputBuffer) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *) pin, strlen(pin), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

void rehash_pin (const char *pin, char *outputBuffer) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *) pin, strlen(pin), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

bool isCardExpired(const char *expiryDate) {
    int month, year;
    sscanf(expiryDate, "%d/%d", &month, &year);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int currentMonth = tm.tm_mon + 1;
    int currentYear = tm.tm_year % 100;
    return (year < currentYear || (year == currentYear && month < currentMonth));
}

bool findCard(Card *card){
    FILE *file = fopen(FILE_NAME, "r");
    if(!file) return false;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        char cardNumber[20], name[50], expiryDate[10], pinHash[65], role[10];
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", cardNumber, name, expiryDate, pinHash, role);
    
        if (strcmp(card->cardNumber, cardNumber) == 0) {
            strcpy(card->name, name);
            strcpy(card->expiryDate, expiryDate);
            strcpy(card->pinHash, pinHash);
            strcpy(card->role, role);
            fclose(file);
            return true;
        }
    }    
    
    fclose(file);
    return false;
}

void saveCard(const Card *card) {
    FILE *file = fopen(FILE_NAME, "a");
    if (file) {
        fprintf(file, "%s,%s,%s,%s,%s\n",
                card->cardNumber, card->name,
                card->expiryDate, card->pinHash, card->role);
        fclose(file);
        printf("Registration successful.\n");
    } else {
        printf("Error: Unable to write to file.\n");
    }
}

void registerCard(Card *card) {
    printf("Card not found. Registering new card.\n");

    printf("Enter your name: ");
    getchar();
    fgets(card->name, sizeof(card->name), stdin);
    card->name[strcspn(card->name, "\n")] = '\0';

    printf("Enter expiry date (MM/YY): ");
    scanf("%9s", card->expiryDate);

    char pin[10];
    printf("Set a 4-digit PIN: ");
    scanf("%9s", pin);

    printf("Enter role (admin/user): ");
    scanf("%9s", card->role);

    hash_pin(pin, card->pinHash);
    saveCard(card);
}

bool verifyPIN(const char *storedHash) {
    char enteredPIN[10], hashedInput[65];
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        printf("Enter PIN: ");
        scanf("%9s", enteredPIN);
        hash_pin(enteredPIN, hashedInput);

        if (strcmp(storedHash, hashedInput) == 0)
            return true;

        printf("Incorrect PIN. Attempts left: %d\n", MAX_ATTEMPTS - attempt);
    }
    return false;
}

bool authenticate(Card *card) {
    if (isCardExpired(card->expiryDate)) {
        printf("Access Denied: Card expired.\n");
        log_attempt(card, "Access Denied - Expired");
        return false;
    }

    if (!verifyPIN(card->pinHash)) {
        printf("Access Denied: Incorrect PIN.\n");
        log_attempt(card, "Access Denied - Incorrect PIN");
        return false;
    }

    log_attempt(card, "Access Granted");
    return true;
}
