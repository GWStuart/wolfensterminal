#include <stdio.h>
#include <unistd.h> // For STDIN_FILENO and read()
#include <termios.h> // For terminal manipulation

typedef struct{
    int postion_x;
    int position_y; 
    int angle;
    int speed;
} player_info;

void read_input(void) {
    struct termios old_settings, new_settings;

    // 1. Get current terminal settings and save them
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;

    // 2. Modify the settings for unbuffered input
    // ICANON: Disables canonical (line-buffered) mode.
    // ECHO: Disables echoing the input characters back to the screen.
    new_settings.c_lflag &= ~(ICANON | ECHO);
    
    // 3. Apply the new settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    printf("Enter characters. Press 'q' to quit.\n");

    char user_input;
    // 4. Loop to read one character at a time
    while (1) {
        // read() is often used here, but fgetc works too
        user_input = fgetc(stdin);

        if (user_input == 'q') {
            break;
        }
        
        // We have to manually print the character because ECHO is off
        printf("You pressed: %c (ASCII: %d)\n", user_input, user_input);
    }

    // 5. IMPORTANT: Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    printf("\nExiting program.\n");
}

int main(char** argv, int argc){
    read_input();

}