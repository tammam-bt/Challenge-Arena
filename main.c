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
    CHALLENGE 1: LE COMPTE EST BON
    Find operations to reach a target number from given numbers
================================================================================
*/

// Simple expression evaluator
int evaluate_expression(const char* expr, int* numbers, int count, int* valid) {
    char clean[500] = {0};
    int clean_idx = 0;
    
    // Remove whitespaces
    for(int i = 0; expr[i]; i++) {
        if (!isspace(expr[i])) {
            clean[clean_idx++] = expr[i];
        }
    }
    
    // Simple validation: check if expression uses only allowed numbers
    int used[6] = {0};
    char temp[100];
    int temp_idx = 0;
    
    for(int i = 0; clean[i]; i++) {
        if (isdigit(clean[i])) {
            temp[temp_idx++] = clean[i];
        } else {
            if (temp_idx > 0) {
                temp[temp_idx] = '\0';
                int num = atoi(temp);
                int found = 0;
                for(int j = 0; j < count; j++) {
                    if (numbers[j] == num && !used[j]) {
                        used[j] = 1;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    *valid = 0;
                    return 0;
                }
                temp_idx = 0;
            }
        }
    }
    if (temp_idx > 0) {
        temp[temp_idx] = '\0';
        int num = atoi(temp);
        int found = 0;
        for(int j = 0; j < count; j++) {
            if (numbers[j] == num && !used[j]) {
                used[j] = 1;
                found = 1;
                break;
            }
        }
        if (!found) {
            *valid = 0;
            return 0;
        }
    }
    
    *valid = 1;
    return atoi(clean + strlen(clean) - 3); // Gets last number as result
}

void challenge_compte_bon() {
    clear_screen();
    display_header("CHALLENGE 1: Le Compte est Bon");
    
    printf("🎯 Objective: Reach the target number using the given numbers\n");
    printf("   You can use +, -, *, / operations\n");
    printf("   Each number can be used only once\n\n");
    
    // Generate random numbers from different ranges
    int numbers[6];
    numbers[0] = (rand() % 9) + 1;        // 1-9
    numbers[1] = (rand() % 9) + 1;        // 1-9
    numbers[2] = (rand() % 9) + 2;        // 2-10
    numbers[3] = ((rand() % 4) + 1) * 5;  // 5, 10, 15, 20
    numbers[4] = ((rand() % 4) + 2) * 10; // 20, 30, 40, 50
    numbers[5] = ((rand() % 4) + 1) * 25; // 25, 50, 75, 100
    
    // Generate random target
    int target = (rand() % 800) + 100; // 100-899
    
    printf("Available numbers: ");
    for(int i = 0; i < 6; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\nTarget: %d\n\n", target);
    
    time_t start_time = time(NULL);
    
    printf("Enter your solution (e.g., (25*5)+10-2)\n");
    printf("Or type 'skip' to skip\n\n");
    
    char solution[500];
    printf("Your solution: ");
    fgets(solution, sizeof(solution), stdin);
    solution[strcspn(solution, "\n")] = 0;
    
    time_t end_time = time(NULL);
    int time_taken = (int)difftime(end_time, start_time);
    
    if (strcmp(solution, "skip") == 0) {
        printf("\n❌ Challenge skipped!\n");
        pause_screen();
        return;
    }
    
    // Validate solution
    int valid = 0;
    int result = evaluate_expression(solution, numbers, 6, &valid);
    
    // Additional simple check: look for target in solution
    char target_str[20];
    sprintf(target_str, "%d", target);
    int contains_target = (strstr(solution, target_str) != NULL);
    
    if (contains_target && valid) {
        // Calculate score based on time
        int base_score = POINTS_COMPTE_BON;
        int time_penalty = (time_taken / 30) * 2; // -2 points per 30 seconds
        int score = base_score - time_penalty;
        if (score < 5) score = 5; // Minimum score
        
        printf("\n✅ Correct! Your solution works!\n");
        printf("⏱️  Time taken: %d seconds\n", time_taken);
        printf("🎯 Score earned: %d points\n", score);
        
        if (current_player.compte_bon_score == 0) {
            current_player.challenges_completed++;
        }
        
        if (score > current_player.compte_bon_score) {
            current_player.total_score = current_player.total_score - current_player.compte_bon_score + score;
            current_player.compte_bon_score = score;
            save_player_scores(&current_player);
            printf("🏆 New personal best!\n");
        }
    } else {
        printf("\n❌ Invalid solution! Make sure you:\n");
        printf("   • Use only the given numbers (each once)\n");
        printf("   • Reach the exact target: %d\n", target);
    }
    
    pause_screen();
}

/*
================================================================================
    CHALLENGE 2: MASTERMIND ALGORITHMIQUE
    Guess the secret sequence with feedback on correct/misplaced digits
================================================================================
*/

void challenge_mastermind() {
    clear_screen();
    display_header("CHALLENGE 2: Mastermind Algorithmique");
    
    printf("🎯 Objective: Guess the secret 4-digit sequence (digits 1-6)\n");
    printf("   After each guess, you'll get feedback for each position:\n");
    printf("   • ✓ = Correct digit in correct position\n");
    printf("   • ? = Correct digit in wrong position\n");
    printf("   • x = Wrong digit\n\n");
    
    // Generate secret sequence
    int secret[4];
    for(int i = 0; i < 4; i++) {
        secret[i] = (rand() % 6) + 1;
    }
    
    int attempts = 0;
    int max_attempts = 10;
    int won = 0;
    
    while(attempts < max_attempts && !won) {
        printf("\nAttempt %d/%d\n", attempts + 1, max_attempts);
        printf("Enter 4 digits (1-6): ");
        
        int guess[4];
        char input[10];
        fgets(input, sizeof(input), stdin);
        
        if (sscanf(input, "%d %d %d %d", &guess[0], &guess[1], &guess[2], &guess[3]) != 4) {
            printf("❌ Invalid input! Please enter 4 numbers separated by spaces.\n");
            continue;
        }
        
        attempts++;
        
        // Generate feedback for each position
        char feedback[4];
        int secret_used[4] = {0};
        int guess_used[4] = {0};
        
        // First pass: mark exact matches
        for(int i = 0; i < 4; i++) {
            if (guess[i] == secret[i]) {
                feedback[i] = 'v'; // correct position
                secret_used[i] = 1;
                guess_used[i] = 1;
            } else {
                feedback[i] = 'x'; // default to wrong
            }
        }
        
        // Second pass: find correct digits in wrong positions
        for(int i = 0; i < 4; i++) {
            if (!guess_used[i]) {
                for(int j = 0; j < 4; j++) {
                    if (!secret_used[j] && guess[i] == secret[j]) {
                        feedback[i] = '?'; // correct digit, wrong position
                        secret_used[j] = 1;
                        break;
                    }
                }
            }
        }
        
        // Display feedback per position
        printf("Feedback: ");
        for(int i = 0; i < 4; i++) {
            if (feedback[i] == 'v') printf("✓ ");
            else if (feedback[i] == '?') printf("? ");
            else printf("x ");
        }
        printf("\n");
        
        // Check win condition
        won = 1;
        for(int i = 0; i < 4; i++) {
            if (feedback[i] != 'v') {
                won = 0;
                break;
            }
        }
    }
    
    if (won) {
        // Score based on number of attempts (fewer is better)
        int score = POINTS_MASTERMIND - (attempts - 1) * 2;
        if (score < 5) score = 5;
        
        printf("\n✅ Congratulations! You cracked the code!\n");
        printf("🎯 Attempts: %d\n", attempts);
        printf("🎯 Score earned: %d points\n", score);
        
        if (current_player.mastermind_score == 0) {
            current_player.challenges_completed++;
        }
        
        if (score > current_player.mastermind_score) {
            current_player.total_score = current_player.total_score - current_player.mastermind_score + score;
            current_player.mastermind_score = score;
            save_player_scores(&current_player);
            printf("🏆 New personal best!\n");
        }
    } else {
        printf("\n❌ Out of attempts! The secret was: ");
        for(int i = 0; i < 4; i++) printf("%d ", secret[i]);
        printf("\n");
    }
    
    pause_screen();
}


/*
================================================================================
    CHALLENGE 3: COURSE DE ROBOTS
    Find the shortest path through a maze
================================================================================
*/

void display_maze(char maze[MAZE_SIZE][MAZE_SIZE]) {
    printf("\n  ");
    for(int i = 0; i < MAZE_SIZE; i++) printf("%d ", i);
    printf("\n");
    
    for(int i = 0; i < MAZE_SIZE; i++) {
        printf("%d ", i);
        for(int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

int is_valid_move(int x, int y, char maze[MAZE_SIZE][MAZE_SIZE], int visited[MAZE_SIZE][MAZE_SIZE]) {
    return (x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE &&
            maze[x][y] != WALL && !visited[x][y]);
}

void challenge_course_robots() {
    clear_screen();
    display_header("CHALLENGE 3: Course de Robots");
    
    printf("🎯 Objective: Find the shortest path from S to E\n");
    printf("   Commands: N (North/Up), S (South/Down), E (East/Right), O (West/Left)\n\n");
    
    // Predefined mazes with unique solutions
    char mazes[4][MAZE_SIZE][MAZE_SIZE] = {
        // Maze 1 - Optimal: 18 steps
        {
            {START, '.', '.', '#', '.', '.', '.', '#', '.', '.'},
            {'#', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
            {'.', '.', '.', '.', '.', '#', '.', '.', '.', '.'},
            {'.', '#', '#', '#', '.', '#', '#', '#', '.', '#'},
            {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
            {'#', '#', '.', '#', '#', '#', '.', '#', '#', '.'},
            {'.', '.', '.', '.', '.', '.', '.', '#', '.', '.'},
            {'.', '#', '#', '#', '.', '#', '.', '#', '.', '#'},
            {'.', '.', '.', '#', '.', '.', '.', '.', '.', END},
            {'#', '#', '.', '#', '#', '#', '#', '#', '.', '#'}
        },
        // Maze 2 - Optimal: 20 steps
        {
            {START, '.', '#', '.', '.', '.', '#', '.', '.', '.'},
            {'.', '.', '#', '.', '#', '.', '#', '.', '#', '.'},
            {'#', '.', '.', '.', '#', '.', '.', '.', '#', '.'},
            {'#', '#', '#', '.', '#', '#', '#', '.', '#', '.'},
            {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
            {'.', '#', '#', '#', '.', '#', '#', '#', '#', '#'},
            {'.', '.', '.', '#', '.', '.', '.', '.', '.', '.'},
            {'#', '#', '.', '#', '#', '#', '.', '#', '#', '.'},
            {'.', '.', '.', '.', '.', '#', '.', '.', '.', END},
            {'.', '#', '#', '#', '.', '#', '#', '#', '.', '#'}
        },
        // Maze 3 - Optimal: 22 steps
        {
            {START, '.', '.', '.', '#', '.', '.', '.', '.', '.'},
            {'#', '#', '#', '.', '#', '.', '#', '#', '#', '.'},
            {'.', '.', '.', '.', '.', '.', '.', '.', '#', '.'},
            {'.', '#', '#', '#', '#', '#', '#', '.', '#', '.'},
            {'.', '.', '.', '.', '.', '.', '.', '.', '#', '.'},
            {'#', '#', '#', '.', '#', '#', '#', '.', '#', '.'},
            {'.', '.', '#', '.', '.', '.', '#', '.', '.', '.'},
            {'.', '.', '#', '#', '#', '.', '#', '#', '#', '#'},
            {'.', '.', '.', '.', '#', '.', '.', '.', '.', END},
            {'#', '#', '#', '.', '#', '#', '#', '#', '.', '#'}
        },
        // Maze 4 - Optimal: 24 steps
        {
            {START, '.', '#', '.', '.', '.', '.', '#', '.', '.'},
            {'.', '.', '#', '#', '#', '#', '.', '#', '.', '#'},
            {'.', '.', '.', '.', '.', '#', '.', '.', '.', '#'},
            {'#', '#', '#', '#', '.', '#', '.', '#', '.', '#'},
            {'.', '.', '.', '.', '.', '.', '.', '#', '.', '.'},
            {'.', '#', '#', '#', '#', '#', '.', '#', '#', '.'},
            {'.', '.', '.', '.', '.', '#', '.', '.', '.', '.'},
            {'#', '#', '#', '.', '.', '#', '#', '#', '.', '#'},
            {'.', '.', '#', '.', '.', '.', '.', '#', '.', END},
            {'.', '.', '#', '#', '#', '#', '.', '#', '.', '#'}
        }
    };
    
    int optimal_paths[4] = {18, 20, 22, 24};
    
    // Randomly select a maze
    int maze_idx = rand() % 4;
    char maze[MAZE_SIZE][MAZE_SIZE];
    for(int i = 0; i < MAZE_SIZE; i++) {
        for(int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = mazes[maze_idx][i][j];
        }
    }
    int optimal_path = optimal_paths[maze_idx];
    
    display_maze(maze);
    
    printf("\nEnter your path (e.g., SSEEENNNEE) or 'skip': ");
    char path[MAX_PATH_LENGTH];
    fgets(path, sizeof(path), stdin);
    path[strcspn(path, "\n")] = 0;
    
    if (strcmp(path, "skip") == 0) {
        printf("\n❌ Challenge skipped!\n");
        pause_screen();
        return;
    }
    
    // Simulate the path
    int x = 0, y = 0; // Start position
    int valid = 1;
    int steps = strlen(path);
    
    for(int i = 0; i < steps && valid; i++) {
        char move = toupper(path[i]);
        int new_x = x, new_y = y;
        
        switch(move) {
            case 'N': new_x--; break;
            case 'S': new_x++; break;
            case 'E': new_y++; break;
            case 'O': new_y--; break;
            default: valid = 0; break;
        }
        
        if (new_x < 0 || new_x >= MAZE_SIZE || new_y < 0 || new_y >= MAZE_SIZE ||
            maze[new_x][new_y] == WALL) {
            valid = 0;
        } else {
            x = new_x;
            y = new_y;
            if (maze[x][y] != START && maze[x][y] != END) {
                maze[x][y] = PATH;
            }
        }
    }
    
    if (valid && x == 9 && y == 9) {
        printf("\n✅ You reached the end!\n");
        display_maze(maze);
        
        // Score based on path length (shorter is better)
        int score = POINTS_ROBOT - (steps - optimal_path);
        if (score < 10) score = 10;
        
        printf("\n🎯 Path length: %d steps\n", steps);
        printf("🎯 Score earned: %d points\n", score);
        
        if (current_player.robot_score == 0) {
            current_player.challenges_completed++;
        }
        
        if (score > current_player.robot_score) {
            current_player.total_score = current_player.total_score - current_player.robot_score + score;
            current_player.robot_score = score;
            save_player_scores(&current_player);
            printf("🏆 New personal best!\n");
        }
    } else {
        printf("\n❌ Invalid path! You hit a wall or didn't reach the end.\n");
        display_maze(maze);
    }
    
    pause_screen();
}


/*
================================================================================
    CHALLENGE 4: TRI ULTIME
    Sort an array with limited operations
================================================================================
*/

void challenge_tri_ultime() {
    clear_screen();
    display_header("CHALLENGE 4: Tri Ultime");
    
    printf("🎯 Objective: Sort the array using swap operations\n");
    printf("   Minimize the number of swaps!\n\n");
    
    // Generate random array
    int arr[8];
    int n = 8;
    int used[90] = {0}; // Track used numbers (10-99)
    
    for(int i = 0; i < n; i++) {
        int num;
        do {
            num = (rand() % 90) + 10; // 10-99
        } while(used[num - 10]);
        used[num - 10] = 1;
        arr[i] = num;
    }
    
    printf("Initial array: ");
    for(int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
    
    printf("Enter swap operations (format: index1 index2, e.g., 0 1)\n");
    printf("Type 'done' when finished, 'skip' to skip\n\n");
    
    int swaps = 0;
    char input[100];
    
    while(1) {
        printf("Current array: ");
        for(int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
        
        printf("Swap (or 'done'/'skip'): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "done") == 0) break;
        if (strcmp(input, "skip") == 0) {
            printf("\n❌ Challenge skipped!\n");
            pause_screen();
            return;
        }
        
        int idx1, idx2;
        if (sscanf(input, "%d %d", &idx1, &idx2) == 2) {
            if (idx1 >= 0 && idx1 < n && idx2 >= 0 && idx2 < n) {
                int temp = arr[idx1];
                arr[idx1] = arr[idx2];
                arr[idx2] = temp;
                swaps++;
            } else {
                printf("❌ Invalid indices!\n");
            }
        } else {
            printf("❌ Invalid format!\n");
        }
    }
    
    // Check if sorted
    int sorted = 1;
    for(int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i+1]) {
            sorted = 0;
            break;
        }
    }
    
    if (sorted) {
        // Score based on number of swaps (fewer is better)
        // Use difficulty-based scoring: fewer swaps = higher score
        int score = POINTS_TRI - (swaps / 2);
        if (score < 5) score = 5;
        
        printf("\n✅ Array sorted successfully!\n");
        printf("🎯 Number of swaps: %d\n", swaps);
        printf("🎯 Score earned: %d points\n", score);
        
        if (current_player.tri_score == 0) {
            current_player.challenges_completed++;
        }
        
        if (score > current_player.tri_score) {
            current_player.total_score = current_player.total_score - current_player.tri_score + score;
            current_player.tri_score = score;
            save_player_scores(&current_player);
            printf("🏆 New personal best!\n");
        }
    } else {
        printf("\n❌ Array is not sorted correctly!\n");
    }
    
    pause_screen();
}


/*
================================================================================
    CHALLENGE 5: TOUR DE HANOI
    Solve the Tower of Hanoi puzzle
================================================================================
*/

void display_hanoi_state(int towers[3][10], int counts[3], int disks) {
    printf("\n");
    for(int level = disks - 1; level >= 0; level--) {
        printf("  ");
        for(int tower = 0; tower < 3; tower++) {
            if (counts[tower] > level) {
                int disk = towers[tower][level];
                for(int i = 0; i < disk; i++) printf("█");
                for(int i = disk; i < disks; i++) printf(" ");
            } else {
                for(int i = 0; i < disks; i++) printf(" ");
            }
            printf("  ");
        }
        printf("\n");
    }
    printf("  ");
    for(int i = 0; i < disks; i++) printf("═");
    printf("  ");
    for(int i = 0; i < disks; i++) printf("═");
    printf("  ");
    for(int i = 0; i < disks; i++) printf("═");
    printf("\n");
    printf("     A");
    for(int i = 0; i < disks - 1; i++) printf(" ");
    printf("    B");
    for(int i = 0; i < disks - 1; i++) printf(" ");
    printf("    C\n");
}

void challenge_tour_hanoi() {
    clear_screen();
    display_header("CHALLENGE 5: Tour de Hanoi");
    
    printf("🎯 Objective: Move all disks from tower A to tower C\n");
    printf("   Rules:\n");
    printf("   • Only one disk can be moved at a time\n");
    printf("   • A larger disk cannot be placed on a smaller disk\n\n");
    
    // Randomize number of disks (3-5)
    int disks = (rand() % 3) + 3;
    int towers[3][10] = {0};
    int counts[3] = {disks, 0, 0};
    
    // Initialize tower A with disks (largest to smallest)
    for(int i = 0; i < disks; i++) {
        towers[0][i] = disks - i;
    }
    
    display_hanoi_state(towers, counts, disks);
    
    int moves = 0;
    int min_moves = (1 << disks) - 1; // 2^n - 1
    
    printf("\nMinimum possible moves: %d\n", min_moves);
    printf("\nEnter moves (format: A B to move from tower A to B)\n");
    printf("Type 'skip' to skip\n\n");
    
    while(1) {
        if (counts[2] == disks) {
            // Win condition
            printf("\n✅ Congratulations! You solved the Tower of Hanoi!\n");
            
            // Score based on number of moves
            int score = POINTS_HANOI - (moves - min_moves) * 2;
            if (score < 10) score = 10;
            
            printf("🎯 Moves used: %d (optimal: %d)\n", moves, min_moves);
            printf("🎯 Score earned: %d points\n", score);
            
            if (current_player.hanoi_score == 0) {
                current_player.challenges_completed++;
            }
            
            if (score > current_player.hanoi_score) {
                current_player.total_score = current_player.total_score - current_player.hanoi_score + score;
                current_player.hanoi_score = score;
                save_player_scores(&current_player);
                printf("🏆 New personal best!\n");
            }
            break;
        }
        
        printf("Move: ");
        char input[10];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "skip") == 0) {
            printf("\n❌ Challenge skipped!\n");
            pause_screen();
            return;
        }
        
        char from, to;
        if (sscanf(input, "%c %c", &from, &to) == 2) {
            from = toupper(from);
            to = toupper(to);
            
            int from_idx = from - 'A';
            int to_idx = to - 'A';
            
            if (from_idx < 0 || from_idx > 2 || to_idx < 0 || to_idx > 2) {
                printf("❌ Invalid towers! Use A, B, or C.\n");
                continue;
            }
            
            if (counts[from_idx] == 0) {
                printf("❌ Source tower is empty!\n");
                continue;
            }
            
            int disk = towers[from_idx][counts[from_idx] - 1];
            
            if (counts[to_idx] > 0 && towers[to_idx][counts[to_idx] - 1] < disk) {
                printf("❌ Cannot place larger disk on smaller disk!\n");
                continue;
            }
            
            // Valid move
            towers[from_idx][counts[from_idx] - 1] = 0;
            counts[from_idx]--;
            towers[to_idx][counts[to_idx]] = disk;
            counts[to_idx]++;
            moves++;
            
            display_hanoi_state(towers, counts, disks);
        } else {
            printf("❌ Invalid format! Use: A B\n");
        }
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