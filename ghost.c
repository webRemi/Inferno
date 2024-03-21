#include <stdio.h>

int main() {
    // Orange color for flames
    printf("\033[38;5;208m");
    
    // Print the ASCII art
    printf("              .-') _               ('-.  _  .-')       .-') _                    \n");
    printf("             ( OO ) )            _(  OO)( \\( -O )     ( OO ) )                   \n");
    printf("  ,-.-') ,--./ ,--,'    ,------.(,------.,------. ,--./ ,--,'  .-'),-----.       \n");
    printf("  |  |OO)|   \\ |  |\\ ('-| _.---' |  .---'|   /`. '|   \\ |  |\\ ( OO'  .-.  '      \n");
    printf("  |  |  \\|    \\|  | )OO|(_\\      |  |    |  /  | ||    \\|  | )/   |  | |  |      \n");
    printf("  |  |(_/|  .     |/ /  |  '--. (|  '--. |  |_.' ||  .     |/ \\_) |  |\\|  |      \n");
    printf(" ,|  |_.'|  |\\    |  \\_)|  .--'  |  .--' |  .  '.'|  |\\    |    \\ |  | |  |      \n");
    printf("(_|  |   |  | \\   |    \\|  |_)   |  `---.|  |\\  \\ |  | \\   |     `'  '-'  '      \n");
    printf("  `--'   `--'  `--'     `--'     `------'`--' '--'`--'  `--'       `-----'       \n");
    
    // Metal-like color (gray)
    printf("\033[38;5;239m");
    
    // Reset colors and formatting
    printf("\033[0m");
    
    return 0;
}
