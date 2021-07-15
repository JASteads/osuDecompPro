#include "osuDecompPro.h"

#define MAX_BUFFER_SIZE 512
#define OBJ_TO_STR_CAP 200
#define HITOBJ_CAP 1024
#define COLORS_CAP 21
#define READ_CHAR_CAP 30

/**************************************************/
/*              .OSU DECOMPILER PRO
/*
/*  Programmed by : Jonavinne Steadham
/*  Date Written : 07/09/2021
/*  Description : A small program that takes user
/*  input to open a valid .osu file and organize
/*  its components into modifiable data. Can also
/*  export modified data into new .osu file.
/*
/**************************************************/


/***************************************************/
/*  Function : ReadElement()
/*  Purpose  : Reads up to the nearest separator 
/*             from the textBuffer.
/***************************************************/
inline void ReadElement(char *textBuffer, int *element, int *pos)
{   
    char bufferResult[READ_CHAR_CAP] = {0}, *bufferPtr = bufferResult;

    while (*(textBuffer + *pos) != ',' && *(textBuffer + *pos) != ':' && *(textBuffer + *pos) != '|' && *(textBuffer + *pos) != '\n')
        *(bufferPtr++) = *(textBuffer + (*pos)++);
    ++*pos;
    *element = atoi(bufferResult);
}

/***************************************************/
/*  Function : ReadElementDouble()
/*  Purpose  : Reads entire textBuffer, but stops if
/*             next character is ':'. 'element' is a
/*             double.
/***************************************************/
inline void ReadElementDouble(char *textBuffer, double *element, int *pos)
{
    char bufferResult[READ_CHAR_CAP] = {0}, *bufferPtr = bufferResult;

    while (*(textBuffer + *pos) != ',' && *(textBuffer + *pos) != '\n')
    {
        *bufferPtr = *(textBuffer + *pos);
        ++bufferPtr, ++*pos;
    }
    ++*pos;
    *element = atof(bufferResult);
}

/***************************************************/
/*  Function : ReadLine()
/*  Purpose  : Reads entire textBuffer, but stops if
/*             next character is ':'.
/***************************************************/
inline void ReadLine(char *textBuffer, int size, FILE *osuFile, int addSpace)
{
    char sample = 0;

    while (sample != ':') { sample = fgetc(osuFile); }

    // Skip a character after finding colon if addSpace is a non-zero
    if (addSpace) sample = fgetc(osuFile);

    fgets(textBuffer, size, osuFile);
}

/***************************************************/
/*  Function : ImportHeader()
/*  Purpose  : Imports and fills an OsuFile object
/*             with contents of osuFile. Stops after
/*             [Colors] section, the end of the .osu
/*             header.
/***************************************************/
struct OsuFile ImportHeader(FILE *osuFile)
{
    char textBuffer[MAX_BUFFER_SIZE] = {0};
    struct OsuFile header = {0};

    for (int i = 0; i < 3; i++) 
        fgets(textBuffer, sizeof(textBuffer), osuFile);

