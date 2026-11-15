// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberBeatSongData.h"

#include "Sound/SoundBase.h"

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

    if (MusicEvents.Num() == 0 && BPM > 0.f && MusicTrack)
    {
        const float Duration = MusicTrack->GetDuration();
        const float Length = (Duration > 0.f && Duration < 36000.f) ? Duration : 180.f;
        const float Interval = 60.f / BPM;
        for (int32 i = 0; i * Interval < Length; ++i)
        {
            FCyberBeatMusicEvent E;
            E.TimeSeconds = i * Interval;
            E.EventType = EMusicEventType::Beat;
            MusicEvents.Add(E);
        }
    }

    MusicEvents.Sort([](const FCyberBeatMusicEvent& A, const FCyberBeatMusicEvent& B)
        {
            return A.TimeSeconds < B.TimeSeconds;
        });
}