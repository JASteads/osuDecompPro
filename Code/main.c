#include "osuDecompPro.h"

int main() 
{
    char filePath[500] = {0};

    printf("Enter location of osu file to open.\n\n>> ");
    fgets(filePath, sizeof(filePath), stdin);
    filePath[strcspn(filePath, "\n")] = 0;

    struct OsuFile osuFile = {0};

    ImportFile(filePath, &osuFile);
    ExportFile(&osuFile);

    printf("Done. Press Enter to close the window.");
    while (getchar() != '\n');

    return 0;
}