    // GENERAL

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    strcpy(header.audioName, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.audioLeadin = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.previewTime = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.countdown = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    strcpy(header.sampleSet, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.stackLeniency = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.mode = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.letterbox = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.widescreen = atoi(textBuffer);

    /* Skip next two lines */
    for (int i = 0; i < 2; i++) fgets(textBuffer, sizeof(textBuffer), osuFile);


    // EDITOR

    /* Check for bookmarks. If none, skip this. */
    {
        char sample;
        if ((sample = fgetc(osuFile)) == 'B')
        {
            int pos = 0;

            while ((sample = fgetc(osuFile)) != '\n')
                if (sample == ',') ++header.countBookmark;
            header.bookmarks = (int *)calloc(++header.countBookmark, 4);

            fseek(osuFile, 0, SEEK_SET);
            for (int i = 0; i < 14; i++) fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
            for (int i = 0; i < 11; i++) sample = fgetc(osuFile);
            fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
            
            for (int i = 0; i < header.countBookmark; i++)
                ReadElement(textBuffer, &header.bookmarks[i], &pos);
        }
    }
    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.distanceSpacing = atof(textBuffer);
    
    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.beatDivisor = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.gridSize = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 1);
    header.timelineZoom = atof(textBuffer);


    // METADATA

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.title, textBuffer);
    header.title[strlen(header.title) - 1] = '\0';

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.titleUni, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.artist, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.artistUni, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.creator, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.version, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.source, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    strcpy(header.tags, textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.beatmapId = atoi(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.beatmapSetId = atoi(textBuffer);


    // DIFFICULTY

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.HP = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.circleSize = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.diff = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.AR = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.sliderMulti = atof(textBuffer);

    ReadLine(textBuffer, MAX_BUFFER_SIZE, osuFile, 0);
    header.sliderTick = atoi(textBuffer);

    for (int i = 0; i < 4; i++) fgets(textBuffer, sizeof(textBuffer), osuFile);
    memset(textBuffer, 0, sizeof(textBuffer));


    // EVENTS : Storyboards are currently not supported.
    
    fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
    while (strcmp(textBuffer, "2,") == 1)
    {
        fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
        ++header.countBreak;
    }
    header.bPeriods = (struct BreakPeriod *)calloc(header.countBreak, sizeof(struct BreakPeriod));

    /* Reset file position and skip to Break Periods section */
    fseek(osuFile, 0, SEEK_SET);
    while(strcmp(textBuffer, "//Break Periods\n")) fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
    
    for (int i = 0; i < header.countBreak; i++)
    {
        int pos = 0;
        fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);

        ReadElement(textBuffer, &header.bPeriods[i].bMark, &pos);
        ReadElement(textBuffer, &header.bPeriods[i].start, &pos);
        ReadElement(textBuffer, &header.bPeriods[i].end, &pos);
    }

    /* Skip next eight lines */
    for (int i = 0; i < 8; i++) fgets(textBuffer, sizeof(textBuffer), osuFile);
    
    
    // TIMING POINTS

    if (!strcmp(textBuffer, "[TimingPoints]\n"))
    {
        /* Get the number of timing points for dynamic allocation of tPoints capacity. */
        while(strcmp(textBuffer, "\n"))
        {
            fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
            ++header.countTiming;
        }
        header.tPoints = (struct TimingPoint *)calloc(--header.countTiming, sizeof(struct TimingPoint));

        /* Reset file position and skip to TimingPoints section */
        fseek(osuFile, 0, SEEK_SET);
        while(strcmp(textBuffer, "[TimingPoints]\n")) 
            fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);

        /* Skip a line */
        fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);

        /* Timing Point copy loop */
        for (int i = 0; i < header.countTiming; i++)
        {
            int pos = 0;
            ReadElementDouble(textBuffer, &header.tPoints[i].time, &pos);
            ReadElementDouble(textBuffer, &header.tPoints[i].length, &pos);
            ReadElement(textBuffer, &header.tPoints[i].meter, &pos);
            ReadElement(textBuffer, &header.tPoints[i].sSet, &pos);
            ReadElement(textBuffer, &header.tPoints[i].sIndex, &pos);
            ReadElement(textBuffer, &header.tPoints[i].vol, &pos);
            ReadElement(textBuffer, &header.tPoints[i].uninherited, &pos);
            ReadElement(textBuffer, &header.tPoints[i].fx, &pos);
            fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
        }

        for (int i = 0; i < 2; i++) fgets(textBuffer, sizeof(textBuffer), osuFile);
    }


    // COLORS

    if (!strcmp(textBuffer, "[Colours]\n"))
        /* There can be a maximum of 8 colors, so loop up to 8 times */
        for (int i = 0; i < 8; i++)
        {
            fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
            if (*textBuffer != '\n')
            {
                /* Copy color values to r, g, and b, then use consolodate into colors[i]. */
                int r, g, b, pos = 9;
                ReadElement(textBuffer, &r, &pos);
                ReadElement(textBuffer, &g, &pos);
                ReadElement(textBuffer, &b, &pos);
                header.colors[i] = (unsigned char)r | (unsigned char)g << 8 | (unsigned char)b << 16;
            }
            else 
            {
                /* STOP is used when exporting data into new file. */
                header.colors[i] = STOP;
                break; 
            }
        }
    else { *header.colors = STOP; }

    return header;
}


