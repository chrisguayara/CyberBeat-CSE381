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

        TArray<FString> Parts;
        Line.ParseIntoArray(Parts, TEXT(","), true);
        if (Parts.Num() == 0) continue;

        FCyberBeatMusicEvent NewEvent;
        NewEvent.TimeSeconds = FCString::Atof(*Parts[0].TrimStartAndEnd());

        const FString TypeText = Parts.Num() > 1 ? Parts[1].TrimStartAndEnd() : TEXT("Beat");
        if (TypeText.Equals(TEXT("Accent"), ESearchCase::IgnoreCase))            NewEvent.EventType = EMusicEventType::Accent;
        else if (TypeText.Equals(TEXT("SectionStart"), ESearchCase::IgnoreCase)) NewEvent.EventType = EMusicEventType::SectionStart;
        else                                                                     NewEvent.EventType = EMusicEventType::Beat;

        NewEvent.Lane = Parts.Num() > 2 ? FCString::Atoi(*Parts[2].TrimStartAndEnd()) : -1;
        MusicEvents.Add(NewEvent);
    }

    if (MusicEvents.Num() == 0 && BPM > 0.f && MusicTrack)
    {
        const float Duration = MusicTrack->GetDuration();
        const float Length = (Duration > 0.f && Duration < 36000.f) ? Duration : 180.f;
        const float Interval = 60.f / BPM;
        const int32 PerBar = FMath::Max(BeatsPerBar, 1);

        for (int32 Beat = 0; Beat * Interval < Length; ++Beat)
        {
            const int32 Bar = Beat / PerBar + 1;

            // The section this beat falls in: the last one that has started
            const FCyberBeatSection* Section = nullptr;
            for (const FCyberBeatSection& S : Sections)
            {
                if (S.StartBar <= Bar) Section = &S;
            }

            int32 Stride = 1;                
            int32 BeatsIntoSection = Beat;
            if (Sections.Num() > 0)
            {
                if (!Section) continue;       // before the first section
                Stride = Section->EnemyEveryNBeats;
                BeatsIntoSection = Beat - (Section->StartBar - 1) * PerBar;
            }
            if (Stride <= 0 || BeatsIntoSection % Stride != 0) continue;

            FCyberBeatMusicEvent E;
            E.TimeSeconds = Beat * Interval;
            E.EventType = (Beat % PerBar == 0) ? EMusicEventType::Accent : EMusicEventType::Beat;
            MusicEvents.Add(E);
        }
    }

        MusicEvents.Sort([](const FCyberBeatMusicEvent& A, const FCyberBeatMusicEvent& B)
            {
                return A.TimeSeconds < B.TimeSeconds;
            });
}