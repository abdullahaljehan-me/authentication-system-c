#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Cross-platform includes for hidden password input
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define FIELD_SIZE 50
#define MAX_LOGIN_ATTEMPTS 3

struct user
{
    char fullName[FIELD_SIZE];
    char email[FIELD_SIZE];
    char newPassword[FIELD_SIZE];
    char username[FIELD_SIZE];
    char phone[FIELD_SIZE];
};

void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void clearBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void waitForEnter(void)
{
    printf("\nPress Enter twice to continue...");
    fflush(stdout);
    clearBuffer(); // Clear any leftover characters
    getchar();     // Wait for the actual Enter key
}

// Input Functions

void takeInput(char ch[FIELD_SIZE])
{
    if (fgets(ch, FIELD_SIZE, stdin) == NULL)
    {
        ch[0] = '\0';
        return;
    }
    int len = strlen(ch);
    if (len > 0 && ch[len - 1] == '\n')
    {
        ch[len - 1] = '\0';
    }
}

// Cross-platform hidden password input
void takeHiddenInput(char pass[FIELD_SIZE])
{
    int i = 0;
    char ch;

#ifdef _WIN32
    while (1)
    {
        ch = _getch();
        if (ch == '\r' || ch == '\n')
            break;
        if (ch == '\b' && i > 0)
        {
            i--;
            printf("\b \b");
        }
        else if (ch != '\b' && i < FIELD_SIZE - 1)
        {
            pass[i++] = ch;
            printf("*");
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO); // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1)
    {
        ch = getchar();
        if (ch == '\n' || ch == '\r')
            break;

        if ((ch == '\b' || ch == 127) && i > 0)
        {
            i--;
            printf("\b \b");
        }
        else if (ch != '\b' && ch != 127 && i < FIELD_SIZE - 1)
        {
            pass[i++] = ch;
            printf("*");
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    pass[i] = '\0';
    printf("\n");
}

// Validation & Helper Functions
void generateUsername(char email[FIELD_SIZE], char username[FIELD_SIZE])
{
    int i;
    for (i = 0; i < FIELD_SIZE - 1 && email[i] != '\0' && email[i] != '@'; i++)
    {
        username[i] = tolower(email[i]); // Make username lowercase for professionalism
    }
    username[i] = '\0';

    if (strlen(username) == 0)
    {
        strcpy(username, "user");
    }
}

int isEmailValid(const char *email)
{
    const char *dot = strchr(email, '.');
    const char *at = strchr(email, '@');
    return (at != NULL && dot != NULL && dot > at);
}

int isPhoneValid(const char *phone)
{
    int len = strlen(phone);
    if (len < 7 || len > 15)
        return 0;
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(phone[i]))
            return 0;
    }
    return 1;
}

int isUserExists(const char *username, const char *email)
{
    FILE *filep = fopen("user.dat", "rb");
    if (filep == NULL)
        return 0;

    struct user temp;
    while (fread(&temp, sizeof(temp), 1, filep) == 1)
    {
        if (strcmp(temp.username, username) == 0 || strcmp(temp.email, email) == 0)
        {
            fclose(filep);
            return 1;
        }
    }
    fclose(filep);
    return 0;
}

void signUp(void)
{
    clearScreen();
    printf("\n=============================\n");
    printf("           SIGN UP\n");
    printf("=============================\n\n");

    struct user newUser;
    char confirmPassword[FIELD_SIZE];
    memset(&newUser, 0, sizeof(newUser));

    printf("ENTER YOUR FULL NAME : ");
    takeInput(newUser.fullName);

    printf("ENTER YOUR EMAIL     : ");
    takeInput(newUser.email);

    printf("ENTER YOUR PHONE     : ");
    takeInput(newUser.phone);

    printf("CREATE PASSWORD      : ");
    takeHiddenInput(newUser.newPassword);

    printf("CONFIRM PASSWORD     : ");
    takeHiddenInput(confirmPassword);

    if (strlen(newUser.fullName) == 0 || strlen(newUser.email) == 0 || strlen(newUser.phone) == 0)
    {
        printf("\n[ERROR] KINDLY FILL UP ALL THE FIELDS.\n");
        waitForEnter();
        return;
    }

    if (!isEmailValid(newUser.email))
    {
        printf("\n[ERROR] INVALID EMAIL FORMAT. MUST CONTAIN '@' AND '.'\n");
        waitForEnter();
        return;
    }

    if (!isPhoneValid(newUser.phone))
    {
        printf("\n[ERROR] INVALID PHONE NUMBER. USE DIGITS ONLY (7-15 chars).\n");
        waitForEnter();
        return;
    }

    if (strlen(newUser.newPassword) < 6)
    {
        printf("\n[ERROR] PASSWORD MUST BE AT LEAST 6 CHARACTERS.\n");
        waitForEnter();
        return;
    }

    if (strcmp(newUser.newPassword, confirmPassword) != 0)
    {
        printf("\n[ERROR] PASSWORDS DO NOT MATCH.\n");
        waitForEnter();
        return;
    }

    generateUsername(newUser.email, newUser.username);

    if (isUserExists(newUser.username, newUser.email))
    {
        printf("\n[ERROR] USERNAME OR EMAIL ALREADY REGISTERED.\n");
        waitForEnter();
        return;
    }

    FILE *filep = fopen("user.dat", "ab");
    if (filep == NULL)
    {
        printf("\n[ERROR] COULD NOT OPEN FILE FOR WRITING.\n");
        waitForEnter();
        return;
    }

    if (fwrite(&newUser, sizeof(newUser), 1, filep) == 1)
    {
        printf("\n[SUCCESS] REGISTRATION SUCCESSFUL!");
        printf("\nYOUR GENERATED USERNAME : %s\n", newUser.username);
    }
    else
    {
        printf("\n[ERROR] FAILED TO SAVE DATA.\n");
    }
    fclose(filep);
    waitForEnter();
}

void signIn(void)
{
    clearScreen();
    printf("\n=============================\n");
    printf("           SIGN IN\n");
    printf("=============================\n\n");

    char username[FIELD_SIZE], verifyPass[FIELD_SIZE];
    struct user usr;
    int attempts = 0;
    int isAuthenticated = 0;

    while (attempts < MAX_LOGIN_ATTEMPTS)
    {
        memset(username, 0, sizeof(username));
        memset(verifyPass, 0, sizeof(verifyPass));

        printf("ENTER YOUR USERNAME : ");
        takeInput(username);

        printf("ENTER YOUR PASSWORD : ");
        takeHiddenInput(verifyPass);

        FILE *filep = fopen("user.dat", "rb");
        if (filep == NULL)
        {
            printf("\n[ERROR] NO USER DATA FOUND. PLEASE SIGN UP FIRST.\n");
            waitForEnter();
            return;
        }

        int userFound = 0;
        while (fread(&usr, sizeof(usr), 1, filep) == 1)
        {
            if (strcmp(usr.username, username) == 0)
            {
                userFound = 1;
                if (strcmp(usr.newPassword, verifyPass) == 0)
                {
                    isAuthenticated = 1;
                    break;
                }
            }
        }
        fclose(filep);

        if (isAuthenticated)
        {
            clearScreen();
            printf("\n=============================\n");
            printf("        DASHBOARD\n");
            printf("=============================\n");
            printf("\nWELCOME BACK, %s!\n\n", usr.fullName);
            printf("FULL NAME : %s\n", usr.fullName);
            printf("EMAIL     : %s\n", usr.email);
            printf("USERNAME  : %s\n", usr.username);
            printf("PHONE     : %s\n", usr.phone);
            printf("=============================\n");
            waitForEnter();
            return;
        }
        else
        {
            attempts++;
            if (userFound)
            {
                printf("\n[ERROR] INCORRECT PASSWORD. (%d/%d ATTEMPTS REMAINING)\n", MAX_LOGIN_ATTEMPTS - attempts, MAX_LOGIN_ATTEMPTS);
            }
            else
            {
                printf("\n[ERROR] USER NOT FOUND. PLEASE CHECK YOUR USERNAME.\n");
            }

            if (attempts < MAX_LOGIN_ATTEMPTS)
            {
                printf("\nPress Enter to try again...");
                clearBuffer();
                getchar();
                clearScreen();
                printf("\n=============================\n");
                printf("           SIGN IN\n");
                printf("=============================\n\n");
            }
        }
    }

    printf("\n[LOCKED] TOO MANY FAILED ATTEMPTS. ACCESS DENIED FOR THIS SESSION.\n");
    waitForEnter();
}

int main(void)
{
    int option;

    while (1)
    {
        clearScreen();
        printf("\n==============================\n");
        printf("   AUTHENTICATION SYSTEM\n");
        printf("==============================\n\n");
        printf("\t[1] SIGN UP\n");
        printf("\t[2] SIGN IN\n");
        printf("\t[3] EXIT\n\n");
        printf("ENTER YOUR CHOICE : ");

        if (scanf("%d", &option) != 1)
        {
            clearBuffer();
            printf("\n[ERROR] INVALID INPUT. PLEASE ENTER 1, 2, OR 3.\n");
            waitForEnter();
            continue;
        }
        clearBuffer();

        switch (option)
        {
        case 1:
            signUp();
            break;
        case 2:
            signIn();
            break;
        case 3:
            clearScreen();
            printf("\nEXITING THE AUTHENTICATION SYSTEM. GOODBYE!\n\n");
            return 0;
        default:
            printf("\n[ERROR] INVALID OPTION. PLEASE ENTER 1, 2, OR 3.\n");
            waitForEnter();
            break;
        }
    }
    return 0;
}