/***************************************************/
/*  Function : BookmarksToStr()
/*  Purpose  : Converts the data from bookmarks
/*             pointer into a string.
/***************************************************/
char *BookmarksToStr(int *bookmarks, int length)
{
    if (bookmarks)
    {
        /* Set to 11 to skip "Bookmarks: " text once appended. */
        int pos = 11;
        char *result = (char *)malloc(OBJ_TO_STR_CAP * length);
        
        sprintf(result, "%s", "Bookmarks: ");
        for (int i = 0; i < length; i++)
        {
            pos += strlen(itoa(bookmarks[i], result + pos, 10));
            if (i != length - 1) *(result + (pos++)) = ',';
        }
        *(result + pos) = '\n', ++pos;
        *(result + pos) = 0;
        return result;
    }
    return "";
}


/***************************************************/
/*  Function : BreaksToStr()
/*  Purpose  : Converts the data from breaks
/*             pointer into a string.
/***************************************************/
char *BreaksToStr(struct BreakPeriod *breaks, int length)
{
    if (breaks)
    {
        int pos = 0;
        char *result = (char *)malloc(OBJ_TO_STR_CAP * length);

        for (int i = 0; i < length; i++)
        {
            pos += strlen(itoa(2, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(breaks[i].start, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(breaks[i].end, result + pos, 10));
            *(result + (pos++)) = '\n';
        }
        *(result + pos) = 0;
        return result;
    }
    return "";
}

/***************************************************/
/*  Function : TPointsToStr()
/*  Purpose  : Converts the data from tpoints
/*             pointer into a string.
/***************************************************/
char *TPointsToStr(struct TimingPoint *tpoints, int length)
{
    if (tpoints)
    {
        int pos = 0;
        char *result = (char *)malloc(OBJ_TO_STR_CAP * length);
        char *const DOUBLE_FORMAT = "%.15g";

        char tempBuffer[READ_CHAR_CAP], *bufferPtr = tempBuffer;
        for (int i = 0; i < length; i++)
        {
            sprintf(bufferPtr, DOUBLE_FORMAT, tpoints[i].time);
            sprintf(result + pos, "%s", bufferPtr);

            pos += strlen(tempBuffer);
            *(result + pos++) = ',';

            sprintf(bufferPtr, DOUBLE_FORMAT, tpoints[i].length);
            sprintf(result + pos, "%s", bufferPtr);

            pos += strlen(tempBuffer);
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].meter, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].sSet, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].sIndex, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].vol, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].uninherited, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(tpoints[i].fx, result + pos, 10));
            *(result + (pos++)) = '\n';
        }
        *(result + (pos++)) = '\n';
        *(result + pos) = 0;
        return result;
    }
    return "";
}


/***************************************************/
/*  Function : ColorsToStr()
/*  Purpose  : Converts the data from colors
/*             pointer into a string.
/***************************************************/
char *ColorsToStr(int *colors)
{
    if (!(*colors == STOP))
    {
        /* Set to 10 to skip "[Colours]\n" text. */
        int pos = 10;
        char *result = calloc(COLORS_CAP, 8);
        

        strcat(result, "[Colours]\n");

        /* There are a maximum of 8 colors, so loop up to 8 times. */
        for (int i = 0; i < 8; i++)
            if (!(colors[i] == STOP))
            {
                /* One colorLine can only be a maximum of 21 characters long. */
                /* Example: "Combo1 : 255,255,255\n"                          */
                char colorLine[21], *colorPtr = colorLine;
                sprintf(colorPtr, "Combo%d : %d,%d,%d\n", i + 1,
                (int)((unsigned char)colors[i]),                        // B
                (int)((unsigned char)((colors[i] & 0x00FF00) >> 8)),    // G
                (int)((unsigned char)((colors[i] & 0xFF0000) >> 16)));  // R
                strcat(result + pos, colorLine);
                pos += strlen(colorLine);
            }
            else 
            {
                *(result + (pos++)) = '\n';
                break; 
            }
        return result;
    }
    return "";
}


