#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<assert.h>


typedef struct
{
	int day;
	int	month;
	int year;


}date;

typedef struct
{
	int playerID;
	char* PlayerName;
	int playerNumber;
	date playerBirthday;

}player;

typedef struct
{
	char* teamName;
	int Maxplayers;
	int Practiceplayers;
	player** Dynamicplayers;
	int WinsNumber;

}team;
void readPlayer(player* P);
void initTeam(team* T);
void addPlayer(team* Team);
team* maxSuccees(team** Team, int value);
struct namelikea* nameLikeA(team* Team);
int isBirthday(date d);
void printPlayer(player p);
void printTeam(team t);


void main()
{
	printf("welcome to The football club\n");
	int i = 0;
	int j = 0;
	team* sizeteam;
	int size;
	printf("1.enter quantity of teams: ");
	scanf("%d", &size);
	assert(sizeteam = (team*)malloc(sizeof(team)));
	while (j < size)
	{
		initTeam(sizeteam + j);
		while (i < 2)
		{
			addPlayer(sizeteam + j);
			i++;
		}
		j++;
	}

	char* maxname = maxSuccees(&sizeteam, size)->teamName;
	printf("the most succees team is: %s\n", maxname);
	
	for (int i = 0; i < size; i++)
	{
		printTeam(*(sizeteam + i));

	}

	for (i = size - 1; i > 0; i--)
	{
		for ((sizeteam + i)->Maxplayers - 1; j > 0; j--)
		{
			if ((*((sizeteam + i)->Dynamicplayers + j))->PlayerName==NULL)
			{
				free(*((sizeteam + i)->Dynamicplayers + j));
				free(((sizeteam + i)->Dynamicplayers + j));
			}
			else
			{
				free((*((sizeteam + i)->Dynamicplayers + j))->PlayerName);
				free(*((sizeteam + i)->Dynamicplayers + j));
				free(((sizeteam + i)->Dynamicplayers + j));
				
			}
		}
		free(sizeteam + i);

	}
	sizeteam = NULL;
	
}


void readPlayer(player* Player)//A function that receives player information from the user
{
	char str1[20];
	printf("1.enter player's id: ");
	scanf("%d", &Player->playerID);
	printf("2.enter player's name: ");
	scanf("%s", &str1);
	printf("3.enter player's number: ");
	scanf("%d", &Player->playerNumber);
	printf("4.enter player's birthday:\n");
	printf("a.day: ");
	scanf("%d", &Player->playerBirthday.day);
	printf("b.month: ");
	scanf("%d", &Player->playerBirthday.month);
	printf("c.year: ");
	scanf("%d", &Player->playerBirthday.year);
	assert(Player->PlayerName = (char*)malloc(sizeof(char) * (strlen(str1) + 1)));
	strcpy(Player->PlayerName, str1);
}
void initTeam(team* Team) //A function that receives a group's details from the user
{
	char strTeam[30];
	printf("2.enter team's name: ");
	scanf("%s", strTeam);
	assert(Team->teamName = (char*)malloc((strlen(strTeam) + 1) * sizeof(char)));
	strcpy(Team->teamName, strTeam);
	printf("3.enter number of players in team :");
	scanf("%d", &Team->Maxplayers);
	assert(Team->Dynamicplayers = (player**)malloc(sizeof(player*) * Team->Maxplayers));
	Team->Practiceplayers = 0;
	printf("Enter a team wins: ");
	scanf("%d", &Team->WinsNumber);
}
void addPlayer(team* Team) //A function that adds a player to the player array
{
	player* newplayer;
	assert(newplayer = (player*)malloc(sizeof(player)));
	readPlayer(newplayer);
	*((Team->Dynamicplayers) + Team->Practiceplayers) = newplayer;
	Team->Practiceplayers++;

}
team* maxSuccees(team** Team, int value) //A function that returns the team with the most wins
{
	team* max;
	team* recursive = &Team;
	recursive++;
	if (value > 1)
	{
		max = maxSuccees(&recursive, value - 1);
		if ((*Team)->WinsNumber > (max)->WinsNumber)
		{
			return*Team;
		}
		else
		{
			return max;

		}
	}
	else
	{
		return (*Team);
	}



}
struct namelikea* nameLikeA(team* Team)

{
	int counter1 = 0, counter2 = 0;
	team* A = NULL;
	assert(A->Dynamicplayers = (player**)malloc(sizeof(player*) * Team->Practiceplayers));
	while (counter1 < Team->Practiceplayers)
	{


		if (((*Team->Dynamicplayers) + counter1)->PlayerName == 'a')
		{
			*((A->Dynamicplayers) + counter2) = ((*Team->Dynamicplayers) + counter1);
			counter2++;
		}
		counter1++;

	}
	struct namelikea
	{
		team* Team;
		int size;

	}*namelikea;
	namelikea = NULL;
	namelikea->Team = A;
	namelikea->size = counter2;
	return namelikea;
}

int isBirthday(date d)
{
	time_t now = time(0);
	struct tm* ltm = localtime(&now);
	if (d.day == ltm->tm_mday && d.month == ltm->tm_mon + 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void printPlayer(player p)
{

	printf("player's id:%d\n", p.playerID);
	printf("player's name:%s\n", p.PlayerName);
	printf("player's number:%d\n", p.playerNumber);
	printf("player's birthday:%d/%d/%d\n", p.playerBirthday.day,p.playerBirthday.month,p.playerBirthday.year);
	isBirthday(p.playerBirthday);
	if (isBirthday(p.playerBirthday) == 1)
	{
		printf("happy birthday\n");
	}
	else
	{
		printf("no birthday\n");
	}
}

void printTeam(team t)
{
	int i = 0;
	printf("team name: %s\t number of players in team: %d\t number of wins: %d\t", t.teamName, t.Practiceplayers, t.WinsNumber);

	while (i<t.Practiceplayers)
	{

		printPlayer(**(t.Dynamicplayers+i));
		i++;
	}
}




