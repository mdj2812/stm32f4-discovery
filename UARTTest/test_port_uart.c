//
//  test_port_serie.c
//  - envoi de caracteres sur le port serie du PC et attente d'un caractere en reception
//  - teste avec Code::Blocks (projet de type "Console"
//
#include <windows.h>            //utilisation de l'API32 (Application Program Interface) de Windows
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char    NomPort[50];        //nom du port
	int     NumPort;            //numero du port
	HANDLE  hCom;               //gestionnaire du port serie7
	COMSTAT EtatCom;
	DWORD   NbOctetsEcrits;
	DWORD   NbOctetsLus;
	int     NbOctetsALire;
	int     i;
	int     retour;
	DCB     dcb;	            //structure "Device-Control Block" definissant les parametres de la communication
    char    c;
    char    chaine[10];         //chaine de 2 caracteres (pour WriteFile)

    NumPort=3;                     //a adapter
	//-----------ouverture------------------
	sprintf(NomPort, "\\\\.\\COM%d", NumPort);
	hCom=CreateFile(NomPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hCom==INVALID_HANDLE_VALUE)
	{
        printf("erreur CreateFile\n");
        return -1;
	}
    printf("CreateFile OK\n");
	//------------configuration-------------
	retour=GetCommState(hCom, &dcb);    //lecture des parametres actuels du port
	if(retour==0)
	{
	    printf("erreur GetCommState\n");
        exit(1);
	}
    printf("GetCommState OK\n");
	/*dcb.ByteSize=8;			            //nombre de bits de donnee
	dcb.StopBits=1;			            //nombre de bit(s) de stop
	dcb.Parity=NOPARITY;				//type de parite
	dcb.BaudRate=9600;				    //vitesse de transfert des donnees
                                        //ou 110, 150, 300, 600, 1200, 1800, 2400, 2800, 7200, 9600, 14400, 19200,
                                        // 38400, 57600, 115200, 128000, 256000
	retour=SetCommState(hCom, &dcb);    //ecriture de ces parametres
	if(retour==0)
	{
	    printf("erreur SetCommState\n");
        exit(1);
	}
    printf("SetCommState OK\n");*/
    i=0;
    while(1)
    {
        scanf("%c",chaine);
        i++;
        retour=WriteFile(hCom, chaine, 1, &NbOctetsEcrits, NULL);   //ecriture dans port serie
        if(!retour)
        {
                printf("erreur WriteFile\n");
        }
        NbOctetsALire=1;
        Sleep(10);
        retour=ReadFile(hCom, &c, 1, &NbOctetsLus , NULL);   //lecture port serie
        if(!retour)
        {
            printf("erreur ReadFile\n");
        }
        printf("%c\n", c);
    }
	CloseHandle(hCom);					//fermeture du gestionnaire de port (jamais atteint !)

    return 0;
}