/***************************************************/
/*  Function : ObjectsToStr()
/*  Purpose  : Converts the data from objects
/*             pointer into a string.
/***************************************************/
char *ObjectsToStr(struct OsuObject *objects, int length)
{
    if (objects)
    {
        int pos = 0;
        char *result = (char *)malloc(HITOBJ_CAP * length);
        char tempBuffer[READ_CHAR_CAP], *bufferPtr = tempBuffer;

        for (int i = 0; i < length; i++)
        {
            if (objects[i].coord.x == STOP) { break; }
            pos += strlen(itoa(objects[i].coord.x, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(objects[i].coord.y, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(objects[i].time, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(objects[i].type, result + pos, 10));
            *(result + pos++) = ',';
            pos += strlen(itoa(objects[i].hitSound, result + pos, 10));
            *(result + pos++) = ',';

            /* If object is a spinner ... */
            if (objects[i].type & 8)
            {
                pos += strlen(itoa(objects[i].spinnerEnd, result + pos, 10));
                *(result + pos++) = ',';
            }

            /* If object is a slider ...  */
            if (objects[i].type & 2)
            {
                int curvePointCount = 0;

                sprintf(result + pos++, "%c", objects[i].sliderData.curveType);

                // Curve Points
                for (int x = 0; x < 10; x++)
                {
                    if (objects[i].sliderData.curvePoint[x].x != STOP)
                    {
                        *(result + pos++) = '|';
                        pos += strlen(itoa(objects[i].sliderData.curvePoint[x].x, result + pos, 10));
                        *(result + pos++) = ':';
                        pos += strlen(itoa(objects[i].sliderData.curvePoint[x].y, result + pos, 10));
                        ++curvePointCount;
                    }
                    else
                    {
                        *(result + pos++) = ',';
                        break;
                    }
                }

                // Slides
                pos += strlen(itoa(objects[i].sliderData.slides, result + pos, 10));
                *(result + pos++) = ',';

                // Slider Length
                sprintf(bufferPtr, "%.15g", objects[i].sliderData.length);
                sprintf(result + pos, "%s", bufferPtr);
                pos += strlen(tempBuffer);
                *(result + pos++) = ',';

                // Edge Sounds

                if (*objects[i].sliderData.edgeSound != STOP)
                {
                    for (int x = 0; x <= objects[i].sliderData.slides; x++)
                    {
                        pos += strlen(itoa(objects[i].sliderData.edgeSound[x], result + pos, 10));
                        if (x < objects[i].sliderData.slides) *(result + pos++) = '|';
                    }
                    *(result + pos++) = ',';

                    // Edge Sets
                    for (int x = 0; x <= objects[i].sliderData.slides; x++)
                    {
                        /* Search for edge sounds, if any. Skip if none. */
                        if (!(objects[i].sliderData.edgeNorm[x] == STOP && objects[i].sliderData.edgeAddition[x] == STOP))
                        {
                            pos += strlen(itoa(objects[i].sliderData.edgeNorm[x], result + pos, 10));
                            *(result + pos++) = ':';
                            pos += strlen(itoa(objects[i].sliderData.edgeAddition[x], result + pos, 10));
                            if (x < objects[i].sliderData.slides) *(result + pos++) = '|';
                        }
                        else { break; }
                    }
                    *(result + pos++) = ',';
                }
            }
            if (*objects[i].sliderData.edgeSound != STOP)
            {
                pos += strlen(itoa(objects[i].normal, result + pos, 10));
                *(result + pos++) = ':';
                pos += strlen(itoa(objects[i].addition, result + pos, 10));
                *(result + pos++) = ':';
                pos += strlen(itoa(objects[i].index, result + pos, 10));
                *(result + pos++) = ':';
                pos += strlen(itoa(objects[i].volume, result + pos, 10));
                *(result + pos++) = ':';
                if (objects[i].fileName) sprintf(result + pos++, "%s", objects[i].fileName);
            }
            else { --pos; }
            *(result + pos++) = '\n';
        }

        *(result + pos) = 0;
        return result;
    }
    return "";
}

/***************************************************/
/*  Function : ImportFile()
/*  Purpose  : If filePath is valid, create new
/*             OsuFile object and fill it with .osu
/*             file contents.
/***************************************************/
void ImportFile(char *filePath, struct OsuFile *osuFilePtr)
{
    FILE *osuFile = fopen(filePath, "r");

    if (osuFile)
    {
        char textBuffer[MAX_BUFFER_SIZE];

        if (feof(osuFile)) fclose(osuFile);
        else
        {
            /* Stores all information prior to the HitObjects section. */
            *osuFilePtr = ImportHeader(osuFile);

            // HIT OBJECTS

            /* File will be at the "[HitObjects]" line by this point. Skip next line */
            fgets(textBuffer, sizeof(textBuffer), osuFile);

            /* This information isn't used later. Keep local to be removed. */
            {
                int offsetNum = 0;
                while (!feof(osuFile))
                {
                    if (fgetc(osuFile) == '\n') ++osuFilePtr->countObj, ++offsetNum;
                    ++offsetNum;
                }
                osuFilePtr->hitObjects = (struct OsuObject *)calloc(++osuFilePtr->countObj, sizeof(struct OsuObject));

                fclose(osuFile);
                osuFile = fopen(filePath, "r");

                do { fgets(textBuffer, MAX_BUFFER_SIZE, osuFile); }
                while (strcmp(textBuffer, "[HitObjects]\n"));
            }
            /* Grab data for every beat in file. */
            if (osuFilePtr->hitObjects)
            {
                for (int i = 0; i <= osuFilePtr->countObj; i++)
                {
                    char *textBufferPtr = fgets(textBuffer, MAX_BUFFER_SIZE, osuFile);
                    if (textBufferPtr)
                    {
                        int pos = 0;

                        /* Basic object information */
                        ReadElement(textBuffer, &osuFilePtr->hitObjects[i].coord.x, &pos);
                        ReadElement(textBuffer, &osuFilePtr->hitObjects[i].coord.y, &pos);
                        ReadElement(textBuffer, &osuFilePtr->hitObjects[i].time, &pos);
                        ReadElement(textBuffer, &osuFilePtr->hitObjects[i].type, &pos);
                        ReadElement(textBuffer, &osuFilePtr->hitObjects[i].hitSound, &pos);
                        if (osuFilePtr->hitObjects[i].type & 8)
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].spinnerEnd, &pos);

                        /* If beat is a slider ... */
                        if ((osuFilePtr->hitObjects[i].type & 2) == 2)
                        {
                            int curvePointPos = 0;

                            osuFilePtr->hitObjects[i].sliderData.curveType = *(textBuffer + pos);
                            pos += 2;
                            do
                            {
                                ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.curvePoint[curvePointPos].x, &pos);
                                ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.curvePoint[curvePointPos].y, &pos);
                                ++curvePointPos;
                            } while (*(textBuffer + pos - 1) != ',');
                            osuFilePtr->hitObjects[i].sliderData.curvePoint[curvePointPos].x = STOP;
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.slides, &pos);
                            ReadElementDouble(textBuffer, &osuFilePtr->hitObjects[i].sliderData.length, &pos);
                            
                            /* If slider has more infortmation ... */
                            if (*(textBuffer + pos))
                            {
                                for (int x = 0; x <= LENGTH(osuFilePtr->hitObjects[i].sliderData.edgeSound); x++)
                                {
                                    ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.edgeSound[x], &pos);
                                    if (*(textBuffer + pos - 1) == ',') 
                                    {
                                        osuFilePtr->hitObjects[i].sliderData.edgeSound[x + 1] = STOP;
                                        break;
                                    }
                                }
                                for (int x = 0; x <= LENGTH(osuFilePtr->hitObjects[i].sliderData.edgeNorm); x++)
                                {
                                    ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.edgeNorm[x], &pos);
                                    ReadElement(textBuffer, &osuFilePtr->hitObjects[i].sliderData.edgeAddition[x], &pos);
                                    if (*(textBuffer + pos - 1) == ',')
                                    {
                                        osuFilePtr->hitObjects[i].sliderData.edgeNorm[x + 1] = STOP;
                                        osuFilePtr->hitObjects[i].sliderData.edgeAddition[x + 1] = STOP;
                                        break;
                                    }
                                }
                            }
                            else { *osuFilePtr->hitObjects[i].sliderData.edgeSound = STOP; }
                        }

                        /* Sound info for hit object. */
                        if (*(textBuffer + pos))
                        {
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].normal, &pos);
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].addition, &pos);
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].index, &pos);
                            ReadElement(textBuffer, &osuFilePtr->hitObjects[i].volume, &pos);
                        }
                    }
                    else 
                    { 
                        osuFilePtr->hitObjects[i].coord.x = STOP; 
                        break;
                    }
                }
            }
            else { perror("File failed to open."); }
            fclose(osuFile);
        }
    }
}

