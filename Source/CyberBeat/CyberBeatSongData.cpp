// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatSongData.h"

void UCyberBeatSongData::ParseMusicEvents()
{
    MusicEvents.Empty();

    TArray<FString> Lines;
    MusicEventText.ParseIntoArrayLines(Lines);

    for (FString Line : Lines)
    {
        Line.TrimStartAndEndInline();

        if (Line.IsEmpty() || Line.StartsWith(TEXT("#")))
        {
            continue;
        }

        FString TimestampText;
        FString EventTypeText;

        if (Line.Split(TEXT(","), &TimestampText, &EventTypeText))
        {
            TimestampText.TrimStartAndEndInline();
            EventTypeText.TrimStartAndEndInline();
        }
        else
        {
            TimestampText = Line;
            EventTypeText = TEXT("Beat");
        }

        FCyberBeatMusicEvent NewEvent;

        NewEvent.TimeSeconds = FCString::Atof(*TimestampText);

        if (EventTypeText.Equals(TEXT("Accent"), ESearchCase::IgnoreCase))
        {
            NewEvent.EventType = EMusicEventType::Accent;
        }
        else if (EventTypeText.Equals(TEXT("SectionStart"), ESearchCase::IgnoreCase))
        {
            NewEvent.EventType = EMusicEventType::SectionStart;
        }
        else
        {
            NewEvent.EventType = EMusicEventType::Beat;
        }

        MusicEvents.Add(NewEvent);
    }

    MusicEvents.Sort([](const FCyberBeatMusicEvent& A, const FCyberBeatMusicEvent& B)
        {
            return A.TimeSeconds < B.TimeSeconds;
        });
}