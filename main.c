#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <ctype.h>

/*
================================================================================
    CONSTANTS AND CONFIGURATION
================================================================================
*/

#define MAX_NAME_LENGTH 50
#define MAX_PLAYERS 100
#define MAX_PATH_LENGTH 1000
#define MAZE_SIZE 10

// Challenge difficulty points
#define POINTS_COMPTE_BON 30
#define POINTS_MASTERMIND 25
#define POINTS_ROBOT 50
#define POINTS_TRI 20
#define POINTS_HANOI 40

// File paths
#define USERS_FILE "users.txt"
#define SCORES_FILE "scores.txt"

/*
================================================================================
    ENUMERATIONS
================================================================================
*/

typedef enum {
    CHALLENGE_COMPTE_BON = 1,
    CHALLENGE_MASTERMIND = 2,
    CHALLENGE_ROBOT = 3,
    CHALLENGE_TRI = 4,
    CHALLENGE_HANOI = 5,
    VIEW_LEADERBOARD = 6,
    VIEW_STATS = 7,
    EXIT = 8
} MenuOption;

typedef enum {
    WALL = '#',
    EMPTY = '.',
    START = 'S',
    END = 'E',
    PATH = '*'
} MazeCell;

/*
================================================================================
    STRUCTURES
================================================================================
*/

typedef struct {
    char name[MAX_NAME_LENGTH];
    int compte_bon_score;
    int mastermind_score;
    int robot_score;
    int tri_score;
    int hanoi_score;
    int total_score;
    int challenges_completed;
} Player;

typedef struct {
    int x;
    int y;
} Position;

/*
================================================================================
    GLOBAL VARIABLES
================================================================================
*/

Player current_player;
char current_username[MAX_NAME_LENGTH];

/*
================================================================================
    UTILITY FUNCTIONS
================================================================================
*/

// Clear the console screen
void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Wait for user to press Enter
void pause_screen() {
    printf("\n\nPress Enter to continue...");
    while(getchar() != '\n');
    getchar();
}

// Display a fancy header
void display_header(const char* title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                       ║\n");
    printf("║          %s", title);
    // Add padding to center the title
    int padding = 62 - strlen(title);
    for(int i = 0; i < padding; i++) printf(" ");
    printf("║\n");
    printf("║                                                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n\n");
}

// Display the main banner
void display_banner() {
    printf("\n");
    printf("  ╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                                                ║\n");
    printf("  ║      ██████╗██╗  ██╗ █████╗ ██╗     ██╗     ███████╗███╗   ██╗ ██████╗ ███████╗║\n");
    printf("  ║     ██╔════╝██║  ██║██╔══██╗██║     ██║     ██╔════╝████╗  ██║██╔════╝ ██╔════╝║\n");
    printf("  ║     ██║     ███████║███████║██║     ██║     █████╗  ██╔██╗ ██║██║  ███╗█████╗  ║\n");
    printf("  ║     ██║     ██╔══██║██╔══██║██║     ██║     ██╔══╝  ██║╚██╗██║██║   ██║██╔══╝  ║\n");
    printf("  ║     ╚██████╗██║  ██║██║  ██║███████╗███████╗███████╗██║ ╚████║╚██████╔╝███████╗║\n");
    printf("  ║      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝║\n");
    printf("  ║                                                                                ║\n");
    printf("  ║      █████╗ ██████╗ ███████╗███╗   ██╗ █████╗                                  ║\n");
    printf("  ║     ██╔══██╗██╔══██╗██╔════╝████╗  ██║██╔══██╗                                 ║\n");
    printf("  ║     ███████║██████╔╝█████╗  ██╔██╗ ██║███████║                                 ║\n");
    printf("  ║     ██╔══██║██╔══██╗██╔══╝  ██║╚██╗██║██╔══██║                                 ║\n");
    printf("  ║     ██║  ██║██║  ██║███████╗██║ ╚████║██║  ██║                                 ║\n");
    printf("  ║     ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝                                 ║\n");
    printf("  ║                                                                                ║\n");
    printf("  ║              🎯 CHALLENGES PLATFORM - Learn By Doing 🎯                       ║\n");
    printf("  ║                                                                                ║\n");
    printf("  ╚════════════════════════════════════════════════════════════════════════════════╝\n\n");
}

/*
================================================================================
    FILE HANDLING FUNCTIONS
================================================================================
*/

