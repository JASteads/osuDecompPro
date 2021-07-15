#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef OSU_DECOMP_PRO_H
#define OSU_DECOMP_PRO_H

#define LENGTH(arr) sizeof(arr) / sizeof(*arr)
#define STOP 1 << 31

/***********************************************************************/
/*          .OSU DECOMPILER PRO HEADER                                 */
/*                                                                     */
/*  Programmed by : Jonavinne Steadham                                 */
/*  Date Written : 07/09/2021                                          */
/*  Description : Header for .Osu Decompiler Pro                       */
/*                                                                     */
/*  NOTE : See full info on .osu file format at                        */
/*  https://osu.ppy.sh/wiki/en/osu!_File_Formats/Osu_(file_format)     */
/***********************************************************************/


struct Point
{ int x, y; };

struct BreakPeriod
{
    int bMark, // The first indicator of a break period. Will be either "2" or "Break"
        start, // Starting point of a break period
        end;   // Ending point of a break period
};

struct TimingPoint
{
    double  length, // Duration of timing point
            time;   // The ms in which the timing point is passed
    int meter, sSet, sIndex,
        vol, uninherited, fx;
};

struct SliderData
{
    struct Point curvePoint[10];
    int     slides,             // Number of bounces
            edgeSound[10],      // First hitsound
            edgeNorm[10],       // Bounce hitsound
            edgeAddition[10];   // Bounce add. hitsound
    double length;              // Slider length in ms
    char curveType;             // Manipulates how slider is drawn
};

struct OsuObject
{
    struct Point coord;
    int time, type, hitSound, spinnerEnd;
    int normal, addition, index, volume;
    char *fileName;
    struct SliderData sliderData;
};

struct OsuFile
{
    // QUANTITIES
    int countBookmark, countTiming, countBreak, countObj;

    // GENERAL
    char audioName[50], sampleSet[6];
    int audioLeadin, previewTime, countdown, mode, widescreen, letterbox;
    double stackLeniency;

    // EDITOR
    int *bookmarks;
    int beatDivisor, gridSize;
    double timelineZoom, distanceSpacing;

    // METADATA
    char title[64], titleUni[64], artist[64], artistUni[64],
        creator[64], version[64], source[64], tags[256];
    int beatmapId, beatmapSetId;

    // DIFFICULTY
    double HP, circleSize, diff, AR, sliderMulti;
    int sliderTick;

    // EVENTS : Storyboard data not yet supported
    struct BreakPeriod *bPeriods;

    // TIMING POINTS

    int colors[8];
    struct TimingPoint *tPoints;
    struct OsuObject *hitObjects;
};

extern void ReadElement(char *beatInfo, int *element, int *pos);
extern void ReadElementDouble(char *textBuffer, double *element, int *pos);
extern void ReadLine(char *textBuffer, int size, FILE *osuFile, int addSpace);

struct OsuFile ImportHeader(FILE *osuFile);

char *BookmarksToStr(int *bookmarks, int length);
char *BreaksToStr(struct BreakPeriod *breaks, int length);
char *TPointsToStr(struct TimingPoint *tpoints, int length);
char *ColorsToStr(int *colors);
char *ObjectsToStr(struct OsuObject *objects, int length);

void ImportFile(char *filePath, struct OsuFile *osuFilePtr);
void ExportFile(struct OsuFile *osuData);

#endif