/***************************************************/
/*  Function : ExportFile()
/*  Purpose  : Writes new .osu file using the data
/*             in osuData.
/***************************************************/
void ExportFile(struct OsuFile *osuData)
{
    char fileNameFinal[MAX_BUFFER_SIZE] = {0};

    sprintf(fileNameFinal, ".\\%[^\n][%[^\n]].osu", osuData->title, osuData->diff);
    FILE *osuFile = fopen(fileNameFinal, "w");
    if (errno)
        perror("File failed to open");
    if (osuFile)
        fprintf(osuFile,
            "osu file format v14\n\n[General]\nAudioFilename: %sAudioLeadIn: %d\nPreviewTime: %d\nCountdown: %d\nSampleSet: %sStackLeniency: %g\nMode: %d\nLetterboxInBreaks: %d\nWidescreenStoryboard: %d\n\n[Editor]\n%sDistanceSpacing: %g\nBeatDivisor: %d\nGridSize: %d\nTimelineZoom: %g\n\n[Metadata]\nTitle:%s\nTitleUnicode:%sArtist:%sArtistUnicode:%sCreator:%sVersion:%sSource:%sTags:%sBeatmapID:%d\nBeatmapSetID:%d\n\n[Difficulty]\nHPDrainRate:%g\nCircleSize:%g\nOverallDifficulty:%g\nApproachRate:%g\nSliderMultiplier:%g\nSliderTickRate:%d\n\n[Events]\n//Background and Video events\n//Break Periods\n%s//Storyboard Layer 0 (Background)\n//Storyboard Layer 1 (Fail)\n//Storyboard Layer 2 (Pass)\n//Storyboard Layer 3 (Foreground)\n//Storyboard Layer 4 (Overlay)\n//Storyboard Sound Samples\n\n[TimingPoints]\n%s\n%s[HitObjects]\n%s", 
            osuData->audioName, osuData->audioLeadin, osuData->previewTime, osuData->countdown, osuData->sampleSet,
            osuData->stackLeniency, osuData->mode, osuData->letterbox, osuData->widescreen,
            BookmarksToStr(osuData->bookmarks, osuData->countBookmark),
            osuData->distanceSpacing, osuData->beatDivisor, osuData->gridSize, osuData->timelineZoom,
            osuData->title, osuData->titleUni, osuData->artist, osuData->artistUni, osuData->creator,
            osuData->version, osuData->source, osuData->tags, osuData->beatmapId, osuData->beatmapSetId,
            osuData->HP, osuData->circleSize, osuData->diff, osuData->AR, osuData->sliderMulti, osuData->sliderTick,
            BreaksToStr(osuData->bPeriods, osuData->countBreak),
            TPointsToStr(osuData->tPoints, osuData->countTiming),
            ColorsToStr(osuData->colors),
            ObjectsToStr(osuData->hitObjects, osuData->countObj)
            );
    fclose(osuFile);
}