// Check if a user exists in the users file
int user_exists(const char* username) {
    FILE* file = fopen(USERS_FILE, "r");
    if (!file) return 0;
    
    char line[MAX_NAME_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        if (strcmp(line, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

// Add a new user to the users file
void add_user(const char* username) {
    FILE* file = fopen(USERS_FILE, "a");
    if (file) {
        fprintf(file, "%s\n", username);
        fclose(file);
    }
}

// Load player scores from the scores file
void load_player_scores(Player* player) {
    FILE* file = fopen(SCORES_FILE, "r");
    if (!file) return;
    
    char name[MAX_NAME_LENGTH];
    int cb, mm, rb, tr, hn, tot, comp;
    
    while (fscanf(file, "%[^,],%d,%d,%d,%d,%d,%d,%d\n", 
                  name, &cb, &mm, &rb, &tr, &hn, &tot, &comp) == 8) {
        if (strcmp(name, player->name) == 0) {
            player->compte_bon_score = cb;
            player->mastermind_score = mm;
            player->robot_score = rb;
            player->tri_score = tr;
            player->hanoi_score = hn;
            player->total_score = tot;
            player->challenges_completed = comp;
            break;
        }
    }
    fclose(file);
}

// Save player scores to the scores file
void save_player_scores(Player* player) {
    FILE* file = fopen(SCORES_FILE, "r");
    FILE* temp = fopen("temp.txt", "w");
    
    if (!temp) return;
    
    int found = 0;
    if (file) {
        char name[MAX_NAME_LENGTH];
        int cb, mm, rb, tr, hn, tot, comp;
        
        while (fscanf(file, "%[^,],%d,%d,%d,%d,%d,%d,%d\n", 
                      name, &cb, &mm, &rb, &tr, &hn, &tot, &comp) == 8) {
            if (strcmp(name, player->name) == 0) {
                fprintf(temp, "%s,%d,%d,%d,%d,%d,%d,%d\n",
                       player->name, player->compte_bon_score,
                       player->mastermind_score, player->robot_score,
                       player->tri_score, player->hanoi_score,
                       player->total_score, player->challenges_completed);
                found = 1;
            } else {
                fprintf(temp, "%s,%d,%d,%d,%d,%d,%d,%d\n",
                       name, cb, mm, rb, tr, hn, tot, comp);
            }
        }
        fclose(file);
    }
    
    if (!found) {
        fprintf(temp, "%s,%d,%d,%d,%d,%d,%d,%d\n",
               player->name, player->compte_bon_score,
               player->mastermind_score, player->robot_score,
               player->tri_score, player->hanoi_score,
               player->total_score, player->challenges_completed);
    }
    
    fclose(temp);
    remove(SCORES_FILE);
    rename("temp.txt", SCORES_FILE);
}

/*
================================================================================
    USER MANAGEMENT FUNCTIONS
================================================================================
*/

// Initialize a new player with zero scores
void initialize_player(Player* player, const char* username) {
    strcpy(player->name, username);
    player->compte_bon_score = 0;
    player->mastermind_score = 0;
    player->robot_score = 0;
    player->tri_score = 0;
    player->hanoi_score = 0;
    player->total_score = 0;
    player->challenges_completed = 0;
}

// Handle user login or registration
void login_user() {
    clear_screen();
    display_banner();
    
    printf("Enter your name: ");
    fgets(current_username, MAX_NAME_LENGTH, stdin);
    current_username[strcspn(current_username, "\n")] = 0;
    
    if (user_exists(current_username)) {
        initialize_player(&current_player, current_username);
        load_player_scores(&current_player);
        
        printf("\n╔═══════════════════════════════════════════════╗\n");
        printf("║  Welcome back, %s!", current_username);
        int padding = 27 - strlen(current_username);
        for(int i = 0; i < padding; i++) printf(" ");
        printf("║\n");
        printf("╚═══════════════════════════════════════════════╝\n");
        printf("\n📊 Your Statistics:\n");
        printf("   • Total Score: %d points\n", current_player.total_score);
        printf("   • Challenges Completed: %d/5\n", current_player.challenges_completed);
    } else {
        add_user(current_username);
        initialize_player(&current_player, current_username);
        
        printf("\n╔═══════════════════════════════════════════════╗\n");
        printf("║  Welcome, %s!", current_username);
        int padding = 32 - strlen(current_username);
        for(int i = 0; i < padding; i++) printf(" ");
        printf("║\n");
        printf("║  Let's start your coding journey! 🚀           ║\n");
        printf("╚════════════════════════════════════════════════╝\n");
    }
    
    pause_screen();
}

/*
================================================================================
    MAIN FUNCTION
================================================================================
*/

int main() {
    // Set console to UTF-8 encoding for Windows
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    // Seed random number generator once
    srand(time(NULL));
    
    login_user();
    
    int running = 1;
    while(running) {
        display_main_menu();
        
        printf("  Select option (1-8): ");
        int choice;
        scanf("%d", &choice);
        while(getchar() != '\n'); // Clear input buffer
        
        switch(choice) {
            case CHALLENGE_COMPTE_BON:
                //challenge_compte_bon();
                break;
            case CHALLENGE_MASTERMIND:
                // challenge_mastermind();
                break;
            case CHALLENGE_ROBOT:
                // challenge_course_robots();
                break;
            case CHALLENGE_TRI:
                // challenge_tri_ultime();
                break;
            case CHALLENGE_HANOI:
                // challenge_tour_hanoi();
                break;
            case VIEW_LEADERBOARD:
                // display_leaderboard();
                break;
            case VIEW_STATS:
                // display_player_stats();
                break;
            case EXIT:
                printf("\n  Thanks for playing, %s! Keep coding! 🚀\n\n", current_player.name);
                running = 0;
                break;
            default:
                printf("\n  ❌ Invalid option! Please choose 1-8.\n");
                pause_screen();
        }
    }
    
    return 0;
}