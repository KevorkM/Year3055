// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGM.h"

#include "TimerManager.h"


AShooterGM::AShooterGM()
{
	TimeBetweenWaves = 2.f;
}

void AShooterGM::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;
}

void AShooterGM::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AShooterGM::SpawnBotTimerElapsed, 1.f, true, 0.f);
}

void AShooterGM::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	PrepareForNextWave();
}

void AShooterGM::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AShooterGM::StartWave, TimeBetweenWaves, false);
}

void AShooterGM::StartPlay()
{
}

void AShooterGM::Tick(float DeltaSeconds)
{
}
